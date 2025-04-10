/**
 * @file lv_blend_helium.S
 *
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "lv_blend_helium.h"

/*GCC Workaround: missing .note.GNU-stack section implies executable stack*/
#ifdef __ELF__
.section .note.GNU-stack,"",%progbits
#endif /* __ELF__ */


#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE && LV_USE_NATIVE_HELIUM_ASM

.data
reciprocal:
.byte 0xFF, 0xE2, 0xCC, 0xB9, 0xAA, 0x9C, 0x91, 0x88

.text
.syntax unified
.p2align 2

TMP         .req r0
DST_ADDR    .req r1
DST_W       .req r2
DST_H       .req r3
DST_STRIDE  .req r4
SRC_ADDR    .req r5
SRC_STRIDE  .req r6
MASK_ADDR   .req r7
MASK_STRIDE .req r8
H           .req r9
OPA         .req r10
RCP         .req r11

S_B         .req q0
S_G         .req q1
S_R         .req q2
S_A         .req q3
D_B         .req q4
D_G         .req q5
D_R         .req q6
D_A         .req q7
N           .req q0
V           .req q1
R           .req q2
L           .req q4
S_565       .req q0
D_565       .req q1
S_L         .req q2
D_L         .req q4
D_T         .req q5
BITMASK     .req q6

.macro ldst st, op, bpp, mem, reg, areg, cvt, alt_index, wb, aligned
.if \bpp == 0
.if \cvt
    ldr             TMP, [\mem\()_ADDR]
    bfi             TMP, TMP, #2, #8
    bfi             TMP, TMP, #3, #16
    lsr             TMP, TMP, #8
    vdup.16         \reg\()_565, TMP
.else
    ldr             TMP, [\mem\()_ADDR]
    vdup.8          \reg\()_B, TMP
    lsr             TMP, #8
    vdup.8          \reg\()_G, TMP
    lsr             TMP, #8
    vdup.8          \reg\()_R, TMP
.endif
.elseif \bpp == 8
.if \cvt
    v\op\()rb.u16   \reg\()_A, [\mem\()_ADDR], #8
.else
    v\op\()rb.8     \reg\()_A, [\mem\()_ADDR], #16
.endif
.elseif \bpp == 16
.if \cvt
.if \st
    vsri.8          \reg\()_R, \reg\()_G, #5
    vshr.u8         \reg\()_G, \reg\()_G, #2
    vshr.u8         \reg\()_B, \reg\()_B, #3
    vsli.8          \reg\()_B, \reg\()_G, #5
.endif
.if \alt_index
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, S_B]
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, S_G]
.else
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, \reg\()_A]
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, \reg\()_A]
.endif
.if \st == 0
    vshl.u8         \reg\()_G, \reg\()_R, #5
    vsri.u8         \reg\()_G, \reg\()_B, #3
    vshl.u8         \reg\()_B, \reg\()_B, #3
    vsri.u8         \reg\()_R, \reg\()_R, #5
    vsri.u8         \reg\()_G, \reg\()_G, #6
    vsri.u8         \reg\()_B, \reg\()_B, #5
.endif
.ifc \wb, !
.if \alt_index
    add             \mem\()_ADDR, #32
.else
    add             \mem\()_ADDR, #31
.endif
.elseif \alt_index == 0
    sub             \mem\()_ADDR, #1
.endif
.else @ cvt
.ifc \wb, !
    v\op\()rh.16    \reg\()_565, [\mem\()_ADDR], #16
.else
    v\op\()rh.16    \reg\()_565, [\mem\()_ADDR]
.endif
.endif
.elseif \bpp == 24
.if \alt_index == 1
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, S_B]
    v\op\()rb.8     \reg\()_G, [\mem\()_ADDR, S_G]
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, S_R]
.elseif \alt_index == 2
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, S_R]
    v\op\()rb.8     \reg\()_G, [\mem\()_ADDR, S_A]
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, D_A]
.else
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, \reg\()_A]
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_G, [\mem\()_ADDR, \reg\()_A]
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, \reg\()_A]
.endif
.ifc \wb, !
.if \alt_index
    add             \mem\()_ADDR, #48
