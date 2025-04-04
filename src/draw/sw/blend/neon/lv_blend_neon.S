/**
 * @file lv_blend_neon.S
 *
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "lv_blend_neon.h"


/*Workaround: missing .note.GNU-stack section implies executable stack*/
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

.text
.fpu neon
.arch armv7a
.syntax unified
.p2align 2

@ d0 ~ d3 : src B,G,R,A
@ d4 ~ d7 : dst B,G,R,A
@ q8 : src RGB565 raw
@ q9 : dst RGB565 raw
@ q10 ~ q12: pre-multiplied src
@ d26~29 : temp
@ d30 : mask
@ d31 : opa

FG_MASK     .req r0
BG_MASK     .req r1
DST_ADDR    .req r2
DST_W       .req r3
DST_H       .req r4
DST_STRIDE  .req r5
SRC_ADDR    .req r6
SRC_STRIDE  .req r7
MASK_ADDR   .req r8
MASK_STRIDE .req r9
W           .req r10
H           .req r11
S_8888_L    .qn  q0
S_8888_H    .qn  q1
D_8888_L    .qn  q2
D_8888_H    .qn  q3
                    S_B       .dn  d0
                    S_G       .dn  d1
                    S_R       .dn  d2
                    S_A       .dn  d3
                    D_B       .dn  d4
                    D_G       .dn  d5
                    D_R       .dn  d6
                    D_A       .dn  d7
S_565       .qn  q8
D_565       .qn  q9
                    S_565_L   .dn  d16
                    S_565_H   .dn  d17
                    D_565_L   .dn  d18
                    D_565_H   .dn  d19
PREMULT_B   .qn  q10
PREMULT_G   .qn  q11
PREMULT_R   .qn  q12
TMP_Q0      .qn  q13
                    TMP_D0    .dn  d26
                    TMP_D1    .dn  d27
TMP_Q1      .qn  q14
                    TMP_D2    .dn  d28
                    TMP_D3    .dn  d29
                    M_A       .dn  d30
                    OPA       .dn  d31

.macro convert reg, bpp, intlv
.if \bpp >= 31
    .if \intlv
        vzip.8          \reg\()_B, \reg\()_R   @ BRBRBRBR GGGGGGGG BRBRBRBR AAAAAAAA
        vzip.8          \reg\()_G, \reg\()_A   @ BRBRBRBR GAGAGAGA BRBRBRBR GAGAGAGA
        vzip.8          \reg\()_R, \reg\()_A   @ BRBRBRBR GAGAGAGA BGRABGRA BGRABGRA
        vzip.8          \reg\()_B, \reg\()_G   @ BGRABGRA BGRABGRA BGRABGRA BGRABGRA
    .else
        vuzp.8          \reg\()_B, \reg\()_G   @ BRBRBRBR GAGAGAGA BGRABGRA BGRABGRA
        vuzp.8          \reg\()_R, \reg\()_A   @ BRBRBRBR GAGAGAGA BRBRBRBR GAGAGAGA
        vuzp.8          \reg\()_G, \reg\()_A   @ BRBRBRBR GGGGGGGG BRBRBRBR AAAAAAAA
        vuzp.8          \reg\()_B, \reg\()_R   @ BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA
    .endif
.elseif \bpp == 24
    .if \intlv   @ for init only (same B,G,R for all channel)
        vzip.8          \reg\()_B, \reg\()_G                @ BGBGBGBG BGBGBGBG RRRRRRRR
        vzip.16         \reg\()_B, \reg\()_R                @ BGRRBGRR BGBGBGBG BGRRBGRR
        vsli.64         \reg\()_8888_L, \reg\()_8888_L, #24 @ BGRBGRRB BGBBGBGB
        vsli.64         \reg\()_B, \reg\()_G, #48           @ BGRBGRBG
        vsri.64         \reg\()_R, \reg\()_B, #8            @                   GRBGRBGR
        vsri.64         \reg\()_G, \reg\()_R, #8            @          RBGRBGRB
    .endif
