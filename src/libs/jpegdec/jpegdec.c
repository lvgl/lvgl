//
// JPEG Decoder
//
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 8/2/2020
// Original JPEG code written 26+ years ago :)
// The goal of this code is to decode baseline JPEG images
// using no more than 18K of RAM (if sent directly to an LCD display)
//
// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include "jpegdec.h"
#if LV_USE_JPEGDEC

// Disable SIMD for now
//#if defined( __x86_64__ ) && !defined(NO_SIMD)
//    #define HAS_SSE
//    #include <emmintrin.h>
//    #include <tmmintrin.h>
//    #include <smmintrin.h>
    //#include <immintrin.h> // AVX2
//#endif
#ifdef HAS_SSE
#undef HAS_SSE
#endif
//#if !defined(HAS_SIMD) && !defined(NO_SIMD) && (defined(__arm64__) || defined(__aarch64__))
//    #include <arm_neon.h>
//    #define HAS_NEON
//#endif
#ifdef HAS_NEON
#undef HAS_NEON
#endif
// forward references
static int JPEGInit(JPEGIMAGE * pJPEG);
static int JPEGParseInfo(JPEGIMAGE * pPage, int bExtractThumb);
static void JPEGGetMoreData(JPEGIMAGE * pPage);
static int DecodeJPEG(JPEGIMAGE * pImage);
static int32_t readRAM(JPEGFILE * pFile, uint8_t * pBuf, int32_t iLen);
static int32_t seekMem(JPEGFILE * pFile, int32_t iPosition);
static void JPEGDither(JPEGIMAGE * pJPEG, int iWidth, int iHeight);
/* JPEG tables */
// zigzag ordering of DCT coefficients
static const unsigned char cZigZag[64] = {0, 1, 5, 6, 14, 15, 27, 28,
                                          2, 4, 7, 13, 16, 26, 29, 42,
                                          3, 8, 12, 17, 25, 30, 41, 43,
                                          9, 11, 18, 24, 31, 40, 44, 53,
                                          10, 19, 23, 32, 39, 45, 52, 54,
                                          20, 22, 33, 38, 46, 51, 55, 60,
                                          21, 34, 37, 47, 50, 56, 59, 61,
                                          35, 36, 48, 49, 57, 58, 62, 63
                                         };

// un-zigzag ordering
static const unsigned char cZigZag2[64] = {0, 1, 8, 16, 9, 2, 3, 10,
                                           17, 24, 32, 25, 18, 11, 4, 5,
                                           12, 19, 26, 33, 40, 48, 41, 34,
                                           27, 20, 13, 6, 7, 14, 21, 28,
                                           35, 42, 49, 56, 57, 50, 43, 36,
                                           29, 22, 15, 23, 30, 37, 44, 51,
                                           58, 59, 52, 45, 38, 31, 39, 46,
                                           53, 60, 61, 54, 47, 55, 62, 63
                                          };

#ifdef HAS_NEON
// 16-bit constants for NEON ycc->rgb conversion
static const int16_t __attribute__((aligned(16))) sYCCRGBConstants[4] = {5742 / 2, -2925 / 2, -1409 / 2, 7258 / 2};
// 16-bit constants for IDCT calculation
static const int16_t __attribute__((aligned(16))) s0414[8] = {1697 * 2, 1697 * 2, 1697 * 2, 1697 * 2, 1697 * 2, 1697 * 2, 1697 * 2, 1697 * 2}; // 1.414213562 - 1.0
static const int16_t __attribute__((aligned(16))) s1414[8] = {5793 * 2, 5793 * 2, 5793 * 2, 5793 * 2, 5793 * 2, 5793 * 2, 5793 * 2, 5793 * 2}; // 1.414213562
static const int16_t __attribute__((aligned(16))) s1847[8] = {7568 * 2, 7568 * 2, 7568 * 2, 7568 * 2, 7568 * 2, 7568 * 2, 7568 * 2, 7568 * 2}; // 1.8477
static const int16_t __attribute__((aligned(16))) s2613[8] = {-10703, -10703, -10703, -10703, -10703, -10703, -10703, -10703}; // -2.6131259
static const int16_t __attribute__((aligned(16))) sp2613[8] = {10703, 10703, 10703, 10703, 10703, 10703, 10703, 10703}; // 2.6131259
static const int16_t __attribute__((aligned(16))) s1082[8] = {4433 * 2, 4433 * 2, 4433 * 2, 4433 * 2, 4433 * 2, 4433 * 2, 4433 * 2, 4433 * 2}; // 1.08239
#endif // HAS_NEON

#ifdef HAS_SSE
#if defined ( __GNUC__ ) || defined( _GCC_ANDROID ) || defined( __APPLE__)
signed short s1402[8] __attribute__((aligned(16))) = { 5742, 5742, 5742, 5742, 5742, 5742, 5742, 5742 };
signed short s0714[8] __attribute__((aligned(16))) = { -2925, -2925, -2925, -2925, -2925, -2925, -2925, -2925 };
signed short s0344[8] __attribute__((aligned(16))) = { -1409, -1409, -1409, -1409, -1409, -1409, -1409, -1409 };
signed short s1772[8] __attribute__((aligned(16))) = { 7258, 7258, 7258, 7258, 7258, 7258, 7258, 7258 };
// 16-bit constants for IDCT calculation
signed short s0414[8] __attribute__((aligned(16))) = { 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4 }; // 1.414213562 - 1.0
signed short s1414[8] __attribute__((aligned(16))) = { 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4 }; // 1.414213562
signed short s1847[8] __attribute__((aligned(16))) = { 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4 }; // 1.8477
signed short s2613[8] __attribute__((aligned(16))) = { -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2 }; // -2.6131259
signed short sp2613[8] __attribute__((aligned(16))) = { 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2 }; // 2.6131259
signed short s1082[8] __attribute__((aligned(16))) = { 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4 }; // 1.08239
signed short sfastDCT[8] __attribute__((aligned(16))) = { 4096, 4096, 4096, 4096, -815, 2320, 3472, 4096 };
#else
// 16-bit Constants for SSE ycc->rgb conversion
__declspec(align(16)) signed short s1402[8] = {5742, 5742, 5742, 5742, 5742, 5742, 5742, 5742};
__declspec(align(16)) signed short s0714[8] = {-2925, -2925, -2925, -2925, -2925, -2925, -2925, -2925};
__declspec(align(16)) signed short s0344[8] = {-1409, -1409, -1409, -1409, -1409, -1409, -1409, -1409};
__declspec(align(16)) signed short s1772[8] = {7258, 7258, 7258, 7258, 7258, 7258, 7258, 7258};
// 16-bit constants for IDCT calculation
__declspec(align(16)) signed short s0414[8] = {1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4, 1697 * 4}; // 1.414213562 - 1.0
__declspec(align(16)) signed short s1414[8] = {5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4, 5793 * 4}; // 1.414213562
__declspec(align(16)) signed short s1847[8] = {7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4, 7568 * 4}; // 1.8477
__declspec(align(16)) signed short s2613[8] = {-10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2, -10703 * 2}; // -2.6131259
__declspec(align(16)) signed short sp2613[8] = {10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2, 10703 * 2}; // 2.6131259
__declspec(align(16)) signed short s1082[8] = {4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4, 4433 * 4}; // 1.08239
__declspec(align(16)) signed short sfastDCT[8] = {4096, 4096, 4096, 4096, -815, 2320, 3472, 4096};
#endif // GCC
#endif // HAS_SSE

// For AA&N IDCT method, multipliers are equal to quantization
// coefficients scaled by scalefactor[row]*scalefactor[col], where
// scalefactor[0] = 1
// scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
// For integer operation, the multiplier table is to be scaled by
// IFAST_SCALE_BITS.
static const int iScaleBits[64] = {16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
                                   22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
                                   21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
                                   19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
                                   16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
                                   12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
                                   8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
                                   4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
                                  };
//
// Range clip and shift for RGB565 output
// input value is 0 to 255, then another 256 for overflow to FF, then 512 more for negative values wrapping around
// Trims a few instructions off the final output stage
//
static const uint8_t ucRangeTable[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                                       0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
                                       0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
                                       0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
                                       0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
                                       0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
                                       0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
                                       0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                                       0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                                       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
                                       0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
                                       0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
                                       0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
                                       0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f
                                      };

//
// Convert 8-bit grayscale into RGB565
//
static const uint16_t usGrayTo565[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0020, 0x0020, 0x0020, // 0
                                       0x0841, 0x0841, 0x0841, 0x0841, 0x0861, 0x0861, 0x0861, 0x0861,
                                       0x1082, 0x1082, 0x1082, 0x1082, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
                                       0x18c3, 0x18c3, 0x18c3, 0x18c3, 0x18e3, 0x18e3, 0x18e3, 0x18e3,
                                       0x2104, 0x2104, 0x2104, 0x2104, 0x2124, 0x2124, 0x2124, 0x2124,
                                       0x2945, 0x2945, 0x2945, 0x2945, 0x2965, 0x2965, 0x2965, 0x2965,
                                       0x3186, 0x3186, 0x3186, 0x3186, 0x31a6, 0x31a6, 0x31a6, 0x31a6,
                                       0x39c7, 0x39c7, 0x39c7, 0x39c7, 0x39e7, 0x39e7, 0x39e7, 0x39e7,
                                       0x4208, 0x4208, 0x4208, 0x4208, 0x4228, 0x4228, 0x4228, 0x4228,
                                       0x4a49, 0x4a49, 0x4a49, 0x4a49, 0x4a69, 0x4a69, 0x4a69, 0x4a69,
                                       0x528a, 0x528a, 0x528a, 0x528a, 0x52aa, 0x52aa, 0x52aa, 0x52aa,
                                       0x5acb, 0x5acb, 0x5acb, 0x5acb, 0x5aeb, 0x5aeb, 0x5aeb, 0x5aeb,
                                       0x630c, 0x630c, 0x630c, 0x630c, 0x632c, 0x632c, 0x632c, 0x632c,
                                       0x6b4d, 0x6b4d, 0x6b4d, 0x6b4d, 0x6b6d, 0x6b6d, 0x6b6d, 0x6b6d,
                                       0x738e, 0x738e, 0x738e, 0x738e, 0x73ae, 0x73ae, 0x73ae, 0x73ae,
                                       0x7bcf, 0x7bcf, 0x7bcf, 0x7bcf, 0x7bef, 0x7bef, 0x7bef, 0x7bef,
                                       0x8410, 0x8410, 0x8410, 0x8410, 0x8430, 0x8430, 0x8430, 0x8430,
                                       0x8c51, 0x8c51, 0x8c51, 0x8c51, 0x8c71, 0x8c71, 0x8c71, 0x8c71,
                                       0x9492, 0x9492, 0x9492, 0x9492, 0x94b2, 0x94b2, 0x94b2, 0x94b2,
                                       0x9cd3, 0x9cd3, 0x9cd3, 0x9cd3, 0x9cf3, 0x9cf3, 0x9cf3, 0x9cf3,
                                       0xa514, 0xa514, 0xa514, 0xa514, 0xa534, 0xa534, 0xa534, 0xa534,
                                       0xad55, 0xad55, 0xad55, 0xad55, 0xad75, 0xad75, 0xad75, 0xad75,
                                       0xb596, 0xb596, 0xb596, 0xb596, 0xb5b6, 0xb5b6, 0xb5b6, 0xb5b6,
                                       0xbdd7, 0xbdd7, 0xbdd7, 0xbdd7, 0xbdf7, 0xbdf7, 0xbdf7, 0xbdf7,
                                       0xc618, 0xc618, 0xc618, 0xc618, 0xc638, 0xc638, 0xc638, 0xc638,
                                       0xce59, 0xce59, 0xce59, 0xce59, 0xce79, 0xce79, 0xce79, 0xce79,
                                       0xd69a, 0xd69a, 0xd69a, 0xd69a, 0xd6ba, 0xd6ba, 0xd6ba, 0xd6ba,
                                       0xdedb, 0xdedb, 0xdedb, 0xdedb, 0xdefb, 0xdefb, 0xdefb, 0xdefb,
                                       0xe71c, 0xe71c, 0xe71c, 0xe71c, 0xe73c, 0xe73c, 0xe73c, 0xe73c,
                                       0xef5d, 0xef5d, 0xef5d, 0xef5d, 0xef7d, 0xef7d, 0xef7d, 0xef7d,
                                       0xf79e, 0xf79e, 0xf79e, 0xf79e, 0xf7be, 0xf7be, 0xf7be, 0xf7be,
                                       0xffdf, 0xffdf, 0xffdf, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff
                                      };
//
// Clip and convert red value into 5-bits for RGB565
//
static const uint16_t usRangeTableR[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0
                                         0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800,
                                         0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                                         0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800,
                                         0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000,
                                         0x2800, 0x2800, 0x2800, 0x2800, 0x2800, 0x2800, 0x2800, 0x2800,
                                         0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000,
                                         0x3800, 0x3800, 0x3800, 0x3800, 0x3800, 0x3800, 0x3800, 0x3800,
                                         0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
                                         0x4800, 0x4800, 0x4800, 0x4800, 0x4800, 0x4800, 0x4800, 0x4800,
                                         0x5000, 0x5000, 0x5000, 0x5000, 0x5000, 0x5000, 0x5000, 0x5000,
                                         0x5800, 0x5800, 0x5800, 0x5800, 0x5800, 0x5800, 0x5800, 0x5800,
                                         0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000, 0x6000,
                                         0x6800, 0x6800, 0x6800, 0x6800, 0x6800, 0x6800, 0x6800, 0x6800,
                                         0x7000, 0x7000, 0x7000, 0x7000, 0x7000, 0x7000, 0x7000, 0x7000,
                                         0x7800, 0x7800, 0x7800, 0x7800, 0x7800, 0x7800, 0x7800, 0x7800,
                                         0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
                                         0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800,
                                         0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000, 0x9000,
                                         0x9800, 0x9800, 0x9800, 0x9800, 0x9800, 0x9800, 0x9800, 0x9800,
                                         0xa000, 0xa000, 0xa000, 0xa000, 0xa000, 0xa000, 0xa000, 0xa000,
                                         0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800, 0xa800,
                                         0xb000, 0xb000, 0xb000, 0xb000, 0xb000, 0xb000, 0xb000, 0xb000,
                                         0xb800, 0xb800, 0xb800, 0xb800, 0xb800, 0xb800, 0xb800, 0xb800,
                                         0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
                                         0xc800, 0xc800, 0xc800, 0xc800, 0xc800, 0xc800, 0xc800, 0xc800,
                                         0xd000, 0xd000, 0xd000, 0xd000, 0xd000, 0xd000, 0xd000, 0xd000,
                                         0xd800, 0xd800, 0xd800, 0xd800, 0xd800, 0xd800, 0xd800, 0xd800,
                                         0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000,
                                         0xe800, 0xe800, 0xe800, 0xe800, 0xe800, 0xe800, 0xe800, 0xe800,
                                         0xf000, 0xf000, 0xf000, 0xf000, 0xf000, 0xf000, 0xf000, 0xf000,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, // 256
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 512
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 768
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                        };
//
// Clip and convert green value into 5-bits for RGB565
//
static const uint16_t usRangeTableG[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0020, 0x0020, 0x0020, // 0
                                         0x0040, 0x0040, 0x0040, 0x0040, 0x0060, 0x0060, 0x0060, 0x0060,
                                         0x0080, 0x0080, 0x0080, 0x0080, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
                                         0x00c0, 0x00c0, 0x00c0, 0x00c0, 0x00e0, 0x00e0, 0x00e0, 0x00e0,
                                         0x0100, 0x0100, 0x0100, 0x0100, 0x0120, 0x0120, 0x0120, 0x0120,
                                         0x0140, 0x0140, 0x0140, 0x0140, 0x0160, 0x0160, 0x0160, 0x0160,
                                         0x0180, 0x0180, 0x0180, 0x0180, 0x01a0, 0x01a0, 0x01a0, 0x01a0,
                                         0x01c0, 0x01c0, 0x01c0, 0x01c0, 0x01e0, 0x01e0, 0x01e0, 0x01e0,
                                         0x0200, 0x0200, 0x0200, 0x0200, 0x0220, 0x0220, 0x0220, 0x0220,
                                         0x0240, 0x0240, 0x0240, 0x0240, 0x0260, 0x0260, 0x0260, 0x0260,
                                         0x0280, 0x0280, 0x0280, 0x0280, 0x02a0, 0x02a0, 0x02a0, 0x02a0,
                                         0x02c0, 0x02c0, 0x02c0, 0x02c0, 0x02e0, 0x02e0, 0x02e0, 0x02e0,
                                         0x0300, 0x0300, 0x0300, 0x0300, 0x0320, 0x0320, 0x0320, 0x0320,
                                         0x0340, 0x0340, 0x0340, 0x0340, 0x0360, 0x0360, 0x0360, 0x0360,
                                         0x0380, 0x0380, 0x0380, 0x0380, 0x03a0, 0x03a0, 0x03a0, 0x03a0,
                                         0x03c0, 0x03c0, 0x03c0, 0x03c0, 0x03e0, 0x03e0, 0x03e0, 0x03e0,
                                         0x0400, 0x0400, 0x0400, 0x0400, 0x0420, 0x0420, 0x0420, 0x0420,
                                         0x0440, 0x0440, 0x0440, 0x0440, 0x0460, 0x0460, 0x0460, 0x0460,
                                         0x0480, 0x0480, 0x0480, 0x0480, 0x04a0, 0x04a0, 0x04a0, 0x04a0,
                                         0x04c0, 0x04c0, 0x04c0, 0x04c0, 0x04e0, 0x04e0, 0x04e0, 0x04e0,
                                         0x0500, 0x0500, 0x0500, 0x0500, 0x0520, 0x0520, 0x0520, 0x0520,
                                         0x0540, 0x0540, 0x0540, 0x0540, 0x0560, 0x0560, 0x0560, 0x0560,
                                         0x0580, 0x0580, 0x0580, 0x0580, 0x05a0, 0x05a0, 0x05a0, 0x05a0,
                                         0x05c0, 0x05c0, 0x05c0, 0x05c0, 0x05e0, 0x05e0, 0x05e0, 0x05e0,
                                         0x0600, 0x0600, 0x0600, 0x0600, 0x0620, 0x0620, 0x0620, 0x0620,
                                         0x0640, 0x0640, 0x0640, 0x0640, 0x0660, 0x0660, 0x0660, 0x0660,
                                         0x0680, 0x0680, 0x0680, 0x0680, 0x06a0, 0x06a0, 0x06a0, 0x06a0,
                                         0x06c0, 0x06c0, 0x06c0, 0x06c0, 0x06e0, 0x06e0, 0x06e0, 0x06e0,
                                         0x0700, 0x0700, 0x0700, 0x0700, 0x0720, 0x0720, 0x0720, 0x0720,
                                         0x0740, 0x0740, 0x0740, 0x0740, 0x0760, 0x0760, 0x0760, 0x0760,
                                         0x0780, 0x0780, 0x0780, 0x0780, 0x07a0, 0x07a0, 0x07a0, 0x07a0,
                                         0x07c0, 0x07c0, 0x07c0, 0x07c0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, // 256
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0, 0x07e0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 512
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 768
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                        };
//
// Clip and convert blue value into 5-bits for RGB565
//
static const uint16_t usRangeTableB[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0
                                         0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
                                         0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
                                         0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003,
                                         0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
                                         0x0005, 0x0005, 0x0005, 0x0005, 0x0005, 0x0005, 0x0005, 0x0005,
                                         0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006,
                                         0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007,
                                         0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
                                         0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009,
                                         0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a,
                                         0x000b, 0x000b, 0x000b, 0x000b, 0x000b, 0x000b, 0x000b, 0x000b,
                                         0x000c, 0x000c, 0x000c, 0x000c, 0x000c, 0x000c, 0x000c, 0x000c,
                                         0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d,
                                         0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e,
                                         0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f,
                                         0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
                                         0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011,
                                         0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012,
                                         0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013,
                                         0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014,
                                         0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015,
                                         0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016,
                                         0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017,
                                         0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
                                         0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019,
                                         0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a,
                                         0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b,
                                         0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c,
                                         0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d,
                                         0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, // 256
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 512
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 768
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                        };
#if defined (__MACH__) || defined( __LINUX__ ) || defined( __MCUXPRESSO )
//
// API for C
//

//
// Memory initialization
//
int JPEG_openRAM(JPEGIMAGE * pJPEG, uint8_t * pData, int iDataSize, JPEG_DRAW_CALLBACK * pfnDraw)
{
    lv_memset(pJPEG, 0, sizeof(JPEGIMAGE));
    pJPEG->ucMemType = JPEG_MEM_RAM;
    pJPEG->pfnRead = readRAM;
    pJPEG->pfnSeek = seekMem;
    pJPEG->pfnDraw = pfnDraw;
    pJPEG->pfnOpen = NULL;
    pJPEG->pfnClose = NULL;
    pJPEG->JPEGFile.iSize = iDataSize;
    pJPEG->JPEGFile.pData = pData;
    pJPEG->iMaxMCUs = 1000; // set to an unnaturally high value to start
    return JPEGInit(pJPEG);
} /* JPEG_openRAM() */

int JPEG_getLastError(JPEGIMAGE * pJPEG)
{
    return pJPEG->iError;
} /* JPEG_getLastError() */

int JPEG_getWidth(JPEGIMAGE * pJPEG)
{
    return pJPEG->iWidth;
} /* JPEG_getWidth() */

int JPEG_getHeight(JPEGIMAGE * pJPEG)
{
    return pJPEG->iHeight;
} /* JPEG_getHeight() */

int JPEG_getOrientation(JPEGIMAGE * pJPEG)
{
    return (int)pJPEG->ucOrientation;
} /* JPEG_getOrientation() */

int JPEG_getBpp(JPEGIMAGE * pJPEG)
{
    return (int)pJPEG->ucBpp;
} /* JPEG_getBpp() */
int JPEG_getSubSample(JPEGIMAGE * pJPEG)
{
    return (int)pJPEG->ucSubSample;
} /* JPEG_getSubSample() */
int JPEG_hasThumb(JPEGIMAGE * pJPEG)
{
    return (int)pJPEG->ucHasThumb;
} /* JPEG_hasThumb() */

int JPEG_getThumbWidth(JPEGIMAGE * pJPEG)
{
    return pJPEG->iThumbWidth;
} /* JPEG_getThumbWidth() */

int JPEG_getThumbHeight(JPEGIMAGE * pJPEG)
{
    return pJPEG->iThumbHeight;
} /* JPEG_getThumbHeight() */

void JPEG_setPixelType(JPEGIMAGE * pJPEG, int iType)
{
    pJPEG->ucPixelType = (uint8_t)iType;
} /* JPEG_setPixelType() */

void JPEG_setMaxOutputSize(JPEGIMAGE * pJPEG, int iMaxMCUs)
{
    if(iMaxMCUs < 1)
        iMaxMCUs = 1; // don't allow invalid value
    pJPEG->iMaxMCUs = iMaxMCUs;
} /* JPEG_setMaxOutputSize() */

int JPEG_decode(JPEGIMAGE * pJPEG, int x, int y, int iOptions)
{
    pJPEG->iXOffset = x;
    pJPEG->iYOffset = y;
    pJPEG->iOptions = iOptions;
    return DecodeJPEG(pJPEG);
} /* JPEG_decode() */

int JPEG_decodeDither(JPEGIMAGE * pJPEG, uint8_t * pDither, int iOptions)
{
    pJPEG->iOptions = iOptions;
    pJPEG->pDitherBuffer = pDither;
    return DecodeJPEG(pJPEG);
} /* JPEG_decodeDither() */

void JPEG_close(JPEGIMAGE * pJPEG)
{
    if(pJPEG->pfnClose)
        (*pJPEG->pfnClose)(pJPEG->JPEGFile.fHandle);
} /* JPEG_close() */

#endif // !__cplusplus

//
// Validate/adjust the requested crop area to land on MCU boundaries
// (expand in all directions if needed)
//
void JPEG_setCropArea(JPEGIMAGE * pJPEG, int x, int y, int w, int h)
{
    int mcuCX = 0, mcuCY = 0;

    if(x < 0)
        x = 0;
    if(y < 0)
        y = 0;
    switch(pJPEG->ucSubSample) { // set up the parameters for the different subsampling options
        case 0x00: // fake value to handle grayscale
        case 0x01: // fake value to handle sRGB/CMYK
        case 0x11:
            mcuCX = mcuCY = 8;
            break;
        case 0x12:
            mcuCX = 8;
            mcuCY = 16;
            break;
        case 0x21:
            mcuCX = 16;
            mcuCY = 8;
            break;
        case 0x22:
            mcuCX = mcuCY = 16;
            break;
        default: // to suppress compiler warning
            break;
    }
    if(w & (mcuCX - 1)) {
        w &= ~(mcuCX - 1);
        w += mcuCX;
    }
    if(h & (mcuCY - 1)) {
        h &= ~(mcuCY - 1);
        h += mcuCY;
    }
    if(x > pJPEG->iWidth - mcuCX) x = pJPEG->iWidth - mcuCX;
    if(y > pJPEG->iHeight - mcuCY) y = pJPEG->iHeight - mcuCY;
    if(x + w > pJPEG->iWidth) w = pJPEG->iWidth - mcuCX;
    if(y + h > pJPEG->iHeight) h = pJPEG->iHeight - mcuCY;
    x &= ~(mcuCX - 1);
    y &= ~(mcuCY - 1);
    pJPEG->iCropX = x;
    pJPEG->iCropY = y;
    pJPEG->iCropCX = w;
    pJPEG->iCropCY = h;
} /* JPEG_setCropArea() */

void JPEG_getCropArea(JPEGIMAGE * pJPEG, int * x, int * y, int * w, int * h)
{
    *x = pJPEG->iCropX;
    *y = pJPEG->iCropY;
    *w = pJPEG->iCropCX;
    *h = pJPEG->iCropCY;
} /* JPEG_getCropArea() */

void JPEG_setFramebuffer(JPEGIMAGE * pJPEG, void * pFramebuffer)
{
    pJPEG->pFramebuffer = pFramebuffer;
} /* JPEG_setFramebuffer() */

