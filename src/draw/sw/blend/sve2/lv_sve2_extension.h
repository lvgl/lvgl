/**
 * @file lv_sve2_extension.h
 *
 */

#if !defined(LV_SVE2_EXTENSION_H) && (defined(__ARM_FEATURE_SVE2) && __ARM_FEATURE_SVE2)
#define LV_SVE2_EXTENSION_H

#include "lv_sve2_util.h"
#include <arm_sve.h>
#include LV_STDINT_INCLUDE

#define svlenu8()  svcntb_pat(SV_ALL)
#define svlenu16() (svcntb_pat(SV_ALL) / sizeof(uint16_t))
#define svlenu32() (svcntb_pat(SV_ALL) / sizeof(uint32_t))
#define svlenu64() (svcntb_pat(SV_ALL) / sizeof(uint64_t))

#define svlens8()  svlenu8()
#define svlens16() svlenu16()
#define svlens32() svlenu32()
#define svlens64() svlenu64()

#define lv_sve_stride_loop_accc8888(ma_stride_size, ma_pred_name)        \
    for (svbool_t ma_pred_name, *pTemp = &ma_pred_name;                  \
         pTemp != NULL;                                                  \
         pTemp = NULL)                                                   \
        for (size_t SVE_SAFE_NAME(n) = 0,                                \
                    sve_iteration_advance = svlenu32() * 4;              \
             ({                                                          \
                 ma_pred_name = svwhilelt_b8((int32_t)SVE_SAFE_NAME(n),  \
                                             (int32_t)(ma_stride_size)); \
                 SVE_SAFE_NAME(n) < (ma_stride_size);                    \
             });                                                         \
             SVE_SAFE_NAME(n) += sve_iteration_advance)

#define lv_sve_stride_loop_rgb32(ma_stride_size, ma_pred_name)           \
    lv_sve_stride_loop_accc8888(ma_stride_size, ma_pred_name)

#define lv_sve_stride_loop_ccc888(ma_stride_size, ma_pred_name)          \
    for (svbool_t ma_pred_name, *pTemp = &ma_pred_name;                  \
         pTemp != NULL;                                                  \
         pTemp = NULL)                                                   \
        for (size_t SVE_SAFE_NAME(n) = 0,                                \
                    sve_iteration_advance = svlenu8();                   \
             ({                                                          \
                 ma_pred_name = svwhilelt_b8((int32_t)SVE_SAFE_NAME(n),  \
                                             (int32_t)(ma_stride_size)); \
                 SVE_SAFE_NAME(n) < (ma_stride_size);                    \
             });                                                         \
             SVE_SAFE_NAME(n) += sve_iteration_advance)

#define lv_sve_stride_loop_rgb24(ma_stride_size, ma_pred_name)            \
    lv_sve_stride_loop_ccc888(ma_stride_size, ma_pred_name)

#define lv_sve_stride_loop_rgb16(ma_stride_size, ma_pred_name)            \
    for (svbool_t ma_pred_name, *pTemp = &ma_pred_name;                   \
         pTemp != NULL;                                                   \
         pTemp = NULL)                                                    \
        for (size_t SVE_SAFE_NAME(n) = 0,                                 \
                    sve_iteration_advance = svlenu16();                   \
             ({                                                           \
                 ma_pred_name = svwhilelt_b16((int32_t)SVE_SAFE_NAME(n),  \
                                              (int32_t)(ma_stride_size)); \
                 SVE_SAFE_NAME(n) < (ma_stride_size);                     \
             });                                                          \
             SVE_SAFE_NAME(n) += sve_iteration_advance)

