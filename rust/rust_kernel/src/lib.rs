#![no_std]
#![no_main]
#![feature(alloc_error_handler)]

extern crate alloc;
pub mod memory;
pub mod fs;

use core::panic::PanicInfo;
use core::alloc::{GlobalAlloc, Layout};
use alloc::vec::Vec;

// Import C functions
extern "C" {
    fn print(s: *const u8);
    fn malloc(size: usize) -> *mut u8;
    fn free(ptr: *mut u8);
}

// --- Global Allocator ---
struct KernelAllocator;

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        malloc(layout.size())
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        free(ptr);
    }
}

#[global_allocator]
static ALLOCATOR: KernelAllocator = KernelAllocator;

#[alloc_error_handler]
fn alloc_error_handler(layout: Layout) -> ! {
    panic!("allocation error: {:?}", layout)
}

// --- Printing Logic ---
pub struct ExternalPrinter;

impl core::fmt::Write for ExternalPrinter {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        // We need to print null-terminated strings for our C print function
        // This is inefficient but works for now. 
        // Ideally `print` should take a length or we implement putc.
        // For now, let's just loop chars.
        for c in s.chars() {
            // Check if it's ascii, if not, skip or handle?
            // Our C print expects *const u8.
             let mut buf = [0u8; 2];
             let c_val = c as u8; // Simplification for ASCII
             buf[0] = c_val;
             buf[1] = 0;
             unsafe { print(buf.as_ptr()); }
        }
        Ok(())
    }
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ({
        use core::fmt::Write;
        let mut writer = $crate::ExternalPrinter;
        writer.write_fmt(format_args!($($arg)*)).unwrap();
    });
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ({
        $crate::print!($($arg)*);
        $crate::print!("\n");
    });
}


#[no_mangle]
pub extern "C" fn rust_main() {
    println!("Hello from Rust Kernel!");
    
    // Initialize Filesystem
    if fs::init() {
        println!("Rust FS Initialized.");
        // Test looking for /sp/programs
        // let _ = fs::find_by_path("sp/programs");
    } else {
        println!("Rust FS Initialization Failed.");
    }

    // Test Allocator
    let mut v = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);
    
    //println!("Vector test: {:?}", v);
    
    //let s = String::from("String allocation working!");
    //println!("{}", s);
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    println!("RUST PANIC: {}", info);
    loop {}
}
