/**
 * @file target.h
 * Board constants for QEMU's mps2-an500 (Cortex-M7).
 */
#ifndef TARGET_H
#define TARGET_H

/* Its SysTick runs at 25 MHz, so a tick is exactly 40 ns and, under -icount shift=0,
 * 40 instructions. Calibrated the same way as the mps3-an547: three instructions per
 * iteration of a loop measured exactly by rdinstret on RISC-V. */
#define PLAT_NS_NUM 40u
#define PLAT_NS_DEN 1u

#endif /*TARGET_H*/