#define lv_sve_pixel_ccc_foreach_chn( ma_source_u16x3,                    \
                                      ma_target_u16x3,                    \
                                      ...)                                \
    do {                                                                  \
        svuint16x3_t sve_source_u16x3 = ma_source_u16x3;                  \
        (void)sve_source_u16x3;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget3((ma_source_u16x3), 0);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget3((ma_source_u16x3), 1);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget3((ma_source_u16x3), 2);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_accc_foreach_chn012( ma_source_u16x4,                \
                                          ma_target_u16x4,                \
                                          ...)                            \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_accc_foreach_chn( ma_source_u16x4,                   \
                                       ma_target_u16x4,                   \
                                       ...)                               \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn( ma_source_u16x4,                  \
                                        ma_target_u16x4,                  \
                                        ...)                              \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_src_dst_rev(ma_source_u16x4,      \
                                                    ma_target_u16x4,      \
                                                    ...)                  \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_accc_ccca(ma_source_u16x4,        \
                                                  ma_target_u16x4,        \
                                                  ...)                    \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_ccca_accc(ma_source_u16x4,        \
                                                  ma_target_u16x4,        \
                                                  ...)                    \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_a123_a321(ma_source_u16x4,        \
                                                  ma_target_u16x4,        \
                                                  ...)                    \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_123a_321a(ma_source_u16x4,        \
                                                  ma_target_u16x4,        \
                                                  ...)                    \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 3;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 3);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 3, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 2, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget4((ma_target_u16x4), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x4 = svset4(ma_target_u16x4, 1, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_argb_rgb565(ma_source_u16x4,      \
                                                    ma_target_u16x3,      \
                                                    ...)                  \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_rgba_rgb565(ma_source_u16x4,      \
                                                    ma_target_u16x3,      \
                                                    ...)                  \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_bgra_rgb565(ma_source_u16x4,      \
                                                    ma_target_u16x3,      \
                                                    ...)                  \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 3;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 3);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

#define lv_sve_pixel_u16x4_foreach_chn_abgr_rgb565(ma_source_u16x4,      \
                                                    ma_target_u16x3,      \
                                                    ...)                  \
    do {                                                                  \
        svuint16x4_t sve_source_u16x4 = ma_source_u16x4;                  \
        (void)sve_source_u16x4;                                           \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 2;                            \
            const uint8_t sve_dst_chn_idx = 0;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 2);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 0);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 0, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 1;                            \
            const uint8_t sve_dst_chn_idx = 1;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 1);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 1);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 1, sve_target_u16); \
        } while (0);                                                      \
        do {                                                              \
            const uint8_t sve_src_chn_idx = 0;                            \
            const uint8_t sve_dst_chn_idx = 2;                            \
            (void)sve_src_chn_idx;                                        \
            (void)sve_dst_chn_idx;                                        \
            svuint16_t sve_source_u16 = svget4((ma_source_u16x4), 0);     \
            svuint16_t sve_target_u16 = svget3((ma_target_u16x3), 2);     \
            (void)sve_source_u16;                                         \
            (void)sve_target_u16;                                         \
            __VA_ARGS__                                                   \
            ma_target_u16x3 = svset3(ma_target_u16x3, 2, sve_target_u16); \
        } while (0);                                                      \
    } while (0)

static inline svuint16x3_t lv_sve_rgb565_unpack(svuint16_t vPixels)
{
    svuint16_t vBlue = svand_n_u16_m(svptrue_b16(), vPixels, 0x1F);
    svuint16_t vGreen = svand_n_u16_m(svptrue_b16(), vPixels, (0x3F << 5));
    svuint16_t vRed = svand_n_u16_m(svptrue_b16(), vPixels, (0x1F << 11));

    return svcreate3_u16(svlsl_n_u16_m(svptrue_b16(), vBlue, 3),
                         svlsr_n_u16_m(svptrue_b16(), vGreen, 3),
                         svlsr_n_u16_m(svptrue_b16(), vRed, 8));
}

static inline svuint16_t lv_sve_rgb565_pack(svuint16x3_t vRGB16x3)
{
    svuint16_t vRed = svlsr_n_u16_m(svptrue_b16(), svget3_u16(vRGB16x3, 0), 3);
    svuint16_t vGreen = svlsl_n_u16_m(svptrue_b16(),
                                      svand_n_u16_m(svptrue_b16(),
                                                    svget3_u16(vRGB16x3, 1),
                                                    (0x3F << 2)),
                                      3);
    svuint16_t vBlue = svlsl_n_u16_m(svptrue_b16(),
                                     svand_n_u16_m(svptrue_b16(),
                                                   svget3_u16(vRGB16x3, 2),
                                                   (0x1F << 3)),
                                     8);

    svuint16_t vPixel = svorr_u16_m(svptrue_b16(), vRed, vGreen);
    return svorr_u16_m(svptrue_b16(), vPixel, vBlue);

    // return  (svget3_u16(vRGB16x3, 0) >> 3)
    //     |   ((svget3_u16(vRGB16x3, 1) & (0x3F << 2)) << 3)
    //     |   ((svget3_u16(vRGB16x3, 2) & (0x1F << 3)) << 8);
}

