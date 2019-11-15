A lot of the functions in the kernel folder (basically 100% of them) assume we're working with an Intel/AMD i386-style CPU.
This kind of goes against our design of having all intel specific architecture code in the i386 folder, so eventually we
should move it all into here in the even we ever want to port to x86_64 or ARM (which isn't all that likely to happen).