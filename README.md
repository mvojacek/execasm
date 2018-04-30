# ExecAsm

This C/asm project implements a register+stack sandbox of sorts, so that
arbitrary snippets of machine code can be run and amended.

# Usage

The program takes one commandline argument, the file from which it reads
bytes, it can be a regular file, fifo, socket, whatever.

The bytes are assembled into simple packets:
The first byte is the length of the data in the packet,
and then follows that amount of bytes of data.

Maximum packet length is the maximum of a byte, 0xFF,
however, a second packet may follow which will continue execution.

Sending the byte 0x00 as a length terminates the program, freeing mmapped memory.

There is no guarantee that the code will remain in the same place between packets,
use only position independent machine code to be completely safe.

Code is appended into a growing mmapped region, you may work with
code from previous packets (loops larger than 0xFF bytes, etc.)

The stack has a fixed maximum size of 2048 bytes and resides in non-dynamic memory.

The program itself is probably position-dependent, because I haven't writen
the asm to be position-independent.

You can't define local variables or constants, but a workaround
is shown in pkt/datastore.pkt.

# Utils

`./unhex` is a simple perl oneliner that translates hex pairs into bytes.

[Defuse.ca Online x86 assembler](https://defuse.ca/online-x86-assembler.htm)
is a very useful tool for these things. (not affiliated)

# License

All rights reserved.

# Disclaimer

I am not responsible for anything that happens,
including but not limited to, alien invasions,
computers becoming self-aware, Mark Zuckerberg
collecting your data, your grandmother
getting scammed online, or the FBI taking you
into custody for cyber crime.
