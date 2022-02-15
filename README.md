# simple-anti-anti-debug

Compile for x86 or x64 depending on game you are targetting. Reads the Process Environment Block and continually patches the BeingDebugged BYTE to 0. Bypasses isBeingDebugged(), plan on adding more functionality in the future.
