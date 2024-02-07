/**
 * @file lv_blend_helium.S
 *
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "lv_blend_helium.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE && LV_USE_NATIVE_HELIUM_ASM

.data
reciprocal:
.byte 0xFF, 0xE2, 0xCC, 0xB9, 0xAA, 0x9C, 0x91, 0x88

.text
.syntax unified
.altmacro
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
S_B         .qn  q0
S_G         .qn  q1
S_R         .qn  q2
S_A         .qn  q3
D_B         .qn  q4
D_G         .qn  q5
D_R         .qn  q6
D_A         .qn  q7
N           .qn  q0
V           .qn  q1
R           .qn  q2
L           .qn  q4

.macro conv_888_to_565 reg
    vsri.8          reg&_R, reg&_G, #5
    vshr.u8         reg&_G, reg&_G, #2
    vshr.u8         reg&_B, reg&_B, #3
    vsli.8          reg&_B, reg&_G, #5
.endm

@ 16bpp is stored on R & B
.macro ldst op, bpp, mem, reg, areg, cvt, alt_index, wb
.if bpp == 0
.if (reg == S) || (wb&1)  @ exclude reg == D and !
    ldr             TMP, [mem&_ADDR]
    vdup.8          reg&_B, TMP
    lsr             TMP, #8
    vdup.8          reg&_G, TMP
    lsr             TMP, #8
    vdup.8          reg&_R, TMP
.if cvt && (wb&1)
    conv_888_to_565 reg
.endif
.endif
.elseif bpp == 8
    v&op&rb.8       reg&_A, [mem&_ADDR], #16
.elseif bpp == 16
.if cvt && (op == st)
    conv_888_to_565 reg
.endif
.if alt_index
    v&op&rb.8       reg&_B, [mem&_ADDR, areg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_R, [mem&_ADDR, areg&_A]
.else
    v&op&rb.8       reg&_B, [mem&_ADDR, reg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_R, [mem&_ADDR, reg&_A]
.endif
.if cvt && (op == ld)
    vshl.u8         reg&_G, reg&_R, #5
    vsri.u8         reg&_G, reg&_B, #3
    vshl.u8         reg&_B, reg&_B, #3
    vsri.u8         reg&_R, reg&_R, #5
    vsri.u8         reg&_G, reg&_G, #6
    vsri.u8         reg&_B, reg&_B, #5
.endif
.if wb&0
    add             mem&_ADDR, #31
.else
    sub             mem&_ADDR, #1
.endif
.elseif bpp >= 24
.if alt_index || (bpp >= 31)
    v&op&rb.8       reg&_B, [mem&_ADDR, areg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_G, [mem&_ADDR, areg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_R, [mem&_ADDR, areg&_A]
.else
    v&op&rb.8       reg&_B, [mem&_ADDR, reg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_G, [mem&_ADDR, reg&_A]
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_R, [mem&_ADDR, reg&_A]
.endif
.if (bpp == 32) || (bpp == 31) && (op == st)
    add             mem&_ADDR, #1
    v&op&rb.8       reg&_A, [mem&_ADDR, areg&_A]
.endif
.if wb&0
    .if bpp == 24
        add         mem&_ADDR, #46
    .elseif (bpp == 32) || (bpp == 31) && (op == st)
        add         mem&_ADDR, #61
    .else
        add         mem&_ADDR, #62
    .endif
.else
    .if (bpp == 32) || (bpp == 31) && (op == st)
        sub         mem&_ADDR, #3
    .else
        sub         mem&_ADDR, #2
    .endif
.endif
.endif
.endm

.macro load_index bpp, reg, areg
.if bpp > 0
    mov             TMP, #0
.endif
.if bpp == 8
    vidup.u8        reg&_A, TMP, #1
.elseif bpp == 16
    vidup.u8        reg&_A, TMP, #2
.elseif bpp == 24
    vidup.u8        reg&_A, TMP, #1
    mov             TMP, #3
    vmul.i8         reg&_A, reg&_A, TMP
.elseif bpp >= 31
    vidup.u8        areg&_A, TMP, #4
.endif
.endm

.macro init src_bpp, dst_bpp, mask, opa
    ldr             DST_ADDR, [r0, #4]
    ldr             DST_W, [r0, #8]
    ldr             DST_H, [r0, #12]
    ldr             DST_STRIDE, [r0, #16]
    ldr             SRC_ADDR, [r0, #20]
.if src_bpp > 0
    ldr             SRC_STRIDE, [r0, #24]
.endif
.if mask
    ldr             MASK_ADDR, [r0, #28]
    ldr             MASK_STRIDE, [r0, #32]
.endif
.if opa
    ldr             OPA, [r0]
.else
    mov             OPA, #0xFF
.endif
    add             TMP, DST_W, #0xF
    bic             TMP, TMP, #0xF
.if dst_bpp == 32
    ldr             RCP, =(reciprocal - 8)
.endif

.if dst_bpp == 16
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #1
.elseif dst_bpp == 24
    sub             DST_STRIDE, DST_STRIDE, TMP
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #1
.elseif dst_bpp >= 31
    sub             DST_STRIDE, DST_STRIDE, TMP, lsl #2
.endif
.if mask
    sub             MASK_STRIDE, MASK_STRIDE, TMP
.endif
.if src_bpp == 0
    .if mask || opa
        ldst        ld, src_bpp, SRC, S, D, 0, 0
        vmov.u8     S_A, #0xFF
    .else
        ldst        ld, src_bpp, SRC, D, S, (dst_bpp == 16), 0
        vmov.u8     D_A, #0xFF
    .endif
.else
    .if src_bpp == 16
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #1
    .elseif src_bpp == 24
        sub         SRC_STRIDE, SRC_STRIDE, TMP
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #1
    .elseif src_bpp >= 31
        sub         SRC_STRIDE, SRC_STRIDE, TMP, lsl #2
    .endif
.endif
.if (src_bpp < 32) && (mask == 0) && (opa == 0)
    .if (src_bpp == 31) || (dst_bpp < 31)
        load_index  src_bpp, S, S
    .endif
    .if (dst_bpp < 31) && (dst_bpp != src_bpp)
        load_index  dst_bpp, D, D
    .else
        load_index  dst_bpp, S, S
        vmov.u8     D_A, #0xFF
    .endif
.endif
.endm

.macro vqrdmulh_u8 Qd, Qn, Qm      @ 1 bit precision loss
    vmulh.u8        Qd, Qn, Qm
    vqshl.u8        Qd, Qd, #1
.endm

.macro premult mem, alpha
    vrmulh.u8       mem&_B, mem&_B, alpha
    vrmulh.u8       mem&_G, mem&_G, alpha
    vrmulh.u8       mem&_R, mem&_R, alpha
.endm

.macro blend src_bpp, dst_bpp, mask, opa, mode
.if (mask == 0) && (opa == 2) && (dst_bpp < 32)
    vhadd.u8        D_B, D_B, S_B
    vhadd.u8        D_G, D_G, S_G
    vhadd.u8        D_R, D_R, S_R
.else
.if dst_bpp < 32
    vmvn            D_A, S_A
    premult         S, S_A
    premult         D, D_A
.else
    vpush           {d0-d5}
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
.if dst_bpp == 31
    vmov.u8         D_A, #0xFF
.endif
.endm

.macro blend_line src_bpp, dst_bpp, mask, opa, mode
    wlstp.8             lr, DST_W, 1f
2:
.if (src_bpp < 32) && (mask == 0) && (opa == 0)
@ no blend
@ dst index: db < 31 ? (db == sb ? S : D) : S
@ src index: sb < 31 && db >= 31 ? D(reload) : S
    .if (src_bpp < 31) && (dst_bpp >= 31)
        load_index      src_bpp, D, D
    .endif
    .if src_bpp == 0
        ldst            st, dst_bpp, DST, D, S, 0, 0, !
    .elseif (src_bpp == dst_bpp) || (src_bpp == 31) && (dst_bpp == 32)
        .if dst_bpp < 31
            .if src_bpp < 31
                ldst    ld, src_bpp, SRC, D, S, 0, 1, !
            .else
                ldst    ld, src_bpp, SRC, D, S, 0, 0, !
            .endif
            ldst        st, dst_bpp, DST, D, S, 0, 1, !
        .else
            ldst        ld, src_bpp, SRC, D, S, 0, 0, !
            ldst        st, dst_bpp, DST, D, S, 0, 0, !
        .endif
    .else
        .if (dst_bpp < 31) && (src_bpp < 31)
            ldst        ld, src_bpp, SRC, D, S, 1, 1, !
        .else
            ldst        ld, src_bpp, SRC, D, S, 1, 0, !
        .endif
        .if (src_bpp < 31) && (dst_bpp >= 31)
            vmov.u8     D_A, #0xFF
        .endif
        ldst            st, dst_bpp, DST, D, S, 1, 0, !
    .endif
.elseif src_bpp < 32
@ no src_a
    load_index          src_bpp, S, D
    ldst                ld, src_bpp, SRC, S, D, 1, 0, !
    load_index          dst_bpp, D, S
    ldst                ld, dst_bpp, DST, D, S, 1, 0
    .if mask
        ldst            ld, 8, MASK, S, D, 1, 0, !
        .if opa == 2
            vshr.u8     S_A, S_A, #1
        .elseif opa == 1
        .if dst_bpp == 32
            vpush       {d14-d15}
        .endif
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .if dst_bpp == 32
            vpop        {d14-d15}
        .endif
        .endif
    .elseif opa == 1
        vdup.8          S_A, OPA
    .endif
    blend               src_bpp, dst_bpp, mask, opa, mode
    .if (dst_bpp == 32) || mask || (opa == 1)
        load_index      dst_bpp, D, S
    .endif
    ldst                st, dst_bpp, DST, D, S, 1, 0, !
.else
@ src_a (+mask) (+opa)
    load_index          dst_bpp, D, S
    ldst                ld, dst_bpp, DST, D, S, 1, 0
    .if dst_bpp == 32
        vpush           {d14-d15}
    .endif
    load_index          src_bpp, S, D
    ldst                ld, src_bpp, SRC, S, D, 1, 0, !
    .if mask == 0
        .if opa
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .endif
    .else
        ldst            ld, 8, MASK, D, S, 1, 0, !
        vrmulh.u8       S_A, S_A, D_A
        .if opa
            vdup.8      D_A, OPA
            vrmulh.u8   S_A, S_A, D_A
        .endif
    .endif
    .if dst_bpp == 32
        vpop            {d14-d15}
    .endif
    blend               src_bpp, dst_bpp, mask, opa, mode
    load_index          dst_bpp, D, S
    ldst                st, dst_bpp, DST, D, S, 1, 0, !
.endif
    letp                lr, 2b
1:
.endm

.macro enter
    push        {r4-r11, lr}
    vpush       {d8-d15}
.endm

.macro exit
    vpop        {d8-d15}
    pop         {r4-r11, pc}
.endm

.macro preload mem, bpp
.if bpp >= 31
    pld         [mem&_ADDR, DST_W, lsl #2]
.elseif bpp == 24
    add         TMP, DST_W, DST_W, lsl #1
    pld         [mem&_ADDR, TMP]
.elseif bpp == 16
    pld         [mem&_ADDR, DST_W, lsl #1]
.elseif bpp == 8
    pld         [mem&_ADDR, DST_W]
.endif
.endm

.macro next src_bpp, mask
    add         DST_ADDR, DST_ADDR, DST_STRIDE
.if src_bpp > 0
    add         SRC_ADDR, SRC_ADDR, SRC_STRIDE
.endif
.if mask
    add         MASK_ADDR, MASK_ADDR, MASK_STRIDE
.endif
.endm

.macro blender src_bpp, dst_bpp, mask, opa, mode
    enter
    init        src_bpp, dst_bpp, mask, opa
    movs        H, DST_H
    beq         0f
    preload     SRC, src_bpp
.if mask || opa || (src_bpp == 32)
    preload     DST, dst_bpp
.endif
.if opa && (src_bpp < 32) && (dst_bpp < 32)
4:
@ 50% OPA can be accelerated (OPA == 0x7F/0x80)
    add         TMP, OPA, #1
    tst         TMP, #0x7E
    bne         3f
    blend_line  src_bpp, dst_bpp, mask, 2, mode
    next        src_bpp, mask
    subs        H, #1
    bne         4b
    b           0f
.endif
3:
    blend_line  src_bpp, dst_bpp, mask, opa, mode
    next        src_bpp, mask
    subs        H, #1
    bne         3b
0:
    exit
.ltorg
.endm

.macro export name, src_bpp, dst_bpp, mask, opa, mode
.thumb_func
.func name
.global name
name&:
    blender     src_bpp, dst_bpp, mask, opa, mode
.endfunc
.endm

.macro export_set src, dst, src_bpp, dst_bpp, mode
.if src == color
    export lv_&src&_blend_to_&dst&_helium, src_bpp, dst_bpp, 0, 0, mode
    export lv_&src&_blend_to_&dst&_with_opa_helium, src_bpp, dst_bpp, 0, 1, mode
    export lv_&src&_blend_to_&dst&_with_mask_helium, src_bpp, dst_bpp, 1, 0, mode
    export lv_&src&_blend_to_&dst&_mix_mask_opa_helium, src_bpp, dst_bpp, 1, 1, mode
.else
    export lv_&src&_blend_&mode&_to_&dst&_helium, src_bpp, dst_bpp, 0, 0, mode
    export lv_&src&_blend_&mode&_to_&dst&_with_opa_helium, src_bpp, dst_bpp, 0, 1, mode
    export lv_&src&_blend_&mode&_to_&dst&_with_mask_helium, src_bpp, dst_bpp, 1, 0, mode
    export lv_&src&_blend_&mode&_to_&dst&_mix_mask_opa_helium, src_bpp, dst_bpp, 1, 1, mode
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
