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

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

    #if defined(GCID_REV_CID)
        #define VG_LITE_OPTIONS  <Series/GCID_REV_CID/vg_lite_options.h>
        #include VG_LITE_OPTIONS
    #else
        #error "GCID_REV_CID is not defined; eg: -DGCID_REV_CID=gc355/0x0_1216"
    #endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_OPTIONS_DISPATCH_H */