LV_NONNULL(2, 3, 4)
static inline void svld3rgb565_u16(svbool_t vPredu8,
                                   uint16_t *phwSource,
                                   svuint16x3_t *pvLow,
                                   svuint16x3_t *pvHigh)
{
    svuint8x2_t vInput8x2 = svld2_u8(vPredu8, (uint8_t *)phwSource);

    svuint16_t vLowByteLowHalf = svunpklo_u16(svget2_u8(vInput8x2, 0));
    svuint16_t vLowByteHighHalf = svunpkhi_u16(svget2_u8(vInput8x2, 0));

    svuint16_t vHighByteLowHalf = svunpklo_u16(svget2_u8(vInput8x2, 1));
    svuint16_t vHighByteHighHalf = svunpkhi_u16(svget2_u8(vInput8x2, 1));

    //*pvLow = lv_sve_rgb565_unpack  (   vLowByteLowHalf
    //                                |   (vHighByteLowHalf << 8));
    *pvLow = lv_sve_rgb565_unpack(
        svorr_u16_m(svptrue_b16(),
                    vLowByteLowHalf,
                    //(vHighByteLowHalf << 8)
                    svlsl_n_u16_m(svptrue_b16(), vHighByteLowHalf, 8)));

    //*pvHigh = lv_sve_rgb565_unpack (   vLowByteHighHalf
    //                                |   (vHighByteHighHalf << 8));
    *pvHigh = lv_sve_rgb565_unpack(
        svorr_u16_m(svptrue_b16(),
                    vLowByteHighHalf,
                    //(vHighByteHighHalf << 8)
                    svlsl_n_u16_m(svptrue_b16(), vHighByteHighHalf, 8)));
}

LV_NONNULL(2)
static inline void svst3rgb565_u16(svbool_t vPredu8,
                                   uint16_t *phwTarget,
                                   svuint16x3_t vLow,
                                   svuint16x3_t vHigh)
{
    svuint16_t vLowByteLowHalf = svundef_u16();
    svuint16_t vHighByteLowHalf = svundef_u16();

    /* pack low half pixels */
    do {
        svuint16_t vPixel = lv_sve_rgb565_pack(vLow);

        // vLowByteLowHalf = vPixel & 0xFF;
        vLowByteLowHalf = svand_n_u16_m(svptrue_b16(), vPixel, 0xFF);

        // vHighByteLowHalf = vPixel >> 8;
        vHighByteLowHalf = svlsr_n_u16_m(svptrue_b16(), vPixel, 8);
    } while (0);

    svuint16_t vLowByteHighHalf = svundef_u16();
    svuint16_t vHighByteHighHalf = svundef_u16();

    /* pack high half pixels */
    do {
        svuint16_t vPixel = lv_sve_rgb565_pack(vHigh);

        // vLowByteHighHalf = vPixel & 0xFF;
        vLowByteHighHalf = svand_n_u16_m(svptrue_b16(), vPixel, 0xFF);

        // vHighByteHighHalf = vPixel >> 8;
        vHighByteHighHalf = svlsr_n_u16_m(svptrue_b16(), vPixel, 8);
    } while (0);

    /* save rgb565 pixels */
    svuint8_t vLowByte = svuzp1_u8(svreinterpret_u8(vLowByteLowHalf),
                                   svreinterpret_u8(vLowByteHighHalf));

    svuint8_t vHighByte = svuzp1_u8(svreinterpret_u8(vHighByteLowHalf),
                                    svreinterpret_u8(vHighByteHighHalf));

    svst2_u8(vPredu8, (uint8_t *)phwTarget, svcreate2_u8(vLowByte, vHighByte));
}

