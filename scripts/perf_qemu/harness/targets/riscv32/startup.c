/**
 * @file startup.c
 * Bare metal startup for QEMU -M virt, rv32imac.
 */
#include <stdint.h>

#include "picolibc_stdio.h"

extern uint32_t _sbss, _ebss;

int main(void);

/*picolibc wants this from the application*/
void _exit(int c)
{
    (void)c;
    for(;;) ;
}

void _cstart(void)
{
    uint32_t * d;
    for(d = &_sbss; d < &_ebss; d++) *d = 0;
    main();
    for(;;) ;
}

/*The stack and gp have to exist before any C runs, so the entry point is bare asm*/
__asm__(
    ".section .text.init\n"
    ".global _start\n"
    "_start:\n"
    "  .option push\n"
    "  .option norelax\n"
    "  la gp, __global_pointer$\n"
    "  .option pop\n"
    "  la sp, _estack\n"
    "  call _cstart\n"
    "1: j 1b\n"
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
