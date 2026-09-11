/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
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

#ifndef VG_LITE_OPTIONS_DISPATCH_H
#define VG_LITE_OPTIONS_DISPATCH_H

#include "../../../lvgl_public.h"
#if LV_USE_VG_LITE_DRIVER

    #if LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC255_0X40A
        #include <../VGLite/Series/gc255/0x40A/vg_lite_options.h>
    #elif LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC355_0X0_1215
        #include <../VGLite/Series/gc355/0x0_1215/vg_lite_options.h>
    #elif LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC355_0X0_1216
        #include <../VGLite/Series/gc355/0x0_1216/vg_lite_options.h>
    #elif LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC555_0X423
        #include <../VGLite/Series/gc555/0x423/vg_lite_options.h>
    #elif LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC555_0X423_ECO
        #include <../VGLite/Series/gc555/0x423_ECO/vg_lite_options.h>
    #elif LV_VG_LITE_GPU == LV_VG_LITE_GPU_GCNANOULTRAV_0X1003
        #include <../VGLite/Series/gcnanoultrav/0x1003/vg_lite_options.h>
    #else
        #error "Unknown LV_VG_LITE_GPU; select a supported VG-Lite GPU in your config"
    #endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_OPTIONS_DISPATCH_H */