#if defined(__GNUC__) && !defined(__clang__)
#define svld4ub_u16(ma_pred,                                                                                         \
                    ma_src_ptr,                                                                                      \
                    ma_svuint16x4_low_ptr,                                                                           \
                    ma_svuint16x4_high_ptr)                                                                          \
    do {                                                                                                             \
        svuint8x4_t vInput8x4 = svld4_u8((ma_pred), (ma_src_ptr));                                                   \
                                                                                                                     \
        *(ma_svuint16x4_low_ptr) = svset4_u16(*(ma_svuint16x4_low_ptr), 0, svunpklo_u16(svget4_u8(vInput8x4, 0)));   \
        *(ma_svuint16x4_low_ptr) = svset4_u16(*(ma_svuint16x4_low_ptr), 1, svunpklo_u16(svget4_u8(vInput8x4, 1)));   \
        *(ma_svuint16x4_low_ptr) = svset4_u16(*(ma_svuint16x4_low_ptr), 2, svunpklo_u16(svget4_u8(vInput8x4, 2)));   \
        *(ma_svuint16x4_low_ptr) = svset4_u16(*(ma_svuint16x4_low_ptr), 3, svunpklo_u16(svget4_u8(vInput8x4, 3)));   \
                                                                                                                     \
        *(ma_svuint16x4_high_ptr) = svset4_u16(*(ma_svuint16x4_high_ptr), 0, svunpkhi_u16(svget4_u8(vInput8x4, 0))); \
        *(ma_svuint16x4_high_ptr) = svset4_u16(*(ma_svuint16x4_high_ptr), 1, svunpkhi_u16(svget4_u8(vInput8x4, 1))); \
        *(ma_svuint16x4_high_ptr) = svset4_u16(*(ma_svuint16x4_high_ptr), 2, svunpkhi_u16(svget4_u8(vInput8x4, 2))); \
        *(ma_svuint16x4_high_ptr) = svset4_u16(*(ma_svuint16x4_high_ptr), 3, svunpkhi_u16(svget4_u8(vInput8x4, 3))); \
    } while (0)

#define svst4ub_u16(ma_pred,                                                                 \
                    ma_dst_ptr,                                                              \
                    ma_svuint16x4_low,                                                       \
                    ma_svuint16x4_high)                                                      \
    do {                                                                                     \
        svuint8_t vCH0u8 = svuzp1_u8(svreinterpret_u8(svget4_u16((ma_svuint16x4_low), 0)),   \
                                     svreinterpret_u8(svget4_u16((ma_svuint16x4_high), 0))); \
                                                                                             \
        svuint8_t vCH1u8 = svuzp1_u8(svreinterpret_u8(svget4_u16((ma_svuint16x4_low), 1)),   \
                                     svreinterpret_u8(svget4_u16((ma_svuint16x4_high), 1))); \
                                                                                             \
        svuint8_t vCH2u8 = svuzp1_u8(svreinterpret_u8(svget4_u16((ma_svuint16x4_low), 2)),   \
                                     svreinterpret_u8(svget4_u16((ma_svuint16x4_high), 2))); \
                                                                                             \
        svuint8_t vCH3u8 = svuzp1_u8(svreinterpret_u8(svget4_u16((ma_svuint16x4_low), 3)),   \
                                     svreinterpret_u8(svget4_u16((ma_svuint16x4_high), 3))); \
                                                                                             \
        svst4_u8((ma_pred), (ma_dst_ptr), svcreate4_u8(vCH0u8, vCH1u8, vCH2u8, vCH3u8));     \
    } while (0)
#else

LV_NONNULL(2, 3, 4)
static inline void svld4ub_u16(svbool_t vPredu8,
                               uint8_t *pchSource,
                               svuint16x4_t *pvLow,
                               svuint16x4_t *pvHigh)
{
    svuint8x4_t vInput8x4 = svld4_u8(vPredu8, pchSource);

    *pvLow = svset4_u16(*pvLow, 0, svunpklo_u16(svget4_u8(vInput8x4, 0)));
    *pvLow = svset4_u16(*pvLow, 1, svunpklo_u16(svget4_u8(vInput8x4, 1)));
    *pvLow = svset4_u16(*pvLow, 2, svunpklo_u16(svget4_u8(vInput8x4, 2)));
    *pvLow = svset4_u16(*pvLow, 3, svunpklo_u16(svget4_u8(vInput8x4, 3)));

    *pvHigh = svset4_u16(*pvHigh, 0, svunpkhi_u16(svget4_u8(vInput8x4, 0)));
    *pvHigh = svset4_u16(*pvHigh, 1, svunpkhi_u16(svget4_u8(vInput8x4, 1)));
    *pvHigh = svset4_u16(*pvHigh, 2, svunpkhi_u16(svget4_u8(vInput8x4, 2)));
    *pvHigh = svset4_u16(*pvHigh, 3, svunpkhi_u16(svget4_u8(vInput8x4, 3)));
}

