#!/bin/bash

qemu-system-i386 -m 512M -cdrom os.iso -drive file=sp_disk.img,if=ide -vga std  -boot d




# Give QEMU some time to start
sleep 2

# Focus QEMU window using AppleScript
osascript -e 'tell application "System Events" to set frontmost of the first process whose name is "qemu-system-i386" to true'
