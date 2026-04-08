/**
 * @file lv_blend_riscv_v.h
 * RISC-V Vector extension blend header
 */

#ifndef LV_BLEND_RISCV_V_H
#define LV_BLEND_RISCV_V_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_draw_sw_blend.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

#include "lv_draw_sw_blend_riscv_v_to_rgb888.h"

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V */

#ifdef __cplusplus
}
#endif

#endif /* LV_BLEND_RISCV_V_H */