LV_NONNULL(2)
static inline void svst4ub_u16(svbool_t vPredu8,
                               uint8_t *pchTarget,
                               svuint16x4_t vLow,
                               svuint16x4_t vHigh)
{

    svuint8_t vCH0u8 = svuzp1_u8(svreinterpret_u8(svget4_u16(vLow, 0)),
                                 svreinterpret_u8(svget4_u16(vHigh, 0)));

    svuint8_t vCH1u8 = svuzp1_u8(svreinterpret_u8(svget4_u16(vLow, 1)),
                                 svreinterpret_u8(svget4_u16(vHigh, 1)));

    svuint8_t vCH2u8 = svuzp1_u8(svreinterpret_u8(svget4_u16(vLow, 2)),
                                 svreinterpret_u8(svget4_u16(vHigh, 2)));

    svuint8_t vCH3u8 = svuzp1_u8(svreinterpret_u8(svget4_u16(vLow, 3)),
                                 svreinterpret_u8(svget4_u16(vHigh, 3)));

    svst4_u8(vPredu8, pchTarget, svcreate4_u8(vCH0u8, vCH1u8, vCH2u8, vCH3u8));
}
#endif


#if defined(__GNUC__) && !defined(__clang__)
#define svld3ub_u16(ma_pred,                                                                                         \
                    ma_src_ptr,                                                                                      \
                    ma_svuint16x3_low_ptr,                                                                           \
                    ma_svuint16x3_high_ptr)                                                                          \
    do {                                                                                                             \
        svuint8x3_t vInput8x3 = svld3_u8((ma_pred), (ma_src_ptr));                                                   \
                                                                                                                     \
        *(ma_svuint16x3_low_ptr) = svset3_u16(*(ma_svuint16x3_low_ptr), 0, svunpklo_u16(svget3_u8(vInput8x3, 0)));   \
        *(ma_svuint16x3_low_ptr) = svset3_u16(*(ma_svuint16x3_low_ptr), 1, svunpklo_u16(svget3_u8(vInput8x3, 1)));   \
        *(ma_svuint16x3_low_ptr) = svset3_u16(*(ma_svuint16x3_low_ptr), 2, svunpklo_u16(svget3_u8(vInput8x3, 2)));   \
                                                                                                                     \
        *(ma_svuint16x3_high_ptr) = svset3_u16(*(ma_svuint16x3_high_ptr), 0, svunpkhi_u16(svget3_u8(vInput8x3, 0))); \
        *(ma_svuint16x3_high_ptr) = svset3_u16(*(ma_svuint16x3_high_ptr), 1, svunpkhi_u16(svget3_u8(vInput8x3, 1))); \
        *(ma_svuint16x3_high_ptr) = svset3_u16(*(ma_svuint16x3_high_ptr), 2, svunpkhi_u16(svget3_u8(vInput8x3, 2))); \
    } while (0)

#define svst3ub_u16(ma_pred,                                                                 \
                    ma_dst_ptr,                                                              \
                    ma_svuint16x3_low,                                                       \
                    ma_svuint16x3_high)                                                      \
    do {                                                                                     \
        svuint8_t vCH0u8 = svuzp1_u8(svreinterpret_u8(svget3_u16((ma_svuint16x3_low), 0)),   \
                                     svreinterpret_u8(svget3_u16((ma_svuint16x3_high), 0))); \
                                                                                             \
        svuint8_t vCH1u8 = svuzp1_u8(svreinterpret_u8(svget3_u16((ma_svuint16x3_low), 1)),   \
                                     svreinterpret_u8(svget3_u16((ma_svuint16x3_high), 1))); \
                                                                                             \
        svuint8_t vCH2u8 = svuzp1_u8(svreinterpret_u8(svget3_u16((ma_svuint16x3_low), 2)),   \
                                     svreinterpret_u8(svget3_u16((ma_svuint16x3_high), 2))); \
                                                                                             \
        svst3_u8((ma_pred), (ma_dst_ptr), svcreate3_u8(vCH0u8, vCH1u8, vCH2u8));             \
    } while (0)
