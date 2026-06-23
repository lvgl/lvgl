/**
 * @file lv_sve2_until.h
 *
 */

#ifndef LV_SVE2_UTIL_H
#define LV_SVE2_UTIL_H

#undef SVE_0_CONNECT2
#undef SVE_0_CONNECT3
#undef SVE_0_CONNECT4
#undef SVE_0_CONNECT5
#undef SVE_0_CONNECT6
#undef SVE_0_CONNECT7
#undef SVE_0_CONNECT8
#undef SVE_0_CONNECT9

#undef SVE_CONNECT2
#undef SVE_CONNECT3
#undef SVE_CONNECT4
#undef SVE_CONNECT5
#undef SVE_CONNECT6
#undef SVE_CONNECT7
#undef SVE_CONNECT8
#undef SVE_CONNECT9
#undef ALT_SVE_CONNECT2

#undef SVE_SAFE_NAME

#undef SVE_CONNECT

#define SVE_0_CONNECT2(ma_A, ma_B)             ma_A##ma_B
#define SVE_0_CONNECT3(ma_A, ma_B, ma_C)       ma_A##ma_B##ma_C
#define SVE_0_CONNECT4(ma_A, ma_B, ma_C, ma_D) ma_A##ma_B##ma_C##ma_D
#define SVE_0_CONNECT5(ma_A, ma_B, ma_C, ma_D, ma_E) \
    ma_A##ma_B##ma_C##ma_D##ma_E
#define SVE_0_CONNECT6(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F) \
    ma_A##ma_B##ma_C##ma_D##ma_E##ma_F
#define SVE_0_CONNECT7(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G) \
    ma_A##ma_B##ma_C##ma_D##ma_E##ma_F##ma_G
#define SVE_0_CONNECT8(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H) \
    ma_A##ma_B##ma_C##ma_D##ma_E##ma_F##ma_G##ma_H
#define SVE_0_CONNECT9(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H, ma_I) \
    ma_A##ma_B##ma_C##ma_D##ma_E##ma_F##ma_G##ma_H##ma_I

#define ALT_SVE_CONNECT2(ma_A, ma_B)   SVE_0_CONNECT2(ma_A, ma_B)
#define SVE_CONNECT2(ma_A, ma_B)       SVE_0_CONNECT2(ma_A, ma_B)
#define SVE_CONNECT3(ma_A, ma_B, ma_C) SVE_0_CONNECT3(ma_A, ma_B, ma_C)
#define SVE_CONNECT4(ma_A, ma_B, ma_C, ma_D) \
    SVE_0_CONNECT4(ma_A, ma_B, ma_C, ma_D)
#define SVE_CONNECT5(ma_A, ma_B, ma_C, ma_D, ma_E) \
    SVE_0_CONNECT5(ma_A, ma_B, ma_C, ma_D, ma_E)
#define SVE_CONNECT6(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F) \
    SVE_0_CONNECT6(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F)
#define SVE_CONNECT7(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G) \
    SVE_0_CONNECT7(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G)
#define SVE_CONNECT8(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H) \
    SVE_0_CONNECT8(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H)
#define SVE_CONNECT9(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H, ma_I) \
    SVE_0_CONNECT9(ma_A, ma_B, ma_C, ma_D, ma_E, ma_F, ma_G, ma_H, ma_I)

#define SVE_CONNECT(...)          \
    ALT_SVE_CONNECT2(SVE_CONNECT, \
                     SVE_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#ifndef SVE_VA_NUM_ARGS_IMPL
#define SVE_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
                             _12, _13, _14, _15, _16, ma_N, ...) ma_N
#endif

