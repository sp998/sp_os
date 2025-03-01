#!/bin/bash

# Start QEMU
qemu-system-i386 -m 512M -cdrom os.iso &

# Give QEMU some time to start
sleep 2

# Focus QEMU window using AppleScript
osascript -e 'tell application "System Events" to set frontmost of the first process whose name is "qemu-system-i386" to true'
