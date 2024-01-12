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

#ifndef _vg_lite_h_
#define _vg_lite_h_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define inline __inline
#endif

#include <stddef.h>
#include <stdint.h>


/*  VGLite API Constants *******************************************************************************************************************/

#define VGLITE_HEADER_VERSION       7

#ifndef VGLITE_VERSION_3_0
#define VGLITE_VERSION_3_0          1

#define VGLITE_MAKE_VERSION(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))
#define VGLITE_VERSION_MAJOR(version) (((uint32_t)(version) >> 16) & 0xff)
#define VGLITE_VERSION_MINOR(version) (((uint32_t)(version) >> 8) & 0xff)
#define VGLITE_VERSION_PATCH(version) ((uint32_t)(version) & 0xff)

#define VGLITE_API_VERSION_3_0      VGLITE_MAKE_VERSION(3, 0, 0)

#define VGLITE_RELEASE_VERSION      VGLITE_MAKE_VERSION(4,0,47)

#define VGL_FALSE                   0
#define VGL_TRUE                    1

/* Path command (op code). */
#define VLC_OP_END                  0x00
#define VLC_OP_CLOSE                0x01
#define VLC_OP_MOVE                 0x02
#define VLC_OP_MOVE_REL             0x03
#define VLC_OP_LINE                 0x04
#define VLC_OP_LINE_REL             0x05
#define VLC_OP_QUAD                 0x06
#define VLC_OP_QUAD_REL             0x07
#define VLC_OP_CUBIC                0x08
#define VLC_OP_CUBIC_REL            0x09
#define VLC_OP_BREAK                0x0A
#define VLC_OP_HLINE                0x0B
#define VLC_OP_HLINE_REL            0x0C
#define VLC_OP_VLINE                0x0D
#define VLC_OP_VLINE_REL            0x0E
#define VLC_OP_SQUAD                0x0F
#define VLC_OP_SQUAD_REL            0x10
#define VLC_OP_SCUBIC               0x11
#define VLC_OP_SCUBIC_REL           0x12
#define VLC_OP_SCCWARC              0x13
#define VLC_OP_SCCWARC_REL          0x14
#define VLC_OP_SCWARC               0x15
#define VLC_OP_SCWARC_REL           0x16
#define VLC_OP_LCCWARC              0x17
#define VLC_OP_LCCWARC_REL          0x18
#define VLC_OP_LCWARC               0x19
#define VLC_OP_LCWARC_REL           0x1A

/* Macros for path manipulating: See path definitions. */
#define VLM_PATH_ENABLE_UPLOAD(path)    (path).uploaded.property |= 1
#define VLM_PATH_DISABLE_UPLOAD(path)   (path).uploaded.property &= (~1)
#define VLM_PATH_GET_UPLOAD_BIT(path)   ((path).uploaded.property & 1)

/* Gradient constants. */
#define VLC_MAX_COLOR_RAMP_STOPS    256             /*! The max number of radial gradient stops. */
#define VLC_MAX_GRADIENT_STOPS      16              /*! The max number of gradient stops. */
#define VLC_GRADIENT_BUFFER_WIDTH   1024            /*! The internal gradient buffer width.*/


/* API name defines for backward compatibility to VGLite 2.0 APIs */
#define vg_lite_buffer_upload                   vg_lite_upload_buffer
#define vg_lite_path_append                     vg_lite_append_path
#define vg_lite_path_calc_length                vg_lite_get_path_length
#define vg_lite_set_ts_buffer                   vg_lite_set_tess_buffer
#define vg_lite_set_draw_path_type              vg_lite_set_path_type
#define vg_lite_create_mask_layer               vg_lite_create_masklayer
#define vg_lite_fill_mask_layer                 vg_lite_fill_masklayer
#define vg_lite_blend_mask_layer                vg_lite_blend_masklayer
#define vg_lite_generate_mask_layer_by_path     vg_lite_render_masklayer
#define vg_lite_set_mask_layer                  vg_lite_set_masklayer
#define vg_lite_destroy_mask_layer              vg_lite_destroy_masklayer
#define vg_lite_enable_mask                     vg_lite_enable_masklayer
#define vg_lite_enable_color_transformation     vg_lite_enable_color_transform
#define vg_lite_set_color_transformation        vg_lite_set_color_transform
#define vg_lite_set_image_global_alpha          vg_lite_source_global_alpha
#define vg_lite_set_dest_global_alpha           vg_lite_dest_global_alpha
#define vg_lite_clear_rad_grad                  vg_lite_clear_radial_grad
#define vg_lite_update_rad_grad                 vg_lite_update_radial_grad
#define vg_lite_get_rad_grad_matrix             vg_lite_get_radial_grad_matrix
#define vg_lite_set_rad_grad                    vg_lite_set_radial_grad
#define vg_lite_draw_linear_gradient            vg_lite_draw_linear_grad
#define vg_lite_draw_radial_gradient            vg_lite_draw_radial_grad
#define vg_lite_draw_gradient                   vg_lite_draw_grad
#define vg_lite_mem_avail                       vg_lite_get_mem_size
#define vg_lite_set_update_stroke               vg_lite_update_stroke

#define vg_lite_buffer_image_mode_t             vg_lite_image_mode_t
#define vg_lite_draw_path_type_t                vg_lite_path_type_t
#define vg_lite_linear_gradient_ext_t           vg_lite_ext_linear_gradient_t
#define vg_lite_buffer_transparency_mode_t      vg_lite_transparency_t


/* VGLite API Types ***********************************************************************************************************************/

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
typedef char*               vg_lite_string;
typedef void*               vg_lite_pointer;
typedef void                vg_lite_void;
typedef unsigned int        vg_lite_color_t;


/* VGLite API Enumerations ****************************************************************************************************************/

#ifndef VG_LITE_ERROR
#define VG_LITE_ERROR  1

    /* Error codes that the vg_lite functions can return. */
    typedef enum vg_lite_error
    {
        VG_LITE_SUCCESS = 0,            /*! Success. */
        VG_LITE_INVALID_ARGUMENT,       /*! An invalid argument was specified. */
        VG_LITE_OUT_OF_MEMORY,          /*! Out of memory. */
        VG_LITE_NO_CONTEXT,             /*! No context or an unintialized context specified. */
        VG_LITE_TIMEOUT,                /*! A timeout has occurred during a wait. */
        VG_LITE_OUT_OF_RESOURCES,       /*! Out of system resources. */
        VG_LITE_GENERIC_IO,             /*! Cannot communicate with the kernel driver. */
        VG_LITE_NOT_SUPPORT,            /*! Function call not supported. */
        VG_LITE_ALREADY_EXISTS,         /*! Object already exists */
        VG_LITE_NOT_ALIGNED,            /*! Data alignment error */
        VG_LITE_FLEXA_TIME_OUT,         /*! VG timeout requesting for segment buffer */
        VG_LITE_FLEXA_HANDSHAKE_FAIL,   /*! VG and SBI synchronizer handshake failed */
    } vg_lite_error_t;