.else
    add             \mem\()_ADDR, #46
.endif
.elseif \alt_index == 0
    sub             \mem\()_ADDR, #2
.endif
.elseif \aligned
    v\op\()40.8     {\reg\()_B, \reg\()_G, \reg\()_R, \reg\()_A}, [\mem\()_ADDR]
    v\op\()41.8     {\reg\()_B, \reg\()_G, \reg\()_R, \reg\()_A}, [\mem\()_ADDR]
    v\op\()42.8     {\reg\()_B, \reg\()_G, \reg\()_R, \reg\()_A}, [\mem\()_ADDR]
    v\op\()43.8     {\reg\()_B, \reg\()_G, \reg\()_R, \reg\()_A}, [\mem\()_ADDR]\wb
.else
    v\op\()rb.8     \reg\()_B, [\mem\()_ADDR, \areg\()_A]
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_G, [\mem\()_ADDR, \areg\()_A]
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_R, [\mem\()_ADDR, \areg\()_A]
.if (\bpp == 32) || (\bpp == 31) && \st
    add             \mem\()_ADDR, #1
    v\op\()rb.8     \reg\()_A, [\mem\()_ADDR, \areg\()_A]
.endif
.ifc \wb, !
    .if (\bpp == 32) || (\bpp == 31) && \st
        add         \mem\()_ADDR, #61
    .else
        add         \mem\()_ADDR, #62
    .endif
.else
    .if (\bpp == 32) || (\bpp == 31) && \st
        sub         \mem\()_ADDR, #3
    .else
        sub         \mem\()_ADDR, #2
    .endif
.endif
.endif
.endm

.macro load_index bpp, reg, areg, aligned
.if (\bpp > 0) && ((\bpp < 31) || (\aligned == 0))
    mov             TMP, #0
.if \bpp == 8
    vidup.u8        \reg\()_A, TMP, #1
.elseif \bpp == 16
    vidup.u8        \reg\()_A, TMP, #2
.elseif \bpp == 24
    vidup.u8        \reg\()_A, TMP, #1
    mov             TMP, #3
    vmul.u8         \reg\()_A, \reg\()_A, TMP
.else
    vidup.u8        \areg\()_A, TMP, #4
.endif
.endif
.endm