#ifndef SVE_VA_NUM_ARGS
#define SVE_VA_NUM_ARGS(...)                                              \
    SVE_VA_NUM_ARGS_IMPL(0, ##__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, \
                         8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif

#define SVE_SAFE_NAME(ma_NAME) SVE_CONNECT3(ma_, ma_NAME, ma_LINEma_)

/* ---------------------------------------------------------------------------*
 * SVE Test Helper                                                            *
 * ---------------------------------------------------------------------------*/

#define SVT_PRINT_VECTOR(ma_VECOTOR, ma_ELEMENT_T, ma_FORMAT_STRING)            \
    do {                                                                        \
        int_fast8_t nElementCount = svcntb_pat(SV_ALL) / sizeof(ma_ELEMENT_T);  \
        uint8_t SVE_SAFE_NAME(chVectorBuffer)                                   \
            [nElementCount * sizeof(ma_ELEMENT_T)];                             \
                                                                                \
        svst1_u8(svptrue_b8(),                                                  \
                 SVE_SAFE_NAME(chVectorBuffer),                                 \
                 svreinterpret_u8(ma_VECOTOR));                                 \
                                                                                \
        ma_ELEMENT_T *pElement = (ma_ELEMENT_T *)SVE_SAFE_NAME(chVectorBuffer); \
        printf("%s\t[", #ma_VECOTOR);                                           \
        do {                                                                    \
            printf(ma_FORMAT_STRING "\t", (int)*pElement++);                    \
        } while (--nElementCount);                                              \
        printf("]\r\n");                                                        \
    } while (0)

#define SVT_INIT_VECOTR(ma_VECTOR, ma_ELEMENT_T, ...)                               \
    do {                                                                            \
        uint8_t SVE_SAFE_NAME(chVectorBuffer)[svcntb_pat(SV_ALL)];                  \
                                                                                    \
        memset(SVE_SAFE_NAME(chVectorBuffer), /* This should NOT be LV_memset() */ \
               0,                                                                   \
               sizeof(SVE_SAFE_NAME(chVectorBuffer)));                              \
        memcpy(SVE_SAFE_NAME(chVectorBuffer), /* This should NOT be LV_memcpy() */ \
               (ma_ELEMENT_T[]){ __VA_ARGS__ },                                     \
               MIN(sizeof(SVE_SAFE_NAME(chVectorBuffer)),                           \
                   sizeof((ma_ELEMENT_T[]){ __VA_ARGS__ })));                       \
                                                                                    \
        ma_VECTOR = svld1(svptrue_b8(),                                             \
                          (ma_ELEMENT_T *)SVE_SAFE_NAME(chVectorBuffer));           \
    } while (0)

#define SVT_INIT_PRED(ma_PREDICT, ...)                                        \
    do {                                                                      \
        uint8_t SVE_SAFE_NAME(chBuffer)[svlen(svundef_u64())];                \
        memset(SVE_SAFE_NAME(chBuffer), /* This should NOT be LV_memset() */ \
               0,                                                             \
               sizeof(SVE_SAFE_NAME(chBuffer)));                              \
                                                                              \
        memcpy(SVE_SAFE_NAME(chBuffer), /* This should NOT be LV_memcpy() */ \
               (uint8_t[]){ __VA_ARGS__ },                                    \
               MIN(sizeof(SVE_SAFE_NAME(chBuffer)),                           \
                   sizeof((uint8_t[]){ __VA_ARGS__ })));                      \
                                                                              \
        ma_PREDICT = (*(svbool_t *)SVE_SAFE_NAME(chBuffer));                  \
    } while (0)

#define SVT_PRINT_PRED(ma_PREDICT, ma_TYPE_T)                                 \
    do {                                                                      \
        printf("%8s\t[", #ma_PREDICT);                                        \
        uint16_t SVE_SAFE_NAME(hwBuffer)[svlen(svundef_u64()) / 2];           \
        memset(SVE_SAFE_NAME(hwBuffer), /* This should NOT be LV_memset() */ \
               0,                                                             \
               sizeof(SVE_SAFE_NAME(hwBuffer)));                              \
        *(volatile svbool_t *)SVE_SAFE_NAME(hwBuffer) = (ma_PREDICT);         \
                                                                              \
        uint_fast16_t SVE_SAFE_NAME(nTotalBits) = svlen(svundef_u8());        \
        uint_fast8_t SVE_SAFE_NAME(nElementBits) = sizeof(ma_TYPE_T);         \
                                                                              \
        uint16_t *phwPred = SVE_SAFE_NAME(hwBuffer);                          \
        do {                                                                  \
            uint16_t hwPred = *phwPred++;                                     \
                                                                              \
            for (uint_fast8_t n = 0;                                          \
                 n < 16;                                                      \
                 n += SVE_SAFE_NAME(nElementBits)) {                          \
                                                                              \
                if (hwPred & 0x01) {                                          \
                    printf("True ");                                          \
                } else {                                                      \
                    printf("False");                                          \
                }                                                             \
                printf("%*s\t", (int)sizeof(ma_TYPE_T) - 1, "");              \
                hwPred >>= SVE_SAFE_NAME(nElementBits);                       \
            }                                                                 \
                                                                              \
            SVE_SAFE_NAME(nTotalBits) -= 16;                                  \
        } while (SVE_SAFE_NAME(nTotalBits));                                  \
                                                                              \
        printf("]\r\n");                                                      \
    } while (0)

#define SVT_PRINT_BUFFER(ma_BUFF_PTR, ma_SIZE, ma_TYPE_T, ma_FMT_STR, ma_STRIDE) \
    do {                                                                         \
        ma_TYPE_T *pBuffer = (ma_TYPE_T *)ma_BUFF_PTR;                           \
        size_t nElementCount = (ma_SIZE) / sizeof(ma_TYPE_T);                    \
                                                                                 \
        size_t nStrideSize = (ma_STRIDE);                                        \
        size_t nLineCount = 0;                                                   \
                                                                                 \
        printf("%s\n\t", #ma_BUFF_PTR);                                          \
        do {                                                                     \
                                                                                 \
            printf(ma_FMT_STR " ", *pBuffer++);                                  \
            nLineCount++;                                                        \
            if (nLineCount >= nStrideSize) {                                     \
                nLineCount = 0;                                                  \
                printf("\n\t");                                                  \
            }                                                                    \
                                                                                 \
        } while (--nElementCount);                                               \
        printf("\n");                                                            \
                                                                                 \
    } while (0)

#define LV_TARGETING(...)       __attribute__((target(__VA_ARGS__)))
#ifndef MIN
#   define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif
#endif /* LV_SVE2_UTIL_H */