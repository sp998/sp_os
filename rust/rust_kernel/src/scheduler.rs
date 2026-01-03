use alloc::collections::VecDeque;
use alloc::boxed::Box;
use crate::process::{Process, ProcessState};
use core::fmt::Write;

// spin::Mutex to allow global mutable access
use spin::Mutex;

pub struct Scheduler {
    processes: VecDeque<Box<Process>>,
    current_pid: u32,
}

impl Scheduler {
    pub const fn new() -> Self {
        Scheduler {
            processes: VecDeque::new(),
            current_pid: 0,
        }
    }

    pub fn add_process(&mut self, process: Process) {
        self.processes.push_back(Box::new(process));
    }

    pub fn schedule(&mut self, current_esp: u32) -> u32 {
        // 1. Update current process ESP
        if let Some(current) = self.processes.front_mut() {
             if current.state == ProcessState::Running {
                 current.esp = current_esp;
                 current.state = ProcessState::Ready; // Move to ready for rotation
             }
        } else {
            // Should not happen if we always have Kernel (PID 0)
        }

        // 2. Rotate to next process
        if self.processes.len() > 1 {
            // Move front to back
            if let Some(p) = self.processes.pop_front() {
                self.processes.push_back(p);
            }
        }
        
        // 3. Pick new front
        if let Some(next) = self.processes.front_mut() {
            next.state = ProcessState::Running;
            self.current_pid = next.pid;
            
            // Switch CR3 if needed (TODO: exposed CR3 update or done in ASM)
            // For now, we return ESP. ASM/C needs to handle CR3 if we need to switch it.
            // But wait, `rust_schedule` only returns ESP. 
            // We might need to handle CR3 switching here via inline asm or FFI if strictly necessary.
            // But since all tasks share kernel mapping and we are identity mapped mostly...
            // Let's assume CR3 switch is handled or not needed yet for basic threads.
            
            if next.cr3 != 0 {
                // unsafe { core::arch::asm!("mov cr3, {}", in(reg) next.cr3) }; // Optional
            }
            
            return next.esp;
        }

        current_esp 
    }
    
    pub fn get_process_list(&self, buffer: &mut [u8]) {
        let mut wrapper = BufferWrapper::new(buffer);
        
        // Ignored result to avoid warning
        let _ = write!(wrapper, "PID  ESP       CR3       State\n");
        let _ = write!(wrapper, "---  --------  --------  -----\n");
        
        for process in &self.processes {
            let state_str = match process.state {
                ProcessState::Ready => "Ready",
                ProcessState::Running => "Run",
                ProcessState::Blocked => "Block",
                ProcessState::Terminated => "Term",
            };
            
            let _ = write!(wrapper, "{:<4} {:08x}  {:08x}  {}\n", 
                           process.pid, process.esp, process.cr3, state_str);
        }
    }
    
    pub fn kill_process(&mut self, pid: u32) -> bool {
        if pid == 0 { return false; } // Cannot kill kernel
        
        if let Some(pos) = self.processes.iter().position(|p| p.pid == pid) {
            self.processes.remove(pos);
            return true;
        }
        
        false
    }
}

// Global Scheduler Instance
pub static SCHEDULER: Mutex<Scheduler> = Mutex::new(Scheduler::new());

// --- FFI Interfaces ---

#[no_mangle]
pub extern "C" fn rust_init_multitasking() {
    unsafe { core::arch::asm!("cli"); }
    {
        let mut scheduler = SCHEDULER.lock();
        if scheduler.processes.is_empty() {
            let kernel_proc = Process {
                pid: 0,
                esp: 0, 
                cr3: 0, 
                state: ProcessState::Running,
                stack_check_val: 0xDEADBEEF,
            };
            scheduler.processes.push_back(Box::new(kernel_proc));
        }
    }
    unsafe { core::arch::asm!("sti"); }
}