#endif

    /* Chip features bit */
    typedef enum vg_lite_feature
    {
        gcFEATURE_BIT_VG_IM_INDEX_FORMAT,
        gcFEATURE_BIT_VG_SCISSOR,
        gcFEATURE_BIT_VG_BORDER_CULLING,
        gcFEATURE_BIT_VG_RGBA2_FORMAT,
        gcFEATURE_BIT_VG_QUALITY_8X,
        gcFEATURE_BIT_VG_IM_FASTCLAER,
        gcFEATURE_BIT_VG_RADIAL_GRADIENT,
        gcFEATURE_BIT_VG_GLOBAL_ALPHA,
        gcFEATURE_BIT_VG_RGBA8_ETC2_EAC,
        gcFEATURE_BIT_VG_COLOR_KEY,
        gcFEATURE_BIT_VG_DOUBLE_IMAGE,
        gcFEATURE_BIT_VG_YUV_OUTPUT,
        gcFEATURE_BIT_VG_FLEXA,
        gcFEATURE_BIT_VG_24BIT,
        gcFEATURE_BIT_VG_DITHER,
        gcFEATURE_BIT_VG_USE_DST,
        gcFEATURE_BIT_VG_PE_CLEAR,
        gcFEATURE_BIT_VG_IM_INPUT,
        gcFEATURE_BIT_VG_DEC_COMPRESS,
        gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT,
        gcFEATURE_BIT_VG_MASK,
        gcFEATURE_BIT_VG_MIRROR,
        gcFEATURE_BIT_VG_GAMMA,
        gcFEATURE_BIT_VG_NEW_BLEND_MODE,
        gcFEATURE_BIT_VG_STENCIL,
        gcFEATURE_BIT_VG_SRC_PREMULTIPLIED, /*! Valid only if gcFEATURE_BIT_VG_HW_PREMULTIPLY is 0   */
        gcFEATURE_BIT_VG_HW_PREMULTIPLY,    /*! HW multiplier can accept either premultiplied or not */
        gcFEATURE_BIT_VG_COLOR_TRANSFORMATION,
        gcFEATURE_BIT_VG_LVGL_SUPPORT,
        gcFEATURE_BIT_VG_INDEX_ENDIAN,
        gcFEATURE_BIT_VG_24BIT_PLANAR,
        gcFEATURE_BIT_VG_PIXEL_MATRIX,
        gcFEATURE_BIT_VG_NEW_IMAGE_INDEX,
        gcFEATURE_BIT_VG_PARALLEL_PATHS,
        gcFEATURE_BIT_VG_STRIPE_MODE,
        gcFEATURE_BIT_VG_IM_DEC_INPUT,
        gcFEATURE_BIT_VG_GAUSSIAN_BLUR,
        gcFEATURE_BIT_VG_RECTANGLE_TILED_OUT,
        gcFEATURE_BIT_VG_TESSELLATION_TILED_OUT,
        gcFEATURE_BIT_VG_IM_REPEAT_REFLECT,
        gcFEATURE_BIT_VG_YUY2_INPUT,
        gcFEATURE_BIT_VG_YUV_INPUT,
        gcFEATURE_BIT_VG_YUV_TILED_INPUT,
        gcFEATURE_BIT_VG_AYUV_INPUT,
        gcFEATURE_BIT_VG_16PIXELS_ALIGN,
        gcFEATURE_BIT_VG_DEC_COMPRESS_2_0,
        gcFEATURE_COUNT
    } vg_lite_feature_t;

    /* Rendering quality enums. */
    typedef enum vg_lite_quality
    {
        VG_LITE_HIGH,   /*! High quality 16x anti-aliasing path. */
        VG_LITE_UPPER,  /*! Upper quality 8x anti-aliasing path. */
        VG_LITE_MEDIUM, /*! Medium quality 4x anti-aliasing path. */
        VG_LITE_LOW,    /*! Low quality path without any anti-aliasing. */
    } vg_lite_quality_t;

    /* Format of path coordinates. */
    typedef enum vg_lite_format
    {
        VG_LITE_S8,     /*! Signed 8-bit coordinates. */
        VG_LITE_S16,    /*! Signed 16-bit coordinates. */
        VG_LITE_S32,    /*! Signed 32-bit coordinates. */
        VG_LITE_FP32,   /*! 32-bit floating point coordinates. */
    } vg_lite_format_t;

    /* Format of pixel buffer. */
    typedef enum vg_lite_buffer_format
    {
        /* OpenVG VGImageFormat enums:
         * Note: The bits for each color channel are stored within a machine word
         * from MSB to LSB in the order indicated by the pixel format name.
         * This is opposite of VG_LITE_* formats (from LSB to MSB).
         */

        /* RGB{A,X} channel ordering */
        VG_sRGBX_8888                               =  0,
        VG_sRGBA_8888                               =  1,
        VG_sRGBA_8888_PRE                           =  2,
        VG_sRGB_565                                 =  3,
        VG_sRGBA_5551                               =  4,
        VG_sRGBA_4444                               =  5,
        VG_sL_8                                     =  6,
        VG_lRGBX_8888                               =  7,
        VG_lRGBA_8888                               =  8,
        VG_lRGBA_8888_PRE                           =  9,
        VG_lL_8                                     = 10,
        VG_A_8                                      = 11,
        VG_BW_1                                     = 12,
        VG_A_1                                      = 13,
        VG_A_4                                      = 14,

        VG_sRGBX_8888_PRE                           = 15,
        VG_sRGB_565_PRE                             = 16,
        VG_sRGBA_5551_PRE                           = 17,
        VG_sRGBA_4444_PRE                           = 18,
        VG_lRGBX_8888_PRE                           = 19,
        VG_lRGB_565                                 = 20,
        VG_lRGB_565_PRE                             = 21,
        VG_lRGBA_5551                               = 22,
        VG_lRGBA_5551_PRE                           = 23,
        VG_lRGBA_4444                               = 24,
        VG_lRGBA_4444_PRE                           = 25,

        /* {A,X}RGB channel ordering */
        VG_sXRGB_8888                               =  0 | (1 << 6),
        VG_sARGB_8888                               =  1 | (1 << 6),
        VG_sARGB_8888_PRE                           =  2 | (1 << 6),
        VG_sARGB_1555                               =  4 | (1 << 6),
        VG_sARGB_4444                               =  5 | (1 << 6),
        VG_lXRGB_8888                               =  7 | (1 << 6),
        VG_lARGB_8888                               =  8 | (1 << 6),
        VG_lARGB_8888_PRE                           =  9 | (1 << 6),

        /* BGR{A,X} channel ordering */
        VG_sBGRX_8888                               =  0 | (1 << 7),
        VG_sBGRA_8888                               =  1 | (1 << 7),
        VG_sBGRA_8888_PRE                           =  2 | (1 << 7),
        VG_sBGR_565                                 =  3 | (1 << 7),
        VG_sBGRA_5551                               =  4 | (1 << 7),
        VG_sBGRA_4444                               =  5 | (1 << 7),
        VG_lBGRX_8888                               =  7 | (1 << 7),
        VG_lBGRA_8888                               =  8 | (1 << 7),
        VG_lBGRA_8888_PRE                           =  9 | (1 << 7),

        /* {A,X}BGR channel ordering */
        VG_sXBGR_8888                               =  0 | (1 << 6) | (1 << 7),
        VG_sABGR_8888                               =  1 | (1 << 6) | (1 << 7),
        VG_sABGR_8888_PRE                           =  2 | (1 << 6) | (1 << 7),
        VG_sABGR_1555                               =  4 | (1 << 6) | (1 << 7),
        VG_sABGR_4444                               =  5 | (1 << 6) | (1 << 7),
        VG_lXBGR_8888                               =  7 | (1 << 6) | (1 << 7),
        VG_lABGR_8888                               =  8 | (1 << 6) | (1 << 7),
        VG_lABGR_8888_PRE                           =  9 | (1 << 6) | (1 << 7),

        /* Original VGLite API image format enums:
         * Note: The bits for each color channel are stored within a machine word
         * from LSB to MSB in the order indicated by the pixel format name.
         * This is opposite of OPENVG VG_* formats (from MSB to LSB).
         */
        VG_LITE_RGBA8888                        =  0 | (1 << 10),
        VG_LITE_BGRA8888                        =  1 | (1 << 10),
        VG_LITE_RGBX8888                        =  2 | (1 << 10),
        VG_LITE_BGRX8888                        =  3 | (1 << 10),
        VG_LITE_RGB565                          =  4 | (1 << 10),
        VG_LITE_BGR565                          =  5 | (1 << 10),
        VG_LITE_RGBA4444                        =  6 | (1 << 10),
        VG_LITE_BGRA4444                        =  7 | (1 << 10),
        VG_LITE_BGRA5551                        =  8 | (1 << 10),
        VG_LITE_A4                              =  9 | (1 << 10),
        VG_LITE_A8                              = 10 | (1 << 10),
        VG_LITE_L8                              = 11 | (1 << 10),
        VG_LITE_YUYV                            = 12 | (1 << 10),
        VG_LITE_YUY2                            = 13 | (1 << 10),
        VG_LITE_ANV12                           = 14 | (1 << 10),
        VG_LITE_AYUY2                           = 15 | (1 << 10),
        VG_LITE_NV12                            = 16 | (1 << 10),
        VG_LITE_YV12                            = 17 | (1 << 10),
        VG_LITE_YV24                            = 18 | (1 << 10),
        VG_LITE_YV16                            = 19 | (1 << 10),
        VG_LITE_NV16                            = 20 | (1 << 10),
        VG_LITE_YUY2_TILED                      = 21 | (1 << 10),
        VG_LITE_NV12_TILED                      = 22 | (1 << 10),
        VG_LITE_ANV12_TILED                     = 23 | (1 << 10),
        VG_LITE_AYUY2_TILED                     = 24 | (1 << 10),
        VG_LITE_RGBA2222                        = 25 | (1 << 10),
        VG_LITE_BGRA2222                        = 26 | (1 << 10),
        VG_LITE_ABGR2222                        = 27 | (1 << 10),
        VG_LITE_ARGB2222                        = 28 | (1 << 10),
        VG_LITE_ABGR4444                        = 29 | (1 << 10),
        VG_LITE_ARGB4444                        = 30 | (1 << 10),
        VG_LITE_ABGR8888                        = 31 | (1 << 10),
        VG_LITE_ARGB8888                        = 32 | (1 << 10),
        VG_LITE_ABGR1555                        = 33 | (1 << 10),
        VG_LITE_RGBA5551                        = 34 | (1 << 10),
        VG_LITE_ARGB1555                        = 35 | (1 << 10),
        VG_LITE_XBGR8888                        = 36 | (1 << 10),
        VG_LITE_XRGB8888                        = 37 | (1 << 10),
        VG_LITE_RGBA8888_ETC2_EAC               = 38 | (1 << 10),
        VG_LITE_RGB888                          = 39 | (1 << 10),
        VG_LITE_BGR888                          = 40 | (1 << 10),
        VG_LITE_ABGR8565                        = 41 | (1 << 10),
        VG_LITE_BGRA5658                        = 42 | (1 << 10),
        VG_LITE_ARGB8565                        = 43 | (1 << 10),
        VG_LITE_RGBA5658                        = 44 | (1 << 10),
        VG_LITE_ABGR8565_PLANAR                 = 45 | (1 << 10),
        VG_LITE_BGRA5658_PLANAR                 = 46 | (1 << 10),
        VG_LITE_ARGB8565_PLANAR                 = 47 | (1 << 10),
        VG_LITE_RGBA5658_PLANAR                 = 48 | (1 << 10),

        VG_LITE_INDEX_1 = 0 | (1 << 11),  /*! Indexed format. */
        VG_LITE_INDEX_2 = 1 | (1 << 11),
        VG_LITE_INDEX_4 = 2 | (1 << 11),
        VG_LITE_INDEX_8 = 3 | (1 << 11),

    } vg_lite_buffer_format_t;

    /* Swizzle of packed YUV format UV channels. */
    typedef enum vg_lite_swizzle
    {
        VG_LITE_SWIZZLE_UV,
        VG_LITE_SWIZZLE_VU,
    } vg_lite_swizzle_t;

    /* The YUV<->RGB conversion rule. */
    typedef enum vg_lite_yuv2rgb
    {
        VG_LITE_YUV601,
        VG_LITE_YUV709,
    } vg_lite_yuv2rgb_t;

    /* The pixel layout in a buffer. */
    typedef enum vg_lite_buffer_layout
    {
        VG_LITE_LINEAR,
        VG_LITE_TILED,
    } vg_lite_buffer_layout_t;

    /* The image (buffer) rendering mode. Match OpenVG enum VGImageMode */
    typedef enum vg_lite_image_mode
    {
        /* For enum value backward compatibility */
        VG_LITE_ZERO                            = 0,
        VG_LITE_NORMAL_IMAGE_MODE               = 0x1F00,
        VG_LITE_MULTIPLY_IMAGE_MODE             = 0x1F01,
        VG_LITE_STENCIL_MODE                    = 0x1F02,
        VG_LITE_NONE_IMAGE_MODE                 = 0x1F03,
        VG_LITE_RECOLOR_MODE                    = 0x1F04,
    } vg_lite_image_mode_t;

    /* The image (buffer) transparency mode. */
    typedef enum vg_lite_transparency
    {
        VG_LITE_IMAGE_OPAQUE,
        VG_LITE_IMAGE_TRANSPARENT
    } vg_lite_transparency_t;

    /* Blending modes. VG_BLEND_* match OpenVG enum VGBlendMode.
     * S and D represent source and destination color channels.
     * Sa and Da represent the source and destination alpha channels.
     */
    typedef enum vg_lite_blend
    {
        VG_LITE_BLEND_NONE                      = 0,        /*! S, No blend, Non-premultiplied */
        VG_LITE_BLEND_SRC_OVER                  = 1,        /*! S + (1 - Sa) * D , Non-premultiplied */
        VG_LITE_BLEND_DST_OVER                  = 2,        /*! (1 - Da) * S + D , Non-premultiplied */
        VG_LITE_BLEND_SRC_IN                    = 3,        /*! Da * S , Non-premultiplied */
        VG_LITE_BLEND_DST_IN                    = 4,        /*! Sa * D , Non-premultiplied */
        VG_LITE_BLEND_MULTIPLY                  = 5,        /*! S * (1 - Da) + D * (1 - Sa) + S * D , Non-premultiplied */
        VG_LITE_BLEND_SCREEN                    = 6,        /*! S + D - S * D , Non-premultiplied */
        VG_LITE_BLEND_DARKEN                    = 7,        /*! min(SrcOver, DstOver) , Non-premultiplied */
        VG_LITE_BLEND_LIGHTEN                   = 8,        /*! max(SrcOver, DstOver) , Non-premultiplied */
        VG_LITE_BLEND_ADDITIVE                  = 9,        /*! S + D , Non-premultiplied */
        VG_LITE_BLEND_SUBTRACT                  = 10,       /*! D * (1 - Sa) , Non-premultiplied */
        VG_LITE_BLEND_SUBTRACT_LVGL             = 11,       /*! D - S , Non-premultiplied */
        VG_LITE_BLEND_NORMAL_LVGL               = 12,       /*! S * Sa + (1 - Sa) * D , Non-premultiplied */
        VG_LITE_BLEND_ADDITIVE_LVGL             = 13,       /*! (S + D) * Sa + D * (1 - Sa) , Non-premultiplied */
        VG_LITE_BLEND_MULTIPLY_LVGL             = 14,       /*! (S * D) * Sa + D * (1 - Sa) , Non-premultiplied */
        VG_LITE_BLEND_PREMULTIPLY_SRC_OVER      = 15,       /*! S * Sa + (1 - Sa) * D , Non-premultiplied */

        OPENVG_BLEND_SRC                        = 0x2000,   /*! Copy SRC, no blend, Premultiplied */
        OPENVG_BLEND_SRC_OVER                   = 0x2001,   /*! Porter-Duff SRC_OVER blend, Premultiplied */
        OPENVG_BLEND_DST_OVER                   = 0x2002,   /*! Porter-Duff DST_OVER blend, Premultiplied */
        OPENVG_BLEND_SRC_IN                     = 0x2003,   /*! Porter-Duff SRC_IN blend, Premultiplied */
        OPENVG_BLEND_DST_IN                     = 0x2004,   /*! Porter-Duff DST_IN blend, Premultiplied */
        OPENVG_BLEND_MULTIPLY                   = 0x2005,   /*! Porter-Duff MULTIPLY blend, Premultiplied */
        OPENVG_BLEND_SCREEN                     = 0x2006,   /*! Porter-Duff SCREEN blend, Premultiplied */
        OPENVG_BLEND_DARKEN                     = 0x2007,   /*! Porter-Duff DARKEN blend, Premultiplied */
        OPENVG_BLEND_LIGHTEN                    = 0x2008,   /*! Porter-Duff LIGHTEN blend, Premultiplied */
        OPENVG_BLEND_ADDITIVE                   = 0x2009,   /*! Porter-Duff ADDITIVE blend, Premultiplied */
    } vg_lite_blend_t;

    /* Fill rules. Match OpenVG enum VGFillRule */
    typedef enum vg_lite_fill
    {
        VG_LITE_FILL_EVEN_ODD                   = 0x1900,   /*! A pixel is drawn it it crosses an odd number of path pixels. */
        VG_LITE_FILL_NON_ZERO                   = 0x1901,   /*! A pixel is drawn if it crosses at least one path pixel. */
    } vg_lite_fill_t;

    /* Global alpha modes. */
    typedef enum vg_lite_global_alpha
    {
        VG_LITE_NORMAL = 0,                     /*! Use original src/dst alpha value. */
        VG_LITE_GLOBAL,                         /*! Use global src/dst alpha value to replace original src/dst alpha value. */
        VG_LITE_SCALED,                         /*! Multiply global src/dst alpha value and original src/dst alpha value. */
    } vg_lite_global_alpha_t;

    /* Filter modes. */
    typedef enum vg_lite_filter
    {
        VG_LITE_FILTER_POINT     = 0,           /*! Fetch the nearest image pixel. */
        VG_LITE_FILTER_LINEAR    = 0x1000,      /*! Used for linear paint. */
        VG_LITE_FILTER_BI_LINEAR = 0x2000,      /*! Use a 2x2 box around the image pixel and perform an interpolation. */
        VG_LITE_FILTER_GAUSSIAN  = 0x3000,      /*! Perform 3x3 gaussian blur with the convolution for image pixel. */
    } vg_lite_filter_t;

    /* Pattern padding mode. Match OpenVG enum VGTilingMode. */
    typedef enum vg_lite_pattern_mode
    {
        VG_LITE_PATTERN_COLOR   = 0x1D00,       /*! Pixel outside the bounds of sourceimage should be taken as the color */
        VG_LITE_PATTERN_PAD     = 0x1D01,       /*! Pixel outside the bounds of sourceimage should be taken as having the same color as the closest edge pixel */
        VG_LITE_PATTERN_REPEAT  = 0x1D02,       /*! Pixel outside the bounds of sourceimage should be repeated indefinitely in all directions */
        VG_LITE_PATTERN_REFLECT = 0x1D03,       /*! Pixel outside the bounds of sourceimage should be reflected indefinitely in all directions */
    } vg_lite_pattern_mode_t;

    /* Paint type. Match OpenVG enum VGPaintType. */
    typedef enum vg_lite_paint_type
    {
        /* For enum value backward compatibility */
        VG_LITE_PAINT_ZERO            = 0,
        VG_LITE_PAINT_COLOR           = 0x1B00,
        VG_LITE_PAINT_LINEAR_GRADIENT = 0x1B01,
        VG_LITE_PAINT_RADIAL_GRADIENT = 0x1B02,
        VG_LITE_PAINT_PATTERN         = 0x1B03,
    } vg_lite_paint_type_t;

    /* Radial gradient padding mode. Match OpenVG enum VGColorRampSpreadMode */
    typedef enum
    {
        VG_LITE_GRADIENT_SPREAD_FILL     = 0,
        VG_LITE_GRADIENT_SPREAD_PAD      = 0x1C00,
        VG_LITE_GRADIENT_SPREAD_REPEAT   = 0x1C01,
        VG_LITE_GRADIENT_SPREAD_REFLECT  = 0x1C02,
    } vg_lite_gradient_spreadmode_t;

    /* Decnano Compress mode. */
    typedef enum vg_lite_compress_mode
    {
        VG_LITE_DEC_DISABLE = 0,                /*! disable compress */
        VG_LITE_DEC_NON_SAMPLE,                 /*! compress ratio is 1.6 if use ARGB8888, compress ratio is 2 if use XRGB8888 */
        VG_LITE_DEC_HSAMPLE,                    /*! compress ratio is 2 if use ARGB8888, compress ratio is 2.6 if use XRGB8888 */
        VG_LITE_DEC_HV_SAMPLE,                  /*! compress ratio is 2.6 if use ARGB8888, compress ratio is 4 if use XRGB8888 */
    } vg_lite_compress_mode_t;

    /* Draw path type. Match OpenVG enum VGPaintMode */
    typedef enum vg_lite_path_type
    {
        /* For enum value backward compatibility */
        VG_LITE_DRAW_ZERO                       = 0,
        VG_LITE_DRAW_STROKE_PATH                = (1<<0),
        VG_LITE_DRAW_FILL_PATH                  = (1<<1),
        VG_LITE_DRAW_FILL_STROKE_PATH           = (1<<1 | 1<<0),
    } vg_lite_path_type_t;

    /* End cap style. Match OpenVG enum VGCapStyle */
    typedef enum vg_lite_cap_style
    {
        VG_LITE_CAP_BUTT                        = 0x1700,
        VG_LITE_CAP_ROUND                       = 0x1701,
        VG_LITE_CAP_SQUARE                      = 0x1702,
    } vg_lite_cap_style_t;

    /* Line join styles. Match OpenVG enum VGJoinStyle */
    typedef enum vg_lite_join_style
    {
        VG_LITE_JOIN_MITER                      = 0x1800,
        VG_LITE_JOIN_ROUND                      = 0x1801,
        VG_LITE_JOIN_BEVEL                      = 0x1802,
    } vg_lite_join_style_t;

    /* Mask operation mode. Match OpenVG enum VGMaskOperation */
    typedef enum vg_lite_mask_operation
    {
        VG_LITE_CLEAR_MASK                      = 0x1500,   /*! Set all dest mask values to 0 */
        VG_LITE_FILL_MASK                       = 0x1501,   /*! Set all dest mask values to 1 */
        VG_LITE_SET_MASK                        = 0x1502,   /*! Copy from src masklayer to dest masklayer. */
        VG_LITE_UNION_MASK                      = 0x1503,   /*! Replace dest masklayer by its union with src masklayer. */
        VG_LITE_INTERSECT_MASK                  = 0x1504,   /*! Replace dest masklayer by its intersection with src masklayer. */
        VG_LITE_SUBTRACT_MASK                   = 0x1505,   /*! Subtract src mask in dest masklayer */
    } vg_lite_mask_operation_t;

    /* Mirror orientation mode. */
    typedef enum vg_lite_orientation
    {
        VG_LITE_ORIENTATION_TOP_BOTTOM,
        VG_LITE_ORIENTATION_BOTTOM_TOP,
    } vg_lite_orientation_t;

    /* Gamma conversion mode. */
    typedef enum vg_lite_gamma_conversion
    {
        VG_LITE_GAMMA_NO_CONVERSION,            /*! Leave color as is. */
        VG_LITE_GAMMA_LINEAR,                   /*! Convert from sRGB to linear space. */
        VG_LITE_GAMMA_NON_LINEAR                /*! Convert from linear to sRGB space. */
    } vg_lite_gamma_conversion_t;

    /* Index endian */
    typedef enum vg_lite_index_endian
    {
        VG_LITE_INDEX_LITTLE_ENDIAN,            /*! Parse the index pixel from low to high,
                                                 *! when using index1, the parsing order is bit0~bit7. 
                                                 *! when using index2, the parsing order is bit0:1,bit2:3,bit4:5.bit6:7.
                                                 *! when using index4, the parsing order is bit0:3,bit4:7.
                                                 */
        VG_LITE_INDEX_BIG_ENDIAN,               /*! Parse the index pixel from low to high,
                                                 *! when using index1, the parsing order is bit7~bit0.
                                                 *! when using index2, the parsing order is bit7:6,bit5:4,bit3:2.bit1:0.
                                                 *! when using index4, the parsing order is bit4:7,bit0:3.
                                                 */
    } vg_lite_index_endian_t;

    /* Map flag*/
    typedef enum vg_lite_map_flag
    {
        VG_LITE_MAP_USER_MEMORY             = 0,
        VG_LITE_MAP_DMABUF                  = 0x01,
    } vg_lite_map_flag_t;

    /*VGLite parameters variable*/
    typedef enum vg_lite_param_type
    {
        VG_LITE_SCISSOR_RECT,                   /*! count must be 4n for x, y, right, bottom */
    } vg_lite_param_type_t;