//
// Helper functions for memory based images
//
static int32_t readRAM(JPEGFILE * pFile, uint8_t * pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if((pFile->iSize - pFile->iPos) < iLen)
        iBytesRead = pFile->iSize - pFile->iPos;
    if(iBytesRead <= 0)
        return 0;
    lv_memcpy(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readRAM() */

#ifdef FUTURE
static int32_t readFLASH(JPEGFILE * pFile, uint8_t * pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if((pFile->iSize - pFile->iPos) < iLen)
        iBytesRead = pFile->iSize - pFile->iPos;
    if(iBytesRead <= 0)
        return 0;
    lv_memcpy(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readFLASH() */
#endif

static int32_t seekMem(JPEGFILE * pFile, int32_t iPosition)
{
    if(iPosition < 0) iPosition = 0;
    else if(iPosition >= pFile->iSize) iPosition = pFile->iSize - 1;
    pFile->iPos = iPosition;
    return iPosition;
} /* seekMem() */

//
// The following functions are written in plain C and have no
// 3rd party dependencies, not even the C runtime library
//
//
// Initialize a JPEG file and callback access from a file on SD or memory
// returns 1 for success, 0 for failure
// Fills in the basic image info fields of the JPEGIMAGE structure
//
static int JPEGInit(JPEGIMAGE * pJPEG)
{
    return JPEGParseInfo(pJPEG, 0); // gather info for image
} /* JPEGInit() */
//
// Unpack the Huffman tables
//
static int JPEGGetHuffTables(uint8_t * pBuf, int iLen, JPEGIMAGE * pJPEG)
{
    int i, j, iOffset, iTableOffset;
    uint8_t ucTable, * pHuffVals;

    iOffset = 0;
    pHuffVals = (uint8_t *)pJPEG->usPixels; // temp holding area to save RAM
    while(iLen > 17) { // while there are tables to copy (we may have combined more than 1 table together)
        ucTable = pBuf[iOffset++]; // get table index
        if(ucTable & 0x10)  // convert AC offset of 0x10 into offset of 4
            ucTable ^= 0x14;
        pJPEG->ucHuffTableUsed |= (1 << ucTable); // mark this table as being defined
        if(ucTable <= 7) { // tables are 0-3, AC+DC
            iTableOffset = ucTable * HUFF_TABLEN;
            j = 0; // total bits
            for(i = 0; i < 16; i++) {
                j += pBuf[iOffset];
                pHuffVals[iTableOffset + i] = pBuf[iOffset++];
            }
            iLen -= 17; // subtract length of bit lengths
            if(j == 0 || j > 256 || j > iLen) { // bogus bit lengths
                return -1;
            }
            iTableOffset += 16;
            for(i = 0; i < j; i++) {
                // copy huffman table
                pHuffVals[iTableOffset + i] = pBuf[iOffset++];
            }
            iLen -= j;
        }
    }
    return 0;
} /* JPEGGetHuffTables() */
#ifdef FUTURE
//
// Create 11-bit lookup tables for some images where it doesn't work
// for 10-bit tables
//
static int JPEGMakeHuffTables_Slow(JPEGIMAGE * pJPEG, int bThumbnail)
{
    int code, repeat, count, codestart;
    int j;
    int iLen, iTable;
    unsigned short * pTable, * pShort, * pLong;
    unsigned char * pucTable, * pucShort, * pucLong;
    uint32_t ul, * pLongTable;
    int iBitNum; // current code bit length
    int cc; // code
    unsigned char * p, * pBits, ucCode;
    int iMaxLength, iMaxMask;

    pJPEG->b11Bit = 1; // indicate we're using the bigger A/C decode tables
    // first do DC components (up to 4 tables of 12-bit codes)
    // we can save time and memory for the DC codes by knowing that there exist short codes (<= 6 bits)
    // and long codes (>6 bits, but the first 5 bits are 1's).  This allows us to create 2 tables: a 6-bit and 7 or 8-bit
    // to handle any DC codes
    iMaxLength = 12; // assume DC codes can be 12-bits
    iMaxMask = 0x7f; // lower 7 bits after truncate 5 leading 1's
    if(pJPEG->ucMode == 0xc3) { // create 13-bit tables for lossless mode
        iMaxLength = 13;
        iMaxMask = 0xff;
    }
    for(iTable = 0; iTable < 2; iTable++) {
        if(pJPEG->ucHuffTableUsed & (1 << iTable)) {
            //         pJPEG->huffdcFast[iTable] = (int *)PILIOAlloc(0x180); // short table = 128 bytes, long table = 256 bytes
            pucShort = (unsigned char *)&pJPEG->ucHuffDC[iTable * DC_TABLE_SIZE];
            //         pJPEG->huffdc[iTable] = pJPEG->huffdcFast[iTable] + 0x20; // 0x20 longs = 128 bytes
            pucLong = (unsigned char *)&pJPEG->ucHuffDC[iTable * DC_TABLE_SIZE + 128];
            pBits = &pJPEG->ucHuffVals[iTable * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            cc = 0; // start with a code of 0
            for(iBitNum = 1; iBitNum <= 16; iBitNum++) {
                iLen = *pBits++; // get number of codes for this bit length
                if(iBitNum > iMaxLength && iLen > 0) { // we can't handle codes longer a certain length
                    return -1;
                }
                while(iLen) {
                    //               if (iBitNum > 6) // do long table
                    if((cc >> (iBitNum - 5)) == 0x1f) { // first 5 bits are 1 - use long table
                        count = iMaxLength - iBitNum;
                        codestart = cc << count;
                        pucTable = &pucLong[codestart & iMaxMask]; // use lower 7/8 bits of code
                    }
                    else { // do short table
                        count = 6 - iBitNum;
                        if(count < 0)
                            return -1; // DEBUG - something went wrong
                        codestart = cc << count;
                        pucTable = &pucShort[codestart];
                    }
                    ucCode = *p++;  // get actual huffman code
                    if(ucCode == 16 && pJPEG->ucMode == 0xc3) { // lossless mode
                        // in lossless mode, this code won't fit in 4 bits, so save it's length in the next slot
                        ucCode = 255;
                        pucLong[256] = (unsigned char)iBitNum;
                    }
                    // does precalculating the DC value save time on ARM?
#ifndef USE_ARM_ASM
                    if(ucCode != 0 && (ucCode + iBitNum) <= 6 &&
                       pJPEG->ucMode != 0xc2) { // we can fit the magnitude value in the code lookup (not for progressive)
                        int k, iLoop;
                        unsigned char ucCoeff;
                        unsigned char * d = &pucTable[512];
                        unsigned char ucMag = ucCode;
                        ucCode |= ((iBitNum + ucCode) << 4); // add magnitude bits to length
                        repeat = 1 << ucMag;
                        iLoop = 1 << (count - ucMag);
                        for(j = 0; j < repeat; j++) {
                            // calcuate the magnitude coeff already
                            if(j & 1 << (ucMag - 1)) // positive number
                                ucCoeff = (unsigned char)j;
                            else // negative number
                                ucCoeff = (unsigned char)(j - ((1 << ucMag) - 1));
                            for(k = 0; k < iLoop; k++) {
                                *d++ = ucCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else {
                        ucCode |= (iBitNum << 4);
                    }
                    if(count) { // do it as dwords to save time
                        repeat = (1 << count);
                        lv_memset(pucTable, ucCode, repeat);
                        //                  pLongTable = (uint32_t *)pTable;
                        //                  repeat = 1 << (count-2); // store as dwords (/4)
                        //                  ul = code | (code << 16);
                        //                  for (j=0; j<repeat; j++)
                        //                     *pLongTable++ = ul;
                    }
                    else {
                        pucTable[0] = ucCode;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            }
        } // if table defined
    }
    // now do AC components (up to 2 tables of 16-bit codes)
    // We split the codes into a short table (9 bits or less) and a long table (first 5 bits are 1)
    for(iTable = 0; iTable < 2; iTable++) {
        if(pJPEG->ucHuffTableUsed & (1 << (iTable + 4))) { // if this table is defined
            pBits = &pJPEG->ucHuffVals[(iTable + 4) * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            pShort = &pJPEG->usHuffAC[iTable * HUFF11SIZE];
            pLong = &pJPEG->usHuffAC[iTable * HUFF11SIZE + 1024]; // long codes start here
            cc = 0; // start with a code of 0
            // construct the decode table
            for(iBitNum = 1; iBitNum <= 16; iBitNum++) {
                iLen = *pBits++; // get number of codes for this bit length
                while(iLen) {
                    if((cc >> (iBitNum - 4)) == 0xf) { // first 4 bits are 1 - use long table
                        count = 16 - iBitNum;
                        codestart = cc << count;
                        pTable = &pLong[codestart & 0xfff]; // use lower 12 bits of code
                    }
                    else {
                        count = 12 - iBitNum;
                        if(count < 0) { // a 13-bit? code - that doesn't fit our optimized scheme, see if we can do a bigger table version
                            return -1; // DEBUG - fatal error, we currently don't support it
                        }
                        codestart = cc << count;
                        pTable = &pShort[codestart]; // 11 bits or shorter
                    }
                    code = *p++;  // get actual huffman code
                    if(bThumbnail && code != 0) { // add "extra" bits to code length since we skip these codes
                        // get rid of extra bits in code and add increment (1) for AC index
                        code = ((iBitNum + (code & 0xf)) << 8) | ((code >> 4) + 1);
                    }
                    else {
                        code |= (iBitNum << 8);
                    }
                    if(count) { // do it as dwords to save time
                        repeat = 1 << (count - 1); // store as dwords (/2)
                        ul = code | (code << 16);
                        pLongTable = (uint32_t *)pTable;
                        for(j = 0; j < repeat; j++)
                            *pLongTable++ = ul;
                    }
                    else {
                        pTable[0] = (unsigned short)code;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            } // for each bit length
        } // if table defined
    }
    return 0;
} /* JPEGMakeHuffTables_Slow() */
#endif // FUTURE
//
// Expand the Huffman tables for fast decoding
// returns 1 for success, 0 for failure
//
static int JPEGMakeHuffTables(JPEGIMAGE * pJPEG, int bThumbnail)
{
    int code, repeat, count, codestart;
    int j;
    int iLen, iTable;
    uint16_t * pTable, * pShort, * pLong;
    uint8_t * pHuffVals, * pucTable, * pucShort, * pucLong;
    uint32_t ul, * pLongTable;
    int iBitNum; // current code bit length
    int cc; // code
    uint8_t * p, * pBits, ucCode;
    int iMaxLength, iMaxMask;
    int iTablesUsed;

    iTablesUsed = 0;
    pHuffVals = (uint8_t *)pJPEG->usPixels;
    for(j = 0; j < 4; j++) {
        if(pJPEG->ucHuffTableUsed & (1 << j))
            iTablesUsed++;
    }
    // first do DC components (up to 4 tables of 12-bit codes)
    // we can save time and memory for the DC codes by knowing that there exist short codes (<= 6 bits)
    // and long codes (>6 bits, but the first 5 bits are 1's).  This allows us to create 2 tables: a 6-bit and 7 or 8-bit
    // to handle any DC codes
    iMaxLength = 12; // assume DC codes can be 12-bits
    iMaxMask = 0x7f; // lower 7 bits after truncate 5 leading 1's
    for(iTable = 0; iTable < 4; iTable++) {
        if(pJPEG->ucHuffTableUsed & (1 << iTable)) {
            //         pJPEG->huffdcFast[iTable] = (int *)PILIOAlloc(0x180); // short table = 128 bytes, long table = 256 bytes
            pucShort = &pJPEG->ucHuffDC[iTable * DC_TABLE_SIZE];
            //         pJPEG->huffdc[iTable] = pJPEG->huffdcFast[iTable] + 0x20; // 0x20 longs = 128 bytes
            pucLong = &pJPEG->ucHuffDC[iTable * DC_TABLE_SIZE + 128];
            pBits = &pHuffVals[iTable * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            cc = 0; // start with a code of 0
            for(iBitNum = 1; iBitNum <= 16; iBitNum++) {
                iLen = *pBits++; // get number of codes for this bit length
                if(iBitNum > iMaxLength && iLen > 0) { // we can't handle codes longer a certain length
                    return 0;
                }
                while(iLen) {
                    //               if (iBitNum > 6) // do long table
                    if((cc >> (iBitNum - 5)) == 0x1f) { // first 5 bits are 1 - use long table
                        count = iMaxLength - iBitNum;
                        codestart = cc << count;
                        pucTable = &pucLong[codestart & iMaxMask]; // use lower 7/8 bits of code
                    }
                    else { // do short table
                        count = 6 - iBitNum;
                        if(count < 0)
                            return 0; // DEBUG - something went wrong
                        codestart = cc << count;
                        pucTable = &pucShort[codestart];
                    }
                    ucCode = *p++;  // get actual huffman code
                    // does precalculating the DC value save time on ARM?
#ifndef USE_ARM_ASM
                    if(ucCode != 0 && (ucCode + iBitNum) <= 6 &&
                       pJPEG->ucMode != 0xc2) { // we can fit the magnitude value in the code lookup (not for progressive)
                        int k, iLoop;
                        unsigned char ucCoeff;
                        unsigned char * d = &pucTable[512];
                        unsigned char ucMag = ucCode;
                        ucCode |= ((iBitNum + ucCode) << 4); // add magnitude bits to length
                        repeat = 1 << ucMag;
                        iLoop = 1 << (count - ucMag);
                        for(j = 0; j < repeat; j++) {
                            // calcuate the magnitude coeff already
                            if(j & 1 << (ucMag - 1)) // positive number
                                ucCoeff = (unsigned char)j;
                            else // negative number
                                ucCoeff = (unsigned char)(j - ((1 << ucMag) - 1));
                            for(k = 0; k < iLoop; k++) {
                                *d++ = ucCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else {
                        ucCode |= (iBitNum << 4);
                    }
                    if(count) { // do it as dwords to save time
                        repeat = (1 << count);
                        lv_memset(pucTable, ucCode, repeat);
                        //                  pLongTable = (uint32_t *)pTable;
                        //                  repeat = 1 << (count-2); // store as dwords (/4)
                        //                  ul = code | (code << 16);
                        //                  for (j=0; j<repeat; j++)
                        //                     *pLongTable++ = ul;
                    }
                    else {
                        pucTable[0] = ucCode;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            }
        } // if table defined
    }
    // now do AC components (up to 4 tables of 16-bit codes)
    // We split the codes into a short table (9 bits or less) and a long table (first 5 bits are 1)
    for(iTable = 0; iTable < 4; iTable++) {
        if(pJPEG->ucHuffTableUsed & (1 << (iTable + 4))) { // if this table is defined
            pBits = &pHuffVals[(iTable + 4) * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            if(iTable * HUFF11SIZE >= (int)sizeof(pJPEG->usHuffAC) / 2)
                return 0;
            pShort = &pJPEG->usHuffAC[iTable * HUFF11SIZE];
            pLong = &pJPEG->usHuffAC[iTable * HUFF11SIZE + 1024];
            cc = 0; // start with a code of 0
            // construct the decode table
            for(iBitNum = 1; iBitNum <= 16; iBitNum++) {
                iLen = *pBits++; // get number of codes for this bit length
                while(iLen) {
                    if((cc >> (iBitNum - 6)) == 0x3f) { // first 6 bits are 1 - use long table
                        count = 16 - iBitNum;
                        codestart = cc << count;
                        pTable = &pLong[codestart & 0x3ff]; // use lower 10 bits of code
                    }
                    else {
                        count = 10 - iBitNum;
                        if(count < 0) { // an 11/12-bit? code - that doesn't fit our optimized scheme, see if we can do a bigger table version
                            if(count == -1 && iTablesUsed <= 4) { // we need to create "slow" tables
                                // DEBUG
                                //                                j = JPEGMakeHuffTables_Slow(pJPEG, bThumbnail);
                                return 0;
                            }
                            else
                                return 0; // DEBUG - fatal error, more than 2 big tables we currently don't support
                        }
                        codestart = cc << count;
                        pTable = &pShort[codestart]; // 10 bits or shorter
                    }
                    code = *p++;  // get actual huffman code
                    if(bThumbnail && code != 0) { // add "extra" bits to code length since we skip these codes
                        // get rid of extra bits in code and add increment (1) for AC index
                        code = ((iBitNum + (code & 0xf)) << 8) | ((code >> 4) + 1);
                    }
#ifdef BOGUS // precalculating the AC coeff makes it run slightly slower
                    else if((code & 0xf) != 0 && (code + iBitNum) <= 10) { // we can fit the magnitude value + huffman code in a single read
                        int k, iLoop;
                        unsigned short usCoeff;
                        unsigned short * d = &pTable[4096]; // use unused table slots 2+3 for extra coeff data
                        unsigned char ucMag = (unsigned char)(code & 0xf);
                        code |= ((iBitNum + (code & 0xf)) << 8); // add magnitude bits to length
                        repeat = 1 << ucMag;
                        iLoop = 1 << (count - ucMag);
                        for(j = 0; j < repeat; j++) {
                            // calcuate the magnitude coeff already
                            if(j & 1 << (ucMag - 1)) // positive number
                                usCoeff = (unsigned short)j;
                            else // negative number
                                usCoeff = (unsigned short)(j - ((1 << ucMag) - 1));
                            for(k = 0; k < iLoop; k++) {
                                *d++ = usCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else {
                        code |= (iBitNum << 8);
                    }
                    if(count) { // do it as dwords to save time
                        repeat = 1 << (count - 1); // store as dwords (/2)
                        ul = code | (code << 16);
                        pLongTable = (uint32_t *)pTable;
                        for(j = 0; j < repeat; j++)
                            *pLongTable++ = ul;
                    }
                    else {
                        pTable[0] = (unsigned short)code;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            } // for each bit length
        } // if table defined
    }
    return 1;
} /* JPEGMakeHuffTables() */

//
// TIFFSHORT
// read a 16-bit unsigned integer from the given pointer
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static uint16_t TIFFSHORT(unsigned char * p, int bMotorola)
{
    unsigned short s;

    if(bMotorola)
        s = *p * 0x100 + *(p + 1); // big endian (AKA Motorola byte order)
    else
        s = *p + *(p + 1) * 0x100; // little endian (AKA Intel byte order)
    return s;
} /* TIFFSHORT() */
//
// TIFFLONG
// read a 32-bit unsigned integer from the given pointer
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static uint32_t TIFFLONG(unsigned char * p, int bMotorola)
{
    uint32_t l;

    if(bMotorola)
        l = *p * 0x1000000 + *(p + 1) * 0x10000 + *(p + 2) * 0x100 + *(p + 3); // big endian
    else
        l = *p + *(p + 1) * 0x100 + *(p + 2) * 0x10000 + *(p + 3) * 0x1000000; // little endian
    return l;
} /* TIFFLONG() */
//
// TIFFVALUE
// read an integer value encoded in a TIFF TAG (12-byte structure)
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static int TIFFVALUE(unsigned char * p, int bMotorola)
{
    int i, iType;

    iType = TIFFSHORT(p + 2, bMotorola);
    /* If pointer to a list of items, must be a long */
    if(TIFFSHORT(p + 4, bMotorola) > 1) {
        iType = 4;
    }
    switch(iType) {
        case 3: /* Short */
            i = TIFFSHORT(p + 8, bMotorola);
            break;
        case 4: /* Long */
        case 7: // undefined (treat it as a long since it's usually a multibyte buffer)
            i = TIFFLONG(p + 8, bMotorola);
            break;
        case 6: // signed byte
            i = (signed char)p[8];
            break;
        case 2: /* ASCII */
        case 5: /* Unsigned Rational */
        case 10: /* Signed Rational */
            i = TIFFLONG(p + 8, bMotorola);
            break;
        default: /* to suppress compiler warning */
            i = 0;
            break;
    }
    return i;

} /* TIFFVALUE() */
static void GetTIFFInfo(JPEGIMAGE * pPage, int bMotorola, int iOffset)
{
    int iTag, iTagCount, i;
    uint8_t * cBuf = pPage->ucFileBuf;

    iTagCount = TIFFSHORT(&cBuf[iOffset], bMotorola);  /* Number of tags in this dir */
    if(iTagCount < 1 || iTagCount > 256)  // invalid tag count
        return; /* Bad header info */
    /*--- Search the TIFF tags ---*/
    for(i = 0; i < iTagCount; i++) {
        unsigned char * p = &cBuf[iOffset + (i * 12) + 2];
        iTag = TIFFSHORT(p, bMotorola);  /* current tag value */
        if(iTag == 274) { // orientation tag
            pPage->ucOrientation = TIFFVALUE(p, bMotorola);
        }
        else if(iTag == 256) { // width of thumbnail
            pPage->iThumbWidth = TIFFVALUE(p, bMotorola);
        }
        else if(iTag == 257) { // height of thumbnail
            pPage->iThumbHeight = TIFFVALUE(p, bMotorola);
        }
        else if(iTag == 513) { // offset to JPEG data
            pPage->iThumbData = TIFFVALUE(p, bMotorola);
        }
    }
} /* GetTIFFInfo() */

static int JPEGGetSOS(JPEGIMAGE * pJPEG, int * iOff)
{
    int16_t sLen;
    int iOffset = *iOff;
    int i, j;
    uint8_t uc, c, cc;
    uint8_t * buf = pJPEG->ucFileBuf;

    sLen = MOTOSHORT(&buf[iOffset]);
    iOffset += 2;

    // Assume no components in this scan
    for(i = 0; i < 4; i++)
        pJPEG->JPCI[i].component_needed = 0;

    uc = buf[iOffset++]; // get number of components
    pJPEG->ucComponentsInScan = uc;
    sLen -= 3;
    if(uc < 1 || uc > MAX_COMPS_IN_SCAN || sLen != (uc * 2 + 3)) // check length of data packet
        return 1; // error
    for(i = 0; i < uc; i++) {
        cc = buf[iOffset++];
        c = buf[iOffset++];
        sLen -= 2;
        for(j = 0; j < 4; j++) { // search for component id
            if(pJPEG->JPCI[j].component_id == cc)
                break;
        }
        if(j == 4)  // error, not found
            return 1;
        if((c & 0xf) > 3 || (c & 0xf0) > 0x30)
            return 1; // bogus table numbers
        pJPEG->JPCI[j].dc_tbl_no = c >> 4;
        pJPEG->JPCI[j].ac_tbl_no = c & 0xf;
        pJPEG->JPCI[j].component_needed = 1; // mark this component as being included in the scan
    }
    pJPEG->iScanStart = buf[iOffset++]; // Get the scan start (or lossless predictor) for this scan
    pJPEG->iScanEnd = buf[iOffset++]; // Get the scan end for this scan
    c = buf[iOffset++]; // successive approximation bits
    pJPEG->cApproxBitsLow = c & 0xf; // also point transform in lossless mode
    pJPEG->cApproxBitsHigh = c >> 4;

    *iOff = iOffset;
    return 0;

} /* JPEGGetSOS() */
//
// Remove markers from the data stream to allow faster decode
// Stuffed zeros and restart interval markers aren't needed to properly decode
// the data, but they make reading VLC data slower, so I pull them out first
//
static int JPEGFilter(uint8_t * pBuf, uint8_t * d, int iLen, uint8_t * bFF)
{
#ifdef HAS_SSE
    __m128i xmmIn, xmmOut;
    __m128i xmmFF = _mm_set1_epi8(-1);
#endif // HAS_SSE
#ifdef HAS_NEON
    uint8x16_t u816FF = vdupq_n_u8(0xff);
    uint8x16_t u816In, u816Out;
#ifdef OLD_NEON
    uint8x8_t u88Merged;
    uint32x2_t u322merged;
#endif // OLD_NEON
#endif // HAS_NEON

    unsigned char c, * s, * pEnd, * pStart;

    pStart = d;
    s = pBuf;
    pEnd = &s[iLen - 1]; // stop just shy of the end to not miss a final marker/stuffed 0
    if(*bFF) { // last byte was a FF, check the next one
        if(s[0] == 0)  // stuffed 0, keep the FF
            *d++ = 0xff;
        s++;
        *bFF = 0;
    }
#ifdef HAS_SSE
    while(s < pEnd - 16) {
        xmmIn = _mm_loadu_si128((__m128i *)s);
        xmmOut = _mm_cmpeq_epi8(xmmFF, xmmIn); // any FF's in these 16 bytes?
        if(_mm_movemask_epi8(xmmOut) == 0) { // no FF's, just copy this block
            _mm_storeu_si128((__m128i *)d, xmmIn);
            s += 16;
            d += 16;
        }
        else {
            int i = 16; // do these 16 bytes the slow way
            while(i) {
                c = *d++ = *s++;
                if(c == 0xff) {  // marker or stuffed zeros?
                    if(s[0] != 0) {  // it's a marker, skip both
                        d--;
                    }
                    s++; // for stuffed 0's, store the FF, skip the 00
                } // found FF
                i--;
            } // while processing the 16 "slow" bytes
        }
    } // while SSE filtering
#endif // HAS_SSE
#ifdef HAS_NEON
    while(s < pEnd - 16) {
        u816In = vld1q_u8(s);
        u816Out = vceqq_u8(u816FF, u816In); // any FF's in these 16 bytes?
#ifdef OLD_NEON
        u88Merged = vpadd_u8(vget_high_u8(u816Out), vget_low_u8(u816Out));
        u322merged = vpadd_u32(vreinterpret_u32_u8(u88Merged), vreinterpret_u32_u8(u88Merged));
        if(vget_lane_u32(u322merged, 0)  == 0)   // no FF's, just copy this block
#else
        if(vaddvq_u8(u816Out) == 0)  // any byte != 0 means FFs
#endif
        {
            vst1q_u8(d, u816In);
            s += 16;
            d += 16;
        }
        else {
            int i = 16; // do these 16 bytes the slow way
            while(i) {
                c = *d++ = *s++;
                if(c == 0xff) {  // marker or stuffed zeros?
                    if(s[0] != 0) {  // it's a marker, skip both
                        d--;
                    }
                    s++; // for stuffed 0's, store the FF, skip the 00
                } // found FF
                i--;
            } // while processing the 16 "slow" bytes
        } // if need to remove stuffed FF's or markers
    } // while processing buffer with SIMD
#endif // HAS_NEON

    while(s < pEnd) {
        c = *d++ = *s++;
        if(c == 0xff) { // marker or stuffed zeros?
            if(s[0] != 0) { // it's a marker, skip both
                d--;
            }
            s++; // for stuffed 0's, store the FF, skip the 00
        }
    }
    if(s == pEnd) { // need to test the last byte
        c = s[0];
        if(c == 0xff)  // last byte is FF, take care of it next time through
            *bFF = 1; // take care of it next time through
        else
            *d++ = c; // nope, just store it
    }
    return (int)(d - pStart); // filtered output length
} /* JPEGFilter() */
//
// Read and filter more VLC data for decoding
//
static void JPEGGetMoreData(JPEGIMAGE * pPage)
{
    int iDelta = pPage->iVLCSize - pPage->iVLCOff;
    //    printf("Getting more data...size=%d, off=%d\n", pPage->iVLCSize, pPage->iVLCOff);
    // move any existing data down
    if(iDelta >= (JPEG_FILE_BUF_SIZE - 64) || iDelta < 0)
        return; // buffer is already full; no need to read more data
    if(pPage->iVLCOff != 0) {
        lv_memcpy(pPage->ucFileBuf, &pPage->ucFileBuf[pPage->iVLCOff], pPage->iVLCSize - pPage->iVLCOff);
        pPage->iVLCSize -= pPage->iVLCOff;
        pPage->iVLCOff = 0;
        pPage->bb.pBuf = pPage->ucFileBuf; // reset VLC source pointer too
    }
    if(pPage->JPEGFile.iPos < pPage->JPEGFile.iSize && pPage->iVLCSize < JPEG_FILE_BUF_SIZE - 64) {
        int i;
        // Try to read enough to fill the buffer
        i = (*pPage->pfnRead)(&pPage->JPEGFile, &pPage->ucFileBuf[pPage->iVLCSize],
                              JPEG_FILE_BUF_SIZE - pPage->iVLCSize); // max length we can read
        // Filter out the markers
        pPage->iVLCSize += JPEGFilter(&pPage->ucFileBuf[pPage->iVLCSize], &pPage->ucFileBuf[pPage->iVLCSize], i, &pPage->ucFF);
    }
} /* JPEGGetMoreData() */

//
// Parse the JPEG header, gather necessary info to decode the image
// Returns 1 for success, 0 for failure
//
static int JPEGParseInfo(JPEGIMAGE * pPage, int bExtractThumb)
{
    int iBytesRead;
    int i, iOffset, iTableOffset;
    uint8_t ucTable, * s = pPage->ucFileBuf;
    uint16_t usMarker, usLen = 0;
    int iFilePos = 0;

    pPage->pFramebuffer = NULL; // this must be set AFTER calling this function
    // make sure usPixels is 16-byte aligned for S3 SIMD (and possibly others)
    i = (int)(intptr_t)pPage->usUnalignedPixels;
    i &= 15;
    if(i == 0) i = 16;  // already 16-byte aligned
    pPage->usPixels = &pPage->usUnalignedPixels[(16 - i) >> 1];
    // do the same for the MCU buffers
    i = (int)(intptr_t)pPage->sUnalignedMCUs;
    i &= 15;
    if(i == 0) i = 16;
    pPage->sMCUs = &pPage->sUnalignedMCUs[(16 - i) >> 1];

    if(bExtractThumb) { // seek to the start of the thumbnail image
        iFilePos = pPage->iThumbData;
        (*pPage->pfnSeek)(&pPage->JPEGFile, iFilePos);
    }
    iBytesRead = (*pPage->pfnRead)(&pPage->JPEGFile, s, JPEG_FILE_BUF_SIZE);
    if(iBytesRead < 256) { // a JPEG file this tiny? probably bad
        pPage->iError = JPEG_INVALID_FILE;
        return 0;
    }
    iFilePos += iBytesRead;
    if(MOTOSHORT(pPage->ucFileBuf) != 0xffd8) {
        pPage->iError = JPEG_INVALID_FILE;
        return 0; // not a JPEG file
    }
    iOffset = 2; /* Start at offset of first marker */
    usMarker = 0; /* Search for SOFx (start of frame) marker */
    while(usMarker != 0xffda && iOffset < pPage->JPEGFile.iSize) {
        if(iOffset >= JPEG_FILE_BUF_SIZE / 2) { // too close to the end, read more data
            // Do we need to seek first?
            if(iOffset >= JPEG_FILE_BUF_SIZE) {
                iFilePos += (iOffset - iBytesRead);
                iOffset = 0;
                (*pPage->pfnSeek)(&pPage->JPEGFile, iFilePos);
                iBytesRead = 0; // throw away any old data
            }
            // move existing bytes down
            if(iOffset) {
                lv_memcpy(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], iBytesRead - iOffset);
                iBytesRead -= iOffset;
                iOffset = 0;
            }
            i = (*pPage->pfnRead)(&pPage->JPEGFile, &pPage->ucFileBuf[iBytesRead], JPEG_FILE_BUF_SIZE - iBytesRead);
            iFilePos += i;
            iBytesRead += i;
        }
        usMarker = MOTOSHORT(&s[iOffset]);
        iOffset += 2;
        usLen = MOTOSHORT(&s[iOffset]); // marker length

        if(usMarker < 0xffc0 ||
           usMarker == 0xffff) { // invalid marker, could be generated by "Arles Image Web Page Creator" or Accusoft
            iOffset++;
            continue; // skip 1 byte and try to resync
        }
        switch(usMarker) {
            case 0xffc1:
            case 0xffc2:
            case 0xffc3:
                pPage->iError = JPEG_UNSUPPORTED_FEATURE;
                return 0; // currently unsupported modes

            case 0xffe1: // App1 (EXIF?)
                if(s[iOffset + 2] == 'E' && s[iOffset + 3] == 'x' && (s[iOffset + 8] == 'M' ||
                                                                      s[iOffset + 8] == 'I')) { // the EXIF data we want
                    int bMotorola, IFD, iTagCount;
                    pPage->iEXIF = iFilePos - iBytesRead + iOffset + 8; // start of TIFF file
                    // Get the orientation value (if present)
                    bMotorola = (s[iOffset + 8] == 'M');
                    IFD = TIFFLONG(&s[iOffset + 12], bMotorola);
                    iTagCount = TIFFSHORT(&s[iOffset + 16], bMotorola);
                    GetTIFFInfo(pPage, bMotorola, IFD + iOffset + 8);
                    // The second IFD defines the thumbnail (if present)
                    if(iTagCount >= 1 && iTagCount < 32) { // valid number of tags for EXIF data 'page'
                        // point to next IFD
                        IFD += (12 * iTagCount) + 2;
                        IFD = TIFFLONG(&s[IFD + iOffset + 8], bMotorola);
                        if(IFD != 0) { // Thumbnail present?
                            pPage->ucHasThumb = 1;
                            GetTIFFInfo(pPage, bMotorola, IFD + iOffset + 8); // info for second 'page' of TIFF
                            pPage->iThumbData += iOffset + 8; // absolute offset in the file
                        }
                    }
                }
                break;
            case 0xffc0: // SOFx - start of frame
                pPage->ucMode = (uint8_t)usMarker;
                pPage->ucBpp = s[iOffset + 2]; // bits per sample
                pPage->iCropX = pPage->iCropY = 0; // initialize crop rectangle to full image size
                pPage->iCropCY = pPage->iHeight = MOTOSHORT(&s[iOffset + 3]);
                pPage->iCropCX = pPage->iWidth = MOTOSHORT(&s[iOffset + 5]);
                pPage->ucNumComponents = s[iOffset + 7];
                pPage->ucBpp = pPage->ucBpp * pPage->ucNumComponents; /* Bpp = number of components * bits per sample */
                if(pPage->ucNumComponents == 1)
                    pPage->ucSubSample = 0; // use this to differentiate from color 1:1
                else {
                    usLen -= 8;
                    iOffset += 8;
                    //                    pPage->ucSubSample = s[iOffset+9]; // subsampling option for the second color component
                    for(i = 0; i < pPage->ucNumComponents; i++) {
                        uint8_t ucSamp;
                        pPage->JPCI[i].component_id = s[iOffset++];
                        pPage->JPCI[i].component_index = (unsigned char)i;
                        ucSamp = s[iOffset++]; // get the h+v sampling factor
                        if(i == 0)  // Y component?
                            pPage->ucSubSample = ucSamp;
                        //                        pPage->JPCI[i].h_samp_factor = ucSamp >> 4;
                        //                        pPage->JPCI[i].v_samp_factor = ucSamp & 0xf;
                        pPage->JPCI[i].quant_tbl_no = s[iOffset++]; // quantization table number
                        if(pPage->JPCI[i].quant_tbl_no > 3) {
                            pPage->iError = JPEG_DECODE_ERROR;
                            return 0; // error
                        }
                        usLen -= 3;
                    }
                }
                break;
            case 0xffdd: // Restart Interval
                if(usLen == 4)
                    pPage->iResInterval = MOTOSHORT(&s[iOffset + 2]);
                break;
            case 0xffc4: /* M_DHT */ // get Huffman tables
                iOffset += 2; // skip length
                usLen -= 2; // subtract length length
                if(JPEGGetHuffTables(&s[iOffset], usLen, pPage) != 0) { // bad tables?
                    pPage->iError = JPEG_DECODE_ERROR;
                    return 0; // error
                }
                break;
            case 0xffdb: /* M_DQT */
                /* Get the quantization tables */
                /* first byte has PPPPNNNN where P = precision and N = table number 0-3 */
                iOffset += 2; // skip length
                usLen -= 2; // subtract length length
                while(usLen > 0) {
                    ucTable = s[iOffset++]; // table number
                    if((ucTable & 0xf) > 3) { // invalid table number
                        pPage->iError = JPEG_DECODE_ERROR;
                        return 0;
                    }
                    iTableOffset = (ucTable & 0xf) * DCTSIZE;
                    if(ucTable & 0xf0) { // if word precision
                        for(i = 0; i < DCTSIZE; i++) {
                            pPage->sQuantTable[i + iTableOffset] = MOTOSHORT(&s[iOffset]);
                            iOffset += 2;
                        }
                        usLen -= (DCTSIZE * 2 + 1);
                    }
                    else { // byte precision
                        for(i = 0; i < DCTSIZE; i++) {
                            pPage->sQuantTable[i + iTableOffset] = (unsigned short)s[iOffset++];
                        }
                        usLen -= (DCTSIZE + 1);
                    }
                }
                break;
        } // switch on JPEG marker
        iOffset += usLen;
    } // while
    if(usMarker == 0xffda) { // start of image
        if(pPage->ucBpp != 8) { // need to match up table IDs
            iOffset -= usLen;
            JPEGGetSOS(pPage, &iOffset); // get Start-Of-Scan info for decoding
        }
        if(!JPEGMakeHuffTables(pPage, 0)) { //int bThumbnail) DEBUG
            pPage->iError = JPEG_UNSUPPORTED_FEATURE;
            return 0;
        }
        // Now the offset points to the start of compressed data
        i = JPEGFilter(&pPage->ucFileBuf[iOffset], pPage->ucFileBuf, iBytesRead - iOffset, &pPage->ucFF);
        pPage->iVLCOff = 0;
        pPage->iVLCSize = i;
        JPEGGetMoreData(pPage); // read more VLC data
        return 1;
    }
    pPage->iError = JPEG_DECODE_ERROR;
    return 0;
} /* JPEGParseInfo() */
//
// Fix and reorder the quantization table for faster decoding.*
//
static void JPEGFixQuantD(JPEGIMAGE * pJPEG)
{
    int iTable, iTableOffset;
    signed short sTemp[DCTSIZE];
    int i;
    uint16_t * p;

    for(iTable = 0; iTable < pJPEG->ucNumComponents; iTable++) {
        iTableOffset = iTable * DCTSIZE;
        p = (uint16_t *)&pJPEG->sQuantTable[iTableOffset];
        for(i = 0; i < DCTSIZE; i++)
            sTemp[i] = p[cZigZag[i]];
        lv_memcpy(&pJPEG->sQuantTable[iTableOffset], sTemp, DCTSIZE * sizeof(short)); // copy back to original spot

        // Prescale for DCT multiplication
        p = (uint16_t *)&pJPEG->sQuantTable[iTableOffset];
        for(i = 0; i < DCTSIZE; i++) {
            p[i] = (uint16_t)((p[i] * iScaleBits[i]) >> 12);
        }
    }
} /* JPEGFixQuantD() */
//
// Decode the DC and 2-63 AC coefficients of the current DCT block
// For 1/4 and 1/8 scaled images, we don't store most of the AC values since we
// won't use them. For skipped MCUs (outside crop area), we don't decode any AC values
//
static int JPEGDecodeMCU(JPEGIMAGE * pJPEG, int iMCU, int * iDCPredictor)
{
    my_ulong ulCode, ulTemp;
    uint8_t * pZig;
    signed char cCoeff;
    unsigned short * pFast;
    unsigned char ucHuff, * pucFast;
    uint32_t usHuff; // this prevents an unnecessary & 65535 for shorts
    uint32_t ulBitOff;
    my_ulong ulBits; // local copies to allow compiler to use register vars
    uint8_t * pBuf, * pEnd, * pEnd2;
    signed short * pMCU = &pJPEG->sMCUs[iMCU];
    uint16_t u16MCUFlags;

#define MIN_DCT_THRESHOLD 8

    ulBitOff = pJPEG->bb.ulBitOff;
    ulBits = pJPEG->bb.ulBits;
    pBuf = pJPEG->bb.pBuf;

    if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
        pBuf += (ulBitOff >> 3);
        ulBitOff &= 7;
        ulBits = MOTOLONG(pBuf);
    }
    if(iMCU < 0) {  // skip this block (cropped, or grayscale output from color)
        pEnd2 = (uint8_t *)&cZigZag2[1]; // we only capture the DC value
    }
    else if(pJPEG->iOptions & (JPEG_SCALE_QUARTER | JPEG_SCALE_EIGHTH)) {    // reduced size DCT
        pMCU[1] = pMCU[8] = pMCU[9] = 0;
        pEnd2 = (uint8_t *)&cZigZag2[5]; // we only need to store the 4 elements we care about
    }
    else {   // decode all the AC coefficients
        lv_memset(pMCU, 0, 64 * sizeof(short)); // pre-fill with zero since we may skip coefficients
        pEnd2 = (uint8_t *)&cZigZag2[64];
    }
    u16MCUFlags = 0;
    pZig = (unsigned char *)&cZigZag2[1];
    pEnd = (unsigned char *)&cZigZag2[64];

    // get the DC component
    pucFast = &pJPEG->ucHuffDC[pJPEG->ucDCTable * DC_TABLE_SIZE];
    ulCode = (ulBits >> (REGISTER_WIDTH - 12 - ulBitOff)) & 0xfff; // get as lower 12 bits
    if(ulCode >= 0xf80)  // it's a long code
        ulCode = (ulCode & 0xff); // point to long table and trim to 7-bits + 0x80 offset into long table
    else
        ulCode >>= 6; // it's a short code, use first 6 bits only
    ucHuff = pucFast[ulCode];
    cCoeff = (signed char)pucFast[ulCode + 512]; // get pre-calculated extra bits for "small" values
    if(ucHuff == 0)  // invalid code
        return -1;
    ulBitOff += (ucHuff >> 4); // add the Huffman length
    ucHuff &= 0xf; // get the actual code (SSSS)
    if(ucHuff) { // if there is a change to the DC value
        // get the 'extra' bits
        if(cCoeff) {
            (*iDCPredictor) += cCoeff;
        }
        else {
            if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = ulBits << ulBitOff;
            ulTemp = ~(my_ulong)(((my_long)ulCode) >> (REGISTER_WIDTH - 1)); // slide sign bit across other 63/31 bits
            ulCode >>= (REGISTER_WIDTH - ucHuff);
            ulCode -= ulTemp >> (REGISTER_WIDTH - ucHuff);
            ulBitOff += ucHuff; // add bit length
            (*iDCPredictor) += (int)ulCode;
        }
    }
    if(iMCU >= 0) {  // non-skipped block
        pMCU[0] = (short) * iDCPredictor; // store in MCU[0]
    }
    if(pJPEG->ucACTable > 1)  // unsupported
        return -1;
    // Now get the other 63 AC coefficients
    pFast = &pJPEG->usHuffAC[pJPEG->ucACTable * HUFF11SIZE];
    if(pJPEG->b11Bit) { // 11-bit "slow" tables used
        //            if (pJPEG->pHuffACFast == pJPEG->huffacFast[1]) // second table
        //                pFast = &pJPEG->ucAltHuff[0];
        while(pZig < pEnd) {
            if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = (ulBits >> (REGISTER_WIDTH - 16 - ulBitOff)) & 0xffff; // get as lower 16 bits
            if(ulCode >= 0xf000)  // first 4 bits = 1, use long table
                ulCode = (ulCode & 0x1fff);
            else
                ulCode >>= 4; // use lower 12 bits (short table)
            usHuff = pFast[ulCode];
            if(usHuff == 0)  // invalid code
                return -1;
            ulBitOff += (usHuff >> 8); // add length
            usHuff &= 0xff; // get code (RRRR/SSSS)
            if(usHuff == 0) { // no more AC components
                goto mcu_done;
            }
            pZig += (usHuff >> 4);  // get the skip amount (RRRR)
            usHuff &= 0xf; // get (SSSS) - extra length
            if(pZig < pEnd2 && usHuff) {
                ulCode = ulBits << ulBitOff;
                ulTemp = ~(my_ulong)(((my_long) ulCode) >> (REGISTER_WIDTH - 1)); // slide sign bit across other 63 bits
                ulCode >>= (REGISTER_WIDTH - usHuff);
                ulCode -= ulTemp >> (REGISTER_WIDTH - usHuff);
                u16MCUFlags |= 1 << (*pZig & 7); // keep track of occupied columns
                u16MCUFlags |= *pZig << 8; // for testing occupied rows
                pMCU[*pZig] = (signed short)ulCode; // store AC coefficient (already reordered)
            }
            ulBitOff += usHuff; // add (SSSS) extra length
            pZig++;
            if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
        } // while
    }
    else { // 10-bit "fast" tables used
        while(pZig < pEnd) {
            if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = (ulBits >> (REGISTER_WIDTH - 16 - ulBitOff)) & 0xffff; // get as lower 16 bits
            if(ulCode >= 0xfc00)  // first 6 bits = 1, use long table
                ulCode = (ulCode & 0x7ff); // (ulCode & 0x3ff) + 0x400;
            else
                ulCode >>= 6; // use lower 10 bits (short table)
            usHuff = pFast[ulCode];
            if(usHuff == 0)  // invalid code
                return -1;
            ulBitOff += (usHuff >> 8); // add length
            usHuff &= 0xff; // get code (RRRR/SSSS)
            if(usHuff == 0) { // no more AC components
                goto mcu_done;
            }
            pZig += (usHuff >> 4);  // get the skip amount (RRRR)
            usHuff &= 0xf; // get (SSSS) - extra length
            if(pZig < pEnd2 && usHuff) {
                ulCode = ulBits << ulBitOff;
                ulTemp = ~(my_ulong)(((my_long) ulCode) >> (REGISTER_WIDTH - 1)); // slide sign bit across other 63 bits
                ulCode >>= (REGISTER_WIDTH - usHuff);
                ulCode -= ulTemp >> (REGISTER_WIDTH - usHuff);
                u16MCUFlags |= 1 << (*pZig & 7); // keep track of occupied columns
                u16MCUFlags |= *pZig << 8; // for testing occupied rows
                pMCU[*pZig] = (signed short)ulCode; // store AC coefficient (already reordered)
            }
            ulBitOff += usHuff; // add (SSSS) extra length
            pZig++;
            if(ulBitOff > (REGISTER_WIDTH - 17)) { // need to get more data
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
        } // while
    } // 10-bit tables
mcu_done:
    pJPEG->bb.pBuf = pBuf;
    pJPEG->iVLCOff = (int)(pBuf - pJPEG->ucFileBuf);
    pJPEG->bb.ulBitOff = ulBitOff;
    pJPEG->bb.ulBits = ulBits;
    pJPEG->u16MCUFlags = u16MCUFlags;
    return 0;
} /* JPEGDecodeMCU() */
//
// Inverse DCT
//
static void JPEGIDCT(JPEGIMAGE * pJPEG, int iMCUOffset, int iQuantTable)
{
    int iRow;
#if !defined (HAS_SSE) && !defined(HAS_NEON)
    int iCol;
#endif
    signed int tmp6, tmp7, tmp10, tmp11, tmp12, tmp13;
    signed int z5, z10, z11, z12, z13;
    signed int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
    signed short * pQuant;
    unsigned char * pOutput;
    uint16_t u16MCUFlags;
    int16_t * pMCUSrc = &pJPEG->sMCUs[iMCUOffset];
#ifdef HAS_SSE
    __m128i mmxRow0, mmxRow1, mmxRow2, mmxRow3, mmxRow4, mmxRow5, mmxRow6, mmxRow7;
    __m128i mmxTemp, mmxTemp0, mmxTemp1, mmxTemp2, mmxTemp3, mmxTemp4, mmxTemp5, mmxTemp6, mmxTemp7, mmxTemp10, mmxTemp11,
            mmxTemp12, mmxTemp13;
    __m128i mmxZ5, mmxZ10, mmxZ11, mmxZ12, mmxZ13;
#endif // HAS_SSE
#ifdef HAS_NEON
    int16x8_t mmxRow0, mmxRow1, mmxRow2, mmxRow3, mmxRow4, mmxRow5, mmxRow6, mmxRow7;
    int16x8_t mmxTemp, mmxTemp0, mmxTemp1, mmxTemp2, mmxTemp3, mmxTemp4, mmxTemp5, mmxTemp6, mmxTemp7, mmxTemp10, mmxTemp11,
              mmxTemp12, mmxTemp13;
    int16x8_t mmxZ5, mmxZ10, mmxZ11, mmxZ12, mmxZ13;
#endif // HAS_NEON

    u16MCUFlags = pJPEG->u16MCUFlags;

    // my shortcut method appears to violate patent 20020080052
    // but the patent is invalidated by prior art:
    // http://netilium.org/~mad/dtj/DTJ/DTJK04/
    pQuant = &pJPEG->sQuantTable[iQuantTable * DCTSIZE];
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER) { // special case
        /* Column 0 */
        tmp4 = pMCUSrc[0] * pQuant[0];
        tmp5 = pMCUSrc[8] * pQuant[8];
        tmp0 = tmp4 + tmp5;
        tmp2 = tmp4 - tmp5;
        /* Column 1 */
        tmp4 = pMCUSrc[1] * pQuant[1];
        tmp5 = pMCUSrc[9] * pQuant[9];
        tmp1 = tmp4 + tmp5;
        tmp3 = tmp4 - tmp5;
        /* Pass 2: process 2 rows, store into output array. */
        /* Row 0 */
        pOutput = (unsigned char *)pMCUSrc; // store output pixels back into MCU
        pOutput[0] = ucRangeTable[(((tmp0 + tmp1) >> 5) & 0x3ff)];
        pOutput[1] = ucRangeTable[(((tmp0 - tmp1) >> 5) & 0x3ff)];
        /* Row 1 */
        pOutput[2] = ucRangeTable[(((tmp2 + tmp3) >> 5) & 0x3ff)];
        pOutput[3] = ucRangeTable[(((tmp2 - tmp3) >> 5) & 0x3ff)];
        return;
    }
#ifdef HAS_SSE // SSE2 version
    // Columns first
    // even part
    if((u16MCUFlags & 0x2000) == 0) { // rows 4-7 are not populated, simpler calculations
        // even part
        mmxTemp10 = _mm_loadu_si128((__m128i *)&pMCUSrc[0]); // row 0
        mmxTemp1 = _mm_loadu_si128((__m128i *)&pMCUSrc[16]); // row 2
        mmxTemp = _mm_loadu_si128((__m128i *)&pQuant[0]);
        mmxTemp2 = _mm_loadu_si128((__m128i *)&pQuant[16]);
        mmxTemp10 = _mm_mullo_epi16(mmxTemp10, mmxTemp); // dequant row 0
        mmxTemp1 = _mm_mullo_epi16(mmxTemp1, mmxTemp2); // dequant row 2
        mmxTemp = _mm_loadu_si128((__m128i *)&s0414[0]); // 0.414
        mmxTemp12 = _mm_mulhi_epi16(_mm_slli_epi16(mmxTemp1, 2), mmxTemp); // tmp12 = ((tmp1*106)>>8)
        mmxTemp0 = _mm_add_epi16(mmxTemp10, mmxTemp1); // 0+2
        mmxTemp3 = _mm_sub_epi16(mmxTemp10, mmxTemp1); // 0-2
        mmxTemp1 = _mm_add_epi16(mmxTemp10, mmxTemp12); // 10+12
        mmxTemp2 = _mm_sub_epi16(mmxTemp10, mmxTemp12); // 10-12
        // odd part
        mmxTemp4 = _mm_loadu_si128((__m128i *)&pMCUSrc[8]); // row 1
        mmxTemp5 = _mm_loadu_si128((__m128i *)&pMCUSrc[24]); // row 3
        mmxTemp = _mm_loadu_si128((__m128i *)&pQuant[8]);
        mmxTemp11 = _mm_loadu_si128((__m128i *)&pQuant[24]);
        mmxTemp4 = _mm_mullo_epi16(mmxTemp4, mmxTemp); // dequant row 1
        mmxTemp5 = _mm_mullo_epi16(mmxTemp5, mmxTemp11); // dequant row 3
        mmxTemp7 = _mm_add_epi16(mmxTemp4, mmxTemp5); // tmp7 = tmp4 + tmp5
        mmxTemp = _mm_loadu_si128((__m128i *)&s1414[0]); // load 1.414213562 constant
        mmxTemp11 = _mm_mulhi_epi16(_mm_slli_epi16(_mm_sub_epi16(mmxTemp4, mmxTemp5), 2),
                                    mmxTemp); // tmp11 = (((tmp4-tmp5)*362)>>8)
        mmxTemp = _mm_loadu_si128((__m128i *)&s1847[0]); // 1.8477
        mmxZ5 = _mm_mulhi_epi16(_mm_slli_epi16(_mm_sub_epi16(mmxTemp4, mmxTemp5), 2), mmxTemp); // z5 = (((tmp4-tmp5)*473)>>8)
        mmxTemp = _mm_loadu_si128((__m128i *)&sp2613[0]); // positive 2.6131259
        mmxTemp12 = _mm_mulhi_epi16(_mm_slli_epi16(mmxTemp5, 2), mmxTemp); // tmp12 = ((-tmp5 * -669)>>8) + z5
        // can't make that constant without overflowing, so double it after
        mmxTemp12 = _mm_add_epi16(mmxTemp12, mmxTemp12);
        mmxTemp12 = _mm_add_epi16(mmxTemp12, mmxZ5);
        mmxTemp6 = _mm_sub_epi16(mmxTemp12, mmxTemp7); // tmp6 = tmp12 - tmp7
        mmxTemp5 = _mm_sub_epi16(mmxTemp11, mmxTemp6); // tmp5 = tmp11 - tmp6
        mmxTemp = _mm_loadu_si128((__m128i *)&s1082[0]); // 1.08239
        mmxTemp10 = _mm_sub_epi16(_mm_mulhi_epi16(_mm_slli_epi16(mmxTemp4, 2), mmxTemp),
                                  mmxZ5); // tmp10 = ((tmp4 * 277)>>8) - z5
        mmxTemp4 = _mm_add_epi16(mmxTemp10, mmxTemp5); // tmp4 = tmp10 + tmp5
    }
    else { // need to do full calculation
        // even part
        mmxTemp0 = _mm_loadu_si128((__m128i *)&pMCUSrc[0]); // get row 0
        mmxTemp2 = _mm_loadu_si128((__m128i *)&pMCUSrc[32]); // get row 4
        mmxTemp10 = _mm_loadu_si128((__m128i *)&pQuant[0]);
        mmxTemp11 = _mm_loadu_si128((__m128i *)&pQuant[32]);
        mmxTemp0 = _mm_mullo_epi16(mmxTemp0, mmxTemp10); // dequant row 0
        mmxTemp2 = _mm_mullo_epi16(mmxTemp2, mmxTemp11); // dequant row 4
        mmxTemp10 = _mm_add_epi16(mmxTemp0, mmxTemp2); // 0+4
        mmxTemp11 = _mm_sub_epi16(mmxTemp0, mmxTemp2); // 0-4
        mmxTemp1 = _mm_loadu_si128((__m128i *)&pMCUSrc[16]); // get row 2
        mmxTemp3 = _mm_loadu_si128((__m128i *)&pMCUSrc[48]); // get row 6
        mmxTemp = _mm_loadu_si128((__m128i *)&pQuant[16]);
        mmxTemp12 = _mm_loadu_si128((__m128i *)&pQuant[48]);
        mmxTemp1 = _mm_mullo_epi16(mmxTemp1, mmxTemp); // dequant row 2
        mmxTemp3 = _mm_mullo_epi16(mmxTemp3, mmxTemp12); // dequant row 6
        mmxTemp13 = _mm_add_epi16(mmxTemp1, mmxTemp3); // 1+3
        mmxTemp = _mm_loadu_si128((__m128i *)&s1414[0]); // load 1.414213562 constant
        mmxTemp12 = _mm_sub_epi16(_mm_mulhi_epi16(_mm_slli_epi16(_mm_sub_epi16(mmxTemp1, mmxTemp3), 2), mmxTemp),
                                  mmxTemp13); // tmp12 = (((tmp1 - tmp3) * 1.414) - tmp13;
        mmxTemp0 = _mm_add_epi16(mmxTemp10, mmxTemp13); // tmp0 = tmp10 + tmp13
        mmxTemp3 = _mm_sub_epi16(mmxTemp10, mmxTemp13); // tmp3 = tmp10 - tmp13
        mmxTemp1 = _mm_add_epi16(mmxTemp11, mmxTemp12); // tmp1 = tmp11 + tmp12
        mmxTemp2 = _mm_sub_epi16(mmxTemp11, mmxTemp12); // tmp2 = tmp11 - tmp12
        // odd part
        mmxTemp5 = _mm_loadu_si128((__m128i *)&pMCUSrc[24]); // get row 3
        mmxTemp6 = _mm_loadu_si128((__m128i *)&pMCUSrc[40]); // get row 5
        mmxTemp10 = _mm_loadu_si128((__m128i *)&pQuant[24]);
        mmxTemp11 = _mm_loadu_si128((__m128i *)&pQuant[40]);
        mmxTemp5 = _mm_mullo_epi16(mmxTemp5, mmxTemp10); // dequant row 3
        mmxTemp6 = _mm_mullo_epi16(mmxTemp6, mmxTemp11); // dequant row 5
        mmxZ13 = _mm_add_epi16(mmxTemp6, mmxTemp5); // z13 = tmp6 + tmp5;
        mmxZ10 = _mm_sub_epi16(mmxTemp6, mmxTemp5); // z10 = tmp6 - tmp5;
        mmxTemp4 = _mm_loadu_si128((__m128i *)&pMCUSrc[8]); // get row 1
        mmxTemp7 = _mm_loadu_si128((__m128i *)&pMCUSrc[56]); // get row 7
        mmxTemp10 = _mm_loadu_si128((__m128i *)&pQuant[8]);
        mmxTemp11 = _mm_loadu_si128((__m128i *)&pQuant[56]);
        mmxTemp4 = _mm_mullo_epi16(mmxTemp4, mmxTemp10); // dequant row 1
        mmxTemp7 = _mm_mullo_epi16(mmxTemp7, mmxTemp11); // dequant row 7
        mmxZ11 = _mm_add_epi16(mmxTemp4, mmxTemp7); // z11 = tmp4 + tmp7;
        mmxZ12 = _mm_sub_epi16(mmxTemp4, mmxTemp7); // z12 = tmp4 - tmp7;
        mmxTemp7 = _mm_add_epi16(mmxZ11, mmxZ13); // tmp7 = z11 + z13;
        mmxTemp11 = _mm_mulhi_epi16(_mm_slli_epi16(_mm_sub_epi16(mmxZ11, mmxZ13), 2),
                                    mmxTemp); // tmp11 = ((z11 - z13) * 1.1414);
        mmxTemp = _mm_loadu_si128((__m128i *)&s1847[0]); // 1.8477
        mmxZ5 = _mm_mulhi_epi16(_mm_slli_epi16(_mm_add_epi16(mmxZ10, mmxZ12), 2), mmxTemp); // z5 = ((z10+z12)*1.8477);
        mmxTemp = _mm_loadu_si128((__m128i *)&s2613[0]); // -2.6131259
        mmxTemp12 = _mm_mulhi_epi16(_mm_slli_epi16(mmxZ10, 2), mmxTemp); // tmp12 = (z10 * -2.6131259) + z5;
        // can't make that constant without overflowing, so double it after
        mmxTemp12 = _mm_add_epi16(mmxTemp12, mmxTemp12);
        mmxTemp12 = _mm_add_epi16(mmxTemp12, mmxZ5);
        mmxTemp = _mm_loadu_si128((__m128i *)&s1082[0]); // 1.08239
        mmxTemp6 = _mm_sub_epi16(mmxTemp12, mmxTemp7); // tmp6 = tmp12 - tmp7
        mmxTemp5 = _mm_sub_epi16(mmxTemp11, mmxTemp6); // tmp5 = tmp11 - tmp6
        mmxTemp10 = _mm_sub_epi16(_mm_mulhi_epi16(_mm_slli_epi16(mmxZ12, 2), mmxTemp), mmxZ5); // tmp10 = (z12 * 1.08239) - z5;
        mmxTemp4 = _mm_add_epi16(mmxTemp10, mmxTemp5); // tmp4 = tmp10 + tmp5;
    }
    mmxRow0 = _mm_add_epi16(mmxTemp0, mmxTemp7); // row 0
    _mm_storeu_si128((__m128i *)&pMCUSrc[0], mmxRow0);
    mmxRow1 = _mm_add_epi16(mmxTemp1, mmxTemp6); // row 1
    _mm_storeu_si128((__m128i *)&pMCUSrc[8], mmxRow1);
    mmxRow2 = _mm_add_epi16(mmxTemp2, mmxTemp5); // row 2
    _mm_storeu_si128((__m128i *)&pMCUSrc[16], mmxRow2);
    mmxRow3 = _mm_sub_epi16(mmxTemp3, mmxTemp4); // row 3
    _mm_storeu_si128((__m128i *)&pMCUSrc[24], mmxRow3);
    mmxRow4 = _mm_add_epi16(mmxTemp3, mmxTemp4); // row 4
    _mm_storeu_si128((__m128i *)&pMCUSrc[32], mmxRow4);
    mmxRow5 = _mm_sub_epi16(mmxTemp2, mmxTemp5); // row 5
    _mm_storeu_si128((__m128i *)&pMCUSrc[40], mmxRow5);
    mmxRow6 = _mm_sub_epi16(mmxTemp1, mmxTemp6); // row 6
    _mm_storeu_si128((__m128i *)&pMCUSrc[48], mmxRow6);
    mmxRow7 = _mm_sub_epi16(mmxTemp0, mmxTemp7); // row 7
    _mm_storeu_si128((__m128i *)&pMCUSrc[56], mmxRow7);
#endif // HAS_SSE
#ifdef HAS_NEON
    if((u16MCUFlags & 0x2000) == 0) { // rows 4-7 are not populated, simpler calculations
        // even part
        mmxTemp10 = vld1q_s16(&pMCUSrc[0]); // row 0
        mmxTemp1 = vld1q_s16(&pMCUSrc[16]); // row 2
        mmxTemp = vld1q_s16(&pQuant[0]);
        mmxTemp2 = vld1q_s16(&pQuant[16]);
        mmxTemp10 = vmulq_s16(mmxTemp10, mmxTemp); // dequant row 0
        mmxTemp1 = vmulq_s16(mmxTemp1, mmxTemp2); // dequant row 2
        mmxTemp = vld1q_s16(&s0414[0]); // 0.414
        mmxTemp12 = vqdmulhq_s16(vshlq_n_s16(mmxTemp1, 2), mmxTemp); // tmp12 = ((tmp1*106)>>8)
        mmxTemp0 = vaddq_s16(mmxTemp10, mmxTemp1); // 0+2
        mmxTemp3 = vsubq_s16(mmxTemp10, mmxTemp1); // 0-2
        mmxTemp1 = vaddq_s16(mmxTemp10, mmxTemp12); // 10+12
        mmxTemp2 = vsubq_s16(mmxTemp10, mmxTemp12); // 10-12
        // odd part
        mmxTemp4 = vld1q_s16(&pMCUSrc[8]); // row 1
        mmxTemp5 = vld1q_s16(&pMCUSrc[24]); // row 3
        mmxTemp = vld1q_s16(&pQuant[8]);
        mmxTemp11 = vld1q_s16(&pQuant[24]);
        mmxTemp4 = vmulq_s16(mmxTemp4, mmxTemp); // dequant row 1
        mmxTemp5 = vmulq_s16(mmxTemp5, mmxTemp11); // dequant row 3
        mmxTemp7 = vaddq_s16(mmxTemp4, mmxTemp5); // tmp7 = tmp4 + tmp5
        mmxTemp = vld1q_s16(&s1414[0]); // load 1.414213562 constant
        mmxTemp11 = vqdmulhq_s16(vshlq_n_s16(vsubq_s16(mmxTemp4, mmxTemp5), 2), mmxTemp); // tmp11 = (((tmp4-tmp5)*362)>>8)
        mmxTemp = vld1q_s16(&s1847[0]); // 1.8477
        mmxZ5 = vqdmulhq_s16(vshlq_n_s16(vsubq_s16(mmxTemp4, mmxTemp5), 2), mmxTemp); // z5 = (((tmp4-tmp5)*473)>>8)
        mmxTemp = vld1q_s16(&sp2613[0]); // positive 2.6131259
        mmxTemp12 = vqdmulhq_s16(vshlq_n_s16(mmxTemp5, 2), mmxTemp); // tmp12 = ((-tmp5 * -669)>>8) + z5
        // can't make that constant without overflowing, so double it after
        mmxTemp12 = vaddq_s16(mmxTemp12, mmxTemp12);
        mmxTemp12 = vaddq_s16(mmxTemp12, mmxZ5);
        mmxTemp6 = vsubq_s16(mmxTemp12, mmxTemp7); // tmp6 = tmp12 - tmp7
        mmxTemp5 = vsubq_s16(mmxTemp11, mmxTemp6); // tmp5 = tmp11 - tmp6
        mmxTemp = vld1q_s16(&s1082[0]); // 1.08239
        mmxTemp10 = vsubq_s16(vqdmulhq_s16(vshlq_n_s16(mmxTemp4, 2), mmxTemp), mmxZ5); // tmp10 = ((tmp4 * 277)>>8) - z5
        mmxTemp4 = vaddq_s16(mmxTemp10, mmxTemp5); // tmp4 = tmp10 + tmp5
    }
    else { // need to do full calculation
        // even part
        mmxTemp0 = vld1q_s16(&pMCUSrc[0]); // get row 0
        mmxTemp2 = vld1q_s16(&pMCUSrc[32]); // get row 4
        mmxTemp10 = vld1q_s16(&pQuant[0]);
        mmxTemp11 = vld1q_s16(&pQuant[32]);
        mmxTemp0 = vmulq_s16(mmxTemp0, mmxTemp10); // dequant row 0
        mmxTemp2 = vmulq_s16(mmxTemp2, mmxTemp11); // dequant row 4
        mmxTemp10 = vaddq_s16(mmxTemp0, mmxTemp2); // 0+4
        mmxTemp11 = vsubq_s16(mmxTemp0, mmxTemp2); // 0-4
        mmxTemp1 = vld1q_s16(&pMCUSrc[16]); // get row 2
        mmxTemp3 = vld1q_s16(&pMCUSrc[48]); // get row 6
        mmxTemp = vld1q_s16(&pQuant[16]);
        mmxTemp12 = vld1q_s16(&pQuant[48]);
        mmxTemp1 = vmulq_s16(mmxTemp1, mmxTemp); // dequant row 2
        mmxTemp3 = vmulq_s16(mmxTemp3, mmxTemp12); // dequant row 6
        mmxTemp13 = vaddq_s16(mmxTemp1, mmxTemp3); // 1+3
        mmxTemp = vld1q_s16(&s1414[0]); // load 1.414213562 constant
        mmxTemp12 = vsubq_s16(vqdmulhq_s16(vshlq_n_s16(vsubq_s16(mmxTemp1, mmxTemp3), 2), mmxTemp),
                              mmxTemp13); // tmp12 = (((tmp1 - tmp3) * 1.414) - tmp13;
        mmxTemp0 = vaddq_s16(mmxTemp10, mmxTemp13); // tmp0 = tmp10 + tmp13
        mmxTemp3 = vsubq_s16(mmxTemp10, mmxTemp13); // tmp3 = tmp10 - tmp13
        mmxTemp1 = vaddq_s16(mmxTemp11, mmxTemp12); // tmp1 = tmp11 + tmp12
        mmxTemp2 = vsubq_s16(mmxTemp11, mmxTemp12); // tmp2 = tmp11 - tmp12
        // odd part
        mmxTemp5 = vld1q_s16(&pMCUSrc[24]); // get row 3
        mmxTemp6 = vld1q_s16(&pMCUSrc[40]); // get row 5
        mmxTemp10 = vld1q_s16(&pQuant[24]);
        mmxTemp11 = vld1q_s16(&pQuant[40]);
        mmxTemp5 = vmulq_s16(mmxTemp5, mmxTemp10); // dequant row 3
        mmxTemp6 = vmulq_s16(mmxTemp6, mmxTemp11); // dequant row 5
        mmxZ13 = vaddq_s16(mmxTemp6, mmxTemp5); // z13 = tmp6 + tmp5;
        mmxZ10 = vsubq_s16(mmxTemp6, mmxTemp5); // z10 = tmp6 - tmp5;
        mmxTemp4 = vld1q_s16(&pMCUSrc[8]); // get row 1
        mmxTemp7 = vld1q_s16(&pMCUSrc[56]); // get row 7
        mmxTemp10 = vld1q_s16(&pQuant[8]);
        mmxTemp11 = vld1q_s16(&pQuant[56]);
        mmxTemp4 = vmulq_s16(mmxTemp4, mmxTemp10); // dequant row 1
        mmxTemp7 = vmulq_s16(mmxTemp7, mmxTemp11); // dequant row 7
        mmxZ11 = vaddq_s16(mmxTemp4, mmxTemp7); // z11 = tmp4 + tmp7;
        mmxZ12 = vsubq_s16(mmxTemp4, mmxTemp7); // z12 = tmp4 - tmp7;
        mmxTemp7 = vaddq_s16(mmxZ11, mmxZ13); // tmp7 = z11 + z13;
        mmxTemp11 = vqdmulhq_s16(vshlq_n_s16(vsubq_s16(mmxZ11, mmxZ13), 2), mmxTemp); // tmp11 = ((z11 - z13) * 1.1414);
        mmxTemp = vld1q_s16(&s1847[0]); // 1.8477
        mmxZ5 = vqdmulhq_s16(vshlq_n_s16(vaddq_s16(mmxZ10, mmxZ12), 2), mmxTemp); // z5 = ((z10+z12)*1.8477);
        mmxTemp = vld1q_s16(&s2613[0]); // -2.6131259
        mmxTemp12 = vqdmulhq_s16(vshlq_n_s16(mmxZ10, 2), mmxTemp); // tmp12 = (z10 * -2.6131259) + z5;
        // can't make that constant without overflowing, so double it after
        mmxTemp12 = vaddq_s16(mmxTemp12, mmxTemp12);
        mmxTemp12 = vaddq_s16(mmxTemp12, mmxZ5);
        mmxTemp = vld1q_s16(&s1082[0]); // 1.08239
        mmxTemp6 = vsubq_s16(mmxTemp12, mmxTemp7); // tmp6 = tmp12 - tmp7
        mmxTemp5 = vsubq_s16(mmxTemp11, mmxTemp6); // tmp5 = tmp11 - tmp6
        mmxTemp10 = vsubq_s16(vqdmulhq_s16(vshlq_n_s16(mmxZ12, 2), mmxTemp), mmxZ5); // tmp10 = (z12 * 1.08239) - z5;
        mmxTemp4 = vaddq_s16(mmxTemp10, mmxTemp5); // tmp4 = tmp10 + tmp5;
    }
    mmxRow0 = vaddq_s16(mmxTemp0, mmxTemp7); // row 0
    vst1q_s16(&pMCUSrc[0], mmxRow0);
    mmxRow1 = vaddq_s16(mmxTemp1, mmxTemp6); // row 1
    vst1q_s16(&pMCUSrc[8], mmxRow1);
    mmxRow2 = vaddq_s16(mmxTemp2, mmxTemp5); // row 2
    vst1q_s16(&pMCUSrc[16], mmxRow2);
    mmxRow3 = vsubq_s16(mmxTemp3, mmxTemp4); // row 3
    vst1q_s16(&pMCUSrc[24], mmxRow3);
    mmxRow4 = vaddq_s16(mmxTemp3, mmxTemp4); // row 4
    vst1q_s16(&pMCUSrc[32], mmxRow4);
    mmxRow5 = vsubq_s16(mmxTemp2, mmxTemp5); // row 5
    vst1q_s16(&pMCUSrc[40], mmxRow5);
    mmxRow6 = vsubq_s16(mmxTemp1, mmxTemp6); // row 6
    vst1q_s16(&pMCUSrc[48], mmxRow6);
    mmxRow7 = vsubq_s16(mmxTemp0, mmxTemp7); // row 7
    vst1q_s16(&pMCUSrc[56], mmxRow7);
#endif // HAS_NEON
#if !defined (HAS_SSE) && !defined(HAS_NEON)
    // do columns first
    u16MCUFlags |= 1; // column 0 must always be calculated
    for(iCol = 0; iCol < 8 && u16MCUFlags; iCol++) {
        if(u16MCUFlags & (1 << iCol)) { // column has data in it
            u16MCUFlags &= ~(1 << iCol); // unmark the col after done
            if((u16MCUFlags & 0x2000) == 0) { // simpler calculations if only half populated
                // even part
                tmp10 = pMCUSrc[iCol] * pQuant[iCol];
                tmp1 = pMCUSrc[iCol + 16] * pQuant[iCol + 16]; // get 2nd row
                tmp12 = ((tmp1 * 106) >> 8); // used to be 362 - 1 (256)
                tmp0 = tmp10 + tmp1;
                tmp3 = tmp10 - tmp1;
                tmp1 = tmp10 + tmp12;
                tmp2 = tmp10 - tmp12;
                // odd part
                tmp4 = pMCUSrc[iCol + 8] * pQuant[iCol + 8]; // get 1st row
                tmp5 = pMCUSrc[iCol + 24];
                if(tmp5) { // this value is usually 0
                    tmp5 *= pQuant[iCol + 24]; // get 3rd row
                    tmp7 = tmp4 + tmp5;
                    tmp11 = (((tmp4 - tmp5) * 362) >> 8);  // 362>>8 = 1.414213562
                    z5 = (((tmp4 - tmp5) * 473) >> 8); // 473>>8 = 1.8477
                    tmp12 = ((-tmp5 * -669) >> 8) + z5; // -669>>8 = -2.6131259
                    tmp6 = tmp12 - tmp7;
                    tmp5 = tmp11 - tmp6;
                    tmp10 = ((tmp4 * 277) >> 8) - z5; // 277>>8 = 1.08239
                    tmp4 = tmp10 + tmp5;
                }
                else { // simpler case when we only have 1 odd row to calculate
                    tmp7 = tmp4;
                    tmp5 = (145 * tmp4) >> 8;
                    tmp6 = (217 * tmp4) >> 8;
                    tmp4 = (-51 * tmp4) >> 8;
                }
                pMCUSrc[iCol] = (short)(tmp0 + tmp7);    // row0
                pMCUSrc[iCol + 8] = (short)(tmp1 + tmp6); // row 1
                pMCUSrc[iCol + 16] = (short)(tmp2 + tmp5); // row 2
                pMCUSrc[iCol + 24] = (short)(tmp3 - tmp4); // row 3
                pMCUSrc[iCol + 32] = (short)(tmp3 + tmp4); // row 4
                pMCUSrc[iCol + 40] = (short)(tmp2 - tmp5); // row 5
                pMCUSrc[iCol + 48] = (short)(tmp1 - tmp6); // row 6
                pMCUSrc[iCol + 56] = (short)(tmp0 - tmp7); // row 7
            }
            else { // need to do full column calculation
                // even part
                tmp0 = pMCUSrc[iCol] * pQuant[iCol];
                tmp2 = pMCUSrc[iCol + 32]; // get 4th row
                if(tmp2) { // 4th row is most likely 0
                    tmp2 = tmp2 * pQuant[iCol + 32];
                    tmp10 = tmp0 + tmp2;
                    tmp11 = tmp0 - tmp2;
                }
                else {
                    tmp10 = tmp11 = tmp0;
                }
                tmp1 = pMCUSrc[iCol + 16] * pQuant[iCol + 16]; // get 2nd row
                tmp3 = pMCUSrc[iCol + 48]; // get 6th row
                if(tmp3) { // 6th row is most likely 0
                    tmp3 = tmp3 * pQuant[iCol + 48];
                    tmp13 = tmp1 + tmp3;
                    tmp12 = (((tmp1 - tmp3) * 362) >> 8) - tmp13;  // 362>>8 = 1.414213562
                }
                else {
                    tmp13 = tmp1;
                    tmp12 = ((tmp1 * 362) >> 8) - tmp1;
                }
                tmp0 = tmp10 + tmp13;
                tmp3 = tmp10 - tmp13;
                tmp1 = tmp11 + tmp12;
                tmp2 = tmp11 - tmp12;
                // odd part
                tmp5 = pMCUSrc[iCol + 24] * pQuant[iCol + 24]; // get 3rd row
                tmp6 = pMCUSrc[iCol + 40]; // get 5th row
                if(tmp6) { // very likely that row 5 = 0
                    tmp6 = tmp6 * pQuant[iCol + 40];
                    z13 = tmp6 + tmp5;
                    z10 = tmp6 - tmp5;
                }
                else {
                    z13 = tmp5;
                    z10 = -tmp5;
                }
                tmp4 = pMCUSrc[iCol + 8] * pQuant[iCol + 8]; // get 1st row
                tmp7 = pMCUSrc[iCol + 56]; // get 7th row
                if(tmp7) { // very likely that row 7 = 0
                    tmp7 = tmp7 * pQuant[iCol + 56];
                    z11 = tmp4 + tmp7;
                    z12 = tmp4 - tmp7;
                }
                else {
                    z11 = z12 = tmp4;
                }
                tmp7 = z11 + z13;
                tmp11 = (((z11 - z13) * 362) >> 8);  // 362>>8 = 1.414213562
                z5 = (((z10 + z12) * 473) >> 8);  // 473>>8 = 1.8477
                tmp12 = ((z10 * -669) >> 8) + z5; // -669>>8 = -2.6131259
                tmp6 = tmp12 - tmp7;
                tmp5 = tmp11 - tmp6;
                tmp10 = ((z12 * 277) >> 8) - z5; // 277>>8 = 1.08239
                tmp4 = tmp10 + tmp5;
                pMCUSrc[iCol] = (short)(tmp0 + tmp7);    // row0
                pMCUSrc[iCol + 8] = (short)(tmp1 + tmp6); // row 1
                pMCUSrc[iCol + 16] = (short)(tmp2 + tmp5); // row 2
                pMCUSrc[iCol + 24] = (short)(tmp3 - tmp4); // row 3
                pMCUSrc[iCol + 32] = (short)(tmp3 + tmp4); // row 4
                pMCUSrc[iCol + 40] = (short)(tmp2 - tmp5); // row 5
                pMCUSrc[iCol + 48] = (short)(tmp1 - tmp6); // row 6
                pMCUSrc[iCol + 56] = (short)(tmp0 - tmp7); // row 7
            } // full calculation needed
        } // if column has data in it
    } // for each column
#endif // NO SIMD
    // now do rows
    u16MCUFlags = pJPEG->u16MCUFlags;
    pOutput = (unsigned char *)pMCUSrc; // store output pixels back into MCU
    for(iRow = 0; iRow < 64; iRow += 8) { // all rows must be calculated
        // even part
        if((u16MCUFlags & 0xf0) == 0) { // quick and dirty calculation (right 4 columns are all 0's)
            if((u16MCUFlags & 0xfc) == 0) { // very likely case (1 or 2 columns occupied)
                // even part
                tmp0 = tmp1 = tmp2 = tmp3 = pMCUSrc[iRow + 0];
                // odd part
                tmp7 = pMCUSrc[iRow + 1];
                tmp6 = (tmp7 * 217) >> 8; // * 0.8477
                tmp5 = (tmp7 * 145) >> 8; // * 0.5663
                tmp4 = -((tmp7 * 51) >> 8); // * -0.199
            }
            else {
                tmp10 = pMCUSrc[iRow + 0];
                tmp13 = pMCUSrc[iRow + 2];
                tmp12 = ((tmp13 * 106) >> 8); // 2-6 * 1.414
                tmp0 = tmp10 + tmp13;
                tmp3 = tmp10 - tmp13;
                tmp1 = tmp10 + tmp12;
                tmp2 = tmp10 - tmp12;
                // odd part
                z13 = pMCUSrc[iRow + 3];
                z11 = pMCUSrc[iRow + 1];
                tmp7 = z11 + z13;
                tmp11 = ((z11 - z13) * 362) >> 8; // * 1.414
                z5 = ((z11 - z13) * 473) >> 8; // * 1.8477
                tmp10 = ((z11 * 277) >> 8) - z5; // * 1.08239
                tmp12 = ((z13 * 669) >> 8) + z5; // * 2.61312
                tmp6 = tmp12 - tmp7;
                tmp5 = tmp11 - tmp6;
                tmp4 = tmp10 + tmp5;
            }
        }
        else { // need to do the full calculation
            tmp10 = pMCUSrc[iRow + 0] + pMCUSrc[iRow + 4];
            tmp11 = pMCUSrc[iRow + 0] - pMCUSrc[iRow + 4];
            tmp13 = pMCUSrc[iRow + 2] + pMCUSrc[iRow + 6];
            tmp12 = (((pMCUSrc[iRow + 2] - pMCUSrc[iRow + 6]) * 362) >> 8) - tmp13; // 2-6 * 1.414
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;
            // odd part
            z13 = pMCUSrc[iRow + 5] + pMCUSrc[iRow + 3];
            z10 = pMCUSrc[iRow + 5] - pMCUSrc[iRow + 3];
            z11 = pMCUSrc[iRow + 1] + pMCUSrc[iRow + 7];
            z12 = pMCUSrc[iRow + 1] - pMCUSrc[iRow + 7];
            tmp7 = z11 + z13;
            tmp11 = ((z11 - z13) * 362) >> 8; // * 1.414
            z5 = ((z10 + z12) * 473) >> 8; // * 1.8477
            tmp10 = ((z12 * 277) >> 8) - z5; // * 1.08239
            tmp12 = ((z10 * -669) >> 8) + z5; // * 2.61312
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;
        }
        // final output stage - scale down and range limit
#ifdef HAS_SIMD
        {
            uint32_t ul, ulOut;
            const uint32_t ulAdj = 0x800080;
            ulOut = __SSAT16((((tmp0 + tmp7) >> 5) & 0xffff) | (((tmp2 + tmp5) >> 5) << 16), 8);
            ulOut = __SADD16(ulOut, ulAdj); // adjust
            ul = __SSAT16((((tmp1 + tmp6) >> 5) & 0xffff) | (((tmp3 - tmp4) >> 5) << 16), 8);
            ul = __SADD16(ul, ulAdj); // adjust
            ulOut |= (ul << 8); // combine 4 outputs
            *(uint32_t *)pOutput = ulOut; // store first 4
            ulOut = __SSAT16((((tmp3 + tmp4) >> 5) & 0xffff) | (((tmp1 - tmp6) >> 5) << 16), 8);
            ulOut = __SADD16(ulOut, ulAdj); // adjust
            ul = __SSAT16((((tmp2 - tmp5) >> 5) & 0xffff) | (((tmp0 - tmp7) >> 5) << 16), 8);
            ul = __SADD16(ul, ulAdj); // adjust
            ulOut |= (ul << 8); // combine 4 outputs
            *(uint32_t *)&pOutput[4] = ulOut; // store second 4
        }
#else
        // I've tried various things to speed this up, but it always seems to take the same amount of time
#ifdef HAS_NEON
        {
            int16x4_t L_in_16x4, R_in_16x4, L_out, R_out;
            int8x8_t LR_out_8x8;
            int16x8_t LR_out;
            L_in_16x4 = vdup_n_s16(tmp0); // suppresses warning of setting lane 0 of uninitialized var
            L_in_16x4 = vset_lane_s16(tmp1, L_in_16x4, 1);
            L_in_16x4 = vset_lane_s16(tmp2, L_in_16x4, 2);
            L_in_16x4 = vset_lane_s16(tmp3, L_in_16x4, 3);
            R_in_16x4 = vdup_n_s16(tmp7);
            R_in_16x4 = vset_lane_s16(tmp6, R_in_16x4, 1);
            R_in_16x4 = vset_lane_s16(tmp5, R_in_16x4, 2);
            R_in_16x4 = vset_lane_s16(-tmp4, R_in_16x4, 3);
            L_out = vadd_s16(L_in_16x4, R_in_16x4); // tmp0 + tmp7, tmp1 + tmp6, ...
            R_out = vsub_s16(L_in_16x4, R_in_16x4); // tmp0 - tmp7, tmp1 - tmp6, ...
            R_out = vrev64_s16(R_out); // flip order of 4-7
            LR_out = vcombine_s16(L_out, R_out);
            LR_out = vaddq_s16(LR_out, vdupq_n_s16(0x80 << 5)); // adjust output +0x80
            LR_out_8x8 = vqshrun_n_s16(LR_out, 5); // shift, narrow and clip to 0-255
            vst1_u8(pOutput, LR_out_8x8);
        }
#else
        pOutput[0] = ucRangeTable[(((tmp0 + tmp7) >> 5) & 0x3ff)];
        pOutput[1] = ucRangeTable[(((tmp1 + tmp6) >> 5) & 0x3ff)];
        pOutput[2] = ucRangeTable[(((tmp2 + tmp5) >> 5) & 0x3ff)];
        pOutput[3] = ucRangeTable[(((tmp3 - tmp4) >> 5) & 0x3ff)];
        pOutput[4] = ucRangeTable[(((tmp3 + tmp4) >> 5) & 0x3ff)];
        pOutput[5] = ucRangeTable[(((tmp2 - tmp5) >> 5) & 0x3ff)];
        pOutput[6] = ucRangeTable[(((tmp1 - tmp6) >> 5) & 0x3ff)];
        pOutput[7] = ucRangeTable[(((tmp0 - tmp7) >> 5) & 0x3ff)];
#endif // !HAS_NEON
#endif
        pOutput += 8;
    } // for each row
} /* JPEGIDCT() */
static void JPEGPutMCU8BitGray(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    int i, j, xcount, ycount;
    uint8_t * pDest, * pSrc = (uint8_t *)&pJPEG->sMCUs[0];

    if(pJPEG->pDitherBuffer)
        pDest = &pJPEG->pDitherBuffer[x];
    else
        pDest = (uint8_t *)&pJPEG->usPixels[x / 2];

    if(pJPEG->ucSubSample <= 0x11) { // single Y
        if(pJPEG->iOptions & JPEG_SCALE_HALF) { // special handling of 1/2 size (pixel averaging)
            int pix;
            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    pix = (pSrc[0] + pSrc[1] + pSrc[8] + pSrc[9] + 2) >> 2; // average 2x2 block
                    pDest[j] = (uint8_t)pix;
                    pSrc += 2;
                }
                pSrc += 8; // skip extra line
                pDest += iPitch;
            }
            return;
        }
        xcount = ycount = 8; // debug
        if(pJPEG->iOptions & JPEG_SCALE_QUARTER)
            xcount = ycount = 2;
        else if(pJPEG->iOptions & JPEG_SCALE_EIGHTH)
            xcount = ycount = 1;
        for(i = 0; i < ycount; i++) { // do up to 8 rows
            for(j = 0; j < xcount; j++)
                *pDest++ = *pSrc++;
            pDest -= xcount;
            pDest += iPitch; // next line
        }
        return;
    } // single Y source
    if(pJPEG->ucSubSample == 0x21) { // stacked horizontally
        if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
            // only 2 pixels emitted
            pDest[0] = pSrc[0];
            pDest[1] = pSrc[128];
            return;
        } /* 1/8 */
        if(pJPEG->iOptions & JPEG_SCALE_HALF) {
            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    int pix;
                    pix = (pSrc[j * 2] + pSrc[j * 2 + 1] + pSrc[j * 2 + 8] + pSrc[j * 2 + 9] + 2) >> 2;
                    pDest[j] = (uint8_t)pix;
                    pix = (pSrc[j * 2 + 128] + pSrc[j * 2 + 129] + pSrc[j * 2 + 136] + pSrc[j * 2 + 137] + 2) >> 2;
                    pDest[j + 4] = (uint8_t)pix;
                }
                pSrc += 16;
                pDest += iPitch;
            }
            return;
        }
        if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
            // each MCU contributes a 2x2 block
            pDest[0] = pSrc[0]; // Y0
            pDest[1] = pSrc[1];
            pDest[iPitch] = pSrc[2];
            pDest[iPitch + 1] = pSrc[3];

            pDest[2] = pSrc[128]; // Y`
            pDest[3] = pSrc[129];
            pDest[iPitch + 2] = pSrc[130];
            pDest[iPitch + 3] = pSrc[131];
            return;
        }
        for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) {
                pDest[j] = pSrc[j];
                pDest[j + 8] = pSrc[128 + j];
            }
            pSrc += 8;
            pDest += iPitch;
        }
    } // 0x21
    if(pJPEG->ucSubSample == 0x12) { // stacked vertically
        if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
            // only 2 pixels emitted
            pDest[0] = pSrc[0];
            pDest[iPitch] = pSrc[128];
            return;
        } /* 1/8 */
        if(pJPEG->iOptions & JPEG_SCALE_HALF) {
            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    int pix;
                    pix = (pSrc[j * 2] + pSrc[j * 2 + 1] + pSrc[j * 2 + 8] + pSrc[j * 2 + 9] + 2) >> 2;
                    pDest[j] = (uint8_t)pix;
                    pix = (pSrc[j * 2 + 128] + pSrc[j * 2 + 129] + pSrc[j * 2 + 136] + pSrc[j * 2 + 137] + 2) >> 2;
                    pDest[4 * iPitch + j] = (uint8_t)pix;
                }
                pSrc += 16;
                pDest += iPitch;
            }
            return;
        }
        if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
            // each MCU contributes a 2x2 block
            pDest[0] = pSrc[0]; // Y0
            pDest[1] = pSrc[1];
            pDest[iPitch] = pSrc[2];
            pDest[iPitch + 1] = pSrc[3];

            pDest[iPitch * 2] = pSrc[128]; // Y`
            pDest[iPitch * 2 + 1] = pSrc[129];
            pDest[iPitch * 3] = pSrc[130];
            pDest[iPitch * 3 + 1] = pSrc[131];
            return;
        }
        for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) {
                pDest[j] = pSrc[j];
                pDest[8 * iPitch + j] = pSrc[128 + j];
            }
            pSrc += 8;
            pDest += iPitch;
        }
    } // 0x12
    if(pJPEG->ucSubSample == 0x22) {
        if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
            // each MCU contributes 1 pixel
            pDest[0] = pSrc[0]; // Y0
            pDest[1] = pSrc[128]; // Y1
            pDest[iPitch] = pSrc[256]; // Y2
            pDest[iPitch + 1] = pSrc[384]; // Y3
            return;
        }
        if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
            // each MCU contributes 2x2 pixels
            pDest[0] = pSrc[0]; // Y0
            pDest[1] = pSrc[1];
            pDest[iPitch] = pSrc[2];
            pDest[iPitch + 1] = pSrc[3];

            pDest[2] = pSrc[128]; // Y1
            pDest[3] = pSrc[129];
            pDest[iPitch + 2] = pSrc[130];
            pDest[iPitch + 3] = pSrc[131];

            pDest[iPitch * 2] = pSrc[256]; // Y2
            pDest[iPitch * 2 + 1] = pSrc[257];
            pDest[iPitch * 3] = pSrc[258];
            pDest[iPitch * 3 + 1] = pSrc[259];

            pDest[iPitch * 2 + 2] = pSrc[384]; // Y3
            pDest[iPitch * 2 + 3] = pSrc[385];
            pDest[iPitch * 3 + 2] = pSrc[386];
            pDest[iPitch * 3 + 3] = pSrc[387];
            return;
        }
        if(pJPEG->iOptions & JPEG_SCALE_HALF) {
            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    int pix;
                    pix = (pSrc[j * 2] + pSrc[j * 2 + 1] + pSrc[j * 2 + 8] + pSrc[j * 2 + 9] + 2) >> 2;
                    pDest[j] = (uint8_t)pix; // Y0
                    pix = (pSrc[j * 2 + 128] + pSrc[j * 2 + 129] + pSrc[j * 2 + 136] + pSrc[j * 2 + 137] + 2) >> 2;
                    pDest[j + 4] = (uint8_t)pix; // Y1
                    pix = (pSrc[j * 2 + 256] + pSrc[j * 2 + 257] + pSrc[j * 2 + 264] + pSrc[j * 2 + 265] + 2) >> 2;
                    pDest[iPitch * 4 + j] = (uint8_t)pix; // Y2
                    pix = (pSrc[j * 2 + 384] + pSrc[j * 2 + 385] + pSrc[j * 2 + 392] + pSrc[j * 2 + 393] + 2) >> 2;
                    pDest[iPitch * 4 + j + 4] = (uint8_t)pix; // Y3
                }
                pSrc += 16;
                pDest += iPitch;
            }
            return;
        }
#ifdef ALLOWS_UNALIGNED
        for(i = 0; i < 8; i++) {
            *(uint32_t *)pDest = *(uint32_t *)pSrc; // Y0
            *(uint32_t *)&pDest[4] = *(uint32_t *)&pSrc[4]; // Y0
            *(uint32_t *)&pDest[8] = *(uint32_t *)&pSrc[128]; // Y1
            *(uint32_t *)&pDest[12] = *(uint32_t *)&pSrc[132]; // Y1
            *(uint32_t *)&pDest[iPitch * 8] = *(uint32_t *)&pSrc[256]; // Y2
            *(uint32_t *)&pDest[(iPitch * 8) + 4] = *(uint32_t *)&pSrc[260]; // Y2
            *(uint32_t *)&pDest[(iPitch * 8) + 8] = *(uint32_t *)&pSrc[384]; // Y3
            *(uint32_t *)&pDest[(iPitch * 8) + 12] = *(uint32_t *)&pSrc[388]; // Y3
            pSrc += 8;
            pDest += iPitch;
        }
#else
        for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) {
                pDest[j] = pSrc[j]; // Y0
                pDest[j + 8] = pSrc[j + 128]; // Y1
                pDest[iPitch * 8 + j] = pSrc[j + 256]; // Y2
                pDest[iPitch * 8 + j + 8] = pSrc[j + 384]; // Y3
            }
            pSrc += 8;
            pDest += iPitch;
        }
#endif
    } // 0x22
} /* JPEGMPutMCU8BitGray() */

static void JPEGPutMCUGray(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    uint16_t * usDest = (uint16_t *)&pJPEG->usPixels[x];
    int i, j, xcount, ycount;
    uint8_t * pSrc = (uint8_t *)&pJPEG->sMCUs[0];

    if(pJPEG->iOptions & JPEG_SCALE_HALF) { // special handling of 1/2 size (pixel averaging)
        int pix;
        for(i = 0; i < 4; i++) {
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                for(j = 0; j < 4; j++) {
                    pix = (pSrc[0] + pSrc[1] + pSrc[8] + pSrc[9] + 2) >> 2; // average 2x2 block
                    usDest[j] = usGrayTo565[pix];
                    pSrc += 2;
                }
            }
            else {
                for(j = 0; j < 4; j++) {
                    pix = (pSrc[0] + pSrc[1] + pSrc[8] + pSrc[9] + 2) >> 2; // average 2x2 block
                    usDest[j] = __builtin_bswap16(usGrayTo565[pix]);
                    pSrc += 2;
                }
            }
            pSrc += 8; // skip extra line
            usDest += iPitch;
        }
        return;
    }
    xcount = ycount = 8; // debug
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER)
        xcount = ycount = 2;
    else if(pJPEG->iOptions & JPEG_SCALE_EIGHTH)
        xcount = ycount = 1;
    for(i = 0; i < ycount; i++) { // do up to 8 rows
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            for(j = 0; j < xcount; j++)
                *usDest++ = usGrayTo565[*pSrc++];
        }
        else {
            for(j = 0; j < xcount; j++)
                *usDest++ = __builtin_bswap16(usGrayTo565[*pSrc++]);
        }
        usDest -= xcount;
        usDest += iPitch; // next line
    }
} /* JPEGPutMCUGray() */