.elseif \bpp == 16
    .if \intlv
        vshll.u8        \reg\()_565, \reg\()_R, #8 @ RRRrrRRR 00000000
        vshll.u8        TMP_Q0, \reg\()_G, #8      @ GGGgggGG 00000000
        vshll.u8        TMP_Q1, \reg\()_B, #8      @ BBBbbBBB 00000000
        vsri.16         \reg\()_565, TMP_Q0, #5    @ RRRrrGGG gggGG000
        vsri.16         \reg\()_565, TMP_Q1, #11   @ RRRrrGGG gggBBBbb
    .else
        vshr.u8         TMP_Q0, \reg\()_565, #3    @ 000RRRrr 000gggBB
        vshrn.i16       \reg\()_G, \reg\()_565, #5 @ rrGGGggg
        vshrn.i16       \reg\()_R, TMP_Q0, #5      @ RRRrr000
        vshl.i8         \reg\()_G, \reg\()_G, #2   @ GGGggg00
        vshl.i16        TMP_Q1, \reg\()_565, #3    @ rrGGGggg BBBbb000
        vsri.8          \reg\()_R, \reg\()_R, #5   @ RRRrrRRR
        vmovn.i16       \reg\()_B, TMP_Q1          @ BBBbb000
        vsri.8          \reg\()_G, \reg\()_G, #6   @ GGGgggGG
        vsri.8          \reg\()_B, \reg\()_B, #5   @ BBBbbBBB
    .endif
.endif
.endm

.macro ldst op, bpp, len, mem, reg, cvt, wb
.if \bpp >= 31
    .if \len == 8
        .if \cvt
            v\op\()4.8        {\reg\()_B, \reg\()_G, \reg\()_R, \reg\()_A}, [\mem\()_ADDR]\wb
        .else
            v\op\()1.32       {\reg\()_8888_L, \reg\()_8888_H}, [\mem\()_ADDR]\wb
        .endif
    .else
        .ifc \op,st
            .if \cvt
                convert       \reg, \bpp, 1
            .endif
        .endif
        .if \len == 7
            v\op\()1.32       {\reg\()_8888_L}, [\mem\()_ADDR]!
            v\op\()1.32       {\reg\()_R}, [\mem\()_ADDR]!
            v\op\()1.32       {\reg\()_A[0]}, [\mem\()_ADDR]!
        .elseif \len == 6
            v\op\()1.32       {\reg\()_8888_L}, [\mem\()_ADDR]!
            v\op\()1.32       {\reg\()_R}, [\mem\()_ADDR]!
        .elseif \len == 5
            v\op\()1.32       {\reg\()_8888_L}, [\mem\()_ADDR]!
            v\op\()1.32       {\reg\()_R[0]}, [\mem\()_ADDR]!
        .elseif \len == 4
            v\op\()1.32       {\reg\()_8888_L}, [\mem\()_ADDR]\wb
        .elseif \len == 3
            v\op\()1.32       {\reg\()_B}, [\mem\()_ADDR]!
            v\op\()1.32       {\reg\()_G[0]}, [\mem\()_ADDR]!
        .elseif \len == 2
            v\op\()1.32       {\reg\()_B}, [\mem\()_ADDR]\wb
        .elseif \len == 1
            v\op\()1.32       {\reg\()_B[0]}, [\mem\()_ADDR]\wb
        .else
            .error "[32bpp]len should be 1~8"
        .endif
        .ifc \op,ld
            .if \cvt
                convert       \reg, \bpp, 0
            .endif
        .endif
        .ifb \wb
            .if (\len != 4) && (\len != 2) && (\len != 1)
                sub           \mem\()_ADDR, #4*\len
            .endif
        .endif
    .endif
