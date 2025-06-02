#![no_std]
#![no_main]


use core::{panic::PanicInfo};

      
extern "C" {
    fn print(s: *const u8);
}


#[no_mangle]
pub extern "C" fn rust_main() {

    let message = b"Hello from rust \n\0";

    unsafe {
        print(message.as_ptr() as *const u8);
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}
