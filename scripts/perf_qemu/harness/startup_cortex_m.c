/**
 * @file startup_cortex_m.c
 * Reset handler, vector table and the newlib syscall stubs, shared by both Cortex-M
 * boards. They differ only in their memory map (link.ld) and their SysTick rate
 * (target.h), and the symbols this file needs are the same in both linker scripts.
 */
#include <stdint.h>

extern uint32_t _sbss, _ebss, _estack, _heap_start, _heap_end;

int main(void);

/*Coprocessor Access Control Register*/
#define SCB_CPACR (*(volatile uint32_t *)0xE000ED88)

void Reset_Handler(void)
{
    uint32_t * d;

    /*The FPU has to be enabled before any floating point instruction runs. LV_USE_FLOAT
     *is on and this builds with -mfloat-abi=hard, so without this the first FP
     *instruction raises a NOCP usage fault (CFSR bit 19) that escalates to a hard fault,
     *and the image sits in Default_Handler forever with no output at all.*/
    SCB_CPACR |= (3u << 20) | (3u << 22); /*full access to CP10 and CP11*/
    __asm__ volatile("dsb\nisb");

    for(d = &_sbss; d < &_ebss; d++) *d = 0;
    main();
    for(;;) ;
}

void Default_Handler(void)
{
    for(;;) ;
}

__attribute__((section(".vectors"), used))
void * const g_vectors[] = {
    (void *)&_estack, (void *)Reset_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    0, 0, 0, 0, Default_Handler, Default_Handler, 0, Default_Handler, Default_Handler
};

/*newlib is linked with --specs=nosys.specs, which expects these*/
int _close(int f) { (void)f; return -1; }
int _fstat(int f, void * st) { (void)f; (void)st; return 0; }
int _getpid(void) { return 1; }
int _isatty(int f) { (void)f; return 1; }
int _kill(int p, int s) { (void)p; (void)s; return -1; }
int _lseek(int f, int o, int w) { (void)f; (void)o; (void)w; return 0; }
int _read(int f, char * b, int n) { (void)f; (void)b; (void)n; return 0; }
int _write(int f, const char * b, int n) { (void)f; (void)b; return n; }
void _exit(int c) { (void)c; for(;;) ; }

void * _sbrk(int incr)
{
    static uint8_t * brk;
    if(!brk) brk = (uint8_t *)&_heap_start;
    if(brk + incr > (uint8_t *)&_heap_end) return (void *) -1;
    uint8_t * p = brk;
    brk += incr;
    return p;
}
