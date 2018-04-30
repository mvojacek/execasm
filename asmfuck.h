//
// Created by michal on 28/04/18.
//

#include <stdint.h>

#ifndef EXECASM_ASMFUCK_H
#define EXECASM_ASMFUCK_H

#define SIZE_CODE_TRAILER 16

typedef struct
{
    register_t rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi;
} sbox_regs_t;

void sbox_asm_run(sbox_regs_t *reg, void *eip);

uint64_t sbox_asm_trailer_start();
uint64_t sbox_asm_trailer_end();

#endif //EXECASM_ASMFUCK_H
