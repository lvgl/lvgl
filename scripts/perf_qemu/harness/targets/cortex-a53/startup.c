/**
 * @file startup.c
 * Bare metal startup for QEMU -M virt, Cortex-A53.
 *
 * QEMU enters at EL1 with the MMU off, and with the MMU off aarch64 treats every access
 * as device memory, where an unaligned load faults. A flat map therefore has to be
 * installed before any C runs.
 */
#include <stdint.h>

#include "picolibc_stdio.h"

extern uint8_t _sbss[], _ebss[];

int main(void);

/*picolibc wants this from the application*/
void _exit(int c)
{
    (void)c;
    for(;;) ;
}

/*Four 1 GB blocks cover everything QEMU virt puts in the low 4 GB*/
static uint64_t l1_table[4] __attribute__((aligned(4096)));

/*attr0 normal write back write allocate, attr1 device nGnRnE*/
#define MAIR_VALUE 0x00000000000000FFull
/*39 bit VA, 4 KB granule, inner shareable, write back, TTBR1 off, 40 bit PA*/
#define TCR_VALUE  0x0000000200803519ull

#define BLK_NORMAL(gb) (((uint64_t)(gb) << 30) | (3ull << 8) | (1ull << 10) | (0ull << 2) | 1ull)
#define BLK_DEVICE(gb) (((uint64_t)(gb) << 30) | (1ull << 10) | (1ull << 2) | 1ull)

void _cstart(void)
{
    uint8_t * p;
    uint64_t v;

    for(p = _sbss; p < _ebss; p++) *p = 0;

    l1_table[0] = BLK_DEVICE(0); /*peripherals*/
    l1_table[1] = BLK_NORMAL(1); /*RAM*/
    l1_table[2] = BLK_NORMAL(2);
    l1_table[3] = BLK_NORMAL(3);

    __asm__ volatile(
        "msr mair_el1, %0\n"
        "msr tcr_el1, %1\n"
        "msr ttbr0_el1, %2\n"
        "dsb sy\n isb\n"
        "tlbi vmalle1\n dsb sy\n isb\n"
        :: "r"((uint64_t)MAIR_VALUE), "r"((uint64_t)TCR_VALUE), "r"((uint64_t)(uintptr_t)l1_table)
        : "memory");

    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(v));
    v |= (1ull << 0) | (1ull << 2) | (1ull << 12); /*MMU, data cache, instruction cache*/
    __asm__ volatile("msr sctlr_el1, %0\n isb\n" :: "r"(v) : "memory");

    main();
    for(;;) ;
}

/*The stack and the FPU have to exist before any C runs, so the entry point is bare asm.
 *The FPU in particular: at -O2 the compiler vectorizes even the BSS clear into SIMD
 *stores, and with CPACR still closed the first one traps into a silent loop with no
 *exception vectors installed and no output at all.*/
__asm__(
    ".section .text.init\n"
    ".global _start\n"
    "_start:\n"
    "  mov  x0, #(3 << 20)\n"
    "  msr  cpacr_el1, x0\n"
    "  isb\n"
    "  adrp x0, _estack\n"
    "  add  x0, x0, :lo12:_estack\n"
    "  mov  sp, x0\n"
    "  b    _cstart\n"
);

/*picolibc's stdio layer references these. There is no file system here.*/
int open(const char * p, int f, ...) { (void)p; (void)f; return -1; }
int close(int fd) { (void)fd; return -1; }
long read(int fd, void * b, unsigned long n) { (void)fd; (void)b; (void)n; return -1; }
long write(int fd, const void * b, unsigned long n) { (void)fd; (void)b; return (long)n; }
long lseek(int fd, long o, int w) { (void)fd; (void)o; (void)w; return -1; }
int unlink(const char * p) { (void)p; return -1; }
int fstat(int fd, void * st) { (void)fd; (void)st; return -1; }
int isatty(int fd) { (void)fd; return 1; }