static void JPEGPixelLE(uint16_t * pDest, int iY, int iCb, int iCr)
{
    //
    // Cortex-M4/M7 has some SIMD instructions which can shave a few cycles
    // off of this function (e.g. Teensy, Arduino Nano 33 BLE, Portenta, etc)
    //
#ifdef HAS_SIMD
    uint32_t ulPixel;
    uint32_t ulCbCr = (iCb | (iCr << 16));
    uint32_t ulTmp = -1409 | (-2925 << 16); // for green calc
    ulCbCr = __SSUB16(ulCbCr, 0x00800080); // dual 16-bit subtraction
    ulPixel = __SMLAD(ulCbCr, ulTmp, iY) >> 14; // G
    ulPixel = __USAT16(ulPixel, 6) << 5; // range limit to 6 bits
    ulTmp = __SMLAD(7258, ulCbCr, iY) >> 15; // Blue
    ulTmp = __USAT16(ulTmp, 5); // range limit to 5 bits
    ulPixel |= ulTmp; // now we have G + B
    ulTmp = __SMLAD(5742, iCr, iY) >> 15; // Red
    ulTmp = __USAT16(ulTmp, 5); // range limit to 5 bits
    ulPixel |= (ulTmp << 11); // now we have R + G + B
    pDest[0] = (uint16_t)ulPixel;
#else
    int iCBB, iCBG, iCRG, iCRR;
    unsigned short usPixel;

    iCBB = 7258  * (iCb - 0x80);
    iCBG = -1409 * (iCb - 0x80);
    iCRG = -2925 * (iCr - 0x80);
    iCRR = 5742  * (iCr - 0x80);
    usPixel = usRangeTableB[((iCBB + iY) >> 12) & 0x3ff]; // blue pixel
    usPixel |= usRangeTableG[((iCBG + iCRG + iY) >> 12) & 0x3ff]; // green pixel
    usPixel |= usRangeTableR[((iCRR + iY) >> 12) & 0x3ff]; // red pixel
    pDest[0] = usPixel;
#endif
} /* JPEGPixelLE() */