.elseif \bpp == 24
    .if \len == 8
        .if \cvt
            v\op\()3.8        {\reg\()_B, \reg\()_G, \reg\()_R}, [\mem\()_ADDR]\wb
        .else
            v\op\()1.8        {\reg\()_B, \reg\()_G, \reg\()_R}, [\mem\()_ADDR]\wb
        .endif
    .elseif (\len < 8) && (\len > 0)
        .if \cvt
            v\op\()3.8        {\reg\()_B[0], \reg\()_G[0], \reg\()_R[0]}, [\mem\()_ADDR]!
            .if \len > 1
                v\op\()3.8    {\reg\()_B[1], \reg\()_G[1], \reg\()_R[1]}, [\mem\()_ADDR]!
            .endif
            .if \len > 2
                v\op\()3.8    {\reg\()_B[2], \reg\()_G[2], \reg\()_R[2]}, [\mem\()_ADDR]!
            .endif
            .if \len > 3
                v\op\()3.8    {\reg\()_B[3], \reg\()_G[3], \reg\()_R[3]}, [\mem\()_ADDR]!
            .endif
            .if \len > 4
                v\op\()3.8    {\reg\()_B[4], \reg\()_G[4], \reg\()_R[4]}, [\mem\()_ADDR]!
            .endif
            .if \len > 5
                v\op\()3.8    {\reg\()_B[5], \reg\()_G[5], \reg\()_R[5]}, [\mem\()_ADDR]!
            .endif
            .if \len > 6
                v\op\()3.8    {\reg\()_B[6], \reg\()_G[6], \reg\()_R[6]}, [\mem\()_ADDR]!
            .endif
            .ifb \wb
                sub           \mem\()_ADDR, #3*\len
            .endif
        .else
            .if \len == 7
                v\op\()1.32   {\reg\()_8888_L}, [\mem\()_ADDR]!
                v\op\()1.32   {\reg\()_R[0]}, [\mem\()_ADDR]!
                v\op\()1.8    {\reg\()_R[4]}, [\mem\()_ADDR]!
            .elseif \len == 6
                v\op\()1.32   {\reg\()_8888_L}, [\mem\()_ADDR]!
                v\op\()1.16   {\reg\()_R[0]}, [\mem\()_ADDR]!
            .elseif \len == 5
                v\op\()1.32   {\reg\()_B}, [\mem\()_ADDR]!
                v\op\()1.32   {\reg\()_G[0]}, [\mem\()_ADDR]!
                v\op\()1.16   {\reg\()_G[2]}, [\mem\()_ADDR]!
                v\op\()1.8    {\reg\()_G[6]}, [\mem\()_ADDR]!
            .elseif \len == 4
                v\op\()1.32   {\reg\()_B}, [\mem\()_ADDR]!
                v\op\()1.32   {\reg\()_G[0]}, [\mem\()_ADDR]!
            .elseif \len == 3
                v\op\()1.32   {\reg\()_B}, [\mem\()_ADDR]!
                v\op\()1.8    {\reg\()_G[0]}, [\mem\()_ADDR]!
            .elseif \len == 2
                v\op\()1.32   {\reg\()_B[0]}, [\mem\()_ADDR]!
                v\op\()1.16   {\reg\()_B[2]}, [\mem\()_ADDR]!
            .elseif \len == 1
                v\op\()1.16   {\reg\()_B[0]}, [\mem\()_ADDR]!
                v\op\()1.8    {\reg\()_B[2]}, [\mem\()_ADDR]!
            .endif
            .ifb \wb
                sub           \mem\()_ADDR, #3*\len
            .endif
        .endif
    .else
        .error "[24bpp]len should be 1~8"
    .endif