/* VGLite API Structures ******************************************************************************************************************/

    /* VGLite driver information */
    typedef struct vg_lite_info {
        vg_lite_uint32_t api_version;
        vg_lite_uint32_t header_version;
        vg_lite_uint32_t release_version;
        vg_lite_uint32_t reserved;
    } vg_lite_info_t;

    /* A 2D Point definition. */
    typedef struct vg_lite_point {
        vg_lite_int32_t x;
        vg_lite_int32_t y;
    } vg_lite_point_t;

    /* Four 2D Point that form a polygon */
    typedef vg_lite_point_t vg_lite_point4_t[4];

    /* A rectangle.*/
    typedef struct vg_lite_rectangle {
        vg_lite_int32_t x;                      /*! Left coordinate of rectangle. */
        vg_lite_int32_t y;                      /*! Top coordinate of rectangle. */
        vg_lite_int32_t width;                  /*! Width of rectangle. */
        vg_lite_int32_t height;                 /*! Height of rectangle. */
    } vg_lite_rectangle_t;

    typedef struct vg_lite_matrix {
        vg_lite_float_t m[3][3];                /*! The 3x3 matrix is in [row][column] order. */
        vg_lite_float_t scaleX;
        vg_lite_float_t scaleY;
        vg_lite_float_t angle;
    } vg_lite_matrix_t;

    typedef struct vg_lite_yuvinfo
    {
        vg_lite_swizzle_t swizzle;              /*! UV swizzle. */
        vg_lite_yuv2rgb_t yuv2rgb;              /*! 601 or 709 conversion standard. */
        vg_lite_uint32_t uv_planar;             /*! UV(U) planar address. */
        vg_lite_uint32_t v_planar;              /*! V planar address. */
        vg_lite_uint32_t alpha_planar;          /*! Alpha planar address. */
        vg_lite_uint32_t uv_stride;             /*! UV(U) stride. */
        vg_lite_uint32_t v_stride;              /*! V stride. */
        vg_lite_uint32_t alpha_stride;          /*! Alpha stride. */
        vg_lite_uint32_t uv_height;             /*! UV(U) height. */
        vg_lite_uint32_t v_height;              /*! V height. */
        vg_lite_pointer uv_memory;              /*! The logical pointer to the UV(U) planar memory. */
        vg_lite_pointer v_memory;               /*! The logical pointer to the V planar memory. */
        vg_lite_pointer uv_handle;              /*! The memory handle of the UV(U) planar. */
        vg_lite_pointer v_handle;               /*! The memory handle of the V planar. */
    } vg_lite_yuvinfo_t;

    typedef struct vg_lite_path_point* vg_lite_path_point_ptr;
    typedef struct vg_lite_path_point
    {
        /* X coordinate. */
        vg_lite_float_t x;

        /* Y coordinate. */
        vg_lite_float_t y;

        /* Flatten flag for flattened path. */
        vg_lite_uint8_t flatten_flag;

        /* Curve type for stroke path. */
        vg_lite_uint8_t curve_type;

        /* X tangent. */
        vg_lite_float_t tangentX;

        /* Y tangent. */
        vg_lite_float_t tangentY;

        /* Length of the line. */
        vg_lite_float_t length;

        /* Pointer to next point node. */
        vg_lite_path_point_ptr next;

        /* Pointer to previous point node. */
        vg_lite_path_point_ptr prev;

    } vg_lite_path_point_t;

    typedef struct vg_lite_sub_path* vg_lite_sub_path_ptr;
    typedef struct vg_lite_sub_path
    {
        /* Pointer to next sub path. */
        vg_lite_sub_path_ptr next;

        /* Number of points. */
        vg_lite_uint32_t point_count;

        /* Point list. */
        vg_lite_path_point_ptr point_list;

        /* Last point. */
        vg_lite_path_point_ptr end_point;

        /* Whether is path is closed. */
        vg_lite_uint8_t closed;

        /* Sub path length. */
        vg_lite_float_t length;

    } vg_lite_sub_path_t;

    /* Save divided path data according to MOVE/MOVE_REL. */
    typedef struct vg_lite_path_list* vg_lite_path_list_ptr;
    typedef struct vg_lite_path_list
    {
        vg_lite_path_point_ptr              path_points;
        vg_lite_path_point_ptr              path_end;
        vg_lite_uint32_t                    point_count;
        vg_lite_path_list_ptr               next;
        vg_lite_uint8_t                     closed;

    } vg_lite_path_list_t;

    typedef struct vg_lite_stroke
    {
        /* Stroke parameters */
        vg_lite_cap_style_t                 cap_style;
        vg_lite_join_style_t                join_style;
        vg_lite_float_t                     line_width;
        vg_lite_float_t                     miter_limit;
        vg_lite_float_t                    *dash_pattern;
        vg_lite_uint32_t                    pattern_count;
        vg_lite_float_t                     dash_phase;
        vg_lite_float_t                     dash_length;
        vg_lite_uint32_t                    dash_index;
        vg_lite_float_t                     half_width;

        /* Total length of stroke dash patterns. */
        vg_lite_float_t                     pattern_length;

        /* For fast checking. */
        vg_lite_float_t                     miter_square;

        /* Temp storage of stroke subPath. */
        vg_lite_path_point_ptr              path_points;
        vg_lite_path_point_ptr              path_end;
        vg_lite_uint32_t                    point_count;
        vg_lite_path_point_ptr              left_point;
        vg_lite_path_point_ptr              right_point;
        vg_lite_path_point_ptr              stroke_points;
        vg_lite_path_point_ptr              stroke_end;
        vg_lite_uint32_t                    stroke_count;

        /* Divide stroke path according to move or move_rel for avoiding implicit closure. */
        vg_lite_path_list_ptr               path_list_divide;

        /* pointer to current divided path data. */
        vg_lite_path_list_ptr               cur_list;

        /* Flag that add end_path in driver. */
        vg_lite_uint8_t                     add_end;
        vg_lite_uint8_t                     dash_reset;

        /* Sub path list. */
        vg_lite_sub_path_ptr                stroke_paths;

        /* Last sub path. */
        vg_lite_sub_path_ptr                last_stroke;

        /* Swing area handling. */
        vg_lite_uint32_t                    swing_handling;
        vg_lite_float_t                     swing_deltax;
        vg_lite_float_t                     swing_deltay;
        vg_lite_path_point_ptr              swing_start;
        vg_lite_path_point_ptr              swing_stroke;
        vg_lite_float_t                     swing_length;
        vg_lite_float_t                     swing_centlen;
        vg_lite_uint32_t                    swing_count;
        vg_lite_uint8_t                     need_swing;
        vg_lite_uint8_t                     swing_ccw;

        vg_lite_float_t                     stroke_length;
        vg_lite_uint32_t                    stroke_size;

        /* The stroke line is fat line. */
        vg_lite_uint8_t                     fattened;
        vg_lite_uint8_t                     closed;

    } vg_lite_stroke_t;

    /* Fast clear buffer. */
    typedef struct vg_lite_fc_buffer
    {
        vg_lite_int32_t width;                  /*! Width of the buffer in pixels. */
        vg_lite_int32_t height;                 /*! height of the buffer in pixels. */
        vg_lite_int32_t stride;                 /*! The number of bytes to move from one line in the buffer to the next line. */
        vg_lite_pointer handle;                 /*! The memory handle of the buffer's memory as allocated by the VGLite kernel. */
        vg_lite_pointer memory;                 /*! The logical pointer to the buffer's memory for the CPU. */
        vg_lite_uint32_t address;               /*! The address to the buffer's memory for the hardware. */
        vg_lite_uint32_t color;                 /*! The fastclear color value. */
    } vg_lite_fc_buffer_t;

    /* Structure for any image or render target. */
    typedef struct vg_lite_buffer
    {
        vg_lite_int32_t width;                  /*! Width of the buffer in pixels. */
        vg_lite_int32_t height;                 /*! Height of the buffer in pixels. */
        vg_lite_int32_t stride;                 /*! The number of bytes to move from one line in the buffer to the next line. */
        vg_lite_buffer_layout_t tiled;          /*! Indicating the buffer memory layout is linear or tiled. */
        vg_lite_buffer_format_t format;         /*! The pixel format of the buffer. */
        vg_lite_pointer handle;                 /*! The memory handle of the buffer's memory as allocated by the VGLite kernel. */
        vg_lite_pointer memory;                 /*! The logical pointer to the buffer's memory for the CPU. */
        vg_lite_uint32_t address;               /*! The address to the buffer's memory for the hardware. */
        vg_lite_yuvinfo_t yuv;                  /*! The yuv format details. */
        vg_lite_image_mode_t image_mode;        /*! The blit image mode. */
        vg_lite_transparency_t transparency_mode; /*! image transparency mode. */
        vg_lite_fc_buffer_t fc_buffer[3];       /*! 3 fastclear buffers,reserved YUV format. */
        vg_lite_compress_mode_t compress_mode;  /*! Refer to the definition by vg_lite_compress_mode_t. */
        vg_lite_index_endian_t index_endian;    /*! Refer to the definition by vg_lite_index_endian_t. */
        vg_lite_paint_type_t paintType;         /*! Get paintcolor from different paint types. */
        vg_lite_uint8_t fc_enable;              /*! enable im fastclear. */
        vg_lite_uint8_t scissor_layer;          /*! The buffer is scissor buffer. */
        vg_lite_uint8_t premultiplied;          /*! The RGB pixel values are alpha-premultipled */
    } vg_lite_buffer_t;

    /* Memory allocation info by kernel. */
    typedef struct vg_lite_hw_memory
    {
        vg_lite_pointer handle;                 /*! gpu memory object handle. */
        vg_lite_pointer memory;                 /*! logical memory address. */
        vg_lite_uint32_t address;               /*! GPU memory address. */
        vg_lite_uint32_t bytes;                 /*! Size of memory. */
        vg_lite_uint32_t property;              /*! Currently bit0 is used for path upload state:
                                                 *!   1 : enable auto path data uploading.
                                                 *!   0 : disable path data uploading. path data is embedded in command buffer. */
    } vg_lite_hw_memory_t;

    /* Path info for drawing command. */
    typedef struct vg_lite_path
    {
        vg_lite_float_t bounding_box[4];        /*! Bounding box specified as left, top, right, and bottom. */
        vg_lite_quality_t quality;              /*! Quality hint for the path. */
        vg_lite_format_t format;                /*! Coordinate format. */
        vg_lite_hw_memory_t uploaded;           /*! Path data that has been upload into GPU addressable memory. */
        vg_lite_uint32_t path_length;           /*! Number of bytes in the path data. */
        vg_lite_pointer path;                   /*! Pointer to the physical description of the path. */
        vg_lite_int8_t path_changed;            /*! Indicate whether path data is synced with command buffer (uploaded) or not. */
        vg_lite_int8_t pdata_internal;          /*! Indicate whether path data memory is allocated by driver. */
        vg_lite_path_type_t path_type;          /*! Refer to the definition by vg_lite_path_type_t. */
        vg_lite_stroke_t *stroke;               /*! Pointer to a vg_lite_stroke_t structure.*/
        vg_lite_pointer stroke_path;            /*! Pointer to the physical description of the stroke path. */
        vg_lite_uint32_t stroke_size;           /*! Number of bytes in the stroke path data. */
        vg_lite_color_t stroke_color;           /*! The stroke path fill color. */
        vg_lite_int8_t add_end;                 /*! Flag that add end_path in driver. */
    } vg_lite_path_t;

    /* Color ramp definition. */
    typedef struct vg_lite_color_ramp
    {
        vg_lite_float_t stop;                   /*! Value for the color stop. */
        vg_lite_float_t red;                    /*! Red color channel value for the color stop. */
        vg_lite_float_t green;                  /*! Green color channel value for the color stop. */
        vg_lite_float_t blue;                   /*! Blue color channel value for the color stop. */
        vg_lite_float_t alpha;                  /*! Alpha color channel value for the color stop. */
    } vg_lite_color_ramp_t;

    /* Linear gradient parameter */
    typedef struct vg_lite_linear_gradient_parameter
    {
        vg_lite_float_t X0;
        vg_lite_float_t Y0;
        vg_lite_float_t X1;
        vg_lite_float_t Y1;
    } vg_lite_linear_gradient_parameter_t;

    typedef struct vg_lite_radial_gradient_parameter
    {
        vg_lite_float_t cx;                                 /*! x coordinate of the center point. */
        vg_lite_float_t cy;                                 /*! y coordinate of the center point. */
        vg_lite_float_t r;                                  /*! radius. */
        vg_lite_float_t fx;                                 /*! x coordinate of the focal point. */
        vg_lite_float_t fy;                                 /*! y coordinate of the focal point. */
    } vg_lite_radial_gradient_parameter_t;

    /* Linear gradient definition. */
    typedef struct vg_lite_linear_gradient {
        vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];    /*! Colors for stops. */
        vg_lite_uint32_t count;                             /*! Count of colors, up to 16. */
        vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];     /*! Color stops, value from 0 to 255. */
        vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
        vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
    } vg_lite_linear_gradient_t;

    /* Extended linear gradient definition. */
    typedef struct vg_lite_ext_linear_gradient {
        vg_lite_uint32_t count;                             /*! Count of colors, up to 256. */
        vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
        vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
        vg_lite_linear_gradient_parameter_t linear_grad;    /*!  Include center point,focal point and radius.*/

        vg_lite_uint32_t ramp_length;                       /*! Color ramp for gradient paints provided to driver. */
        vg_lite_color_ramp_t color_ramp[VLC_MAX_COLOR_RAMP_STOPS];

        vg_lite_uint32_t converted_length;                  /*! Converted internal color ramp. */
        vg_lite_color_ramp_t converted_ramp[VLC_MAX_COLOR_RAMP_STOPS + 2];

        vg_lite_uint8_t pre_multiplied;                     /*! If color values of color_ramp[] are multiply by alpha value of color_ramp[]. */
        vg_lite_gradient_spreadmode_t spread_mode;          /*! The spread mode that applied to the pixels out of the image after transformed. */
    } vg_lite_ext_linear_gradient_t;

    /* Radial gradient definition. */
    typedef struct vg_lite_radial_gradient
    {
        vg_lite_uint32_t count;                             /*! Count of colors, up to 256. */
        vg_lite_matrix_t matrix;                            /*! The matrix to transform the gradient. */
        vg_lite_buffer_t image;                             /*! The image for rendering as gradient pattern. */
        vg_lite_radial_gradient_parameter_t radial_grad;    /*!  Include center point,focal point and radius.*/

        vg_lite_uint32_t ramp_length;                       /*! Color ramp for gradient paints provided to the driver. */
        vg_lite_color_ramp_t color_ramp[VLC_MAX_COLOR_RAMP_STOPS];

        vg_lite_uint32_t converted_length;                  /*! Converted internal color ramp. */
        vg_lite_color_ramp_t converted_ramp[VLC_MAX_COLOR_RAMP_STOPS + 2];

        vg_lite_uint8_t pre_multiplied;                     /*! If color values of color_ramp[] are multiply by alpha value of color_ramp[]. */
        vg_lite_gradient_spreadmode_t spread_mode;          /*! The spread mode that applied to the pixels out of the image after transformed. */
    } vg_lite_radial_gradient_t;

    /* Colorkey definition */
    typedef struct vg_lite_color_key
    {
        vg_lite_uint8_t enable;                 /*! The color key is effective only when "enable" is ture, */
        vg_lite_uint8_t low_r;                  /*! The R chanel of low_rgb. */
        vg_lite_uint8_t low_g;                  /*! The G chanel of low_rgb. */
        vg_lite_uint8_t low_b;                  /*! The B chanel of low_rgb. */
        vg_lite_uint8_t alpha;                  /*! The alpha channel to replace destination pixel alpha channel.*/
        vg_lite_uint8_t hign_r;                 /*! The R chanel of hign_rgb. */
        vg_lite_uint8_t hign_g;                 /*! The G chanel of hign_rgb. */
        vg_lite_uint8_t hign_b;                 /*! The B chanel of hign_rgb. */
    } vg_lite_color_key_t;

    /* Four colorkey definition.
     * rgb_hi_0, rgb_lo_0, alpha_0, enable_0;
     * rgb_hi_1, rgb_lo_1, alpha_1, enable_1;
     * rgb_hi_2, rgb_lo_2, alpha_2, enable_2;
     * rgb_hi_3, rgb_lo_3, alpha_3, enable_3;
     * Priority order: color_key_0 > color_key_1 > color_key_2 > color_key_3.
    */
    typedef vg_lite_color_key_t vg_lite_color_key4_t[4];

    /* Pixel matrix values */
    typedef vg_lite_float_t vg_lite_pixel_matrix_t[20];

    /* HW pixel channel enable flags */
    typedef struct vg_lite_pixel_channel_enable
    {
        vg_lite_uint8_t enable_a;               /*! Enable A channel.*/
        vg_lite_uint8_t enable_b;               /*! Enable B channel. */
        vg_lite_uint8_t enable_g;               /*! Enable G channel. */
        vg_lite_uint8_t enable_r;               /*! Enable R channel. */
    } vg_lite_pixel_channel_enable_t;

    /* Pixel color transform */
    typedef struct vg_lite_color_transform
    {
        vg_lite_float_t a_scale;
        vg_lite_float_t a_bias;
        vg_lite_float_t r_scale;
        vg_lite_float_t r_bias;
        vg_lite_float_t g_scale;
        vg_lite_float_t g_bias;
        vg_lite_float_t b_scale;
        vg_lite_float_t b_bias;
    } vg_lite_color_transform_t;

