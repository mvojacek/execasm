.intel_syntax noprefix

.data
sbox_save_rsp:
    .quad 0
sbox_save_rbp:
    .quad 0
sbox_save_reg:
    .quad 0

sbox_save_asm_run_return:
    .quad 0

.text

.globl sbox_asm_run
sbox_asm_run:
    // --------- PROLOGUE
    // new stack frame
    push rbp
    mov rbp, rsp
    // callee-saved registers
    push rbx

    //----------- PREPARE

    // save args
    mov [sbox_save_reg], rdi // 1. sbox_regs_t *reg
    mov r11, rsi // 2. void *rip

    // save current rsp,rbp
    mov [sbox_save_rsp], rsp
    mov [sbox_save_rbp], rbp

    // save return addr
    lea r10, sbox_asm_run_return
    mov [sbox_save_asm_run_return], r10

    // r10 is temp
    mov r10, rdi

    // resotre sbox regs
    mov rax, [r10]    // orig rax
    mov rcx, [r10+8]  // orig rcx
    mov rbx, [r10+16] // orig rbx
    mov rdx, [r10+24] // orig rdx
    mov rsp, [r10+32] // orig rsp
    mov rbp, [r10+40] // orig rbp
    mov rsi, [r10+48] // orig rsi
    mov rdi, [r10+56] // orig rdi

    // jump to sandbox
    //mov r10, [sbox_save_rip]
    jmp r11

    // stuff happens

    //----------- CLEANUP

sbox_asm_run_return:

    // r10 is temp
    mov r10, [sbox_save_reg]

    // save sbox regs
    mov [r10],    rax // orig rax
    mov [r10+8],  rcx // orig rcx
    mov [r10+16], rdx // orig rdx
    mov [r10+24], rbx // orig rbx
    mov [r10+32], rsp // orig rsp
    mov [r10+40], rbp // orig rbp
    mov [r10+48], rsi // orig rsi
    mov [r10+56], rdi // orig rdi
    
    //restore rsp,rbp
    mov rsp, [sbox_save_rsp]
    mov rbp, [sbox_save_rbp]

    //----------- EPILOGUE

    // calle-saved registers
    pop rbx
    // deallocate our stack
    mov rsp, rbp
    // restore caller rbp
    pop rbp

    ret

.globl sbox_asm_trailer_start
sbox_asm_trailer_start:
    lea rax, sbox_trailer_start
    ret

.globl sbox_asm_trailer_end
sbox_asm_trailer_end:
    lea rax, sbox_trailer_end
    ret

sbox_trailer_start:
    // jump back to run_sandbox
    mov r10, [sbox_save_asm_run_return]
    jmp r10
sbox_trailer_end:
    nop
