
// Import C functions for memory management
extern "C" {
    fn pmm_alloc_block() -> *mut u8;
    fn pmm_free_block(p: *mut u8);
    fn map_page(phys: *mut u8, virt: *mut u8, flags: u32);
    fn enable_paging();
}

pub const PAGE_PRESENT: u32 = 1;
pub const PAGE_RW: u32 = 2;
pub const PAGE_USER: u32 = 4;

pub fn alloc_frame() -> Option<*mut u8> {
    let ptr = unsafe { pmm_alloc_block() };
    if ptr.is_null() {
        None
    } else {
        Some(ptr)
    }
}

pub fn free_frame(ptr: *mut u8) {
    unsafe { pmm_free_block(ptr) };
}

pub fn map(phys: *mut u8, virt: *mut u8, flags: u32) {
    unsafe { map_page(phys, virt, flags) };
}

pub fn enable() {
    unsafe { enable_paging() };
}
