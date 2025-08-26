/*****************************************************************************
*
*    copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/
#ifndef VG_LITE_TYPE_H
#define VG_LITE_TYPE_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#if __KERNEL__
    #include <asm/bitsperlong.h>
#endif
#include "vg_lite_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VG_FALSE                   0
#define VG_TRUE                    1

#define VG_SUCCESS                 0
#define VG_FAIL                   -1

typedef int                 vg_lite_bool_t;
typedef unsigned char       vg_lite_uint8_t;
typedef char                vg_lite_int8_t;
typedef short               vg_lite_int16_t;
typedef unsigned short      vg_lite_uint16_t;
typedef int                 vg_lite_int32_t;
typedef unsigned int        vg_lite_uint32_t;
typedef unsigned long long  vg_lite_uint64_t;
typedef float               vg_lite_float_t;
typedef double              vg_lite_double_t;
typedef char                vg_lite_char;
typedef char        *       vg_lite_string;
typedef void        *       vg_lite_pointer;
typedef void                vg_lite_void;
typedef unsigned int        vg_lite_color_t;
typedef unsigned long       vg_lite_flag_t;
typedef unsigned long       vg_lite_long_t;

#if __KERNEL__
# if BITS_PER_LONG == 64
typedef unsigned long long vg_lite_uintptr_t;
# else
typedef unsigned int       vg_lite_uintptr_t;
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_TYPE_H */