static void JPEGPixelBE(uint16_t * pDest, int iY, int iCb, int iCr)
{
    int iCBB, iCBG, iCRG, iCRR;
    unsigned short usPixel;

    iCBB = 7258  * (iCb - 0x80);
    iCBG = -1409 * (iCb - 0x80);
    iCRG = -2925 * (iCr - 0x80);
    iCRR = 5742  * (iCr - 0x80);
    usPixel = usRangeTableB[((iCBB + iY) >> 12) & 0x3ff]; // blue pixel
    usPixel |= usRangeTableG[((iCBG + iCRG + iY) >> 12) & 0x3ff]; // green pixel
    usPixel |= usRangeTableR[((iCRR + iY) >> 12) & 0x3ff]; // red pixel
    pDest[0] = __builtin_bswap16(usPixel);
} /* JPEGPixelBE() */

static void JPEGPixelRGB(uint32_t * pDest, int iY, int iCb, int iCr)
{
    int iCBB, iCBG, iCRG, iCRR;
    uint32_t u32Pixel;
    int32_t i32;

    iCBB = 7258  * (iCb - 0x80);
    iCBG = -1409 * (iCb - 0x80);
    iCRG = -2925 * (iCr - 0x80);
    iCRR = 5742  * (iCr - 0x80);
    u32Pixel = 0xff000000; // Alpha = 0xff
    i32 = ((iCBB + iY) >> 12);
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel |= (uint32_t)i32; // blue
    i32 = ((iCBG + iCRG + iY) >> 12); // green pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel |= (uint32_t)(i32 << 8);
    i32 = ((iCRR + iY) >> 12); // red pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel |= (uint32_t)(i32 << 16);
    pDest[0] = u32Pixel;
} /* JPEGPixelRGB() */

