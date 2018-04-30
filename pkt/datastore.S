.byte 0xeb,17 # relative jump by 17 insns
.ascii "i am a datastore\n"
lea    rsi,[rip+0x0] # get rip
sub    rsi,7 # length of lea insn + args
mov    rdx,17 # length of data
sub    rsi,rdx # move rsi to start of data
mov    rax,1 # WRITE syscall
mov    rdi,1 # fd1 stdout
syscall