#else

LV_NONNULL(2, 3, 4)
static inline void svld3ub_u16(svbool_t vPredu8,
                               uint8_t *pchSource,
                               svuint16x3_t *pvLow,
                               svuint16x3_t *pvHigh)
{
    svuint8x3_t vInput8x3 = svld3_u8(vPredu8, pchSource);

    *pvLow = svset3_u16(*pvLow, 0, svunpklo_u16(svget3_u8(vInput8x3, 0)));
    *pvLow = svset3_u16(*pvLow, 1, svunpklo_u16(svget3_u8(vInput8x3, 1)));
    *pvLow = svset3_u16(*pvLow, 2, svunpklo_u16(svget3_u8(vInput8x3, 2)));

    *pvHigh = svset3_u16(*pvHigh, 0, svunpkhi_u16(svget3_u8(vInput8x3, 0)));
    *pvHigh = svset3_u16(*pvHigh, 1, svunpkhi_u16(svget3_u8(vInput8x3, 1)));
    *pvHigh = svset3_u16(*pvHigh, 2, svunpkhi_u16(svget3_u8(vInput8x3, 2)));
}

LV_NONNULL(2)
static inline void svst3ub_u16(svbool_t vPredu8,
                               uint8_t *pchTarget,
                               svuint16x3_t vLow,
                               svuint16x3_t vHigh)
{

    svuint8_t vCH0u8 = svuzp1_u8(svreinterpret_u8(svget3_u16(vLow, 0)),
                                 svreinterpret_u8(svget3_u16(vHigh, 0)));

    svuint8_t vCH1u8 = svuzp1_u8(svreinterpret_u8(svget3_u16(vLow, 1)),
                                 svreinterpret_u8(svget3_u16(vHigh, 1)));

    svuint8_t vCH2u8 = svuzp1_u8(svreinterpret_u8(svget3_u16(vLow, 2)),
                                 svreinterpret_u8(svget3_u16(vHigh, 2)));

    svst3_u8(vPredu8, pchTarget, svcreate3_u8(vCH0u8, vCH1u8, vCH2u8));
}
#endif

/*! \note the Element range of vMask is [0, 0xFF]
 */
static inline svuint16_t lv_sve_chn_blend_with_mask( svuint16_t vSource,
                                                     svuint16_t vTarget,
                                                     svuint16_t vMask)
{
    // vTarget = vSource * vMask + vTarget * (255 - vMask);
    svuint16_t vTemp0 = svdup_u16(1);
    vTemp0 = svmla_u16_m(svptrue_b16(), vTemp0, vSource, vMask);
    vTemp0 = svmla_u16_m(svptrue_b16(),
                         vTemp0,
                         vTarget,
                         svsub_u16_m(svptrue_b16(),
                                     svdup_u16(255),
                                     vMask));

    /* x += x >> 8 */
    return svreinterpret_u16_u8(
        svaddhnb_u16(vTemp0,
                     svlsr_n_u16_m(svptrue_b16(),
                                   vTemp0,
                                   8)));
}

/*! \note the Element range of vMask is [0, 0xFF]
 */
static inline svuint16_t lv_sve_chn_blend_with_mask_fast(svuint16_t vSource,
                                                          svuint16_t vTarget,
                                                          svuint16_t vMask)
{
    // vTarget = vSource * vMask + vTarget * (255 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    vTemp0 = svmla_u16_m(svptrue_b16(),
                         vTemp0,
                         vTarget,
                         svsub_u16_m(svptrue_b16(),
                                     svdup_u16(255),
                                     vMask));

    return svlsr_n_u16_m(svptrue_b16(), vTemp0, 8); // vTarget >> 8;
}

