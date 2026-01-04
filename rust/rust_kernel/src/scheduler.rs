use alloc::collections::VecDeque;
use alloc::boxed::Box;
use crate::process::{Process, ProcessState};
use core::fmt::Write;

// spin::Mutex to allow global mutable access
use spin::Mutex;

#[repr(C, packed)]
pub struct TssEntry {
    pub prev_tss: u32,
    pub esp0: u32,
    pub ss0: u32,
    pub esp1: u32,
    pub ss1: u32,
    pub esp2: u32,
    pub ss2: u32,
    pub cr3: u32,
    pub eip: u32,
    pub eflags: u32,
    pub eax: u32,
    pub ecx: u32,
    pub edx: u32,
    pub ebx: u32,
    pub esp: u32,
    pub ebp: u32,
    pub esi: u32,
    pub edi: u32,
    pub es: u32,
    pub cs: u32,
    pub ss: u32,
    pub ds: u32,
    pub fs: u32,
    pub gs: u32,
    pub ldt: u32,
    pub trap: u16,
    pub iomap_base: u16,
}

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
        // If there's only one process (PID 0 / kernel), don't switch
        if self.processes.len() <= 1 {
            return current_esp;
        }
        
        // 1. Update current process ESP
        if let Some(current) = self.processes.front_mut() {
             if current.state == ProcessState::Running {
                 current.esp = current_esp;
                 current.state = ProcessState::Ready; // Move to ready for rotation
             }
        }

        // 2. Rotate to next process
        // Move front to back
        if let Some(p) = self.processes.pop_front() {
            self.processes.push_back(p);
        }
        
        // 3. Pick new front and switch to it
        if let Some(next) = self.processes.front_mut() {
            next.state = ProcessState::Running;
            self.current_pid = next.pid;
            
            // Update TSS esp0 for syscalls/interrupts from Ring 3
            // The CPU will use this stack when transitioning from user to kernel mode
            if next.kstack_top != 0 {
                unsafe {
                    extern "C" {
                        static mut tss_entry: TssEntry;
                   
                    
                    }
                    tss_entry.esp0 = next.kstack_top;
                    // Debug print to verify context switch details
                 
                }
            }
            
            // Switch CR3 if this process has its own page directory
            if next.cr3 != 0 {
                unsafe {
                    core::arch::asm!("mov cr3, {}", in(reg) next.cr3);
                }
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
            // Capture the current state for the kernel process (PID 0)
            let mut current_esp: u32;
            let mut current_cr3: u32;
            unsafe {
                // We don't actually need ESP for PID 0 since we never switch TO it
                // from another process - we're already running it
                // But we set it to a safe non-zero value
                core::arch::asm!("mov {}, esp", out(reg) current_esp);
                core::arch::asm!("mov {}, cr3", out(reg) current_cr3);
            }
            
            let kernel_proc = Process {
                pid: 0,
                esp: current_esp,  // Capture current ESP
                cr3: current_cr3,  // Capture current CR3
                kstack_top: 0,     // Kernel doesn't need separate kstack
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
    unsafe {
        extern "C" { fn timer_tick(); }
        timer_tick();
    }
    let mut scheduler = SCHEDULER.lock();
    scheduler.schedule(current_esp)
}

#[no_mangle]
pub extern "C" fn rust_spawn_process(entry_point: u32, _ignored_stack_top: u32) {
    unsafe { core::arch::asm!("cli"); }
    {
        let mut scheduler = SCHEDULER.lock();
        let pid = scheduler.processes.len() as u32;

        unsafe {
            extern "C" {
                fn malloc(size: usize) -> *mut u8;
            }
            
            // Capture the current CR3 (page directory) - the ELF loader has already
            // mapped the program's memory into this page directory
            let mut current_cr3: u32;
            core::arch::asm!("mov {}, cr3", out(reg) current_cr3);
            
            // 1. Allocate a separate 16KB KERNEL stack for this process
            let kstack = malloc(16384);
            if kstack.is_null() {
                unsafe { core::arch::asm!("sti"); }
                return;
            }
            let kstack_top = (kstack as u32) + 16384;

            // 2. Allocate a separate 16KB USER stack for this process
            // This ensures every process has its own isolated stack in Ring 3
            let user_stack = malloc(16384);
            if user_stack.is_null() {
                // TODO: Free kstack
                unsafe { core::arch::asm!("sti"); }
                return;
            }
            let mut user_stack_top = (user_stack as u32) + 16384;
            
            // Push a dummy return address (0xDEADBEEF) onto the User Stack.
            // If sp_main returns, it will pop this and crash at 0xDEADBEEF.
            // This helps verify if the process is returning unexpectedly.
            user_stack_top -= 4;
            *(user_stack_top as *mut u32) = 0xDEADBEEF;
            
            unsafe {
                 extern "C" { 
                    #[link_name = "printf_"]
                    fn printf(fmt: *const u8, ...); 
                 }
                 let fmt = b"Spawning PID %d: Entry %x Kstack %x UStack %x\n\0";
                 printf(fmt.as_ptr(), pid, entry_point, kstack_top, user_stack_top);
            }

            // Build the interrupt frame on the kernel stack
            // Start from the top of the kernel stack
            let mut esp = kstack_top;
            
            // IRET frame (bottom of stack, popped last)
            esp -= 4; *(esp as *mut u32) = 0x23;           // SS
            esp -= 4; *(esp as *mut u32) = user_stack_top; // User ESP (top of NEW user stack)
            esp -= 4; *(esp as *mut u32) = 0x202;          // EFLAGS (IF=1)
            esp -= 4; *(esp as *mut u32) = 0x1B;           // CS
            esp -= 4; *(esp as *mut u32) = entry_point;    // EIP
            
            // Error code and interrupt number (skipped by add esp,8)
            esp -= 4; *(esp as *mut u32) = 0;  // err_code
            esp -= 4; *(esp as *mut u32) = 32; // int_no
            
            // PUSHA frame (8 general purpose registers)
            // Order on stack (highest address to lowest): EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
            esp -= 4; *(esp as *mut u32) = 0; // EAX
            esp -= 4; *(esp as *mut u32) = 0; // ECX
            esp -= 4; *(esp as *mut u32) = 0; // EDX
            esp -= 4; *(esp as *mut u32) = 0; // EBX
            esp -= 4; *(esp as *mut u32) = 0; // ESP
            esp -= 4; *(esp as *mut u32) = 0; // EBP
            esp -= 4; *(esp as *mut u32) = 0; // ESI
            esp -= 4; *(esp as *mut u32) = 0; // EDI
            
            // Segment registers (popped in order: GS, FS, ES, DS)
            // So on stack (highest to lowest): DS, ES, FS, GS
            // Correct order for pop:
            // pop gs
            // pop fs
            // pop es
            // pop ds
            //
            // So we must push: DS, ES, FS, GS (top of stack)
            // But we push them manually in reverse order of popping?
            // Wait, pop gs -> stack has GS at top.
            // pop fs -> stack has FS at top.
            
            // Current irq0 handler:
            // mov eax, ds; push eax
            // mov eax, es; push eax
            // mov eax, fs; push eax
            // mov eax, gs; push eax
            // So on stack (top): GS, FS, ES, DS (bottom)
            
            // Restore:
            // pop eax; mov gs, ax  (pops GS)
            // pop eax; mov fs, ax  (pops FS)
            // pop eax; mov es, ax  (pops ES)
            // pop eax; mov ds, ax  (pops DS)
            
            // So we need to push DS, then ES, then FS, then GS (so GS is at top)
            esp -= 4; *(esp as *mut u32) = 0x23; // DS
            esp -= 4; *(esp as *mut u32) = 0x23; // ES
            esp -= 4; *(esp as *mut u32) = 0x23; // FS
            esp -= 4; *(esp as *mut u32) = 0x23; // GS
            
            let new_proc = Process {
                pid,
                esp,  // This is now pointing to the kernel stack with the frame (at GS)
                cr3: current_cr3,
                kstack_top,
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
    let _path = unsafe { core::ffi::CStr::from_ptr(path_ptr as *const core::ffi::c_char).to_str().unwrap() };
    
    extern "C" {
        fn malloc(size: usize) -> *mut u8;
        fn free(ptr: *mut u8);
        fn load_and_print_elf(elf_buffer: *mut u8) -> u32;
        // fn get_user_stack() -> *mut u8; // Removed: We allocate stack per process now
    }

    let buf = unsafe { malloc(128 * 1024) }; // 128KB buffer for ELF
    let size = crate::fs::rust_fs_load_file(path_ptr, buf);

    if size > 0 {
        let entry_point = unsafe { load_and_print_elf(buf) };
        if entry_point != 0 {
            // Pass 0 as stack_top, rust_spawn_process will allocate a new one
            rust_spawn_process(entry_point, 0);
        }
    }
    
    unsafe { free(buf); }
}

extern "C" {
    fn trigger_process(); // ASM helper if needed? Unused for now.
}