#[no_mangle]
pub extern "C" fn rust_schedule(current_esp: u32) -> u32 {
    let mut scheduler = SCHEDULER.lock();
    scheduler.schedule(current_esp)
}

#[no_mangle]
pub extern "C" fn rust_spawn_process(entry_point: u32, stack_top: u32) {
    unsafe { core::arch::asm!("cli"); }
    {
        let mut scheduler = SCHEDULER.lock();
        let pid = scheduler.processes.len() as u32; // pid 0 is kernel

        unsafe {
            let mut esp = stack_top;
            
            // 1. IRET Frame (Ring 3)
            esp -= 4; *(esp as *mut u32) = 0x23;  // SS (User Data) 
            esp -= 4; *(esp as *mut u32) = stack_top - 128; // User ESP
            esp -= 4; *(esp as *mut u32) = 0x202; // EFLAGS (IF=1)
            esp -= 4; *(esp as *mut u32) = 0x1B;  // CS (User Code)
            esp -= 4; *(esp as *mut u32) = entry_point; // EIP
            
            // 2. Dummy error code and int no
            esp -= 4; *(esp as *mut u32) = 0; // err_code
            esp -= 4; *(esp as *mut u32) = 32; // int_no

            // 3. Pusha (8 registers) - Initialize to 0
            for _ in 0..8 {
                esp -= 4; *(esp as *mut u32) = 0;
            }
            
            // 4. DS
            esp -= 4; *(esp as *mut u32) = 0x23; // DS (User Data)
            
            let new_proc = Process {
                pid,
                esp,
                cr3: 0, 
                state: ProcessState::Ready,
                stack_check_val: 0xDEADBEEF,
            };
            
            scheduler.processes.push_back(Box::new(new_proc));
        }
    }
    unsafe { core::arch::asm!("sti"); }
}

// Wrapper for writing to a C buffer
struct BufferWrapper<'a> {
    buffer: &'a mut [u8],
    offset: usize,
}

impl<'a> BufferWrapper<'a> {
    fn new(buffer: &'a mut [u8]) -> Self {
        BufferWrapper { buffer, offset: 0 }
    }
}

impl<'a> Write for BufferWrapper<'a> {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        for b in s.bytes() {
            if self.offset < self.buffer.len() - 1 {
                self.buffer[self.offset] = b;
                self.offset += 1;
            }
        }
        self.buffer[self.offset] = 0; // Keep null-terminated
        Ok(())
    }
}

#[no_mangle]
pub extern "C" fn rust_process_list(buffer: *mut u8, max_len: usize) {
    let scheduler = SCHEDULER.lock();
    let buf_slice = unsafe { core::slice::from_raw_parts_mut(buffer, max_len) };
    scheduler.get_process_list(buf_slice);
}

#[no_mangle]
pub extern "C" fn rust_kill_process(pid: u32) -> bool {
    let mut scheduler = SCHEDULER.lock();
    scheduler.kill_process(pid)
}

#[no_mangle]
pub extern "C" fn rust_spawn_process_from_file(path_ptr: *const u8) {
    let path = unsafe { core::ffi::CStr::from_ptr(path_ptr as *const core::ffi::c_char).to_str().unwrap() };
    
    extern "C" {
        fn malloc(size: usize) -> *mut u8;
        fn load_and_print_elf(elf_buffer: *mut u8) -> u32;
        fn get_user_stack() -> *mut u8;
    }

    let buf = unsafe { malloc(128 * 1024) }; // 128KB buffer for ELF
    let size = crate::fs::rust_fs_load_file(path_ptr, buf);

    if size > 0 {
        let entry_point = unsafe { load_and_print_elf(buf) };
        if entry_point != 0 {
            // Stack top
            let stack_top = unsafe { get_user_stack().add(32768) as u32 };
            rust_spawn_process(entry_point, stack_top);
        }
    }
}

extern "C" {
    fn trigger_process(); // ASM helper if needed? Unused for now.
}