static void JPEGPixel2LE(uint16_t * pDest, int iY1, int iY2, int iCb, int iCr)
{
    uint32_t ulPixel1, ulPixel2;
    //
    // Cortex-M4/M7 has some SIMD instructions which can shave a few cycles
    // off of this function (e.g. Teensy, Arduino Nano 33 BLE, Portenta, etc)
    //
#ifdef HAS_SIMD
    uint32_t ulCbCr = (iCb | (iCr << 16));
    uint32_t ulTmp2, ulTmp = -1409 | (-2925 << 16); // for green calc
    ulCbCr = __SSUB16(ulCbCr, 0x00800080); // dual 16-bit subtraction
    ulPixel1 = __SMLAD(ulCbCr, ulTmp, iY1) >> 14; // G for pixel 1
    ulPixel2 = __SMLAD(ulCbCr, ulTmp, iY2) >> 14; // G for pixel 2
    ulPixel1 |= (ulPixel2 << 16);
    ulPixel1 = __USAT16(ulPixel1, 6) << 5; // range limit both to 6 bits
    ulTmp = __SMLAD(7258, ulCbCr, iY1) >> 15; // Blue 1
    ulTmp2 = __SMLAD(7258, ulCbCr, iY2) >> 15; // Blue 2
    ulTmp = __USAT16(ulTmp | (ulTmp2 << 16), 5); // range limit both to 5 bits
    ulPixel1 |= ulTmp; // now we have G + B
    ulTmp = __SMLAD(5742, iCr, iY1) >> 15; // Red 1
    ulTmp2 = __SMLAD(5742, iCr, iY2) >> 15; // Red 2
    ulTmp = __USAT16(ulTmp | (ulTmp2 << 16), 5); // range limit both to 5 bits
    ulPixel1 |= (ulTmp << 11); // now we have R + G + B
    *(uint32_t *)&pDest[0] = ulPixel1;
#else
    int iCBB, iCBG, iCRG, iCRR;
    iCBB = 7258  * (iCb - 0x80);
    iCBG = -1409 * (iCb - 0x80);
    iCRG = -2925 * (iCr - 0x80);
    iCRR = 5742  * (iCr - 0x80);
    ulPixel1 = usRangeTableB[((iCBB + iY1) >> 12) & 0x3ff]; // blue pixel
    ulPixel1 |= usRangeTableG[((iCBG + iCRG + iY1) >> 12) & 0x3ff]; // green pixel
    ulPixel1 |= usRangeTableR[((iCRR + iY1) >> 12) & 0x3ff]; // red pixel

    ulPixel2 = usRangeTableB[((iCBB + iY2) >> 12) & 0x3ff]; // blue pixel
    ulPixel2 |= usRangeTableG[((iCBG + iCRG + iY2) >> 12) & 0x3ff]; // green pixel
    ulPixel2 |= usRangeTableR[((iCRR + iY2) >> 12) & 0x3ff]; // red pixel
    *(uint32_t *)&pDest[0] = (ulPixel1 | (ulPixel2 << 16));
#endif
} /* JPEGPixel2LE() */

static void JPEGPixel2BE(uint16_t * pDest, int32_t iY1, int32_t iY2, int32_t iCb, int32_t iCr)
{
    int32_t iCBB, iCBG, iCRG, iCRR;
    uint32_t ulPixel1, ulPixel2;

    iCBB = 7258L  * (iCb - 0x80);
    iCBG = -1409L * (iCb - 0x80);
    iCRG = -2925L * (iCr - 0x80);
    iCRR = 5742L  * (iCr - 0x80);
    ulPixel1 = usRangeTableB[((iCBB + iY1) >> 12) & 0x3ff]; // blue pixel
    ulPixel1 |= usRangeTableG[((iCBG + iCRG + iY1) >> 12) & 0x3ff]; // green pixel
    ulPixel1 |= usRangeTableR[((iCRR + iY1) >> 12) & 0x3ff]; // red pixel

    ulPixel2 = usRangeTableB[((iCBB + iY2) >> 12) & 0x3ff]; // blue pixel
    ulPixel2 |= usRangeTableG[((iCBG + iCRG + iY2) >> 12) & 0x3ff]; // green pixel
    ulPixel2 |= usRangeTableR[((iCRR + iY2) >> 12) & 0x3ff]; // red pixel
    *(uint32_t *)&pDest[0] = __builtin_bswap16(ulPixel1) | ((uint32_t)__builtin_bswap16(ulPixel2) << 16);
} /* JPEGPixel2BE() */

static void JPEGPixel2RGB(uint32_t * pDest, int32_t iY1, int32_t iY2, int32_t iCb, int32_t iCr)
{
    int32_t iCBB, iCBG, iCRG, iCRR;
    uint32_t u32Pixel1, u32Pixel2;
    int32_t i32;

    iCBB = 7258L  * (iCb - 0x80);
    iCBG = -1409L * (iCb - 0x80);
    iCRG = -2925L * (iCr - 0x80);
    iCRR = 5742L  * (iCr - 0x80);
    i32 = ((iCBB + iY1) >> 12); // blue pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel1 = u32Pixel2 = 0xff000000; // Alpha = 255
    u32Pixel1 |= (uint32_t)i32; // blue
    i32 = ((iCBG + iCRG + iY1) >> 12); // green pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel1 |= (uint32_t)(i32 << 8); // green
    i32 = ((iCRR + iY1) >> 12); // red pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel1 |= (uint32_t)(i32 << 16); // red

    i32 = ((iCBB + iY2) >> 12); // blue pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel2 |= (uint32_t)i32;
    i32 = ((iCBG + iCRG + iY2) >> 12); // green pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel2 |= (uint32_t)(i32 << 8);
    i32 = ((iCRR + iY2) >> 12); // red pixel
    if(i32 < 0) i32 = 0;
    else if(i32 > 255) i32 = 255;
    u32Pixel2 |= (uint32_t)(i32 << 16);
    pDest[0] = u32Pixel1;
    pDest[1] = u32Pixel2;
} /* JPEGPixel2RGB() */

