## System Root
The sysroot is a folder heirarchy that mimics what the final OS installation will look like. While we may not do much with it now, it will make the transition of booting and userland easier when we get that far. According to the OSDev Wiki, the heirarchy should look something like this:

***sysroot/usr/include***: System headers
***sysroot/usr/lib***: System libraries
***sysroot/boot***: Kernel install directory
***sysroot/usr/include/kernel***: Public kernel headers