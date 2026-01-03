#[derive(Debug, Clone, Copy, PartialEq)]
pub enum ProcessState {
    Ready,
    Running,
    Blocked,
    Terminated,
}

#[derive(Debug)]
pub struct Process {
    pub pid: u32,
    pub esp: u32,       // Stack Pointer (kernel stack)
    pub cr3: u32,       // Page Directory
    pub kstack_top: u32, // Top of kernel stack (for TSS esp0)
    pub state: ProcessState,
    pub stack_check_val: u32, // For debugging stack corruption
}

impl Process {
    pub fn new(pid: u32, esp: u32, cr3: u32) -> Self {
        Process {
            pid,
            esp,
            cr3,
            kstack_top: 0,
            state: ProcessState::Ready,
            stack_check_val: 0xDEADBEEF,
        }
    }
}