static void JPEGPutMCU11(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    int iCr, iCb;
    signed int Y;
    int iCol;
    int iRow;
    uint8_t * pY, * pCr, * pCb;
    uint16_t * pOutput = &pJPEG->usPixels[x];

    if(pJPEG->ucPixelType == RGB8888) {
        pOutput += x; // 4 bytes per pixel, not 2
    }

    pY  = (unsigned char *)&pJPEG->sMCUs[0 * DCTSIZE];
    pCb = (unsigned char *)&pJPEG->sMCUs[1 * DCTSIZE];
    pCr = (unsigned char *)&pJPEG->sMCUs[2 * DCTSIZE];

    if(pJPEG->iOptions & JPEG_SCALE_HALF) {
        for(iRow = 0; iRow < 4; iRow++) { // up to 8 rows to do
            for(iCol = 0; iCol < 4; iCol++) { // up to 4x2 cols to do
                iCr = (pCr[0] + pCr[1] + pCr[8] + pCr[9] + 2) >> 2;
                iCb = (pCb[0] + pCb[1] + pCb[8] + pCb[9] + 2) >> 2;
                Y = (pY[0] + pY[1] + pY[8] + pY[9]) << 10;
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol, Y, iCb, iCr);
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol, Y, iCb, iCr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y, iCb, iCr);
                pCr += 2;
                pCb += 2;
                pY += 2;
            } // for col
            pCr += 8;
            pCb += 8;
            pY += 8;
            pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
        } // for row
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) { // special case for 1/8 scaling
        // only 4 pixels to draw, so no looping needed
        iCr = pCr[0];
        iCb = pCb[0];
        Y = (int)(pY[0]) << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixelLE(pOutput, Y, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixelBE(pOutput, Y, iCb, iCr);
        else
            JPEGPixelRGB((uint32_t *)pOutput, Y, iCb, iCr);
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER) { // special case for 1/4 scaling
        // only 4 pixels to draw, so no looping needed
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelLE(pOutput, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelLE(pOutput + 1, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelLE(pOutput + iPitch, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelLE(pOutput + 1 + iPitch, Y, iCb, iCr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelBE(pOutput, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelBE(pOutput + 1, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelBE(pOutput + iPitch, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelBE(pOutput + 1 + iPitch, Y, iCb, iCr);
        }
        else {   // RGB8888
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelRGB((uint32_t *)pOutput, Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelRGB((uint32_t *)&pOutput[2], Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 2], Y, iCb, iCr);
            iCr = *pCr++;
            iCb = *pCb++;
            Y = (int)(*pY++) << 12;
            JPEGPixelRGB((uint32_t *)&pOutput[2 + iPitch * 2], Y, iCb, iCr);
        }
        return;
    }
    // full size

#ifdef HAS_SSE
    // SSE2 version
    // R = Y                + 1.40200 * Cr
    // G = Y - 0.34414 * Cb + 0.28586 * Cr - Cr
    // B = Y - 0.22800 * Cb + Cb + Cb

    if(pJPEG->ucPixelType == RGB8888) {
        __m128i mmxY, mmxCr, mmxCb, mmxTemp;
        __m128i mmxTemp2, mmxR, mmxG, mmxB;
        iPitch *= 2; // points to 32-bit values, not 16-bit
        mmxTemp2 = _mm_cmpeq_epi16(_mm_setzero_si128(), _mm_setzero_si128()); // fix Cr/Cb values by subtracting 0x80
        mmxTemp2 = _mm_slli_epi16(mmxTemp2, 15);  // now has 0x8000, 0x8000...
        for(iRow = 0; iRow < 8; iRow++) { // do 8 rows
            mmxCr = _mm_loadl_epi64((__m128i *)pCr); // load 1 row of Cr
            mmxCb = _mm_loadl_epi64((__m128i *)pCb); // load 1 row of Cb
            mmxY = _mm_loadl_epi64((__m128i *)pY); // load 1 row of Y
            pCr += 8;
            pCb += 8;
            pY += 8;
            mmxCr = _mm_unpacklo_epi8(_mm_setzero_si128(), mmxCr);  // zero-extend 8 Cr values to 16-bits
            mmxCb = _mm_unpacklo_epi8(_mm_setzero_si128(), mmxCb);  // zero-extend 8 Cb values to 16-bits
            mmxY = _mm_unpacklo_epi8(mmxY, _mm_setzero_si128());  // zero-extend 8 Y values to 16-bits
            mmxCr = _mm_add_epi16(mmxCr, mmxTemp2); // subtract 0x80
            mmxCb = _mm_add_epi16(mmxCb, mmxTemp2);  // subtract 0x80
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxTemp =  _mm_loadu_si128((__m128i *)&s1402[0]); // load the 1.402 constant
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxTemp); // almost ready with R
            mmxR = _mm_add_epi16(mmxTemp, mmxY); // now we have 8 R values
            mmxTemp = _mm_loadu_si128((__m128i *)&s0714[0]);
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxTemp); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, mmxTemp);
            mmxTemp = _mm_loadu_si128((__m128i *)&s0344[0]); // Y -= 0.34414*Cb
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxTemp);
            mmxG = _mm_add_epi16(mmxG, mmxTemp); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_loadu_si128((__m128i *)&s1772[0]); // B = Y - 1.772*Cb
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxTemp);
            mmxB = _mm_add_epi16(mmxY, mmxTemp); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_cmpeq_epi16(mmxTemp, mmxTemp); // Alpha set to FFFF
            mmxCr = _mm_unpacklo_epi8(mmxB, mmxG); // interleave 8 B's and 8 G's
            mmxCb = _mm_unpacklo_epi8(mmxR, mmxTemp); // interlave 8 R's and 8 A's
            mmxTemp = _mm_unpacklo_epi16(mmxCr, mmxCb); // interleave 4 BG's and 4 RA's
            mmxCr = _mm_unpackhi_epi16(mmxCr, mmxCb); // interleave 4 BG's and 4 RA's
            //         _mm_stream_si128((__m128i*)pOutput, mmxTemp);
            //  _mm_stream_si128((__m128i*)(pOutput+8), mmxCr);
            _mm_storeu_si128((__m128i *)pOutput, mmxTemp); // write 4 RGBA pixels
            _mm_storeu_si128((__m128i *)(pOutput + 8), mmxCr); // write 4 RGBA pixels
            pOutput += iPitch;
        } // for each row
        return;
    }
    else {   // 16-bpp
        __m128i mmxY, mmxCr, mmxCb, mmxTemp;
        __m128i mmxTemp2, mmxR, mmxG, mmxB;
        for(iRow = 0; iRow < 8; iRow++) { // do 8 rows
            mmxCr = _mm_loadl_epi64((__m128i *)pCr); // load 1 row of Cr
            mmxCb = _mm_loadl_epi64((__m128i *)pCb); // load 1 row of Cb
            mmxY = _mm_loadl_epi64((__m128i *)pY); // load 1 row of Y
            pCr += 8;
            pCb += 8;
            pY += 8;
            mmxTemp = _mm_cmpeq_epi16(mmxY, mmxY); // fix Cr/Cb values by subtracting 0x80
            mmxTemp = _mm_slli_epi16(mmxTemp, 15);  // now has 0x8000, 0x8000...
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxCr = _mm_unpacklo_epi8(mmxTemp2, mmxCr);  // zero-extend 8 Cr values to 16-bits
            mmxCb = _mm_unpacklo_epi8(mmxTemp2, mmxCb);  // zero-extend 8 Cb values to 16-bits
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxCr = _mm_add_epi16(mmxCr, mmxTemp); // subtract 0x80
            mmxCb = _mm_add_epi16(mmxCb, mmxTemp);  // subtract 0x80
            mmxTemp =  _mm_loadu_si128((__m128i *)&s1402[0]); // load the 1.402 constant
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxTemp); // almost ready with R
            mmxR = _mm_add_epi16(mmxTemp, mmxY); // now we have 8 R values
            mmxTemp = _mm_loadu_si128((__m128i *)&s0714[0]);
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxTemp); // Y-0.71414*Cr
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxG = _mm_add_epi16(mmxY, mmxTemp);
            mmxTemp = _mm_loadu_si128((__m128i *)&s0344[0]); // Y -= 0.34414*Cb
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxTemp);
            mmxG = _mm_add_epi16(mmxG, mmxTemp); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_loadu_si128((__m128i *)&s1772[0]); // B = Y - 1.772*Cb
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxTemp);
            mmxB = _mm_add_epi16(mmxY, mmxTemp); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxTemp = _mm_setzero_si128(); // interleave with 0 to get back to 16-bit values
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxR = _mm_unpacklo_epi8(mmxR, mmxTemp); // zero-extend to 16-bits again
            mmxB = _mm_unpacklo_epi8(mmxB, mmxTemp);
            mmxG = _mm_unpacklo_epi8(mmxG, mmxTemp);
            mmxR = _mm_srli_epi16(mmxR, 3); // reduce to 5-bits
            mmxR = _mm_slli_epi16(mmxR, 11); // set in proper position
            mmxB = _mm_srli_epi16(mmxB, 3); // reduce to 5-bits
            mmxG = _mm_srli_epi16(mmxG, 2); // reduce to 6-bits
            mmxG = _mm_slli_epi16(mmxG, 5); // set in proper position
            mmxTemp = _mm_or_si128(mmxR, mmxG); // R+G
            mmxTemp = _mm_or_si128(mmxTemp, mmxB); // R+G+B
            _mm_storeu_si128((__m128i *)pOutput, mmxTemp); // write 8 RGB565 pixels
            pOutput += iPitch;
        } // for each row
        return;
    }
#endif // HAS_SSE

    for(iRow = 0; iRow < 8; iRow++) { // up to 8 rows to do
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            for(iCol = 0; iCol < 8; iCol++) { // up to 4x2 cols to do
                iCr = *pCr++;
                iCb = *pCb++;
                Y = (int)(*pY++) << 12;
                if(x + iCol < pJPEG->iWidth) {
                    JPEGPixelLE(pOutput + iCol, Y, iCb, iCr);
                }
            } // for col
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            for(iCol = 0; iCol < 8; iCol++) { // up to 4x2 cols to do
                iCr = *pCr++;
                iCb = *pCb++;
                Y = (int)(*pY++) << 12;
                if(x + iCol < pJPEG->iWidth) {
                    JPEGPixelBE(pOutput + iCol, Y, iCb, iCr);
                }
            } // for col
        }
        else {   // RGB888
            for(iCol = 0; iCol < 8; iCol++) { // up to 4x2 cols to do
                iCr = *pCr++;
                iCb = *pCb++;
                Y = (int)(*pY++) << 12;
                if(x + iCol < pJPEG->iWidth) {
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y, iCb, iCr);
                }
            } // for col
        }
        pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
    } // for row
} /* JPEGPutMCU11() */

static void JPEGPutMCU22(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    uint32_t Cr, Cb;
    signed int Y1, Y2, Y3, Y4;
    int iRow, iCol, iXCount1, iXCount2, iYCount;
    unsigned char * pY, * pCr, * pCb;
    int bUseOdd1, bUseOdd2; // special case where 24bpp odd sized image can clobber first column
    uint16_t * pOutput = &pJPEG->usPixels[x];
    if(pJPEG->ucPixelType == RGB8888) {
        pOutput += x; // 4 bytes per pixel, not 2
    }
    pY  = (unsigned char *)&pJPEG->sMCUs[0 * DCTSIZE];
    pCb = (unsigned char *)&pJPEG->sMCUs[4 * DCTSIZE];
    pCr = (unsigned char *)&pJPEG->sMCUs[5 * DCTSIZE];

    if(pJPEG->iOptions & JPEG_SCALE_HALF) { // special handling of 1/2 size (pixel averaging)
        for(iRow = 0; iRow < 4; iRow++) { // 16x16 becomes 8x8 of 2x2 pixels
            for(iCol = 0; iCol < 4; iCol++) {
                Y1 = (pY[iCol * 2] + pY[iCol * 2 + 1] + pY[iCol * 2 + 8] + pY[iCol * 2 + 9]) << 10;
                Cb = pCb[iCol];
                Cr = pCr[iCol];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol, Y1, Cb, Cr); // top left
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol, Y1, Cb, Cr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y1, Cb, Cr);
                Y1 = (pY[iCol * 2 + (DCTSIZE * 2)] + pY[iCol * 2 + 1 + (DCTSIZE * 2)] + pY[iCol * 2 + 8 +
                                                                                           (DCTSIZE * 2)] + pY[iCol * 2 + 9 + (DCTSIZE * 2)]) << 10;
                Cb = pCb[iCol + 4];
                Cr = pCr[iCol + 4];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol + 4, Y1, Cb, Cr); // top right
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol + 4, Y1, Cb, Cr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[(iCol * 2) + 8], Y1, Cb, Cr);
                Y1 = (pY[iCol * 2 + (DCTSIZE * 4)] + pY[iCol * 2 + 1 + (DCTSIZE * 4)] + pY[iCol * 2 + 8 +
                                                                                           (DCTSIZE * 4)] + pY[iCol * 2 + 9 + (DCTSIZE * 4)]) << 10;
                Cb = pCb[iCol + 32];
                Cr = pCr[iCol + 32];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol + iPitch * 4, Y1, Cb, Cr); // bottom left
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol + iPitch * 4, Y1, Cb, Cr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[(iCol + iPitch * 4) * 2], Y1, Cb, Cr);
                Y1 = (pY[iCol * 2 + (DCTSIZE * 6)] + pY[iCol * 2 + 1 + (DCTSIZE * 6)] + pY[iCol * 2 + 8 +
                                                                                           (DCTSIZE * 6)] + pY[iCol * 2 + 9 + (DCTSIZE * 6)]) << 10;
                Cb = pCb[iCol + 32 + 4];
                Cr = pCr[iCol + 32 + 4];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol + 4 + iPitch * 4, Y1, Cb, Cr); // bottom right
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol + 4 + iPitch * 4, Y1, Cb, Cr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[(iCol + 4 + iPitch * 4) * 2], Y1, Cb, Cr);
            }
            pY += 16;
            pCb += 8;
            pCr += 8;
            pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
        }
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
        Y1 =  pY[0] << 12; // scale to level of conversion table
        Cb  = pCb[0];
        Cr  = pCr[0];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixelLE(pOutput, Y1, Cb, Cr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixelBE(pOutput, Y1, Cb, Cr);
        else
            JPEGPixelRGB((uint32_t *)pOutput, Y1, Cb, Cr);
        // top right block
        Y1 =  pY[DCTSIZE * 2] << 12; // scale to level of conversion table
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixelLE(pOutput + 1, Y1, Cb, Cr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixelBE(pOutput + 1, Y1, Cb, Cr);
        else
            JPEGPixelRGB((uint32_t *)&pOutput[2], Y1, Cb, Cr);
        // bottom left block
        Y1 =  pY[DCTSIZE * 4] << 12; // scale to level of conversion table
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixelLE(pOutput + iPitch, Y1, Cb, Cr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixelBE(pOutput + iPitch, Y1, Cb, Cr);
        else
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 2], Y1, Cb, Cr);
        // bottom right block
        Y1 =  pY[DCTSIZE * 6] << 12; // scale to level of conversion table
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixelLE(pOutput + 1 + iPitch, Y1, Cb, Cr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixelBE(pOutput + 1 + iPitch, Y1, Cb, Cr);
        else
            JPEGPixelRGB((uint32_t *)&pOutput[2 + iPitch * 2], Y1, Cb, Cr);
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER) { // special case of 1/4
        for(iRow = 0; iRow < 2; iRow++) {
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                for(iCol = 0; iCol < 2; iCol++) {
                    // top left block
                    Y1 =  pY[iCol] << 12; // scale to level of conversion table
                    Cb  = pCb[0];
                    Cr  = pCr[0];
                    JPEGPixelLE(pOutput + iCol, Y1, Cb, Cr);
                    // top right block
                    Y1 =  pY[iCol + (DCTSIZE * 2)] << 12; // scale to level of conversion table
                    Cb = pCb[1];
                    Cr = pCr[1];
                    JPEGPixelLE(pOutput + 2 + iCol, Y1, Cb, Cr);
                    // bottom left block
                    Y1 =  pY[iCol + DCTSIZE * 4] << 12; // scale to level of conversion table
                    Cb = pCb[2];
                    Cr = pCr[2];
                    JPEGPixelLE(pOutput + iPitch * 2 + iCol, Y1, Cb, Cr);
                    // bottom right block
                    Y1 =  pY[iCol + DCTSIZE * 6] << 12; // scale to level of conversion table
                    Cb  = pCb[3];
                    Cr  = pCr[3];
                    JPEGPixelLE(pOutput + iPitch * 2 + 2 + iCol, Y1, Cb, Cr);
                } // for each column
            }
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                for(iCol = 0; iCol < 2; iCol++) {
                    // top left block
                    Y1 =  pY[iCol] << 12; // scale to level of conversion table
                    Cb  = pCb[0];
                    Cr  = pCr[0];
                    JPEGPixelBE(pOutput + iCol, Y1, Cb, Cr);
                    // top right block
                    Y1 =  pY[iCol + (DCTSIZE * 2)] << 12; // scale to level of conversion table
                    Cb = pCb[1];
                    Cr = pCr[1];
                    JPEGPixelBE(pOutput + 2 + iCol, Y1, Cb, Cr);
                    // bottom left block
                    Y1 =  pY[iCol + DCTSIZE * 4] << 12; // scale to level of conversion table
                    Cb = pCb[2];
                    Cr = pCr[2];
                    JPEGPixelBE(pOutput + iPitch * 2 + iCol, Y1, Cb, Cr);
                    // bottom right block
                    Y1 =  pY[iCol + DCTSIZE * 6] << 12; // scale to level of conversion table
                    Cb  = pCb[3];
                    Cr  = pCr[3];
                    JPEGPixelBE(pOutput + iPitch * 2 + 2 + iCol, Y1, Cb, Cr);
                } // for each column
            }
            else {   // RGB8888
                for(iCol = 0; iCol < 2; iCol++) {
                    // top left block
                    Y1 =  pY[iCol] << 12; // scale to level of conversion table
                    Cb  = pCb[0];
                    Cr  = pCr[0];
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y1, Cb, Cr);
                    // top right block
                    Y1 =  pY[iCol + (DCTSIZE * 2)] << 12; // scale to level of conversion table
                    Cb = pCb[1];
                    Cr = pCr[1];
                    JPEGPixelRGB((uint32_t *)&pOutput[(2 + iCol) * 2], Y1, Cb, Cr);
                    // bottom left block
                    Y1 =  pY[iCol + DCTSIZE * 4] << 12; // scale to level of conversion table
                    Cb = pCb[2];
                    Cr = pCr[2];
                    JPEGPixelRGB((uint32_t *)&pOutput[(iPitch * 2 + iCol) * 2], Y1, Cb, Cr);
                    // bottom right block
                    Y1 =  pY[iCol + DCTSIZE * 6] << 12; // scale to level of conversion table
                    Cb  = pCb[3];
                    Cr  = pCr[3];
                    JPEGPixelRGB((uint32_t *)&pOutput[(iPitch * 2 + 2 + iCol) * 2], Y1, Cb, Cr);
                } // for each column
            }
            pY += 2; // skip 1 line of source pixels
            pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
        }
        return;
    }
    // full size

#ifdef HAS_NEON
    if(pJPEG->ucPixelType == RGB8888) {
        int8x8_t i88Cr, i88Cb;
        uint8x16_t u816YL, u816YR;
        int16x8_t i168Cr, i168Cb, i168Y, i168Temp;
        int16x4_t i164Constants;
        int16x8_t i168R, i168G, i168B;
        uint8x8_t u88R, u88G, u88B, u88A;
        int16x8x2_t i168Crx2, i168Cbx2;
        uint8x8x4_t u884Hack;
        i164Constants = vld1_s16(&sYCCRGBConstants[0]); // 4 different constants used for "lane" multiplications by scalar
        u88A = vdup_n_u8(0xff); // Alpha set to FF

        for(iRow = 0; iRow < 8; iRow++) { // do 8 rows
            i88Cr = vld1_s8((const int8_t *)pCr); // load 1 row of Cr
            i88Cb = vld1_s8((const int8_t *) pCb); // load 1 row of Cb
            u816YL = vld1q_u8(pY); // load 2 rows of Y (left block)
            u816YR = vld1q_u8(pY + 128); // load 2 rows of Y (right block)
            // top left block
            i168Temp = vdupq_n_s16((int16_t)0x8000); // fix Cr/Cb values by subtracting 0x80
            i168Cr = vshll_n_s8(i88Cr, 8); // widen 8 Cr values and shift left 8
            i168Cb = vshll_n_s8(i88Cb, 8); // widen 8 Cb values and shift left 8
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(u816YL), 4)); // widen and x16 to put on par with Cr/Cb values
            i168Cr = vsubq_s16(i168Cr, i168Temp); // fix Cr/Cb (-0x80)
            i168Cb = vsubq_s16(i168Cb, i168Temp);
            i168Crx2 = vzipq_s16(i168Cr, i168Cr); // double elements in horizonal direction
            i168Cbx2 = vzipq_s16(i168Cb, i168Cb);
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(u816YR),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            // ugly hack due to bug in GCC of vst4 intrinsics
            u884Hack.val[0] = u88B;
            u884Hack.val[1] = u88G;
            u884Hack.val[2] = u88R;
            u884Hack.val[3] = u88A;
            vst4_u8((uint8_t *)pOutput, u884Hack);
            // top right block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(u816YL),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            // ugly hack due to bug in GCC of vst4 intrinsics
            u884Hack.val[0] = u88B;
            u884Hack.val[1] = u88G;
            u884Hack.val[2] = u88R;
            u884Hack.val[3] = u88A;
            vst4_u8((uint8_t *)(pOutput + 16), u884Hack);
            // bottom left block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(u816YR),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (bottom right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            // ugly hack due to bug in GCC of vst4 intrinsics
            u884Hack.val[0] = u88B;
            u884Hack.val[1] = u88G;
            u884Hack.val[2] = u88R;
            u884Hack.val[3] = u88A;
            vst4_u8((uint8_t *)(pOutput + iPitch * 2), u884Hack);
            // bottom right block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            // ugly hack due to bug in GCC of vst4 intrinsics
            u884Hack.val[0] = u88B;
            u884Hack.val[1] = u88G;
            u884Hack.val[2] = u88R;
            u884Hack.val[3] = u88A;
            vst4_u8((uint8_t *)(pOutput + iPitch * 2 + 16), u884Hack);
            pCr += 8;
            pCb += 8;
            if(iRow == 3)  // bottom 4 rows Y values are in 2 other MCUs
                pY += 16 + 192; // skip to other 2 Y blocks
            else
                pY += 16;
            pOutput += 4 * iPitch;
        } // for each row
        return; // 32bpp
    }
    else {   // 16bpp
        int8x8_t i88Cr, i88Cb;
        uint8x16_t u816YL, u816YR;
        int16x8_t i168Cr, i168Cb, i168Y, i168Temp;
        int16x4_t i164Constants;
        int16x8x2_t i168Crx2, i168Cbx2;
        int16x8_t i168R, i168G, i168B;
        uint8x8_t u88R, u88G, u88B;
        uint16x8_t u168Temp, u168Temp2;
        uint8_t ucPixelType = pJPEG->ucPixelType;
        i164Constants = vld1_s16(&sYCCRGBConstants[0]); // 4 different constants used for "lane" multiplications by scalar

        for(iRow = 0; iRow < 8; iRow++) { // do 8 rows
            i88Cr = vld1_s8((const int8_t *) pCr); // load 1 row of Cr
            i88Cb = vld1_s8((const int8_t *) pCb); // load 1 row of Cb
            u816YL = vld1q_u8(pY); // load 2 rows of Y (left block)
            u816YR = vld1q_u8(pY + 128); // load 2 rows of Y (right block)
            // top left block
            i168Temp = vdupq_n_s16((int16_t) 0x8000); // fix Cr/Cb values by subtracting 0x80
            i168Cr = vshll_n_s8(i88Cr, 8); // widen 8 Cr values and shift left 8
            i168Cb = vshll_n_s8(i88Cb, 8); // widen 8 Cb values and shift left 8
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(u816YL), 4)); // widen and x16 to put on par with Cr/Cb values
            i168Cr = vsubq_s16(i168Cr, i168Temp); // fix Cr/Cb (-0x80)
            i168Cb = vsubq_s16(i168Cb, i168Temp);
            i168Crx2 = vzipq_s16(i168Cr, i168Cr); // double elements in horizonal direction
            i168Cbx2 = vzipq_s16(i168Cb, i168Cb);
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_low_u8(u816YR),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            u168Temp = vshll_n_u8(u88R, 8); // place red in upper part of 16-bit words
            u168Temp2 = vshll_n_u8(u88G, 8); // shift green elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 5); // shift green elements right and insert red elements
            u168Temp2 = vshll_n_u8(u88B, 8); // shift blue elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 11); // shift blue elements right and insert
            if(ucPixelType == RGB565_BIG_ENDIAN) {  // reverse the bytes
                u168Temp = vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(u168Temp)));
            }
            vst1q_u16((uint16_t *)pOutput, u168Temp); // top left block
            // top right block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(u816YL),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            u168Temp = vshll_n_u8(u88R, 8); // place red in upper part of 16-bit words
            u168Temp2 = vshll_n_u8(u88G, 8); // shift green elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 5); // shift green elements right and insert red elements
            u168Temp2 = vshll_n_u8(u88B, 8); // shift blue elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 11); // shift blue elements right and insert
            if(ucPixelType == RGB565_BIG_ENDIAN) {  // reverse the bytes
                u168Temp = vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(u168Temp)));
            }
            vst1q_u16((uint16_t *)(pOutput + 8), u168Temp); // top right block
            // bottom left block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[0], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[0], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            i168Y = vreinterpretq_s16_u16(vshll_n_u8(vget_high_u8(u816YR),
                                                     4)); // widen and x16 to put on par with Cr/Cb values (bottom right block)
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            u168Temp = vshll_n_u8(u88R, 8); // place red in upper part of 16-bit words
            u168Temp2 = vshll_n_u8(u88G, 8); // shift green elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 5); // shift green elements right and insert red elements
            u168Temp2 = vshll_n_u8(u88B, 8); // shift blue elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 11); // shift blue elements right and insert
            if(ucPixelType == RGB565_BIG_ENDIAN) {  // reverse the bytes
                u168Temp = vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(u168Temp)));
            }
            vst1q_u16((uint16_t *)(pOutput + iPitch), u168Temp); // bottom left block
            // bottom right block
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 0); // Cr x 1.402
            i168R = vaddq_s16(i168Temp, i168Y); // now we have 8 R values
            i168Temp = vqdmulhq_lane_s16(i168Crx2.val[1], i164Constants, 1); // Cr x -0.71414
            u88R = vqshrun_n_s16(i168R, 4); // narrow and saturate to 8-bit unsigned
            i168G = vaddq_s16(i168Y, i168Temp);
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 2); // Cb x -0.34414
            i168G = vaddq_s16(i168G, i168Temp); // now we have 8 G values
            u88G = vqrshrun_n_s16(i168G, 4); // shift right, narrow and saturate to 8-bit unsigned
            i168Temp = vqdmulhq_lane_s16(i168Cbx2.val[1], i164Constants, 3); // Cb x -1.772
            i168B = vaddq_s16(i168Y, i168Temp); // now we have 8 B values
            u88B = vqrshrun_n_s16(i168B, 4); // shift right, narrow and saturate to 8-bit unsigned
            u168Temp = vshll_n_u8(u88R, 8); // place red in upper part of 16-bit words
            u168Temp2 = vshll_n_u8(u88G, 8); // shift green elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 5); // shift green elements right and insert red elements
            u168Temp2 = vshll_n_u8(u88B, 8); // shift blue elements to top of 16-bit words
            u168Temp = vsriq_n_u16(u168Temp, u168Temp2, 11); // shift blue elements right and insert
            if(ucPixelType == RGB565_BIG_ENDIAN) {  // reverse the bytes
                u168Temp = vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(u168Temp)));
            }
            vst1q_u16((uint16_t *)(pOutput + iPitch + 8), u168Temp); // bottom right block
            // advance to next pair of lines
            pCr += 8;
            pCb += 8;
            if(iRow == 3)  // bottom 4 rows Y values are in 2 other MCUs
                pY += 16 + 192; // skip to other 2 Y blocks
            else
                pY += 16;
            pOutput += iPitch * 2;
        } // for each row
        return;
    } // 16bpp
#endif // HAS_NEON