.macro init src_bpp, dst_bpp, mask, opa
    ldr             DST_ADDR, [r0, #4]
    ldr             DST_W, [r0, #8]
    ldr             DST_H, [r0, #12]
    ldr             DST_STRIDE, [r0, #16]
    ldr             SRC_ADDR, [r0, #20]
.if \src_bpp > 0
    ldr             SRC_STRIDE, [r0, #24]
.endif
.if \mask
    ldr             MASK_ADDR, [r0, #28]
    ldr             MASK_STRIDE, [r0, #32]
.endif
.if \opa
    ldr             OPA, [r0]
.endif
.if (\src_bpp <= 16) && (\dst_bpp == 16)
.if \opa || \mask
    mov             TMP, #0xF81F
    movt            TMP, #0x7E0
    vdup.32         BITMASK, TMP
.endif
    add             TMP, DST_W, #0x7
    bic             TMP, TMP, #0x7
.else
    add             TMP, DST_W, #0xF
    bic             TMP, TMP, #0xF
.endif
.if \dst_bpp == 32
    ldr             RCP, =(reciprocal - 8)
.endif

.if \dst_bpp == 16
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #1
.elseif \dst_bpp == 24
    sub             DST_STRIDE, DST_STRIDE, TMP
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #1
.elseif \dst_bpp >= 31
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #2
.endif
.if \mask
    sub             MASK_STRIDE, MASK_STRIDE, TMP
.endif
.if \src_bpp == 0
.if \mask || \opa
    .if \dst_bpp > 16
        ldst        0, ld, \src_bpp, SRC, S, D, 0, 0
        vmov.u8     S_A, #0xFF
    .else
        ldst        0, ld, \src_bpp, SRC, S, D, 1, 0
        vmovlb.u16  S_L, S_565
        vsli.32     S_L, S_L, #16
        vand        S_L, S_L, BITMASK
    .endif
.else
    .if \dst_bpp > 16
        ldst        0, ld, \src_bpp, SRC, D, S, 0, 0
    .else
        ldst        0, ld, \src_bpp, SRC, D, S, 1, 0
    .endif
.endif
.else
    .if \src_bpp == 16
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #1
    .elseif \src_bpp == 24
        sub         SRC_STRIDE, SRC_STRIDE, TMP
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #1
    .elseif \src_bpp >= 31
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #2
    .endif
.endif
.if (\src_bpp < 32) && (\mask == 0) && (\opa == 0) && !((\src_bpp <= 16) && (\dst_bpp == 16))
@ 16 to 31/32 or reverse: index @ q0, q1
@ 24 to 31/32 or reverse: index @ q0, q1, q2
@ 16 to 24 or reverse: 16 index @ q0, q1, 24 index @ q2, q3, q7
@ 31 to 31/32: index @ q3 (tail only)
    mov         TMP, #0
.if (\src_bpp == 16) || (\dst_bpp == 16)
    vidup.u8    S_B, TMP, #2
    mov         TMP, #1
    vadd.u8     S_G, S_B, TMP
.if (\src_bpp == 24) || (\dst_bpp == 24)
    vshl.u8     S_R, S_B, #1
    vadd.u8     S_R, S_R, S_B
    vshr.u8     S_R, S_R, #1
    vadd.u8     S_A, S_R, TMP
    vadd.u8     D_A, S_A, TMP
.endif
.elseif (\src_bpp == 24) || (\dst_bpp == 24)
    vidup.u8    S_B, TMP, #1
    mov         TMP, #3
    vmul.u8     S_B, S_B, TMP
    mov         TMP, #1
    vadd.u8     S_G, S_B, TMP
    vadd.u8     S_R, S_G, TMP
.endif
.if \dst_bpp >= 31
    load_index  \dst_bpp, D, S, 0
    vmov.u8     D_A, #0xFF
.endif
.endif
.endm

.macro vqrdmulh_u8 Qd, Qn, Qm      @ 1 bit precision loss
    vmulh.u8       \Qd, \Qn, \Qm
    vqshl.u8       \Qd, \Qd, #1
.endm

.macro premult mem, alpha
    vrmulh.u8       \mem\()_B, \mem\()_B, \alpha
    vrmulh.u8       \mem\()_G, \mem\()_G, \alpha
    vrmulh.u8       \mem\()_R, \mem\()_R, \alpha
.endm

.macro blend_565 p
    vmovl\p\().u16  D_L, D_565
    vsli.32         D_L, D_L, #16
    vand            D_L, D_L, BITMASK
    vsub.u32        D_T, S_L, D_L
    vmovl\p\().u16  D_A, S_A
    vmul.u32        D_T, D_T, D_A
    vshr.u32        D_T, D_T, #5
    vadd.u32        D_L, D_L, D_T
    vand            D_L, D_L, BITMASK
    vshr.u32        D_T, D_L, #16
    vorr            D_L, D_L, D_T
    vmovn\p\().u32  D_565, D_L
.endm

.macro late_init src_bpp, dst_bpp, mask, opa, mode
.if (\src_bpp <= 16) && (\dst_bpp == 16) && (\mask == 0)
.if \opa == 2
    mov             TMP, #0x7BEF
    vdup.16         BITMASK, TMP
.if \src_bpp == 0
    vshr.u16        S_L, S_565, #1
    vand            S_L, S_L, BITMASK
.endif
.elseif \opa == 1
    vdup.16         S_A, OPA
    mov             TMP, #4
    vadd.u16        S_A, S_A, TMP
    vshr.u16        S_A, S_A, #3
.endif
.endif
.endm

.macro blend src_bpp, dst_bpp, mask, opa, mode
.if (\mask == 0) && (\opa == 2)
.if (\src_bpp <= 16) && (\dst_bpp == 16)
.if \src_bpp > 0
    vshr.u16        S_L, S_565, #1
    vand            S_L, S_L, BITMASK
.endif
    vshr.u16        D_L, D_565, #1
    vand            D_L, D_L, BITMASK
    vadd.u16        D_565, S_L, D_L
.else
    vhadd.u8        D_B, D_B, S_B
    vhadd.u8        D_G, D_G, S_G
    vhadd.u8        D_R, D_R, S_R
.endif
.elseif (\src_bpp <= 16) && (\dst_bpp == 16)
    lsl             lr, #1
.if \src_bpp > 0
    vmovlb.u16      S_L, S_565
    vsli.32         S_L, S_L, #16
    vand            S_L, S_L, BITMASK
.endif
    blend_565       b
.if \src_bpp > 0
    vmovlt.u16      S_L, S_565
    vsli.32         S_L, S_L, #16
    vand            S_L, S_L, BITMASK
.endif
    blend_565       t
    lsr             lr, #1
.else
.if \dst_bpp < 32
.if (\opa == 0) && (\mask == 0)
    vmov.u8         D_A, #0xFF
    mov             TMP, #0
    vabav.u8        TMP, S_A, D_A
    cbnz            TMP, 91f
    vmov            D_B, S_B
    vmov            D_G, S_G
    vmov            D_R, S_R
    b               88f
91:
.endif
    vmvn            D_A, S_A
    premult         S, S_A
    premult         D, D_A
.else
    vpush           {d0-d5}
    vmov.u8         S_B, #0xFF
    vmov.u8         S_G, #0
    mov             TMP, #0
    vabav.u8        TMP, S_A, S_B
    cbz             TMP, 91f        @ if(fg.alpha == 255
    mov             TMP, #0
    vabav.u8        TMP, D_A, S_G
    cbnz            TMP, 90f        @    || bg.alpha == 0)
91:
    vpop            {d8-d13}        @   return fg;
    vmov.u8         D_A, #0xFF
    b               88f
90:
    mov             TMP, #0
    vabav.u8        TMP, S_A, S_G
    cmp             TMP, #2         @ if(fg.alpha <= LV_OPA_MIN)
    itt             le              @   return bg;
    vpople          {d0-d5}
    ble             88f
    mov             TMP, #0
    vabav.u8        TMP, D_A, S_B   @ if (bg.alpha == 255)
    cbnz            TMP, 89f        @   return lv_color_mix32(fg, bg);
    vpop            {d0-d5}
    vmvn            D_A, S_A
    premult         S, S_A
    premult         D, D_A
    vqadd.u8        D_B, D_B, S_B
    vqadd.u8        D_G, D_G, S_G
    vqadd.u8        D_R, D_R, S_R
    vmov.u8         D_A, #0xFF
    b               88f
89:
    vmvn            N, S_A
    vmvn            D_A, D_A
    vrmulh.u8       D_A, N, D_A
    vmvn            D_A, D_A        @ D_A = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha)
    vclz.i8         N, D_A          @ n = clz(D_A)
    vshl.u8         V, D_A, N       @ v = D_A << n
    vshl.u8         S_A, S_A, N
    vshr.u8         N, V, #4        @ N is used as tmp from now on
    vldrb.u8        R, [RCP, N]     @ r = reciprocal[(v >> 4) - 8]
    vrmulh.u8       N, V, R         @ r = newton(v,r)
    vmvn            N, N            @   = vqrdmulh.u8(vmvn(vrmulh(v, r)), r)
    vqrdmulh_u8     R, N, R         @ but vqrdmulh does not support u8, so we implement one
    vrmulh.u8       N, V, R         @ and do it twice
    vmvn            N, N
    vqrdmulh_u8     R, N, R
    vqrdmulh_u8     S_A, S_A, R     @ S_A' = S_A * 255 / D_A = vrdmulh(S_A << n, r)
    vpop            {d0-d5}
    premult         S, S_A
    vmvn            S_A, S_A
    premult         D, S_A
.endif
    vqadd.u8        D_B, D_B, S_B
    vqadd.u8        D_G, D_G, S_G
    vqadd.u8        D_R, D_R, S_R
.endif
.if \dst_bpp == 31
    vmov.u8         D_A, #0xFF
.endif
88:
.endm

.macro blend_line src_bpp, dst_bpp, mask, opa, mode
.if (\src_bpp < 31) && (\dst_bpp < 31)
    blend_block     \src_bpp, \dst_bpp, \mask, \opa, \mode, DST_W, 0
.else
    bics            TMP, DST_W, #0xF
    beq             87f
    blend_block     \src_bpp, \dst_bpp, \mask, \opa, \mode, TMP, 1
87:
    ands            TMP, DST_W, #0xF
    beq             86f
    blend_block     \src_bpp, \dst_bpp, \mask, \opa, \mode, TMP, 0
86:
.endif
.endm

.macro blend_block src_bpp, dst_bpp, mask, opa, mode, w, aligned
.if (\src_bpp <= 16) && (\dst_bpp == 16)
    wlstp.16            lr, \w, 1f
.else
    wlstp.8             lr, \w, 1f
.endif
2:
.if (\src_bpp < 32) && (\mask == 0) && (\opa == 0)
@ no blend
    .if \src_bpp == 0
        ldst            1, st, \dst_bpp, DST, D, S, 0, 1, !, \aligned
    .elseif (\src_bpp == \dst_bpp) || (\src_bpp == 31) && (\dst_bpp == 32)
        .if \dst_bpp < 31
            .if \src_bpp < 31
                ldst    0, ld, \src_bpp, SRC, D, S, 0, 1, !, \aligned
            .else
                ldst    0, ld, \src_bpp, SRC, D, S, 0, 1, !, \aligned
            .endif
            ldst        1, st, \dst_bpp, DST, D, S, 0, 1, !, \aligned
        .else
            ldst        0, ld, \src_bpp, SRC, D, S, 0, 1, !, \aligned
            ldst        1, st, \dst_bpp, DST, D, S, 0, 1, !, \aligned
        .endif
    .else
        .if (\dst_bpp < 31) && (\src_bpp < 31)
            ldst        0, ld, \src_bpp, SRC, D, S, 1, 2, !, \aligned
            ldst        1, st, \dst_bpp, DST, D, S, 1, 2, !, \aligned
        .else
            ldst        0, ld, \src_bpp, SRC, D, S, 1, 1, !, \aligned
            ldst        1, st, \dst_bpp, DST, D, S, 1, 1, !, \aligned
        .endif
    .endif
.elseif (\src_bpp <= 16) && (\dst_bpp == 16)
    .if \src_bpp > 0
        ldst            0, ld, \src_bpp, SRC, S, D, 0, 0, !, \aligned
    .endif
        ldst            0, ld, \dst_bpp, DST, D, S, 0, 0, , \aligned
    .if \mask
        ldst            0, ld, 8, MASK, S, D, 1, 0, !
        .if \opa == 2
            vshr.u16    S_A, S_A, #1
        .elseif \opa == 1
            vmul.u16    S_A, S_A, OPA
            vshr.u16    S_A, S_A, #8
        .endif
        mov             TMP, #4
        vadd.u16        S_A, S_A, TMP
        vshr.u16        S_A, S_A, #3
    .endif
    blend               \src_bpp, \dst_bpp, \mask, \opa, \mode
    ldst                1, st, \dst_bpp, DST, D, S, 0, 0, !, \aligned
.elseif \src_bpp < 32
@ no src_a
.if \src_bpp > 0
    load_index          \src_bpp, S, D, \aligned
    ldst                0, ld, \src_bpp, SRC, S, D, 1, 0, !, \aligned
.elseif (\opa == 1) || \mask
    vpush               {d0-d5}
.endif
    load_index          \dst_bpp, D, S, \aligned
    ldst                0, ld, \dst_bpp, DST, D, S, 1, 0, , \aligned
    .if \mask
        ldst            0, ld, 8, MASK, S, D, 0, 0, !, \aligned
        .if \opa == 2
            vshr.u8     S_A, S_A, #1
        .elseif \opa == 1
        .if \dst_bpp == 32
            vpush       {d14-d15}
        .endif
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .if \dst_bpp == 32
            vpop        {d14-d15}
        .endif
        .endif
    .elseif \opa == 1
        vdup.8          S_A, OPA
    .endif
    blend               \src_bpp, \dst_bpp, \mask, \opa, \mode
.if (\src_bpp == 0) && ((\opa == 1) || \mask)
    vpop                {d0-d5}
.endif
    .if (\dst_bpp == 32) || \mask || (\opa == 1)
        load_index      \dst_bpp, D, S, \aligned
    .endif
    ldst                1, st, \dst_bpp, DST, D, S, 1, 0, !, \aligned
.else
@ src_a (+\mask) (+\opa)
    load_index          \dst_bpp, D, S, \aligned
    ldst                0, ld, \dst_bpp, DST, D, S, 1, 0, , \aligned
    .if (\dst_bpp == 32) && (\mask || \opa || (\aligned == 0))
        vpush           {d14-d15}
    .endif
    load_index          \src_bpp, S, D, \aligned
    ldst                0, ld, \src_bpp, SRC, S, D, 1, 0, !, \aligned
    .if \mask == 0
        .if \opa
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .endif
    .else
        ldst            0, ld, 8, MASK, D, S, 0, 0, !, \aligned
        vrmulh.u8       S_A, S_A, D_A
        .if \opa
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .endif
    .endif
    .if (\dst_bpp == 32) && (\mask || \opa || (\aligned == 0))
        vpop            {d14-d15}
    .endif
    blend               \src_bpp, \dst_bpp, \mask, \opa, \mode
    load_index          \dst_bpp, D, S, \aligned
    ldst                1, st, \dst_bpp, DST, D, S, 1, 0, !, \aligned
.endif
    letp                lr, 2b
1:
.endm

.macro enter complex
    push        {r4-r11, lr}
.if \complex
    vpush       {d8-d15}
.endif
.endm

.macro exit complex
.if \complex
    vpop        {d8-d15}
.endif
    pop         {r4-r11, pc}
.endm

.macro preload mem, bpp
.if \bpp >= 31
    pld         [\mem\()_ADDR, DST_W, lsl #2]
.elseif \bpp == 24
    add         TMP, DST_W, DST_W, lsl #1
    pld         [\mem\()_ADDR, TMP]
.elseif \bpp == 16
    pld         [\mem\()_ADDR, DST_W, lsl #1]
.elseif \bpp == 8
    pld         [\mem\()_ADDR, DST_W]
.endif
.endm

.macro next src_bpp, mask
    add         DST_ADDR, DST_ADDR, DST_STRIDE
.if \src_bpp > 0
    add         SRC_ADDR, SRC_ADDR, SRC_STRIDE
.endif
.if \mask
    add         MASK_ADDR, MASK_ADDR, MASK_STRIDE
.endif
.endm

.macro blender src_bpp, dst_bpp, mask, opa, mode
.if (\src_bpp <= 16) && (\dst_bpp == 16) && (\opa == 0) && (\mask == 0)
    enter       0
.else
    enter       1
.endif
    init        \src_bpp, \dst_bpp, \mask, \opa
    movs        H, DST_H
    beq         0f
    preload     SRC, \src_bpp
.if \mask || \opa || (\src_bpp == 32)
    preload     DST, \dst_bpp
.endif
.if \opa && (\src_bpp < 32) && (\dst_bpp < 32)
4:
@ 50% OPA can be accelerated (OPA == 0x7F/0x80)
    add         TMP, OPA, #1
    tst         TMP, #0x7E
    bne         3f
    late_init   \src_bpp, \dst_bpp, \mask, 2, \mode
    blend_line  \src_bpp, \dst_bpp, \mask, 2, \mode
    next        \src_bpp, \mask
    subs        H, #1
    bne         4b
    b           0f
.endif
3:
    late_init   \src_bpp, \dst_bpp, \mask, \opa, \mode
    blend_line  \src_bpp, \dst_bpp, \mask, \opa, \mode
    next        \src_bpp, \mask
    subs        H, #1
    bne         3b
0:
.if (\src_bpp <= 16) && (\dst_bpp == 16) && (\opa == 0) && (\mask == 0)
    exit        0
.else
    exit        1
.endif
.ltorg
.endm

.macro export name, src_bpp, dst_bpp, mask, opa, mode
.thumb_func
.global \name
\name\():
    blender     \src_bpp, \dst_bpp, \mask, \opa, \mode
.endm

.macro export_set src, dst, src_bpp, dst_bpp, mode
.ifc \src, color
    export lv_\src\()_blend_to_\dst\()_helium, \src_bpp, \dst_bpp, 0, 0, \mode
    export lv_\src\()_blend_to_\dst\()_with_opa_helium, \src_bpp, \dst_bpp, 0, 1, \mode
    export lv_\src\()_blend_to_\dst\()_with_mask_helium, \src_bpp, \dst_bpp, 1, 0, \mode
    export lv_\src\()_blend_to_\dst\()_mix_mask_opa_helium, \src_bpp, \dst_bpp, 1, 1, \mode
.else
    export lv_\src\()_blend_\mode\()_to_\dst\()_helium, \src_bpp, \dst_bpp, 0, 0, \mode
    export lv_\src\()_blend_\mode\()_to_\dst\()_with_opa_helium, \src_bpp, \dst_bpp, 0, 1, \mode
    export lv_\src\()_blend_\mode\()_to_\dst\()_with_mask_helium, \src_bpp, \dst_bpp, 1, 0, \mode
    export lv_\src\()_blend_\mode\()_to_\dst\()_mix_mask_opa_helium, \src_bpp, \dst_bpp, 1, 1, \mode
.endif
.endm

export_set color, rgb565, 0, 16, normal
export_set rgb565, rgb565, 16, 16, normal
export_set rgb888, rgb565, 24, 16, normal
export_set xrgb8888, rgb565, 31, 16, normal
export_set argb8888, rgb565, 32, 16, normal
export_set color, rgb888, 0, 24, normal
export_set rgb565, rgb888, 16, 24, normal
export_set rgb888, rgb888, 24, 24, normal
export_set xrgb8888, rgb888, 31, 24, normal
export_set argb8888, rgb888, 32, 24, normal
export_set color, xrgb8888, 0, 31, normal
export_set rgb565, xrgb8888, 16, 31, normal
export_set rgb888, xrgb8888, 24, 31, normal
export_set xrgb8888, xrgb8888, 31, 31, normal
export_set argb8888, xrgb8888, 32, 31, normal
export_set color, argb8888, 0, 32, normal
export_set rgb565, argb8888, 16, 32, normal
export_set rgb888, argb8888, 24, 32, normal
export_set xrgb8888, argb8888, 31, 32, normal
export_set argb8888, argb8888, 32, 32, normal

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE && LV_USE_NATIVE_HELIUM_ASM*/

