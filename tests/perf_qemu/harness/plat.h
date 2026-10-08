/**
 * @file plat.h
 * The whole porting layer for the benchmark harness: a retired-instruction count, a
 * semihosting call, and a small formatter. One branch per target; a new target needs a
 * branch here, a linker script and a startup file, nothing else.
 *
 * Two targets can count instructions directly: RISC-V reads `instret`, and Cortex-A reads
 * its PMU's INST_RETIRED event, which QEMU implements against the instruction count. Both
 * are exact.
 *
 * Cortex-M has neither, so it falls back on virtual time. QEMU runs with -icount shift=0,
 * where one instruction is one nanosecond of virtual time, so a SysTick delta scaled by
 * the board's SysTick frequency is an instruction count. The scale factor is a fraction in
 * the board's target.h, because it is not always a whole number of nanoseconds per tick.
 *
 * This counts instructions, not cycles. QEMU charges one cycle per instruction and models
 * no branch predictor, wait states or cache, so a change that removes memory traffic looks
 * smaller here than on hardware, and one that folds N stores into a single instruction
 * looks larger.
 */
#ifndef PLAT_H
#define PLAT_H

#include <stdint.h>
#include <stddef.h>

/*Per-board constants. Every target directory has one, even where there is nothing to set.*/
#include "target.h"

/*Semihosting operations used here*/
#define SH_OPEN   0x01
#define SH_CMDLINE 0x15
#define SH_CLOSE  0x02
#define SH_WRITE  0x05
#define SH_WRITE0 0x04
#define SH_EXIT   0x18

/*Semihosting open mode 5 is "wb"*/
#define SH_MODE_WB 5

/**********************
 *   TARGET BRANCHES
 **********************/

#if defined(__riscv)

static inline long sh_call(long op, void * arg)
{
    register long a0 __asm__("a0") = op;
    register void * a1 __asm__("a1") = arg;
    /*The RISC-V semihosting sequence: a magic triple around ebreak*/
    __asm__ volatile(".option push\n.option norvc\n"
                     "slli zero, zero, 0x1f\n"
                     "ebreak\n"
                     "srai zero, zero, 0x7\n"
                     ".option pop\n"
                     : "+r"(a0) : "r"(a1) : "memory");
    return a0;
}

static inline void plat_timer_init(void) { }

static inline uint64_t plat_instr(void)
{
    uint32_t hi, lo, hi2;
    do {
        __asm__ volatile("csrr %0, instreth" : "=r"(hi));
        __asm__ volatile("csrr %0, instret"  : "=r"(lo));
        __asm__ volatile("csrr %0, instreth" : "=r"(hi2));
    } while(hi != hi2);
    return ((uint64_t)hi << 32) | lo;
}

#elif defined(__aarch64__)

static inline long sh_call(long op, void * arg)
{
    register long x0 __asm__("x0") = op;
    register void * x1 __asm__("x1") = arg;
    __asm__ volatile("hlt #0xF000" : "+r"(x0) : "r"(x1) : "memory");
    return x0;
}

/*The PMU's INST_RETIRED event is exact. The generic timer is not: cntvct ticks at
 *62.5 MHz on QEMU virt, so every read quantises to 16 instructions, and two runs of the
 *same tree differ by a few instructions. PMUv3's counters here are 32 bit, so the deltas
 *are accumulated; that is safe as long as plat_instr() is called more often than every
 *4 billion instructions, and the widest gap between two calls is one rendered frame.*/
static uint64_t g_instr_acc;
static uint32_t g_instr_last;

static inline uint32_t pmu_cnt(void)
{
    uint64_t v;
    __asm__ volatile("isb\nmrs %0, pmevcntr0_el0" : "=r"(v));
    return (uint32_t)v;
}

static inline void plat_timer_init(void)
{
    uint64_t pmcr;
    __asm__ volatile("msr pmevtyper0_el0, %0" :: "r"(0x08ull)); /*INST_RETIRED*/
    __asm__ volatile("msr pmcntenset_el0, %0" :: "r"(1ull));
    __asm__ volatile("mrs %0, pmcr_el0" : "=r"(pmcr));
    __asm__ volatile("msr pmcr_el0, %0\nisb" :: "r"(pmcr | 1ull)); /*E: enable*/
    g_instr_last = pmu_cnt();
    g_instr_acc = 0;
}

static inline uint64_t plat_instr(void)
{
    uint32_t v = pmu_cnt();
    g_instr_acc += (uint32_t)(v - g_instr_last);
    g_instr_last = v;
    return g_instr_acc;
}

