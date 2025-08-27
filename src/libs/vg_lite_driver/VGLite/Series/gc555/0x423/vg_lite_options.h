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

#ifndef VG_LITE_OPTIONS_H
#define VG_LITE_OPTIONS_H

#include "../../../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

    #define CHIPID          0x555
    #define REVISION        0x1230
    #define CID             0x423
    #define ECOID           0x0

    #define gcFEATURE_VG_IM_INDEX_FORMAT    1
    #define gcFEATURE_VG_SCISSOR    1
    #define gcFEATURE_VG_BORDER_CULLING    1
    #define gcFEATURE_VG_RGBA2_FORMAT    1
    #define gcFEATURE_VG_QUALITY_8X    0
    #define gcFEATURE_VG_IM_FASTCLEAR    0
    #define gcFEATURE_VG_RADIAL_GRADIENT    1
    #define gcFEATURE_VG_GLOBAL_ALPHA    1
    #define gcFEATURE_VG_RGBA8_ETC2_EAC    1
    #define gcFEATURE_VG_COLOR_KEY    1
    #define gcFEATURE_VG_DOUBLE_IMAGE    0
    #define gcFEATURE_VG_YUV_OUTPUT    0
    #define gcFEATURE_VG_FLEXA    0
    #define gcFEATURE_VG_24BIT    1
    #define gcFEATURE_VG_DITHER    1
    #define gcFEATURE_VG_USE_DST    0
    #define gcFEATURE_VG_PE_CLEAR    1
    #define gcFEATURE_VG_IM_INPUT    1
    #define gcFEATURE_VG_DEC_COMPRESS    1
    #define gcFEATURE_VG_LINEAR_GRADIENT_EXT    1
    #define gcFEATURE_VG_MASK    1
    #define gcFEATURE_VG_MIRROR    1
    #define gcFEATURE_VG_GAMMA    1
    #define gcFEATURE_VG_NEW_BLEND_MODE    1
    #define gcFEATURE_VG_STENCIL    1
    #define gcFEATURE_VG_SRC_PREMULTIPLIED    0
    #define gcFEATURE_VG_HW_PREMULTIPLY    1
    #define gcFEATURE_VG_COLOR_TRANSFORMATION    1
    #define gcFEATURE_VG_LVGL_SUPPORT    1
    #define gcFEATURE_VG_INDEX_ENDIAN    0
    #define gcFEATURE_VG_24BIT_PLANAR    0
    #define gcFEATURE_VG_PIXEL_MATRIX    0
    #define gcFEATURE_VG_NEW_IMAGE_INDEX    1
    #define gcFEATURE_VG_PARALLEL_PATHS    1
    #define gcFEATURE_VG_STRIPE_MODE    1
    #define gcFEATURE_VG_IM_DEC_INPUT    1
    #define gcFEATURE_VG_GAUSSIAN_BLUR    0
    #define gcFEATURE_VG_RECTANGLE_TILED_OUT    1
    #define gcFEATURE_VG_TESSELLATION_TILED_OUT    1
    #define gcFEATURE_VG_IM_REPEAT_REFLECT    1
    #define gcFEATURE_VG_YUY2_INPUT    1
    #define gcFEATURE_VG_YUV_INPUT    1
    #define gcFEATURE_VG_YUV_TILED_INPUT    0
    #define gcFEATURE_VG_AYUV_INPUT    0
    #define gcFEATURE_VG_16PIXELS_ALIGNED    1
    #define gcFEATURE_VG_MATH_PRECISION_FIX    1
    #define gcFEATURE_VG_SPLIT_PATH    1
    #define gcFEATURE_VG_DEC_COMPRESS_2_0    0
    #define gcFEATURE_VG_CLOCK_GATING_TS_MODULE    1
    #define gcFEATURE_VG_CLOCK_GATING_VG_MODULE    0
    #define gcFEATURE_VG_NV24_INPUT    0
    #define gcFEATURE_VG_TILED_LIMIT   3
    #define gcFEATURE_VG_SRC_ADDRESS_64BYTES_ALIGNED 1
    #define gcFEATURE_VG_SRC_TILE_4PIXELS_ALIGNED 1
    #define gcFEATURE_VG_SRC_BUF_ALINGED 0
    #define gcFEATURE_VG_SRC_ADDRESS_16BYTES_ALIGNED 0
    #define gcFEATURE_VG_DST_ADDRESS_64BYTES_ALIGNED 0
    #define gcFEATURE_VG_DST_TILE_4PIXELS_ALIGNED 1
    #define gcFEATURE_VG_DST_BUF_ALIGNED 0
    #define gcFEATURE_VG_DST_24BIT_PLANAR_ALIGNED 0
    #define gcFEATURE_VG_DST_BUFLEN_ALIGNED 0
    #define gcFEATURE_VG_FORMAT_SUPPORT_CHECK 0
    #define gcFEATURE_VG_YUV_ALIGNED_CHECK 1
    #define gcFEATURE_VG_512_PARALLEL_PATHS 0

    /* SW Features */
    #define gcFEATURE_VG_STROKE_PATH    1
    #define gcFEATURE_VG_ARC_PATH    1
    #define gcFEATURE_VG_ERROR_CHECK    1
    #define gcFEATURE_VG_TRACE_API    0
    #define gcFEATURE_VG_POWER_MANAGEMENT           1
    #define gcFEATURE_VG_TILED_MODE    1
    #define gcFEATURE_VG_SINGLE_COMMAND_BUFFER   0

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_OPTIONS_H */