.elseif \bpp == 16
    .ifc \op,st
        .if \cvt
            convert           \reg, \bpp, 1
        .endif
    .endif
    .if \len == 8
        v\op\()1.16           {\reg\()_565}, [\mem\()_ADDR]\wb
    .elseif \len == 7
        v\op\()1.16           {\reg\()_565_L}, [\mem\()_ADDR]!
        v\op\()1.32           {\reg\()_565_H[0]}, [\mem\()_ADDR]!
        v\op\()1.16           {\reg\()_565_H[2]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #14
        .endif
    .elseif \len == 6
        v\op\()1.16           {\reg\()_565_L}, [\mem\()_ADDR]!
        v\op\()1.32           {\reg\()_565_H[0]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #12
        .endif
    .elseif \len == 5
        v\op\()1.16           {\reg\()_565_L}, [\mem\()_ADDR]!
        v\op\()1.16           {\reg\()_565_H[0]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #10
        .endif
    .elseif \len == 4
        v\op\()1.16           {\reg\()_565_L}, [\mem\()_ADDR]\wb
    .elseif \len == 3
        v\op\()1.32           {\reg\()_565_L[0]}, [\mem\()_ADDR]!
        v\op\()1.16           {\reg\()_565_L[2]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #6
        .endif
    .elseif \len == 2
        v\op\()1.32           {\reg\()_565_L[0]}, [\mem\()_ADDR]\wb
    .elseif \len == 1
        v\op\()1.16           {\reg\()_565_L[0]}, [\mem\()_ADDR]\wb
    .else
        .error "[16bpp]len should be 1~8"
    .endif
    .ifc \op,ld
        .if \cvt
            convert           \reg, \bpp, 0
        .endif
    .endif
.elseif \bpp == 8
    .if \len == 8
        v\op\()1.8            {\reg\()_A}, [\mem\()_ADDR]\wb
    .elseif \len == 7
        v\op\()1.32           {\reg\()_A[0]}, [\mem\()_ADDR]!
        v\op\()1.16           {\reg\()_A[2]}, [\mem\()_ADDR]!
        v\op\()1.8            {\reg\()_A[6]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #7
        .endif
    .elseif \len == 6
        v\op\()1.32           {\reg\()_A[0]}, [\mem\()_ADDR]!
        v\op\()1.16           {\reg\()_A[2]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #6
        .endif
    .elseif \len == 5
        v\op\()1.32           {\reg\()_A[0]}, [\mem\()_ADDR]!
        v\op\()1.8            {\reg\()_A[4]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #5
        .endif
    .elseif \len == 4
        v\op\()1.32           {\reg\()_A[0]}, [\mem\()_ADDR]\wb
    .elseif \len == 3
        v\op\()1.16           {\reg\()_A[0]}, [\mem\()_ADDR]!
        v\op\()1.8            {\reg\()_A[2]}, [\mem\()_ADDR]!
        .ifb \wb
            sub               \mem\()_ADDR, #3
        .endif
    .elseif \len == 2
        v\op\()1.16           {\reg\()_A[0]}, [\mem\()_ADDR]\wb
    .elseif \len == 1
        v\op\()1.8            {\reg\()_A[0]}, [\mem\()_ADDR]\wb
    .else
        .error "[8bpp]len should be 1~8"
    .endif
.elseif \bpp == 0
    .ifb \wb
        .if \len == 8
            v\op\()3.8        {\reg\()_B[], \reg\()_G[], \reg\()_R[]}, [\mem\()_ADDR]
        .else
            .error "[color]len should be 8"
        .endif
    .endif
.endif
.ifc \op,ld
    .if \cvt && (\bpp > 8) && (\bpp < 32)
        vmov.u8               \reg\()_A, #0xFF
    .endif
.endif
.endm

.macro premult alpha
    vmull.u8        PREMULT_B, S_B, \alpha
    vmull.u8        PREMULT_G, S_G, \alpha
    vmull.u8        PREMULT_R, S_R, \alpha
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
    sub             MASK_STRIDE, MASK_STRIDE, DST_W
.endif
.if \opa
    vld1.8          {OPA[]}, [r0]
.else
    vmov.u8         OPA, #0xFF
.endif

    vmvn            D_A, OPA
.if \dst_bpp == 16
    sub             DST_STRIDE, DST_STRIDE, DST_W, lsl #1
.elseif \dst_bpp == 24
    sub             DST_STRIDE, DST_STRIDE, DST_W
    sub             DST_STRIDE, DST_STRIDE, DST_W, lsl #1
.elseif \dst_bpp >= 31
    sub             DST_STRIDE, DST_STRIDE, DST_W, lsl #2
.endif
.if \src_bpp == 0
    .if \mask || \opa
        ldst        ld, \src_bpp, 8, SRC, S, 1
        vmov.u8     S_A, #0xFF
        premult     OPA
    .else
        ldst        ld, \src_bpp, 8, SRC, D, 1
        vmov.u8     D_A, #0xFF
        convert     D, \dst_bpp, 1
    .endif
.else
.if \src_bpp == 16
    sub             SRC_STRIDE, SRC_STRIDE, DST_W, lsl #1
.elseif \src_bpp == 24
    sub             SRC_STRIDE, SRC_STRIDE, DST_W
    sub             SRC_STRIDE, SRC_STRIDE, DST_W, lsl #1
.elseif \src_bpp >= 31
    sub             SRC_STRIDE, SRC_STRIDE, DST_W, lsl #2
.endif
.endif
    mvn             FG_MASK, #0
    mvn             BG_MASK, #0
.endm

@ input: M_A = 255 - fg.alpha
.macro calc_alpha len
    vmov.u8             TMP_D0, #0xFD
    vmvn                D_A, D_A
    vcge.u8             TMP_D1, S_A, TMP_D0      @ if (fg.alpha >= LV_OPA_MAX
    vcge.u8             TMP_D2, D_A, TMP_D0      @ || bg.alpha <= LV_OPA_MIN)
    vorr                TMP_D2, TMP_D1
    vcge.u8             TMP_D3, M_A, TMP_D0      @ elseif (fg.alpha <= LV_OPA_MIN)
    vmvn                TMP_Q1, TMP_Q1
    vshrn.i16           TMP_D0, TMP_Q1, #4
    vmov                FG_MASK, BG_MASK, TMP_D0
    cbz                 FG_MASK, 99f             @ return fg;
    vmull.u8            TMP_Q0, M_A, D_A         @ D_A = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha)
    vqrshrn.u16         M_A, TMP_Q0, #8
    vbif                M_A, D_A, TMP_D3         @ insert original D_A when fg.alpha <= LV_OPA_MIN
    vmvn                D_A, M_A
    cbz                 BG_MASK, 99f             @ return bg;
    vmov.u8             TMP_D2, #0xFF
    vmovl.u8            TMP_Q0, D_A
    .if \len > 4
        vmovl.u16       S_565, TMP_D1
    .endif
    vmovl.u16           TMP_Q0, TMP_D0
    vmull.u8            TMP_Q1, S_A, TMP_D2
    vcvt.f32.u32        TMP_Q0, TMP_Q0
    .if \len > 4
        vmovl.u16       D_565, TMP_D3
        vcvt.f32.u32    S_565, S_565
    .endif
    vmovl.u16           TMP_Q1, TMP_D2
    vrecpe.f32          TMP_Q0, TMP_Q0
    vcvt.f32.u32        TMP_Q1, TMP_Q1
    .if \len > 4
        vcvt.f32.u32    D_565, D_565
        vrecpe.f32      S_565, S_565
    .endif
    vmul.f32            TMP_Q0, TMP_Q0, TMP_Q1
    .if \len > 4
        vmul.f32        S_565, S_565, D_565
    .endif
    vcvt.u32.f32        TMP_Q0, TMP_Q0
    .if \len > 4
        vcvt.u32.f32    S_565, S_565
    .endif
    vmovn.u32           TMP_D0, TMP_Q0
    .if \len > 4
    vmovn.u32           TMP_D1, S_565
    .endif
    vmovn.u16           TMP_D0, TMP_Q0
    premult             TMP_D0
    vmvn                M_A, TMP_D0
99:
.endm

.macro blend mode, dst_bpp
.if \dst_bpp == 32
    vmov            TMP_D0, FG_MASK, BG_MASK
    vmovl.s8        TMP_Q0, TMP_D0
    vsli.8          TMP_Q0, TMP_Q0, #4
    cbz             FG_MASK, 98f
.endif
.ifc \mode,normal
.if \dst_bpp == 32
    cbz             BG_MASK, 97f
    mvns            BG_MASK, BG_MASK
    beq             96f
    vmov            S_565_L, D_B
    vmov            S_565_H, D_G
    vmov            D_565_L, D_R
.endif
96:
    vmlal.u8        PREMULT_B, D_B, M_A
    vmlal.u8        PREMULT_G, D_G, M_A
    vmlal.u8        PREMULT_R, D_R, M_A
    vqrshrn.u16     D_B, PREMULT_B, #8
    vqrshrn.u16     D_G, PREMULT_G, #8
    vqrshrn.u16     D_R, PREMULT_R, #8
.if \dst_bpp == 32
    beq             97f
    vbif            D_B, S_565_L, TMP_D1
    vbif            D_G, S_565_H, TMP_D1
    vbif            D_R, D_565_L, TMP_D1
97:
    mvns            FG_MASK, FG_MASK
    beq             99f
.endif
.else
    .error "blend mode is unsupported"
.endif
.if \dst_bpp == 32
98:
    vbif            D_B, S_B, TMP_D0
    vbif            D_G, S_G, TMP_D0
    vbif            D_R, S_R, TMP_D0
    vbif            D_A, S_A, TMP_D0
99:
.endif
.endm

.macro process len, src_bpp, dst_bpp, mask, opa, mode
.if (\src_bpp < 32) && (\mask == 0) && (\opa == 0)
@ no blend
    .if \src_bpp == 0 || \src_bpp == \dst_bpp
        ldst            ld, \src_bpp, \len, SRC, D, 0, !
        ldst            st, \dst_bpp, \len, DST, D, 0, !
    .else
        ldst            ld, \src_bpp, \len, SRC, D, 1, !
        ldst            st, \dst_bpp, \len, DST, D, 1, !
    .endif
.elseif \src_bpp < 32
@ no src_a
    .if \src_bpp > 0
        ldst            ld, \src_bpp, \len, SRC, S, 1, !
    .endif
    ldst                ld, \dst_bpp, \len, DST, D, 1
    .if \mask
        ldst            ld, 8, \len, MASK, S, 1, !
        .if \opa
            vmull.u8    TMP_Q0, S_A, OPA
            vqrshrn.u16 S_A, TMP_Q0, #8
        .endif
        vmvn            M_A, S_A
        .if \dst_bpp < 32
            premult     S_A
        .else
            calc_alpha  \len
        .endif
    .else
        vmvn            M_A, OPA
        .if \dst_bpp < 32
            premult     OPA
        .else
            vmov        S_A, OPA
            calc_alpha  \len
        .endif
    .endif
    blend               \mode, \dst_bpp
    ldst                st, \dst_bpp, \len, DST, D, 1, !
.else
@ src_a (+\mask) (+\opa)
    ldst                ld, \src_bpp, \len, SRC, S, 1, !
    ldst                ld, \dst_bpp, \len, DST, D, 1
    .if \mask == 0
        .if \opa
            vmull.u8    TMP_Q0, S_A, OPA
            vqrshrn.u16 S_A, TMP_Q0, #8
        .endif
    .else
        ldst            ld, 8, \len, MASK, M, 1, !
        vmull.u8        TMP_Q0, S_A, M_A
        vqrshrn.u16     S_A, TMP_Q0, #8
        .if \opa
            vmull.u8    TMP_Q0, S_A, OPA
            vqrshrn.u16 S_A, TMP_Q0, #8
        .endif
    .endif
    vmvn                M_A, S_A
    .if \dst_bpp < 32
        premult         S_A
    .else
        calc_alpha      \len
    .endif
    blend               \mode, \dst_bpp
    ldst                st, \dst_bpp, \len, DST, D, 1, !
.endif
.endm

.macro tail src_bpp, dst_bpp, mask, opa, mode
    tst         DST_W, #4
    beq         3f
    tst         DST_W, #2
    beq         5f
    tst         DST_W, #1
    beq         6f
    process     7, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
6:
    process     6, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
5:
    tst         DST_W, #1
    beq         4f
    process     5, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
4:
    process     4, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
3:
    tst         DST_W, #2
    beq         1f
    tst         DST_W, #1
    beq         2f
    process     3, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
2:
    process     2, \src_bpp, \dst_bpp, \mask, \opa, \mode
    b           0f
1:
    process     1, \src_bpp, \dst_bpp, \mask, \opa, \mode
0:
.endm

.macro next src_bpp, mask
    add         DST_ADDR, DST_ADDR, DST_STRIDE
.if \src_bpp
    add         SRC_ADDR, SRC_ADDR, SRC_STRIDE
.endif
.if \mask
    add         MASK_ADDR, MASK_ADDR, MASK_STRIDE
.endif
.endm

.macro enter
    push        {r4-r11, lr}
.endm

.macro exit
    pop         {r4-r11, pc}
.endm

.macro preload mem, bpp
.if \bpp >= 31
    pld         [\mem\()_ADDR, DST_W, lsl #2]
.elseif \bpp == 24
    add         W, DST_W, DST_W, lsl #1
    pld         [\mem\()_ADDR, W]
.elseif \bpp == 16
    pld         [\mem\()_ADDR, DST_W, lsl #1]
.elseif \bpp == 8
    pld         [\mem\()_ADDR, DST_W]
.endif
.endm

.macro blender src_bpp, dst_bpp, mask, opa, mode
    enter
    init        \src_bpp, \dst_bpp, \mask, \opa
    movs        H, DST_H
    beq         0f
    preload     SRC, \src_bpp
.if \mask || \opa || (\src_bpp == 32)
    preload     DST, \dst_bpp
.endif
    subs        W, DST_W, #8
    blt         7f
9:
    process     8, \src_bpp, \dst_bpp, \mask, \opa, \mode
    subs        W, W, #8
    bge         9b
    tst         DST_W, #7
    beq         8f
    tail        \src_bpp, \dst_bpp, \mask, \opa, \mode
8:
    next        \src_bpp, \mask
    preload     SRC, \src_bpp
.if \mask || \opa || (\src_bpp == 32)
    preload     DST, \dst_bpp
.endif
    sub         W, DST_W, #8
    subs        H, H, #1
    bgt         9b
    exit
7:
    tail        \src_bpp, \dst_bpp, \mask, \opa, \mode
    next        \src_bpp, \mask
    subs        H, H, #1
    bgt         7b
    exit
.endm

.macro export name, src_bpp, dst_bpp, mask, opa, mode
.thumb_func
.func \name
.global \name
.hidden \name
\name\():
    blender     \src_bpp, \dst_bpp, \mask, \opa, \mode
.endfunc
.endm

.macro export_set src, dst, src_bpp, dst_bpp, mode
.ifc \src,color
    export _lv_\src\()_blend_to_\dst\()_neon, \src_bpp, \dst_bpp, 0, 0, \mode
    export _lv_\src\()_blend_to_\dst\()_with_opa_neon, \src_bpp, \dst_bpp, 0, 1, \mode
    export _lv_\src\()_blend_to_\dst\()_with_mask_neon, \src_bpp, \dst_bpp, 1, 0, \mode
    export _lv_\src\()_blend_to_\dst\()_mix_mask_opa_neon, \src_bpp, \dst_bpp, 1, 1, \mode
.else
    export _lv_\src\()_blend_\mode\()_to_\dst\()_neon, \src_bpp, \dst_bpp, 0, 0, \mode
    export _lv_\src\()_blend_\mode\()_to_\dst\()_with_opa_neon, \src_bpp, \dst_bpp, 0, 1, \mode
    export _lv_\src\()_blend_\mode\()_to_\dst\()_with_mask_neon, \src_bpp, \dst_bpp, 1, 0, \mode
    export _lv_\src\()_blend_\mode\()_to_\dst\()_mix_mask_opa_neon, \src_bpp, \dst_bpp, 1, 1, \mode
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

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON*/