#else /*Cortex-M*/

static inline long sh_call(long op, void * arg)
{
    register long r0 __asm__("r0") = op;
    register void * r1 __asm__("r1") = arg;
    __asm__ volatile("bkpt #0xAB" : "+r"(r0) : "r"(r1) : "memory");
    return r0;
}

#define ST_CTRL (*(volatile uint32_t *)0xE000E010)
#define ST_LOAD (*(volatile uint32_t *)0xE000E014)
#define ST_VAL  (*(volatile uint32_t *)0xE000E018)

#if !defined(PLAT_NS_NUM) || !defined(PLAT_NS_DEN)
    #error "the board's target.h must give PLAT_NS_NUM/PLAT_NS_DEN, the nanoseconds per SysTick tick"
#endif

static uint64_t g_ticks;
static uint32_t g_last;

static inline void plat_timer_init(void)
{
    ST_LOAD = 0xFFFFFF;
    ST_VAL = 0;
    ST_CTRL = 5; /*enabled, processor clock, no interrupt*/
    g_last = ST_VAL & 0xFFFFFF;
    g_ticks = 0;
}

static inline uint64_t plat_instr(void)
{
    /*SysTick counts down and wraps at 24 bits, so accumulate the deltas*/
    uint32_t v = ST_VAL & 0xFFFFFF;
    g_ticks += (g_last - v) & 0xFFFFFF;
    g_last = v;
    return g_ticks * PLAT_NS_NUM / PLAT_NS_DEN;
}

#endif

/**********************
 *      PORTABLE
 **********************/

static inline void sh_write0(const char * s)
{
    sh_call(SH_WRITE0, (void *)s);
}

static inline void sh_exit(void)
{
    /*ADP_Stopped_ApplicationExit*/
    static volatile uintptr_t blk[2] = { 0x20026, 0 };
    sh_call(SH_EXIT, (void *)blk);
    for(;;) ;
}

/**
 * Open a file on the host for writing.
 * @return a semihosting handle, or -1
 */
static inline long sh_open_w(const char * name)
{
    size_t n = 0;
    while(name[n]) n++;
    uintptr_t blk[3] = { (uintptr_t)name, SH_MODE_WB, (uintptr_t)n };
    return sh_call(SH_OPEN, blk);
}

/**
 * Write a whole buffer to a semihosting handle.
 * @return 0 on success
 */
static inline long sh_write(long handle, const void * buf, size_t len)
{
    const uint8_t * p = (const uint8_t *)buf;
    while(len) {
        uintptr_t blk[3] = { (uintptr_t)handle, (uintptr_t)p, (uintptr_t)len };
        /*SYS_WRITE returns the number of bytes NOT written*/
        long left = sh_call(SH_WRITE, blk);
        if(left < 0 || (size_t)left > len) return -1;
        if((size_t)left == len) return -1; /*no progress*/
        p += len - (size_t)left;
        len = (size_t)left;
    }
    return 0;
}

/**
 * Read the command line QEMU was given with -semihosting-config arg=...
 * @param buf   filled with a null terminated string, empty if there was none
 */
static inline void sh_cmdline(char * buf, size_t size)
{
    uintptr_t blk[2] = { (uintptr_t)buf, (uintptr_t)(size - 1) };
    buf[0] = '\0';
    if(sh_call(SH_CMDLINE, blk) != 0) return;
    if(blk[1] >= size) blk[1] = size - 1;
    buf[blk[1]] = '\0';
}

static inline void sh_close(long handle)
{
    uintptr_t blk[1] = { (uintptr_t)handle };
    sh_call(SH_CLOSE, blk);
}

/*A printf would drag in the C library's formatting, which is not what is being measured
 *and is not available on every target here. These cover everything the mains print.
 *They all build into plat_ob, and plat_flush() expects a pointer into it.*/
static char plat_ob[256];

static inline char * plat_str(char * p, const char * t)
{
    while(*t) *p++ = *t++;
    return p;
}

static inline char * plat_u64(char * p, uint64_t v)
{
    char t[24];
    int n = 0;
    if(!v) t[n++] = '0';
    while(v) {
        t[n++] = (char)('0' + v % 10);
        v /= 10;
    }
    while(n) *p++ = t[--n];
    return p;
}

static inline void plat_flush(char * end)
{
    *end = '\0';
    sh_write0(plat_ob);
}

#endif /*PLAT_H*/