/*! \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_opacity( svuint16_t vSource,
                                                        svuint16_t vTarget,
                                                        uint16_t hwOpacity)
{
    // svuint16_t vOpacity = svdup_u16(hwOpacity);
    // vTarget = vSource * vOpacity + vTarget * (256 - vOpacity);

    svuint16_t vTemp0 = svmul_n_u16_m(svptrue_b16(), vSource, hwOpacity);
    vTemp0 = svmla_n_u16_m( svptrue_b16(), 
                            vTemp0,
                            vTarget,
                            256 - hwOpacity);

    return svlsr_n_u16_m(svptrue_b16(), vTemp0, 8);
}

/*! \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_opacity_fast(svuint16_t vSource,
                                                             svuint16_t vTarget,
                                                             uint16_t hwOpacity)
{
    // vTarget = vSource * vMask + vTarget * (255 - vMask);
    svuint16_t vTemp0 = svmul_n_u16_m(svptrue_b16(), vSource, hwOpacity);
    vTemp0 = svmla_n_u16_m(svptrue_b16(),
                           vTemp0,
                           vTarget,
                           256 - hwOpacity);

    return svlsr_n_u16_m(svptrue_b16(), vTemp0, 8); // vTarget >> 8;
}

/*! \note the Element range of vMask is [0, 0xFF]
 *  \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_mask_and_opacity(svuint16_t vSource,
                                                                 svuint16_t vTarget,
                                                                 svuint16_t vMask,
                                                                 uint16_t hwOpacity)
{
    vMask = svsel(svcmpeq_n_u16(svptrue_b16(), vMask, 255),
                  svdup_u16(hwOpacity),
                  //(vMask * hwOpacity) >> 8,
                  svlsr_n_u16_m(svptrue_b16(),
                                svmul_n_u16_m(svptrue_b16(), vMask, hwOpacity),
                                8));

    return lv_sve_chn_blend_with_mask(vSource, vTarget, vMask);

#if 0
    // vTarget = vSource * vMask + vTarget * (256 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    svuint16_t vTemp1 = svmul_u16_m(svptrue_b16(),
                                    vTarget,
                                    svsub_u16_m(svptrue_b16(),
                                                svdup_u16(256),
                                                vMask));
    vTarget = svadd_u16_m(svptrue_b16(), vTemp0, vTemp1);

    return svlsr_n_u16_m(svptrue_b16(), vTarget, 8); // vTarget >> 8;
#endif
}

/*! \note the Element range of vMask is [0, 0xFF]
 *  \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_mask_and_opacity_fast(
                                                                 svuint16_t vSource,
                                                                 svuint16_t vTarget,
                                                                 svuint16_t vMask,
                                                                 uint16_t hwOpacity)
{
    vMask = svsel(svcmpeq_n_u16(svptrue_b16(), vMask, 255),
                  svdup_u16(hwOpacity),
                  //(vMask * hwOpacity) >> 8,
                  svlsr_n_u16_m(svptrue_b16(),
                                svmul_n_u16_m(svptrue_b16(), vMask, hwOpacity),
                                8));

    return lv_sve_chn_blend_with_mask_fast(vSource, vTarget, vMask);

}

/*! \note the Element range of vMask0/1 is [0, 0xFF]
 */
static inline svuint16_t lv_sve_chn_blend_with_masks(svuint16_t vSource,
                                                      svuint16_t vTarget,
                                                      svuint16_t vMask0,
                                                      svuint16_t vMask1)
{
    vMask1 = svadd_u16_m(svcmpeq_n_u16(svptrue_b16(), vMask1, 255),
                         vMask1,
                         svdup_u16(1));

    svuint16_t vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask0, 255),
              vMask1,
              //(vMask0 * vMask1) >> 8,
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask0, vMask1),
                            8));

    // vTarget = vSource * vMask + vTarget * (256 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    svuint16_t vTemp1 = svmul_u16_m(svptrue_b16(),
                                    vTarget,
                                    svsub_u16_m(svptrue_b16(),
                                                svdup_u16(256),
                                                vMask));
    vTarget = svadd_u16_m(svptrue_b16(), vTemp0, vTemp1);

    return svlsr_n_u16_m(svptrue_b16(), vTarget, 8); // vTarget >> 8;
}