#ifdef HAS_SSE
    // SSE2 version
    // R = Y                + 1.40200 * Cr
    // G = Y - 0.34414 * Cb + 0.28586 * Cr - Cr
    // B = Y - 0.22800 * Cb + Cb + Cb

    if(pJPEG->ucPixelType == RGB8888) {
        __m128i mmxY, mmxCr, mmxCb, mmxTemp;
        __m128i mmxTemp2, mmxR, mmxG, mmxB;
        __m128i mmxConst1402, mmxConst0714, mmxConst0344, mmxConst1772;
        mmxConst0344 = _mm_load_si128((__m128i *)&s0344[0]);
        mmxConst1402 = _mm_load_si128((__m128i *)&s1402[0]);
        mmxConst0714 = _mm_load_si128((__m128i *)&s0714[0]);
        mmxConst1772 = _mm_load_si128((__m128i *)&s1772[0]);
        iPitch *= 2; // destination is 32-bit values, not 16
        for(iRow = 0; iRow < 8; iRow++) { // do 8 pairs of rows in 4 quadrants
            // left block
            mmxCr = _mm_loadl_epi64((__m128i *)pCr); // load 1 row of Cr
            mmxCb = _mm_loadl_epi64((__m128i *)pCb); // load 1 row of Cb
            mmxY = _mm_loadl_epi64((__m128i *)pY); // load 1 row of Y (top left block)
            mmxTemp = _mm_cmpeq_epi16(mmxY, mmxY); // fix Cr/Cb values by subtracting 0x80
            mmxTemp = _mm_slli_epi16(mmxTemp, 15);  // now has 0x8000, 0x8000...
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxCr = _mm_unpacklo_epi8(mmxTemp2, mmxCr);  // zero-extend 8 Cr values to 16-bits
            mmxCb = _mm_unpacklo_epi8(mmxTemp2, mmxCb);  // zero-extend 8 Cb values to 16-bits (left shifted 8)
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxCr = _mm_add_epi16(mmxCr, mmxTemp); // subtract 0x80
            mmxCb = _mm_add_epi16(mmxCb, mmxTemp);  // subtract 0x80
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpacklo_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344);
            mmxG = _mm_add_epi16(mmxG, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772);
            mmxB = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_cmpeq_epi16(mmxTemp, mmxTemp); // Alpha set to FFFF
            mmxY = _mm_unpacklo_epi8(mmxB, mmxG); // interleave 8 B's and 8 G's
            mmxTemp2 = _mm_unpacklo_epi8(mmxR, mmxTemp); // interlave 8 R's and 8 A's
            mmxTemp = _mm_unpacklo_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            mmxTemp2 = _mm_unpackhi_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            // store first row of pair
            _mm_storeu_si128((__m128i *)pOutput, mmxTemp); // write 4 RGBA pixels
            _mm_storeu_si128((__m128i *)(pOutput + 8), mmxTemp2); // write 4 RGBA pixels
            // second row of left block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 8)); // load 1 row of Y (top left block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxR = _mm_add_epi16(_mm_unpacklo_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344); // Y -= 0.34414*Cb
            mmxG = _mm_add_epi16(mmxG, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772); // B = Y - 1.772*Cb
            mmxB = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_cmpeq_epi16(mmxTemp, mmxTemp); // Alpha set to FFFF
            mmxY = _mm_unpacklo_epi8(mmxB, mmxG); // interleave 8 B's and 8 G's
            mmxTemp2 = _mm_unpacklo_epi8(mmxR, mmxTemp); // interlave 8 R's and 8 A's
            mmxTemp = _mm_unpacklo_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            mmxTemp2 = _mm_unpackhi_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            // store second row of pair
            _mm_storeu_si128((__m128i *)(pOutput + iPitch), mmxTemp); // write 4 RGBA pixels
            _mm_storeu_si128((__m128i *)(pOutput + iPitch + 8), mmxTemp2); // write 4 RGBA pixels
            // right block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 128)); // load 1 row of Y (right block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpackhi_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344);
            mmxG = _mm_add_epi16(mmxG, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772);
            mmxB = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_cmpeq_epi16(mmxTemp, mmxTemp); // Alpha set to FFFF
            mmxY = _mm_unpacklo_epi8(mmxB, mmxG); // interleave 8 B's and 8 G's
            mmxTemp2 = _mm_unpacklo_epi8(mmxR, mmxTemp); // interlave 8 R's and 8 A's
            mmxTemp = _mm_unpacklo_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            mmxTemp2 = _mm_unpackhi_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            // store first row of right block
            _mm_storeu_si128((__m128i *)(pOutput + 16), mmxTemp); // write 4 RGBA pixels
            _mm_storeu_si128((__m128i *)(pOutput + 24), mmxTemp2); // write 4 RGBA pixels
            // prepare second row of right block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 136)); // load 1 row of Y (right block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpackhi_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344);
            mmxG = _mm_add_epi16(mmxG, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772);
            mmxB = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_cmpeq_epi16(mmxTemp, mmxTemp); // Alpha set to FFFF
            mmxY = _mm_unpacklo_epi8(mmxB, mmxG); // interleave 8 B's and 8 G's
            mmxTemp2 = _mm_unpacklo_epi8(mmxR, mmxTemp); // interlave 8 R's and 8 A's
            mmxTemp = _mm_unpacklo_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            mmxTemp2 = _mm_unpackhi_epi16(mmxY, mmxTemp2); // interleave 4 BG's and 4 RA's
            // store second row of right block
            _mm_storeu_si128((__m128i *)(pOutput + iPitch + 16), mmxTemp); // write 4 RGBA pixels
            _mm_storeu_si128((__m128i *)(pOutput + iPitch + 24), mmxTemp2); // write 4 RGBA pixels

            pOutput += iPitch * 2;
            pCr += 8;
            pCb += 8;
            if(iRow == 3)  // bottom 4 rows Y values are in 2 other MCUs
                pY += 16 + 192; // skip to other 2 Y blocks
            else
                pY += 16;
        } // for each row
        return;
    }
    else {   // 16-bit pixels
        __m128i mmxY, mmxCr, mmxCb, mmxTemp;
        __m128i mmxTemp2, mmxR, mmxG, mmxB;
        __m128i mmxConst1402, mmxConst0714, mmxConst0344, mmxConst1772;
        mmxConst0344 = _mm_load_si128((__m128i *)&s0344[0]);
        mmxConst1402 = _mm_load_si128((__m128i *)&s1402[0]);
        mmxConst0714 = _mm_load_si128((__m128i *)&s0714[0]);
        mmxConst1772 = _mm_load_si128((__m128i *)&s1772[0]);
        for(iRow = 0; iRow < 8; iRow++) { // do 8 pairs of rows in 4 quadrants
            // left block
            mmxCr = _mm_loadl_epi64((__m128i *)pCr); // load 1 row of Cr
            mmxCb = _mm_loadl_epi64((__m128i *)pCb); // load 1 row of Cb
            mmxY = _mm_loadl_epi64((__m128i *)pY); // load 1 row of Y (top left block)
            mmxTemp = _mm_cmpeq_epi16(mmxY, mmxY); // fix Cr/Cb values by subtracting 0x80
            mmxTemp = _mm_slli_epi16(mmxTemp, 7);  // now has 0xff80, 0xff80...
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxCr = _mm_unpacklo_epi8(mmxCr, mmxTemp2);  // zero-extend 8 Cr values to 16-bits
            mmxCb = _mm_unpacklo_epi8(mmxCb, mmxTemp2);  // zero-extend 8 Cb values to 16-bits
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxCr = _mm_add_epi16(mmxCr, mmxTemp); // subtract 0x80
            mmxCb = _mm_add_epi16(mmxCb, mmxTemp);  // subtract 0x80
            mmxCr = _mm_slli_epi16(mmxCr, 8); // put in top half of 16-bits
            mmxCb = _mm_slli_epi16(mmxCb, 8); // put in top half of 16-bits
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpacklo_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344); // Y -= 0.34414*Cb
            mmxG = _mm_add_epi16(mmxG, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772); // B = Y - 1.772*Cb
            mmxB = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_setzero_si128(); // interleave with 0 to get back to 16-bit values
            mmxR = _mm_unpacklo_epi8(mmxR, mmxTemp); // zero-extend to 16-bits again
            mmxB = _mm_unpacklo_epi8(mmxB, mmxTemp);
            mmxG = _mm_unpacklo_epi8(mmxG, mmxTemp);
            mmxR = _mm_srli_epi16(mmxR, 3); // reduce to 5-bits
            mmxR = _mm_slli_epi16(mmxR, 11); // set in proper position
            mmxB = _mm_srli_epi16(mmxB, 3); // reduce to 5-bits
            mmxG = _mm_srli_epi16(mmxG, 2); // reduce to 6-bits
            mmxG = _mm_slli_epi16(mmxG, 5); // set in proper position
            mmxTemp = _mm_or_si128(mmxR, mmxG); // R+G
            mmxTemp = _mm_or_si128(mmxTemp, mmxB); // R+G+B
            // store first row of pair
            _mm_storeu_si128((__m128i *)pOutput, mmxTemp); // write 8 RGB565 pixels
            // second row of left block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 8)); // load 1 row of Y (top left block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxR = _mm_add_epi16(_mm_unpacklo_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344);
            mmxG = _mm_add_epi16(mmxG, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772); // B = Y - 1.772*Cb
            mmxB = _mm_add_epi16(mmxY, _mm_unpacklo_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_setzero_si128(); // interleave with 0 to get back to 16-bit values
            mmxR = _mm_unpacklo_epi8(mmxR, mmxTemp); // zero-extend to 16-bits again
            mmxB = _mm_unpacklo_epi8(mmxB, mmxTemp);
            mmxG = _mm_unpacklo_epi8(mmxG, mmxTemp);
            mmxR = _mm_srli_epi16(mmxR, 3); // reduce to 5-bits
            mmxR = _mm_slli_epi16(mmxR, 11); // set in proper position
            mmxB = _mm_srli_epi16(mmxB, 3); // reduce to 5-bits
            mmxG = _mm_srli_epi16(mmxG, 2); // reduce to 6-bits
            mmxG = _mm_slli_epi16(mmxG, 5); // set in proper position
            mmxTemp = _mm_or_si128(mmxR, mmxG); // R+G
            mmxTemp = _mm_or_si128(mmxTemp, mmxB); // R+G+B
            // store second row of pair
            _mm_storeu_si128((__m128i *)(pOutput + iPitch), mmxTemp); // write 8 RGB565 pixels
            // right block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 128)); // load 1 row of Y (right block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpackhi_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344); // Y -= 0.34414*Cb
            mmxG = _mm_add_epi16(mmxG, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772); // B = Y - 1.772*Cb
            mmxB = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_setzero_si128(); // interleave with 0 to get back to 16-bit values
            mmxR = _mm_unpacklo_epi8(mmxR, mmxTemp); // zero-extend to 16-bits again
            mmxB = _mm_unpacklo_epi8(mmxB, mmxTemp);
            mmxG = _mm_unpacklo_epi8(mmxG, mmxTemp);
            mmxR = _mm_srli_epi16(mmxR, 3); // reduce to 5-bits
            mmxR = _mm_slli_epi16(mmxR, 11); // set in proper position
            mmxB = _mm_srli_epi16(mmxB, 3); // reduce to 5-bits
            mmxG = _mm_srli_epi16(mmxG, 2); // reduce to 6-bits
            mmxG = _mm_slli_epi16(mmxG, 5); // set in proper position
            mmxTemp = _mm_or_si128(mmxR, mmxG); // R+G
            mmxTemp = _mm_or_si128(mmxTemp, mmxB); // R+G+B
            // store first row of right block
            _mm_storeu_si128((__m128i *)(pOutput + 16), mmxTemp); // write 8 RGB565 pixels
            // prepare second row of right block
            mmxY = _mm_loadl_epi64((__m128i *)(pY + 136)); // load 1 row of Y (right block)
            mmxTemp2 = _mm_setzero_si128(); // zero it to use to set upper bits to 0
            mmxY = _mm_unpacklo_epi8(mmxY, mmxTemp2);  // zero-extend 8 Y values to 16-bits
            mmxY = _mm_slli_epi16(mmxY, 4); // x16 to put on par with Cr/Cb values
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst1402); // almost ready with R
            mmxR = _mm_add_epi16(_mm_unpackhi_epi16(mmxTemp, mmxTemp), mmxY); // now we have 8 R values
            mmxR = _mm_srai_epi16(mmxR, 4);
            mmxR = _mm_packus_epi16(mmxR, mmxR);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCr, mmxConst0714); // Y-0.71414*Cr
            mmxG = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp));
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst0344);
            mmxG = _mm_add_epi16(mmxG, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 G values
            mmxG = _mm_srai_epi16(mmxG, 4);
            mmxG = _mm_packus_epi16(mmxG, mmxG);  // i16->u8 bit and saturate
            mmxTemp = _mm_mulhi_epi16(mmxCb, mmxConst1772); // B = Y - 1.772*Cb
            mmxB = _mm_add_epi16(mmxY, _mm_unpackhi_epi16(mmxTemp, mmxTemp)); // now we have 8 B values
            mmxB = _mm_srai_epi16(mmxB, 4);
            mmxB = _mm_packus_epi16(mmxB, mmxB);  // i16->u8 bit and saturate
            mmxTemp = _mm_setzero_si128(); // interleave with 0 to get back to 16-bit values
            mmxR = _mm_unpacklo_epi8(mmxR, mmxTemp); // zero-extend to 16-bits again
            mmxB = _mm_unpacklo_epi8(mmxB, mmxTemp);
            mmxG = _mm_unpacklo_epi8(mmxG, mmxTemp);
            mmxR = _mm_srli_epi16(mmxR, 3); // reduce to 5-bits
            mmxR = _mm_slli_epi16(mmxR, 11); // set in proper position
            mmxB = _mm_srli_epi16(mmxB, 3); // reduce to 5-bits
            mmxG = _mm_srli_epi16(mmxG, 2); // reduce to 6-bits
            mmxG = _mm_slli_epi16(mmxG, 5); // set in proper position
            mmxTemp = _mm_or_si128(mmxR, mmxG); // R+G
            mmxTemp = _mm_or_si128(mmxTemp, mmxB); // R+G+B
            // store second row of right block
            _mm_storeu_si128((__m128i *)(pOutput + 16 + iPitch), mmxTemp); // write 8 RGB565 pixels

            pOutput += iPitch * 2;
            pCr += 8;
            pCb += 8;
            if(iRow == 3)  // bottom 4 rows Y values are in 2 other MCUs
                pY += 16 + 192; // skip to other 2 Y blocks
            else
                pY += 16;
        } // for each row
        return;
    } // 16bpp
#endif // HAS_SSE
    /* Reference C code */
    /* Convert YCC pixels into RGB pixels and store in output image */
    iYCount = 4;
    bUseOdd1 = bUseOdd2 = 1; // assume odd column can be used
    if((x + 15) >= pJPEG->iWidth) {
        iCol = (((pJPEG->iWidth & 15) + 1) >> 1);
        if(iCol >= 4) {
            iXCount1 = 4;
            iXCount2 = iCol - 4;
            if((pJPEG->iWidth & 1) && (iXCount2 * 2) + 8 + (x * 16) > pJPEG->iWidth)
                bUseOdd2 = 0;
        }
        else {
            iXCount1 = iCol;
            iXCount2 = 0;
            if((pJPEG->iWidth & 1) && (iXCount1 * 2) + (x * 16) > pJPEG->iWidth)
                bUseOdd1 = 0;
        }
    }
    else
        iXCount1 = iXCount2 = 4;
    for(iRow = 0; iRow < iYCount; iRow++) { // up to 4 rows to do
        for(iCol = 0; iCol < iXCount1; iCol++) { // up to 4 cols to do
            // for top left block
            Y1 = pY[iCol * 2];
            Y2 = pY[iCol * 2 + 1];
            Y3 = pY[iCol * 2 + 8];
            Y4 = pY[iCol * 2 + 9];
            Y1 <<= 12;  // scale to level of conversion table
            Y2 <<= 12;
            Y3 <<= 12;
            Y4 <<= 12;
            Cb = pCb[iCol];
            Cr = pCr[iCol];
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2LE(pOutput + (iCol << 1), Y1, Y2, Cb, Cr);
                    JPEGPixel2LE(pOutput + iPitch + (iCol << 1), Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelLE(pOutput + (iCol << 1), Y1, Cb, Cr);
                    JPEGPixelLE(pOutput + iPitch + (iCol << 1), Y3, Cb, Cr);
                }
            }
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2BE(pOutput + (iCol << 1), Y1, Y2, Cb, Cr);
                    JPEGPixel2BE(pOutput + iPitch + (iCol << 1), Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelBE(pOutput + (iCol << 1), Y1, Cb, Cr);
                    JPEGPixelBE(pOutput + iPitch + (iCol << 1), Y3, Cb, Cr);
                }
            }
            else {   // RGB8888
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2RGB((uint32_t *)&pOutput[iCol << 2], Y1, Y2, Cb, Cr);
                    JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch + (iCol << 1))], Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol << 2], Y1, Cb, Cr);
                    JPEGPixelRGB((uint32_t *)&pOutput[(iPitch + (iCol << 1)) * 2], Y3, Cb, Cr);
                }
            } // RGB8888
            // for top right block
            if(iCol < iXCount2) {
                Y1 = pY[iCol * 2 + DCTSIZE * 2];
                Y2 = pY[iCol * 2 + 1 + DCTSIZE * 2];
                Y3 = pY[iCol * 2 + 8 + DCTSIZE * 2];
                Y4 = pY[iCol * 2 + 9 + DCTSIZE * 2];
                Y1 <<= 12;  // scale to level of conversion table
                Y2 <<= 12;
                Y3 <<= 12;
                Y4 <<= 12;
                Cb = pCb[iCol + 4];
                Cr = pCr[iCol + 4];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2LE(pOutput + 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                        JPEGPixel2LE(pOutput + iPitch + 8 + (iCol << 1), Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelLE(pOutput + 8 + (iCol << 1), Y1, Cb, Cr);
                        JPEGPixelLE(pOutput + iPitch + 8 + (iCol << 1), Y3, Cb, Cr);
                    }
                }
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2BE(pOutput + 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                        JPEGPixel2BE(pOutput + iPitch + 8 + (iCol << 1), Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelBE(pOutput + 8 + (iCol << 1), Y1, Cb, Cr);
                        JPEGPixelBE(pOutput + iPitch + 8 + (iCol << 1), Y3, Cb, Cr);
                    }
                }
                else {   // RGB8888
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2RGB((uint32_t *)&pOutput[16 + (iCol << 2)], Y1, Y2, Cb, Cr);
                        JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch + 8 + (iCol << 1))], Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelRGB((uint32_t *)&pOutput[16 + (iCol << 2)], Y1, Cb, Cr);
                        JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch + 8 + (iCol << 1))], Y3, Cb, Cr);
                    }
                } // RGB8888
            }
            // for bottom left block
            Y1 = pY[iCol * 2 + DCTSIZE * 4];
            Y2 = pY[iCol * 2 + 1 + DCTSIZE * 4];
            Y3 = pY[iCol * 2 + 8 + DCTSIZE * 4];
            Y4 = pY[iCol * 2 + 9 + DCTSIZE * 4];
            Y1 <<= 12;  // scale to level of conversion table
            Y2 <<= 12;
            Y3 <<= 12;
            Y4 <<= 12;
            Cb = pCb[iCol + 32];
            Cr = pCr[iCol + 32];
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2LE(pOutput + iPitch * 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                    JPEGPixel2LE(pOutput + iPitch * 9 + (iCol << 1), Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelLE(pOutput + iPitch * 8 + (iCol << 1), Y1, Cb, Cr);
                    JPEGPixelLE(pOutput + iPitch * 9 + (iCol << 1), Y3, Cb, Cr);
                }
            }
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2BE(pOutput + iPitch * 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                    JPEGPixel2BE(pOutput + iPitch * 9 + (iCol << 1), Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelBE(pOutput + iPitch * 8 + (iCol << 1), Y1, Cb, Cr);
                    JPEGPixelBE(pOutput + iPitch * 9 + (iCol << 1), Y3, Cb, Cr);
                }
            }
            else {   // RGB8888
                if(bUseOdd1 || iCol != (iXCount1 - 1)) { // only render if it won't go off the right edge
                    JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch * 8 + (iCol << 1))], Y1, Y2, Cb, Cr);
                    JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch * 9 + (iCol << 1))], Y3, Y4, Cb, Cr);
                }
                else {
                    JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch * 8 + (iCol << 1))], Y1, Cb, Cr);
                    JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch * 9 + (iCol << 1))], Y3, Cb, Cr);
                }
            } // RGB8888
            // for bottom right block
            if(iCol < iXCount2) {
                Y1 = pY[iCol * 2 + DCTSIZE * 6];
                Y2 = pY[iCol * 2 + 1 + DCTSIZE * 6];
                Y3 = pY[iCol * 2 + 8 + DCTSIZE * 6];
                Y4 = pY[iCol * 2 + 9 + DCTSIZE * 6];
                Y1 <<= 12;  // scale to level of conversion table
                Y2 <<= 12;
                Y3 <<= 12;
                Y4 <<= 12;
                Cb = pCb[iCol + 36];
                Cr = pCr[iCol + 36];
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2LE(pOutput + iPitch * 8 + 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                        JPEGPixel2LE(pOutput + iPitch * 9 + 8 + (iCol << 1), Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelLE(pOutput + iPitch * 8 + 8 + (iCol << 1), Y1, Cb, Cr);
                        JPEGPixelLE(pOutput + iPitch * 9 + 8 + (iCol << 1), Y3, Cb, Cr);
                    }
                }
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2BE(pOutput + iPitch * 8 + 8 + (iCol << 1), Y1, Y2, Cb, Cr);
                        JPEGPixel2BE(pOutput + iPitch * 9 + 8 + (iCol << 1), Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelBE(pOutput + iPitch * 8 + 8 + (iCol << 1), Y1, Cb, Cr);
                        JPEGPixelBE(pOutput + iPitch * 9 + 8 + (iCol << 1), Y3, Cb, Cr);
                    }
                }
                else {   // RGB8888
                    if(bUseOdd2 || iCol != (iXCount2 - 1)) { // only render if it won't go off the right edge
                        JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch * 8 + 8 + (iCol << 1))], Y1, Y2, Cb, Cr);
                        JPEGPixel2RGB((uint32_t *)&pOutput[2 * (iPitch * 9 + 8 + (iCol << 1))], Y3, Y4, Cb, Cr);
                    }
                    else {
                        JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch * 8 + 8 + (iCol << 1))], Y1, Cb, Cr);
                        JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch * 9 + 8 + (iCol << 1))], Y3, Cb, Cr);
                    }
                } // RGB8888
            }
        } // for each column
        pY += 16; // skip to next line of source pixels
        pCb += 8;
        pCr += 8;
        pOutput += iPitch * 2;
        if(pJPEG->ucPixelType == RGB8888) {
            pOutput += iPitch * 2;
        }
    }
} /* JPEGPutMCU22() */

static void JPEGPutMCU12(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    uint32_t Cr, Cb;
    signed int Y1, Y2;
    int iRow, iCol, iXCount, iYCount;
    uint8_t * pY, * pCr, * pCb;
    uint16_t * pOutput = &pJPEG->usPixels[x];

    if(pJPEG->ucPixelType == RGB8888) {
        pOutput += x; // 4 bytes per pixel, not 2
    }

    pY  = (uint8_t *)&pJPEG->sMCUs[0 * DCTSIZE];
    pCb = (uint8_t *)&pJPEG->sMCUs[2 * DCTSIZE];
    pCr = (uint8_t *)&pJPEG->sMCUs[3 * DCTSIZE];

    if(pJPEG->iOptions & JPEG_SCALE_HALF) {
        for(iRow = 0; iRow < 8; iRow++) {
            for(iCol = 0; iCol < 4; iCol++) {
                Y1 = (pY[0] + pY[1] + pY[8] + pY[9]) << 10;
                Cb = (pCb[0] + pCb[1] + 1) >> 1;
                Cr = (pCr[0] + pCr[1] + 1) >> 1;
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol, Y1, Cb, Cr);
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol, Y1, Cb, Cr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y1, Cb, Cr);
                pCb += 2;
                pCr += 2;
                pY += 2;
            }
            pY += 8;
            if(iRow == 3)  // skip to next Y MCU block
                pY += 64;
            pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
        }
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
        Y1 = pY[0] << 12;
        Y2 = pY[DCTSIZE * 2] << 12;
        Cb = pCb[0];
        Cr = pCr[0];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            JPEGPixelLE(pOutput, Y1, Cb, Cr);
            JPEGPixelLE(pOutput + iPitch, Y2, Cb, Cr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            JPEGPixelBE(pOutput, Y1, Cb, Cr);
            JPEGPixelBE(pOutput + iPitch, Y2, Cb, Cr);
        }
        else { // RGB8888
            JPEGPixelRGB((uint32_t *)pOutput, Y1, Cb, Cr);
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 2], Y2, Cb, Cr);
        }
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
        // draw a 2x4 block
        Y1 = pY[0] << 12;
        Y2 = pY[2] << 12;
        Cb = pCb[0];
        Cr = pCr[0];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            JPEGPixelLE(pOutput, Y1, Cb, Cr);
            JPEGPixelLE(pOutput + iPitch, Y2, Cb, Cr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            JPEGPixelBE(pOutput, Y1, Cb, Cr);
            JPEGPixelBE(pOutput + iPitch, Y2, Cb, Cr);
        }
        else {   // RGB8888
            JPEGPixelRGB((uint32_t *)&pOutput, Y1, Cb, Cr);
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 2], Y2, Cb, Cr);
        }
        Y1 = pY[1] << 12;
        Y2 = pY[3] << 12;
        Cb = pCb[1];
        Cr = pCr[1];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            JPEGPixelLE(pOutput + 1, Y1, Cb, Cr);
            JPEGPixelLE(pOutput + 1 + iPitch, Y2, Cb, Cr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            JPEGPixelBE(pOutput + 1, Y1, Cb, Cr);
            JPEGPixelBE(pOutput + 1 + iPitch, Y2, Cb, Cr);
        }
        else {   // RGB8888
            JPEGPixelRGB((uint32_t *)&pOutput[2], Y1, Cb, Cr);
            JPEGPixelRGB((uint32_t *)&pOutput[(1 + iPitch) * 2], Y2, Cb, Cr);
        }
        pY += DCTSIZE * 2; // next Y block below
        Y1 = pY[0] << 12;
        Y2 = pY[2] << 12;
        Cb = pCb[2];
        Cr = pCr[2];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            JPEGPixelLE(pOutput + iPitch * 2, Y1, Cb, Cr);
            JPEGPixelLE(pOutput + iPitch * 3, Y2, Cb, Cr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            JPEGPixelBE(pOutput + iPitch * 2, Y1, Cb, Cr);
            JPEGPixelBE(pOutput + iPitch * 3, Y2, Cb, Cr);
        }
        else {   // RGB8888
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 4], Y1, Cb, Cr);
            JPEGPixelRGB((uint32_t *)&pOutput[iPitch * 6], Y2, Cb, Cr);
        }
        Y1 = pY[1] << 12;
        Y2 = pY[3] << 12;
        Cb = pCb[3];
        Cr = pCr[3];
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
            JPEGPixelLE(pOutput + 1 + iPitch * 2, Y1, Cb, Cr);
            JPEGPixelLE(pOutput + 1 + iPitch * 3, Y2, Cb, Cr);
        }
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
            JPEGPixelBE(pOutput + 1 + iPitch * 2, Y1, Cb, Cr);
            JPEGPixelBE(pOutput + 1 + iPitch * 3, Y2, Cb, Cr);
        }
        else { // RGB8888
            JPEGPixelRGB((uint32_t *)&pOutput[2 + iPitch * 4], Y1, Cb, Cr);
            JPEGPixelRGB((uint32_t *)&pOutput[2 + iPitch * 6], Y2, Cb, Cr);
        }
        return;
    }
    // full size
    /* Convert YCC pixels into RGB pixels and store in output image */
    iYCount = 16;
    iXCount = 8;
    for(iRow = 0; iRow < iYCount; iRow += 2) { // up to 16 rows to do
        for(iCol = 0; iCol < iXCount; iCol++) { // up to 8 cols to do
            Y1 = pY[iCol];
            Y2 = pY[iCol + 8];
            Y1 <<= 12;  // scale to level of conversion table
            Y2 <<= 12;
            Cb = pCb[iCol];
            Cr = pCr[iCol];
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN) {
                JPEGPixelLE(pOutput + iCol, Y1, Cb, Cr);
                JPEGPixelLE(pOutput + iPitch + iCol, Y2, Cb, Cr);
            }
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN) {
                JPEGPixelBE(pOutput + iCol, Y1, Cb, Cr);
                JPEGPixelBE(pOutput + iPitch + iCol, Y2, Cb, Cr);
            }
            else {   // RGB8888
                JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y1, Cb, Cr);
                JPEGPixelRGB((uint32_t *)&pOutput[2 * (iPitch + iCol)], Y2, Cb, Cr);
            } // RGB8888
        }
        pY += 16; // skip to next 2 lines of source pixels
        if(iRow == 6)  // next MCU block, skip ahead to correct spot
            pY += (128 - 64);
        pCb += 8;
        pCr += 8;
        pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 4 : iPitch * 2; // next 2 lines of dest pixels
    }
} /* JPEGPutMCU12() */

