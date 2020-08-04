## System Architectures
A lot of the functions in the kernel folder (basically 100% of them) assume we're working with an Intel/AMD i386-style CPU.
All code that makes this assumption should go into the i386 folder and all code that enables long mode (x64 execution) should go in an x86_64 folder. Likewise, any ARM specific code should go in an armvX folder where X is the ARM processor version.

There's still some cleanup to do in terms of moving processor specific code into this folder, but we'll do that eventually.

## TODO List:
**amd64**: 
- [ ] Convert IDT to amd64. There needs to be a new descriptor table moved into arch.hpp [Source](https://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_AMD64)
- [ ] Fix `uint32_t` issue in paging code (ugh...) 
`kernel//mem/paging.cpp:143: Error: unsupported instruction 'mov'`