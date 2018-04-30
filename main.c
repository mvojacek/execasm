#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <memory.h>
#include <sys/mman.h>
#include <errno.h>

#include "asmfuck.h"

#define SIZE_STACK 2048

typedef struct
{
    sbox_regs_t regs;
    uintptr_t eip_offset;
    void *mmap_code;
    size_t mmap_code_size;
    uint8_t stack[SIZE_STACK];
} sbox_t;

void set_unbuffered(int fd);

int sbox_init(sbox_t *sbox);

int sbox_grow_code(sbox_t *sbox, size_t size);

int exec_sandbox_asm(sbox_t *sbox, void *newcode, size_t len);

int main(int argc, char **argv)
{
    int err;
    sleep(1);

    if (argc < 2)
    {
        fprintf(stderr, "Not enough arguments; need: file\n");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return -1;
    }

    set_unbuffered(fd);
    set_unbuffered(STDOUT_FILENO);
    set_unbuffered(STDERR_FILENO);

    u_int8_t packet[0xFF];
    u_int8_t len;

    sbox_t sbox;
    err = sbox_init(&sbox);
    if (err != 0)
        return err;

    while (true)
    {
        while (read(fd, &len, 1) == 0)
        {}

        if (len == 0x00)
            break;

        u_int8_t i = 0;
        while (i < len)
        {
            i += read(fd, packet + i, len - i);
        }

        //write(OUT_FD, packet, len);

        err = exec_sandbox_asm(&sbox, packet, len);
        if (err != 0)
            return 100 + err;
    }

    munmap(sbox.mmap_code, sbox.mmap_code_size);
}

void set_unbuffered(int fd)
{
    struct termios tty_settings;
    tcgetattr(fd, &tty_settings);
    tty_settings.c_lflag &= ~ICANON;
    tcsetattr(fd, TCSANOW, &tty_settings);
}

int sbox_init(sbox_t *sbox)
{
    memset(sbox, 0, sizeof(sbox_t));

    size_t size = SIZE_CODE_TRAILER;
    void *code = mmap(NULL,
                      size,
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_ANONYMOUS | MAP_PRIVATE,
                      -1, 0);
    if (code == MAP_FAILED || code == NULL)
    {
        fprintf(stderr, "\n%s\n", strerror(errno));
        return 1;
    }

    sbox->mmap_code = code;
    sbox->mmap_code_size = size;

    sbox->regs.rbp = sbox->regs.rsp = (register_t) sbox->stack + SIZE_STACK;
    return 0;
}

int sbox_grow_code(sbox_t *sbox, size_t size)
{
    size_t newsize = sbox->mmap_code_size + size;
    void *code = mremap(sbox->mmap_code, sbox->mmap_code_size, newsize, MREMAP_MAYMOVE);
    if (code == MAP_FAILED)
    {
        fprintf(stderr, "\n%s\n", strerror(errno));
        return 1;
    }
    sbox->mmap_code = code;
    sbox->mmap_code_size = newsize;
    return 0;
}

int exec_sandbox_asm(sbox_t *sbox, void *newcode, size_t len)
{
    int i = sbox_grow_code(sbox, len);
    if (i != 0)
        return 10 + i;

    void *real_eip = sbox->mmap_code + sbox->eip_offset;
    void *real_trailer = real_eip + len;
    memcpy(real_eip, newcode, len);

    void *trailer_start = (void *) sbox_asm_trailer_start();
    void *trailer_end = (void *) sbox_asm_trailer_end();
    size_t trailer_len = trailer_end - trailer_start;
    memcpy(real_trailer, trailer_start, trailer_len);

    sbox_asm_run(&sbox->regs, real_eip);

    sbox->eip_offset += len;
    return 0;
}