static void JPEGPutMCU21(JPEGIMAGE * pJPEG, int x, int iPitch)
{
    int iCr, iCb;
    signed int Y1, Y2;
    int iCol;
    int iRow;
    uint8_t * pY, * pCr, * pCb;
    uint16_t * pOutput = &pJPEG->usPixels[x];

    if(pJPEG->ucPixelType == RGB8888) {
        pOutput += x; // 4 bytes per pixel, not 2
    }

    pY  = (uint8_t *)&pJPEG->sMCUs[0 * DCTSIZE];
    pCb = (uint8_t *)&pJPEG->sMCUs[2 * DCTSIZE];
    pCr = (uint8_t *)&pJPEG->sMCUs[3 * DCTSIZE];

    if(pJPEG->iOptions & JPEG_SCALE_HALF) {
        for(iRow = 0; iRow < 4; iRow++) {
            for(iCol = 0; iCol < 4; iCol++) {
                // left block
                iCr = (pCr[0] + pCr[8] + 1) >> 1;
                iCb = (pCb[0] + pCb[8] + 1) >> 1;
                Y1 = (signed int)(pY[0] + pY[1] + pY[8] + pY[9]) << 10;
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol, Y1, iCb, iCr);
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol, Y1, iCb, iCr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[iCol * 2], Y1, iCb, iCr);
                // right block
                iCr = (pCr[4] + pCr[12] + 1) >> 1;
                iCb = (pCb[4] + pCb[12] + 1) >> 1;
                Y1 = (signed int)(pY[128] + pY[129] + pY[136] + pY[137]) << 10;
                if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                    JPEGPixelLE(pOutput + iCol + 4, Y1, iCb, iCr);
                else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                    JPEGPixelBE(pOutput + iCol + 4, Y1, iCb, iCr);
                else
                    JPEGPixelRGB((uint32_t *)&pOutput[(iCol + 4) * 2], Y1, iCb, iCr);
                pCb++;
                pCr++;
                pY += 2;
            }
            pCb += 12;
            pCr += 12;
            pY += 8;
            pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
        }
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
        // draw 2 pixels
        iCr = pCr[0];
        iCb = pCb[0];
        Y1 = (signed int)(pY[0]) << 12;
        Y2 = (signed int)(pY[DCTSIZE * 2]) << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixel2LE(pOutput, Y1, Y2, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixel2BE(pOutput, Y1, Y2, iCb, iCr);
        else
            JPEGPixel2RGB((uint32_t *)pOutput, Y1, Y2, iCb, iCr);
        return;
    }
    if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
        // draw 4x2 pixels
        // top left
        iCr = pCr[0];
        iCb = pCb[0];
        Y1 = (signed int)(pY[0]) << 12;
        Y2 = (signed int)(pY[1]) << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixel2LE(pOutput, Y1, Y2, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixel2BE(pOutput, Y1, Y2, iCb, iCr);
        else
            JPEGPixel2RGB((uint32_t *)pOutput, Y1, Y2, iCb, iCr);
        // top right
        iCr = pCr[1];
        iCb = pCb[1];
        Y1 = (signed int)pY[DCTSIZE * 2] << 12;
        Y2 = (signed int)pY[DCTSIZE * 2 + 1] << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixel2LE(pOutput + 2, Y1, Y2, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixel2BE(pOutput + 2, Y1, Y2, iCb, iCr);
        else
            JPEGPixel2RGB((uint32_t *)&pOutput[4], Y1, Y2, iCb, iCr);
        // bottom left
        iCr = pCr[2];
        iCb = pCb[2];
        Y1 = (signed int)(pY[2]) << 12;
        Y2 = (signed int)(pY[3]) << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixel2LE(pOutput + iPitch, Y1, Y2, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixel2BE(pOutput + iPitch, Y1, Y2, iCb, iCr);
        else
            JPEGPixel2RGB((uint32_t *)&pOutput[iPitch * 2], Y1, Y2, iCb, iCr);
        // bottom right
        iCr = pCr[3];
        iCb = pCb[3];
        Y1 = (signed int)pY[DCTSIZE * 2 + 2] << 12;
        Y2 = (signed int)pY[DCTSIZE * 2 + 3] << 12;
        if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
            JPEGPixel2LE(pOutput + iPitch + 2, Y1, Y2, iCb, iCr);
        else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
            JPEGPixel2BE(pOutput + iPitch + 2, Y1, Y2, iCb, iCr);
        else
            JPEGPixel2RGB((uint32_t *)&pOutput[(iPitch + 2) * 2], Y1, Y2, iCb, iCr);
        return;
    }
    // Full size
    /* Convert YCC pixels into RGB pixels and store in output image */
    for(iRow = 0; iRow < 8; iRow++) { // up to 8 rows to do
        for(iCol = 0; iCol < 4; iCol++) { // up to 4x2 cols to do
            // left block
            iCr = *pCr++;
            iCb = *pCb++;
            Y1 = (signed int)(*pY++) << 12;
            Y2 = (signed int)(*pY++) << 12;
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                JPEGPixel2LE(pOutput + iCol * 2, Y1, Y2, iCb, iCr);
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                JPEGPixel2BE(pOutput + iCol * 2, Y1, Y2, iCb, iCr);
            else // RGB8888
                JPEGPixel2RGB((uint32_t *)&pOutput[iCol * 4], Y1, Y2, iCb, iCr);
            // right block
            iCr = pCr[3];
            iCb = pCb[3];
            Y1 = (signed int)pY[126] << 12;
            Y2 = (signed int)pY[127] << 12;
            if(pJPEG->ucPixelType == RGB565_LITTLE_ENDIAN)
                JPEGPixel2LE(pOutput + 8 + iCol * 2, Y1, Y2, iCb, iCr);
            else if(pJPEG->ucPixelType == RGB565_BIG_ENDIAN)
                JPEGPixel2BE(pOutput + 8 + iCol * 2, Y1, Y2, iCb, iCr);
            else // RGB8888
                JPEGPixel2RGB((uint32_t *)&pOutput[16 + iCol * 4], Y1, Y2, iCb, iCr);
        } // for col
        pCb += 4;
        pCr += 4;
        pOutput += (pJPEG->ucPixelType == RGB8888) ? iPitch * 2 : iPitch;
    } // for row
} /* JPEGPutMCU21() */

// Dither the 8-bit gray pixels into 1, 2, or 4-bit gray
static void JPEGDither(JPEGIMAGE * pJPEG, int iWidth, int iHeight)
{
    int x, y, xmask = 0, iDestPitch = 0;
    int32_t cNew, lFErr, v = 0, h;
    int32_t e1, e2, e3, e4;
    uint8_t cOut, ucPixelType; // forward errors for gray
    uint8_t * pSrc, * pDest, * errors, * pErrors = NULL, * d, * pPixels; // destination 8bpp image
    uint8_t pixelmask = 0, shift = 0;

    ucPixelType = pJPEG->ucPixelType;
    errors = (uint8_t *)pJPEG->usPixels; // plenty of space here
    errors[0] = errors[1] = errors[2] = 0;
    pDest = pSrc = pJPEG->pDitherBuffer; // write the new pixels over the original
    switch(ucPixelType) {
        case FOUR_BIT_DITHERED:
            iDestPitch = (iWidth + 1) / 2;
            pixelmask = 0xf0;
            shift = 4;
            xmask = 1;
            break;
        case TWO_BIT_DITHERED:
            iDestPitch = (iWidth + 3) / 4;
            pixelmask = 0xc0;
            shift = 2;
            xmask = 3;
            break;
        case ONE_BIT_DITHERED:
            iDestPitch = (iWidth + 7) / 8;
            pixelmask = 0x80;
            shift = 1;
            xmask = 7;
            break;
    }
    for(y = 0; y < iHeight; y++) {
        pPixels = &pSrc[y * iWidth];
        d = &pDest[y * iDestPitch];
        pErrors = &errors[1]; // point to second pixel to avoid boundary check
        lFErr = 0;
        cOut = 0;
        for(x = 0; x < iWidth; x++) {
            cNew = *pPixels++; // get grayscale uint8_t pixel
            // add forward error
            cNew += lFErr;
            if(cNew > 255) cNew = 255;      // clip to uint8_t
            cOut <<= shift;                 // pack new pixels into a byte
            cOut |= (cNew >> (8 - shift));  // keep top N bits
            if((x & xmask) == xmask) {      // store it when the byte is full
                *d++ = cOut;
                cOut = 0;
            }
            // calculate the Floyd-Steinberg error for this pixel
            v = cNew - (cNew & pixelmask); // new error for N-bit gray output (always positive)
            h = v >> 1;
            e1 = (7 * h) >> 3; // 7/16
            e2 = h - e1;  // 1/16
            e3 = (5 * h) >> 3; // 5/16
            e4 = h - e3;  // 3/16
            // distribute error to neighbors
            lFErr = e1 + pErrors[1];
            pErrors[1] = (uint8_t)e2;
            pErrors[0] += e3;
            pErrors[-1] += e4;
            pErrors++;
        } // for x
    } // for y
} /* JPEGDither() */

//
// Decode the image
// returns 0 for error, 1 for success
//
static int DecodeJPEG(JPEGIMAGE * pJPEG)
{
    int cx, cy, x, y, mcuCX, mcuCY;
    int iLum0, iLum1, iLum2, iLum3, iCr, iCb;
    signed int iDCPred0, iDCPred1, iDCPred2;
    int i, iQuant1, iQuant2, iQuant3, iErr;
    int iSkipMask, bSkipRow;
    uint8_t c;
    int iMCUCount, xoff, iPitch, bThumbnail = 0;
    int bContinue = 1; // early exit if the DRAW callback wants to stop
    uint32_t l, * pl;
    unsigned char cDCTable0, cACTable0, cDCTable1, cACTable1, cDCTable2, cACTable2;
    JPEGDRAW jd;
    int iMaxFill = 16, iScaleShift = 0;

    // Requested the Exif thumbnail
    if(pJPEG->iOptions & JPEG_EXIF_THUMBNAIL) {
        if(pJPEG->iThumbData == 0 || pJPEG->iThumbWidth == 0) { // doesn't exist
            pJPEG->iError = JPEG_INVALID_PARAMETER;
            return 0;
        }
        if(!JPEGParseInfo(pJPEG, 1))  // parse the embedded thumbnail file header
            return 0; // something went wrong
    }
    // Fast downscaling options
    if(pJPEG->iOptions & JPEG_SCALE_HALF)
        iScaleShift = 1;
    else if(pJPEG->iOptions & JPEG_SCALE_QUARTER) {
        iScaleShift = 2;
        iMaxFill = 1;
    }
    else if(pJPEG->iOptions & JPEG_SCALE_EIGHTH) {
        iScaleShift = 3;
        iMaxFill = 1;
        bThumbnail = 1;
    }

    // reorder and fix the quantization table for decoding
    JPEGFixQuantD(pJPEG);
    pJPEG->bb.ulBits = MOTOLONG(&pJPEG->ucFileBuf[0]); // preload first 4/8 bytes
    pJPEG->bb.pBuf = pJPEG->ucFileBuf;
    pJPEG->bb.ulBitOff = 0;

    cDCTable0 = pJPEG->JPCI[0].dc_tbl_no;
    cACTable0 = pJPEG->JPCI[0].ac_tbl_no;
    cDCTable1 = pJPEG->JPCI[1].dc_tbl_no;
    cACTable1 = pJPEG->JPCI[1].ac_tbl_no;
    cDCTable2 = pJPEG->JPCI[2].dc_tbl_no;
    cACTable2 = pJPEG->JPCI[2].ac_tbl_no;
    iDCPred0 = iDCPred1 = iDCPred2 = mcuCX = mcuCY = 0;

    switch(pJPEG->ucSubSample) { // set up the parameters for the different subsampling options
        case 0x00: // fake value to handle grayscale
        case 0x01: // fake value to handle sRGB/CMYK
        case 0x11:
            cx = (pJPEG->iWidth + 7) >> 3;  // number of MCU blocks
            cy = (pJPEG->iCropY + pJPEG->iCropCY + 7) >> 3;
            iCr = MCU1;
            iCb = MCU2;
            mcuCX = mcuCY = 8;
            break;
        case 0x12:
            cx = (pJPEG->iWidth + 7) >> 3;  // number of MCU blocks
            cy = (pJPEG->iCropY + pJPEG->iCropCY + 15) >> 4;
            iCr = MCU2;
            iCb = MCU3;
            mcuCX = 8;
            mcuCY = 16;
            break;
        case 0x21:
            cx = (pJPEG->iWidth + 15) >> 4;  // number of MCU blocks
            cy = (pJPEG->iCropY + pJPEG->iCropCY + 7) >> 3;
            iCr = MCU2;
            iCb = MCU3;
            mcuCX = 16;
            mcuCY = 8;
            break;
        case 0x22:
            cx = (pJPEG->iWidth + 15) >> 4;  // number of MCU blocks
            cy = (pJPEG->iCropY + pJPEG->iCropCY + 15) >> 4;
            iCr = MCU4;
            iCb = MCU5;
            mcuCX = mcuCY = 16;
            break;
        default: // to suppress compiler warning
            cx = cy = 0;
            iCr = iCb = 0;
            break;
    }
    // Scale down the MCUs by the requested amount
    mcuCX >>= iScaleShift;
    mcuCY >>= iScaleShift;

    iQuant1 = pJPEG->sQuantTable[pJPEG->JPCI[0].quant_tbl_no * DCTSIZE]; // DC quant values
    iQuant2 = pJPEG->sQuantTable[pJPEG->JPCI[1].quant_tbl_no * DCTSIZE];
    iQuant3 = pJPEG->sQuantTable[pJPEG->JPCI[2].quant_tbl_no * DCTSIZE];
    // luminance values are always in these positions
    iLum0 = MCU0;
    iLum1 = MCU1;
    iLum2 = MCU2;
    iLum3 = MCU3;
    iErr = 0;
    pJPEG->iResCount = pJPEG->iResInterval;
    // Calculate how many MCUs we can fit in the pixel buffer to maximize LCD drawing speed
    iMCUCount = MAX_BUFFERED_PIXELS / (mcuCX * mcuCY);
    if(pJPEG->ucPixelType == RGB8888) {
        iMCUCount /= 2; // half as many will fit
    }
    if(pJPEG->ucPixelType == EIGHT_BIT_GRAYSCALE)
        iMCUCount *= 2; // each pixel is only 1 byte
    if(iMCUCount > cx)
        iMCUCount = cx; // don't go wider than the image
    if(iMCUCount > pJPEG->iMaxMCUs)  // did the user set an upper bound on how many pixels per JPEGDraw callback?
        iMCUCount = pJPEG->iMaxMCUs;
    if(pJPEG->ucPixelType > EIGHT_BIT_GRAYSCALE)  // dithered, override the max MCU count
        iMCUCount = cx; // do the whole row
    jd.iBpp = 16;
    switch(pJPEG->ucPixelType) {
        case RGB8888:
            jd.iBpp = 32;
            break;
        case EIGHT_BIT_GRAYSCALE:
            jd.iBpp = 8;
            break;
        case FOUR_BIT_DITHERED:
            jd.iBpp = 4;
            break;
        case TWO_BIT_DITHERED:
            jd.iBpp = 2;
            break;
        case ONE_BIT_DITHERED:
            jd.iBpp = 1;
            break;
    }
    if(pJPEG->ucPixelType > EIGHT_BIT_GRAYSCALE)
        jd.pPixels = (uint16_t *)pJPEG->pDitherBuffer;
    else
        jd.pPixels = pJPEG->usPixels;
    jd.iHeight = mcuCY;
    for(y = 0; y < cy && bContinue && iErr == 0; y++) {
        bSkipRow = (y * mcuCY < pJPEG->iCropY);
        jd.x = pJPEG->iXOffset;
        xoff = 0; // start of new LCD output group
        if(pJPEG->pFramebuffer) {  // user-supplied buffer is full width
            int ty = (y * mcuCY) - pJPEG->iCropY;
            iPitch = pJPEG->iCropCX; // size of cropped width
            pJPEG->usPixels = (uint16_t *)pJPEG->pFramebuffer;
            if(pJPEG->ucPixelType >= EIGHT_BIT_GRAYSCALE) {
                pJPEG->usPixels += (ty * iPitch / 2); // 1 byte per pixel
            }
            else  if(pJPEG->ucPixelType == RGB8888) {
                pJPEG->usPixels += (ty * iPitch * 2); // 4 bytes per pixel
            }
            else {   // 2 bytes per pixel
                pJPEG->usPixels += (ty * iPitch);
            }
        }
        else {   // use our internal buffer to do it a block at a time
            iPitch = iMCUCount * mcuCX; // pixels per line of LCD buffer
        }
        for(x = 0; x < cx && bContinue && iErr == 0; x++) {
            iSkipMask = 0; // assume not skipping
            if(bSkipRow || x * mcuCX < pJPEG->iCropX || x * mcuCX >= pJPEG->iCropX + pJPEG->iCropCX) {
                iSkipMask = MCU_SKIP;
            }
            pJPEG->ucACTable = cACTable0;
            pJPEG->ucDCTable = cDCTable0;
            // do the first luminance component
            iErr = JPEGDecodeMCU(pJPEG, iLum0 | iSkipMask, &iDCPred0);
            if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                pl = (uint32_t *)&pJPEG->sMCUs[iLum0];
                c = ucRangeTable[((iDCPred0 * iQuant1) >> 5) & 0x3ff];
                l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                // dct stores byte values
                for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                    pl[i] = l;
            }
            else {
                JPEGIDCT(pJPEG, iLum0, pJPEG->JPCI[0].quant_tbl_no); // first quantization table
            }
            // do the second luminance component
            if(pJPEG->ucSubSample > 0x11) { // subsampling
                iErr |= JPEGDecodeMCU(pJPEG, iLum1 | iSkipMask, &iDCPred0);
                if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                    c = ucRangeTable[((iDCPred0 * iQuant1) >> 5) & 0x3ff];
                    l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                    // dct stores byte values
                    pl = (uint32_t *)&pJPEG->sMCUs[iLum1];
                    for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                        pl[i] = l;
                }
                else {
                    JPEGIDCT(pJPEG, iLum1, pJPEG->JPCI[0].quant_tbl_no); // first quantization table
                }
                if(pJPEG->ucSubSample == 0x22) {
                    iErr |= JPEGDecodeMCU(pJPEG, iLum2 | iSkipMask, &iDCPred0);
                    if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                        c = ucRangeTable[((iDCPred0 * iQuant1) >> 5) & 0x3ff];
                        l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                        // dct stores byte values
                        pl = (uint32_t *)&pJPEG->sMCUs[iLum2];
                        for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                            pl[i] = l;
                    }
                    else {
                        JPEGIDCT(pJPEG, iLum2, pJPEG->JPCI[0].quant_tbl_no); // first quantization table
                    }
                    iErr |= JPEGDecodeMCU(pJPEG, iLum3 | iSkipMask, &iDCPred0);
                    if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                        c = ucRangeTable[((iDCPred0 * iQuant1) >> 5) & 0x3ff];
                        l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                        // dct stores byte values
                        pl = (uint32_t *)&pJPEG->sMCUs[iLum3];
                        for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                            pl[i] = l;
                    }
                    else {
                        JPEGIDCT(pJPEG, iLum3, pJPEG->JPCI[0].quant_tbl_no); // first quantization table
                    }
                } // if 2:2 subsampling
            } // if subsampling used
            if(pJPEG->ucSubSample && pJPEG->ucNumComponents == 3) { // if color (not CMYK)
                // first chroma
                pJPEG->ucACTable = cACTable1;
                pJPEG->ucDCTable = cDCTable1;
                if(pJPEG->ucPixelType >= EIGHT_BIT_GRAYSCALE) {
                    // We're not going to use the color channels, so avoid as much work as possible
                    iErr |= JPEGDecodeMCU(pJPEG, MCU_SKIP, &iDCPred1); // decode Cr block
                    iErr |= JPEGDecodeMCU(pJPEG, MCU_SKIP, &iDCPred2); // decode Cb block
                }
                else {
                    iErr |= JPEGDecodeMCU(pJPEG, iCr | iSkipMask, &iDCPred1);
                    if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                        c = ucRangeTable[((iDCPred1 * iQuant2) >> 5) & 0x3ff];
                        l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                        // dct stores byte values
                        pl = (uint32_t *)&pJPEG->sMCUs[iCr];
                        for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                            pl[i] = l;
                    }
                    else {
                        JPEGIDCT(pJPEG, iCr, pJPEG->JPCI[1].quant_tbl_no); // second quantization table
                    }
                    // second chroma
                    pJPEG->ucACTable = cACTable2;
                    pJPEG->ucDCTable = cDCTable2;
                    iErr |= JPEGDecodeMCU(pJPEG, iCb | iSkipMask, &iDCPred2);
                    if(pJPEG->u16MCUFlags == 0 || bThumbnail) { // no AC components, save some time
                        c = ucRangeTable[((iDCPred2 * iQuant3) >> 5) & 0x3ff];
                        l = c | ((uint32_t) c << 8) | ((uint32_t) c << 16) | ((uint32_t) c << 24);
                        // dct stores byte values
                        pl = (uint32_t *)&pJPEG->sMCUs[iCb];
                        for(i = 0; i < iMaxFill; i++) // 8x8 bytes = 16 longs
                            pl[i] = l;
                    }
                    else {
                        JPEGIDCT(pJPEG, iCb, pJPEG->JPCI[2].quant_tbl_no);
                    }
                }
            } // if color components present
            if(!iSkipMask) {  // this MCU is not skipped
                if(pJPEG->ucPixelType >= EIGHT_BIT_GRAYSCALE) {
                    JPEGPutMCU8BitGray(pJPEG, xoff, iPitch); // grayscale or color is being drawn as grayscale
                }
                else {
                    switch(pJPEG->ucSubSample) {
                        case 0x00: // grayscale
                            JPEGPutMCUGray(pJPEG, xoff, iPitch);
                            break;
                        case 0x11:
                            JPEGPutMCU11(pJPEG, xoff, iPitch);
                            break;
                        case 0x12:
                            JPEGPutMCU12(pJPEG, xoff, iPitch);
                            break;
                        case 0x21:
                            JPEGPutMCU21(pJPEG, xoff, iPitch);
                            break;
                        case 0x22:
                            JPEGPutMCU22(pJPEG, xoff, iPitch);
                            break;
                    } // switch on color option
                } // normal MCU drawing
                xoff += mcuCX;
            } // if not skipped
            if(pJPEG->pFramebuffer == NULL && (xoff == iPitch || x == cx - 1) && !bSkipRow) { // time to draw
                jd.iWidth = jd.iWidthUsed = iPitch; // width of each LCD block group
                jd.pUser = pJPEG->pUser;
                if(pJPEG->ucPixelType > EIGHT_BIT_GRAYSCALE)  // dither to 4/2/1 bits
                    JPEGDither(pJPEG, cx * mcuCX, mcuCY);
                if((x + 1) * mcuCX > pJPEG->iWidth) { // right edge has clipped pixels
                    jd.iWidthUsed = iPitch - (cx * mcuCX - pJPEG->iWidth);
                }
                else if(jd.x + iPitch > pJPEG->iCropCX) {    // not a full width
                    jd.iWidthUsed = pJPEG->iCropCX - jd.x;
                }
                jd.y = pJPEG->iYOffset + (y * mcuCY) - pJPEG->iCropY;
                if((jd.y - pJPEG->iYOffset + mcuCY) > (pJPEG->iHeight >> iScaleShift)) { // last row needs to be trimmed
                    jd.iHeight = (pJPEG->iHeight >> iScaleShift) - (jd.y - pJPEG->iYOffset);
                }
                bContinue = (*pJPEG->pfnDraw)(&jd);
                jd.x += iPitch;
                if((cx - 1 - x) < iMCUCount)  // change pitch for the last set of MCUs on this row
                    iPitch = (cx - 1 - x) * mcuCX;
                xoff = 0;
            }
            if(pJPEG->iResInterval) {
                if(--pJPEG->iResCount == 0) {
                    pJPEG->iResCount = pJPEG->iResInterval;
                    iDCPred0 = iDCPred1 = iDCPred2 = 0; // reset DC predictors
                    if(pJPEG->bb.ulBitOff & 7) { // need to start at the next even byte
                        pJPEG->bb.ulBitOff += (8 - (pJPEG->bb.ulBitOff & 7));  // new restart interval starts on byte boundary
                    }
                } // if restart interval needs to reset
            } // if there is a restart interval
            // See if we need to feed it more data
            if(pJPEG->iVLCOff >= FILE_HIGHWATER)
                JPEGGetMoreData(pJPEG); // need more 'filtered' VLC data
        } // for x
    } // for y
    if(iErr != 0)
        pJPEG->iError = JPEG_DECODE_ERROR;
    return (iErr == 0);
} /* DecodeJPEG() */
#endif // LV_USE_JPEGDEC
