/**
 * @file target.h
 * Board constants for QEMU's mps3-an547 (Cortex-M55).
 */
#ifndef TARGET_H
#define TARGET_H

/* Its SysTick runs at 32 MHz, so a tick is 31.25 ns and, under -icount shift=0,
 * 31.25 instructions. A fraction rather than a constant because 1000000000 / 32000000 in
 * integers is 31, which makes every count 0.8% low.
 *
 * Calibrated against a loop whose length is known exactly from rdinstret on RISC-V and
 * from the PMU on Cortex-A53: three instructions per iteration on all three, and this
 * board reports 96001 ticks per million iterations. */
#define PLAT_NS_NUM 125u
#define PLAT_NS_DEN 4u

#endif /*TARGET_H*/