/* VGLite API Functions *******************************************************************************************************************/

    /* Initialize a vglite context. */
    vg_lite_error_t vg_lite_init(vg_lite_int32_t tess_width, vg_lite_int32_t tess_height);

    /* Destroy a vglite context. */
    vg_lite_error_t vg_lite_close(void);

    /* Get the VGLite driver information. */
    vg_lite_error_t vg_lite_get_info(vg_lite_info_t* info);

    /* Get the GPU chip information. */
    vg_lite_uint32_t vg_lite_get_product_info(vg_lite_char *name, vg_lite_uint32_t *chip_id, vg_lite_uint32_t *chip_rev);

    /* Query if a specific feature is supported. */
    vg_lite_uint32_t vg_lite_query_feature(vg_lite_feature_t feature);

    /* Flush command buffer and wait for GPU to complete. */
    vg_lite_error_t vg_lite_finish(void);

    /* Flush the command buffer without waiting for GPU to complete. */
    vg_lite_error_t vg_lite_flush(void);

    /* Get the value of register from register's address. */
    vg_lite_error_t vg_lite_get_register(vg_lite_uint32_t address, vg_lite_uint32_t* result);

    /* Generate a 3x3 homogenous matrix to transform 4 source coordinates to 4 target coordinates. */
    vg_lite_error_t vg_lite_get_transform_matrix(vg_lite_point4_t src, vg_lite_point4_t dst, vg_lite_matrix_t *mat);

    /* Allocate a buffer from GPU hardware accessible memory. */
    vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t *buffer);

    /* Free a buffer allocated by vg_lite_allocate() */
    vg_lite_error_t vg_lite_free(vg_lite_buffer_t *buffer);

    /* Upload RGB or YUV pixel data to an allocated buffer. */
    vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t *buffer, vg_lite_uint8_t *data[3], vg_lite_uint32_t stride[3]);

    /* Map a buffer into hardware accessible address space. */
    vg_lite_error_t vg_lite_map(vg_lite_buffer_t *buffer, vg_lite_map_flag_t flag, int32_t fd);

    /* Unmap a buffer that is mapped */
    vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t *buffer);

    /* flush cache */
    vg_lite_error_t vg_lite_flush_mapped_buffer(vg_lite_buffer_t * buffer);

    /* Fill a buffer rectangle area with a specified color. */
    vg_lite_error_t vg_lite_clear(vg_lite_buffer_t *target, vg_lite_rectangle_t *rect, vg_lite_color_t color);

    /* Copy a source image to target buffer with transformation, blending, color mixing, and filtering. */
    vg_lite_error_t vg_lite_blit(vg_lite_buffer_t *target,
                                    vg_lite_buffer_t *source,
                                    vg_lite_matrix_t *matrix,
                                    vg_lite_blend_t   blend,
                                    vg_lite_color_t   color,
                                    vg_lite_filter_t  filter);

    /* Copy a rectangle area of source image to target buffer with transformation, blending, color mixing, and filtering. */
    vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t *target,
                                    vg_lite_buffer_t *source,
                                    vg_lite_rectangle_t *rect,
                                    vg_lite_matrix_t *matrix,
                                    vg_lite_blend_t   blend,
                                    vg_lite_color_t   color,
                                    vg_lite_filter_t  filter);

    /* Copy two source images to the target buffer with transformation, blending, and filtering. */
    vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t *target,
                                    vg_lite_buffer_t *source0,
                                    vg_lite_buffer_t *source1,
                                    vg_lite_matrix_t *matrix0,
                                    vg_lite_matrix_t *matrix1,
                                    vg_lite_blend_t blend,
                                    vg_lite_filter_t  filter);

    /* Copy a rectangle area of source image to target buffer without transformation, blending, color mixing, and filtering. */
    vg_lite_error_t vg_lite_copy_image(vg_lite_buffer_t *target,
                                    vg_lite_buffer_t *source,
                                    vg_lite_int32_t   sx,
                                    vg_lite_int32_t   sy,
                                    vg_lite_int32_t   dx,
                                    vg_lite_int32_t   dy,
                                    vg_lite_int32_t   width,
                                    vg_lite_int32_t   height);

    /* Draw a path to a target buffer with transformation, color, and blending */
    vg_lite_error_t vg_lite_draw(vg_lite_buffer_t *target,
                                    vg_lite_path_t   *path,
                                    vg_lite_fill_t    fill_rule,
                                    vg_lite_matrix_t *matrix,
                                    vg_lite_blend_t   blend,
                                    vg_lite_color_t   color);

    /* Set stroke path attributes. */
    vg_lite_error_t vg_lite_set_stroke(vg_lite_path_t *path,
                                    vg_lite_cap_style_t cap_style,
                                    vg_lite_join_style_t join_style,
                                    vg_lite_float_t line_width,
                                    vg_lite_float_t miter_limit,
                                    vg_lite_float_t *dash_pattern,
                                    vg_lite_uint32_t pattern_count,
                                    vg_lite_float_t dash_phase,
                                    vg_lite_color_t color);

    /* Update stroke path. */
    vg_lite_error_t vg_lite_update_stroke(vg_lite_path_t *path);

    /* Set path type. */
    vg_lite_error_t vg_lite_set_path_type(vg_lite_path_t *path, vg_lite_path_type_t path_type);

    /* Clears all attributes of a path. */
    vg_lite_error_t vg_lite_clear_path(vg_lite_path_t *path);

    /* Upload a path to GPU memory so GPU can access it directly. */
    vg_lite_error_t vg_lite_upload_path(vg_lite_path_t *path);

    /* Initialize a path object with attributes. */
    vg_lite_error_t vg_lite_init_path(vg_lite_path_t *path,
                                    vg_lite_format_t format,
                                    vg_lite_quality_t quality,
                                    vg_lite_uint32_t length,
                                    vg_lite_pointer data,
                                    vg_lite_float_t min_x,
                                    vg_lite_float_t min_y,
                                    vg_lite_float_t max_x,
                                    vg_lite_float_t max_y);

     /* Initializes a arc path with attributes. */
    vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t *path,
                                    vg_lite_format_t format,
                                    vg_lite_quality_t quality,
                                    vg_lite_uint32_t length,
                                    vg_lite_pointer data,
                                    vg_lite_float_t min_x,
                                    vg_lite_float_t min_y,
                                    vg_lite_float_t max_x,
                                    vg_lite_float_t max_y);

    /* Return the size (in bytes) of command buffer for a path opcode array. */
    vg_lite_uint32_t vg_lite_get_path_length(vg_lite_uint8_t *opcode,
                                    vg_lite_uint32_t count,
                                    vg_lite_format_t format);

    /* Generate command buffer for the (path) based on input opcodes (opcode) and coordinates (data). */
    vg_lite_error_t vg_lite_append_path(vg_lite_path_t *path,
                                    vg_lite_uint8_t *opcode,
                                    vg_lite_pointer data,
                                    vg_lite_uint32_t seg_count);

    /* Set CLUT (Color Look Up Table) for index image. The (colors) is in ARGB format. */
    vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count, vg_lite_uint32_t *colors);

    /* Draw a path that is filled by a transformed image pattern. */
    vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t *target,
                                    vg_lite_path_t *path,
                                    vg_lite_fill_t fill_rule,
                                    vg_lite_matrix_t *path_matrix,
                                    vg_lite_buffer_t *pattern_image,
                                    vg_lite_matrix_t *pattern_matrix,
                                    vg_lite_blend_t blend,
                                    vg_lite_pattern_mode_t pattern_mode,
                                    vg_lite_color_t  pattern_color,
                                    vg_lite_color_t  color,
                                    vg_lite_filter_t filter);

    /* Initialize a linear gradient object with default attributes. */
    vg_lite_error_t vg_lite_init_grad(vg_lite_linear_gradient_t *grad);

    /* Reset a linear gradient object attributes. */
    vg_lite_error_t vg_lite_clear_grad(vg_lite_linear_gradient_t *grad);

    /* Update a linear gradient object. */
    vg_lite_error_t vg_lite_update_grad(vg_lite_linear_gradient_t *grad);

    /* Return pointer to a linear gradient object's matrix. */
    vg_lite_matrix_t* vg_lite_get_grad_matrix(vg_lite_linear_gradient_t *grad);

    /* Set attributes for a linear gradient object. */
    vg_lite_error_t vg_lite_set_grad(vg_lite_linear_gradient_t *grad,
                                    vg_lite_uint32_t count,
                                    vg_lite_uint32_t *colors,
                                    vg_lite_uint32_t *stops);

    /* Draw a path with a linear gradient object pattern. */
    vg_lite_error_t vg_lite_draw_grad(vg_lite_buffer_t *target,
                                    vg_lite_path_t *path,
                                    vg_lite_fill_t fill_rule,
                                    vg_lite_matrix_t *matrix,
                                    vg_lite_linear_gradient_t *grad,
                                    vg_lite_blend_t blend);

    /* Reset an extended linear gradient object attributes and free image buffer. */
    vg_lite_error_t vg_lite_clear_linear_grad(vg_lite_ext_linear_gradient_t *grad);

    /* Update an extended linear gradient object. */
    vg_lite_error_t vg_lite_update_linear_grad(vg_lite_ext_linear_gradient_t *grad);

    /* Return pointer to an extended linear gradient object's matrix. */
    vg_lite_matrix_t* vg_lite_get_linear_grad_matrix(vg_lite_ext_linear_gradient_t *grad);

    /* Set attributes for an extended linear gradient object. */
    vg_lite_error_t vg_lite_set_linear_grad(vg_lite_ext_linear_gradient_t *grad,
                                    vg_lite_uint32_t count,
                                    vg_lite_color_ramp_t *color_ramp,
                                    vg_lite_linear_gradient_parameter_t grad_param,
                                    vg_lite_gradient_spreadmode_t spread_mode,
                                    vg_lite_uint8_t pre_mult);

    /* Draw a path with an extended linear gradient object. */
    vg_lite_error_t vg_lite_draw_linear_grad(vg_lite_buffer_t *target,
                                    vg_lite_path_t *path,
                                    vg_lite_fill_t fill_rule,
                                    vg_lite_matrix_t *path_matrix,
                                    vg_lite_ext_linear_gradient_t *grad,
                                    vg_lite_color_t paint_color,
                                    vg_lite_blend_t blend,
                                    vg_lite_filter_t filter);

    /* Reset a radial gradient object attributes and free image buffer. */
    vg_lite_error_t vg_lite_clear_radial_grad(vg_lite_radial_gradient_t *grad);

    /* Update a radial gradient object. */
    vg_lite_error_t vg_lite_update_radial_grad(vg_lite_radial_gradient_t *grad);

    /* Return pointer to a radial gradient object's matrix. */
    vg_lite_matrix_t* vg_lite_get_radial_grad_matrix(vg_lite_radial_gradient_t *grad);

    /* Set attributes for a radial gradient object. */
    vg_lite_error_t vg_lite_set_radial_grad(vg_lite_radial_gradient_t *grad,
                                    vg_lite_uint32_t count,
                                    vg_lite_color_ramp_t *color_ramp,
                                    vg_lite_radial_gradient_parameter_t grad_param,
                                    vg_lite_gradient_spreadmode_t spread_mode,
                                    vg_lite_uint8_t pre_mult);

    /* Draw a path with a radial gradient object pattern. */
    vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t *target,
                                    vg_lite_path_t *path,
                                    vg_lite_fill_t fill_rule,
                                    vg_lite_matrix_t *path_matrix,
                                    vg_lite_radial_gradient_t *grad,
                                    vg_lite_color_t  paint_color,
                                    vg_lite_blend_t blend,
                                    vg_lite_filter_t filter);

    /* Load an identity matrix. */
    vg_lite_error_t vg_lite_identity(vg_lite_matrix_t *matrix);

    /* Translate a matrix. */
    vg_lite_error_t vg_lite_translate(vg_lite_float_t x, vg_lite_float_t y, vg_lite_matrix_t *matrix);

    /* Scale a matrix. */
    vg_lite_error_t vg_lite_scale(vg_lite_float_t scale_x, vg_lite_float_t scale_y, vg_lite_matrix_t *matrix);

    /* Rotate a matrix. */
    vg_lite_error_t vg_lite_rotate(vg_lite_float_t degrees, vg_lite_matrix_t *matrix);

    /* Set and enable a scissor rectangle for render target. */
    vg_lite_error_t vg_lite_set_scissor(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right, vg_lite_int32_t bottom);

    /* Set scissor rectangles on mask layer. Scissor rects are enabled/disabled by following APIs. */
    vg_lite_error_t vg_lite_scissor_rects(vg_lite_uint32_t nums, vg_lite_rectangle_t rect[]);

    /* Enable scissor rects defined on mask layer. */
    vg_lite_error_t vg_lite_enable_scissor(void);

    /* Disable scissor rects defined on mask layer. */
    vg_lite_error_t vg_lite_disable_scissor(void);

    /* Query size of available contiguous video memory. */
    vg_lite_error_t vg_lite_get_mem_size(vg_lite_uint32_t *size);

    /* Set global alpha value for source image */
    vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);

    /* Set global alpha value for destination image. */
    vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);

    /* Set colorkey. */
    vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey);

    /* Enable dither function. Dither is OFF by default. */
    vg_lite_error_t vg_lite_enable_dither(void);

    /* Disable dither function. Dither is OFF by default. */
    vg_lite_error_t vg_lite_disable_dither(void);

    /* Set a 64-byte aligned memory buffer (physical) as VGLite tessellation buffer. */
    vg_lite_error_t vg_lite_set_tess_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size);

    /* Can be called before vg_lite_init() to overwrite the default VG_LITE_COMMAND_BUFFER_SIZE */
    vg_lite_error_t vg_lite_set_command_buffer_size(vg_lite_uint32_t size);

    /* Set a user-defined external memory buffer (physical, 64-byte aligned) as VGLite command buffer. */
    vg_lite_error_t vg_lite_set_command_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size);

    /* Setup a pixel transform matrix m[20] which transforms each pixel as following:
     *
     *              |a'|   |m0  m1  m2  m3  m4 | |a|
     *              |r'|   |m5  m6  m7  m8  m9 | |r|
     *              |g'| = |m10 m11 m12 m13 m14|.|g|
     *              |b'|   |m15 m16 m17 m18 m19| |b|
     *              |1 |   |0   0   0   0   1  | |1|
     *
     *  The pixel transform for A, R, G, B channel can be enabled/disabled individually with (channel) parameter.
     */
    vg_lite_error_t vg_lite_set_pixel_matrix(vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t *channel);

    /* Setup 3x3 gaussian blur weight values to filter image pixels.
     *
     *  Paramters w0, w1, w2 define a 3x3 gaussian blur weight matrix as below
     *
     *                  |  w2   w1   w2 |
     *                  |  w1   w0   w1 |
     *                  |  w2   w1   w2 |
     *
     *  The sum of 9 kernel weights must be 1.0 to avoid convolution overflow ( w0 + 4*w1 + 4*w2 = 1.0 ).
     *  The 3x3 weight matrix applies to a 3x3 pixel block
     *
     *                  | pixel[i-1][j-1]     pixel[i][j-1]       pixel[i+1][j-1]|
     *                  | pixel[i-1][j]       pixel[i][j]         pixel[i+1][j]  |
     *                  | pixel[i-1][j+1]     pixel[i][j+1]       pixel[i+1][j+1]|
     *
     *  With the following dot product equation:
     *
     *     color[i][j] = w2*pixel[i-1][j-1] + w1*pixel[i][j-1] + w2*pixel[i+1][j-1]
     *                 + w1*pixel[i-1][j]   + w0*pixel[i][j]   + w1*pixel[i+1][j]
     *                 + w2*pixel[i-1][j+1] + w1*pixel[i][j+1] + w2*pixel[i+1][j+1];
     */
    vg_lite_error_t vg_lite_gaussian_filter(vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2);

    /*  Enable masklayer function. Masklayer is OFF by default. */
    vg_lite_error_t vg_lite_enable_masklayer(void);

    /*  Disable masklayer function. Masklayer is OFF by default. */
    vg_lite_error_t vg_lite_disable_masklayer(void);

    /* Setup a masklayer. */
    vg_lite_error_t vg_lite_set_masklayer(vg_lite_buffer_t *masklayer);

    /* Free a masklayer and disable mask operation. */
    vg_lite_error_t vg_lite_destroy_masklayer(vg_lite_buffer_t *masklayer);

    /* Create a masklayer with default format A8 and default pixel value 255. */
    vg_lite_error_t vg_lite_create_masklayer(vg_lite_buffer_t *masklayer,
                                    vg_lite_uint32_t width,
                                    vg_lite_uint32_t height);

    /* Set pixel values for a rectangle area in a masklayer */
    vg_lite_error_t vg_lite_fill_masklayer(vg_lite_buffer_t *masklayer,
                                    vg_lite_rectangle_t *rect,
                                    vg_lite_uint8_t value);

    /* Blend a rectangle area of src masklayer with dst masklayer according to (operation). */
    vg_lite_error_t vg_lite_blend_masklayer(vg_lite_buffer_t *dst,
                                    vg_lite_buffer_t *src,
                                    vg_lite_mask_operation_t operation,
                                    vg_lite_rectangle_t *rect);

    /* Render a (path) with (fill_rule), (color), (matrix) to the masklayer. */
    vg_lite_error_t vg_lite_render_masklayer(vg_lite_buffer_t *masklayer,
                                    vg_lite_mask_operation_t operation,
                                    vg_lite_path_t *path,
                                    vg_lite_fill_t fill_rule,
                                    vg_lite_color_t color,
                                    vg_lite_matrix_t *matrix);

    /* Set mirror orientation. */
    vg_lite_error_t vg_lite_set_mirror(vg_lite_orientation_t orientation);

    /* Set gamma value. */
    vg_lite_error_t vg_lite_set_gamma(vg_lite_gamma_conversion_t gamma_value);

    /* Enable color transformation, which is OFF by default. */
    vg_lite_error_t vg_lite_enable_color_transform(void);

    /* Disable color transformation, which is OFF by default. */
    vg_lite_error_t vg_lite_disable_color_transform(void);

    /* Set pixel color transformation scale and bias values for each pixel channel. */
    vg_lite_error_t vg_lite_set_color_transform(vg_lite_color_transform_t *values);

    /* Set flexa stream id. */
    vg_lite_error_t vg_lite_flexa_set_stream(vg_lite_uint8_t stream_id);

    /* set flexa background buffer.*/
    vg_lite_error_t vg_lite_flexa_bg_buffer(vg_lite_uint8_t stream_id,
                                    vg_lite_buffer_t *buffer,
                                    vg_lite_uint32_t seg_count,
                                    vg_lite_uint32_t seg_size);

    /* Enable flexa. */
    vg_lite_error_t vg_lite_flexa_enable(void);

    /* Disable flexa.*/
    vg_lite_error_t vg_lite_flexa_disable(void);

    /* Set flexa stop flag after the last frame. */
    vg_lite_error_t vg_lite_flexa_stop_frame(void);

    /* Dump command buffer */
    vg_lite_error_t vg_lite_dump_command_buffer(void);

    /* Return VGLite parameters in params[] array */
    vg_lite_error_t vg_lite_get_parameter(vg_lite_param_type_t type,
                                    vg_lite_int32_t count,
                                    vg_lite_float_t* params);

#endif /* VGLITE_VERSION_3_0 */

#ifdef __cplusplus
}
#endif
#endif /* _vg_lite_h_ */