/*! \note the Element range of vMask0/1 is [0, 0xFF]
 *  \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_masks_and_opacity(
    svuint16_t vSource,
    svuint16_t vTarget,
    svuint16_t vMask0,
    svuint16_t vMask1,
    uint16_t hwOpacity)
{
    vMask0 = svadd_u16_m(svcmpeq_n_u16(svptrue_b16(), vMask0, 255),
                         vMask0,
                         svdup_u16(1));

    svuint16_t vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask1, 255), /* >= 255 */
              vMask0,
              //(vMask0 * vMask1) >> 8
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask0, vMask1),
                            8));

    vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask, 255),
              svdup_u16(hwOpacity),
              //(vMask * hwOpacity) >> 8,
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_n_u16_m(svptrue_b16(), vMask, hwOpacity),
                            8));

    // vTarget = vSource * vMask + vTarget * (256 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    svuint16_t vTemp1 = svmul_u16_m(svptrue_b16(),
                                    vTarget,
                                    svsub_u16_m(svptrue_b16(),
                                                svdup_u16(256),
                                                vMask));
    vTarget = svadd_u16_m(svptrue_b16(), vTemp0, vTemp1);

    return svlsr_n_u16_m(svptrue_b16(), vTarget, 8); // vTarget >> 8;
}

/*! \note the Element range of vMask0/1 is [0, 0xFF]
 */
static inline svuint16_t lv_sve_chn_blend_with_3masks(svuint16_t vSource,
                                                       svuint16_t vTarget,
                                                       svuint16_t vMask0,
                                                       svuint16_t vMask1,
                                                       svuint16_t vMask2)
{
    vMask0 = svadd_u16_m(svcmpeq_n_u16(svptrue_b16(), vMask0, 255),
                         vMask0,
                         svdup_u16(1));

    svuint16_t vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask1, 255),
              vMask0,
              //(vMask0 * vMask1) >> 8
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask0, vMask1),
                            8));

    vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask2, 255),
              vMask,
              //(vMask * vMask2) >> 8
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask, vMask2),
                            8));

    // vTarget = vSource * vMask + vTarget * (256 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    svuint16_t vTemp1 = svmul_u16_m(svptrue_b16(),
                                    vTarget,
                                    svsub_u16_m(svptrue_b16(),
                                                svdup_u16(256),
                                                vMask));
    vTarget = svadd_u16_m(svptrue_b16(), vTemp0, vTemp1);

    return svlsr_n_u16_m(svptrue_b16(), vTarget, 8); // vTarget >> 8;
}

/*! \note the Element range of vMask0/1 is [0, 0xFF]
 *  \note the hwOpacity range [0, 0x100]
 */
static inline svuint16_t lv_sve_chn_blend_with_3masks_and_opacity(
    svuint16_t vSource,
    svuint16_t vTarget,
    svuint16_t vMask0,
    svuint16_t vMask1,
    svuint16_t vMask2,
    uint16_t hwOpacity)
{
    vMask0 = svadd_u16_m(svcmpeq_n_u16(svptrue_b16(), vMask0, 255),
                         vMask0,
                         svdup_u16(1));

    svuint16_t vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask1, 255),
              vMask0,
              //(vMask0 * vMask1) >> 8
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask0, vMask1),
                            8));

    vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask2, 255),
              vMask,
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_u16_m(svptrue_b16(), vMask, vMask2),
                            8));
    //(vMask * vMask2) >> 8);

    vMask =
        svsel(svcmpge_n_u16(svptrue_b16(), vMask, 255),
              svdup_u16(hwOpacity),
              //(vMask * hwOpacity) >> 8
              svlsr_n_u16_m(svptrue_b16(),
                            svmul_n_u16_m(svptrue_b16(), vMask, hwOpacity),
                            8));

    // vTarget = vSource * vMask + vTarget * (256 - vMask);
    svuint16_t vTemp0 = svmul_u16_m(svptrue_b16(), vSource, vMask);
    svuint16_t vTemp1 = svmul_u16_m(svptrue_b16(),
                                    vTarget,
                                    svsub_u16_m(svptrue_b16(),
                                                svdup_u16(256),
                                                vMask));
    vTarget = svadd_u16_m(svptrue_b16(), vTemp0, vTemp1);

    return svlsr_n_u16_m(svptrue_b16(), vTarget, 8); // vTarget >> 8;
}

#endif /* LV_SVE2_EXTENSION_H */