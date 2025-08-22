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

#ifndef _dump_vgl_h_
#define _dump_vgl_h_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"vg_lite.h"

#ifdef _MSC_VER
#define alignof(type) _Alignof(type)
#else
#include<stdalign.h>
#endif //  _MSC_VER

#define ALIGN(size, alignto) (((size) + (alignto) - 1) & ~((alignto) - 1))
#define ALIGN_TO(size, alignto)\
    do{\
        size = ALIGN(size, alignto);\
    }while(0)
#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

typedef struct DumpVGLHandler {
    FILE* file;
    size_t offset;

    vg_lite_buffer_t* buffer[512];
    int bufferNum;

    vg_lite_ext_linear_gradient_t* lgradEX[512];
    int lgradEXNum;

    vg_lite_linear_gradient_t* lgrad[512];
    int lgradNum;

    vg_lite_radial_gradient_t* rgrad[512];
    int rgradNum;
} DumpVGLHandler;

extern DumpVGLHandler handler;

#define FUNC_DUMP(func)                   _##func##_dump

DLLEXPORT void FUNC_DUMP(vg_lite_init)(vg_lite_int32_t tess_width, vg_lite_int32_t tess_height);
DLLEXPORT void FUNC_DUMP(vg_lite_close)(void);
DLLEXPORT void FUNC_DUMP(vg_lite_finish)(void);
DLLEXPORT void FUNC_DUMP(vg_lite_flush)(void);
DLLEXPORT void FUNC_DUMP(vg_lite_allocate)(vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_free)(vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_upload_buffer)(vg_lite_buffer_t* buffer, vg_lite_uint8_t* data[3], vg_lite_uint32_t stride[3]);
DLLEXPORT void FUNC_DUMP(vg_lite_map)(vg_lite_buffer_t* buffer, vg_lite_map_flag_t flag, int32_t fd);
DLLEXPORT void FUNC_DUMP(vg_lite_unmap)(vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_flush_mapped_buffer)(vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_clear)(vg_lite_buffer_t* target, vg_lite_rectangle_t* rect, vg_lite_color_t color);
DLLEXPORT void FUNC_DUMP(vg_lite_blit)(vg_lite_buffer_t* target, vg_lite_buffer_t* source, vg_lite_matrix_t* matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_blit_rect)(vg_lite_buffer_t* target, vg_lite_buffer_t* source, vg_lite_rectangle_t* rect, vg_lite_matrix_t* matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_blit2)(vg_lite_buffer_t* target, vg_lite_buffer_t* source0, vg_lite_buffer_t* source1, vg_lite_matrix_t* matrix0, vg_lite_matrix_t* matrix1, vg_lite_blend_t blend, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_draw)(vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_matrix_t* matrix, vg_lite_blend_t blend, vg_lite_color_t color);
DLLEXPORT void FUNC_DUMP(vg_lite_upload_path)(vg_lite_path_t* path);
DLLEXPORT void FUNC_DUMP(vg_lite_draw_pattern)(vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_matrix_t* path_matrix, vg_lite_buffer_t* pattern_image, vg_lite_matrix_t* pattern_matrix, vg_lite_blend_t blend, vg_lite_pattern_mode_t pattern_mode, vg_lite_color_t  pattern_color, vg_lite_color_t  color, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_draw_grad)(vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_matrix_t* matrix, vg_lite_linear_gradient_t* grad, vg_lite_blend_t blend);
DLLEXPORT void FUNC_DUMP(vg_lite_draw_linear_grad)(vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_matrix_t* path_matrix, vg_lite_ext_linear_gradient_t* grad, vg_lite_color_t paint_color, vg_lite_blend_t blend, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_draw_radial_grad)(vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_matrix_t* path_matrix, vg_lite_radial_gradient_t* grad, vg_lite_color_t  paint_color, vg_lite_blend_t blend, vg_lite_filter_t filter);
DLLEXPORT void FUNC_DUMP(vg_lite_update_linear_grad)(vg_lite_ext_linear_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_clear_linear_grad)(vg_lite_ext_linear_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_update_grad)(vg_lite_linear_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_clear_grad)(vg_lite_linear_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_update_radial_grad)(vg_lite_radial_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_clear_radial_grad)(vg_lite_radial_gradient_t* grad);
DLLEXPORT void FUNC_DUMP(vg_lite_set_scissor)(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right, vg_lite_int32_t bottom);
DLLEXPORT void FUNC_DUMP(vg_lite_enable_scissor)();
DLLEXPORT void FUNC_DUMP(vg_lite_disable_scissor)();
DLLEXPORT void FUNC_DUMP(vg_lite_source_global_alpha)(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);
DLLEXPORT void FUNC_DUMP(vg_lite_dest_global_alpha)(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value);
DLLEXPORT void FUNC_DUMP(vg_lite_set_color_key)(vg_lite_color_key4_t colorkey);
DLLEXPORT void FUNC_DUMP(vg_lite_enable_dither)();
DLLEXPORT void FUNC_DUMP(vg_lite_disable_dither)();
DLLEXPORT void FUNC_DUMP(vg_lite_set_pixel_matrix)(vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t* channel);
DLLEXPORT void FUNC_DUMP(vg_lite_gaussian_filter)(vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2);
DLLEXPORT void FUNC_DUMP(vg_lite_enable_masklayer)();
DLLEXPORT void FUNC_DUMP(vg_lite_disable_masklayer)();
DLLEXPORT void FUNC_DUMP(vg_lite_set_masklayer)(vg_lite_buffer_t* masklayer);
DLLEXPORT void FUNC_DUMP(vg_lite_destroy_masklayer)(vg_lite_buffer_t* masklayer);
DLLEXPORT void FUNC_DUMP(vg_lite_create_masklayer)(vg_lite_buffer_t* masklayer, vg_lite_uint32_t width, vg_lite_uint32_t height);
DLLEXPORT void FUNC_DUMP(vg_lite_fill_masklayer)(vg_lite_buffer_t* masklayer, vg_lite_rectangle_t* rect, vg_lite_uint8_t value);
DLLEXPORT void FUNC_DUMP(vg_lite_blend_masklayer)(vg_lite_buffer_t* dst_masklayer, vg_lite_buffer_t* src_masklayer, vg_lite_mask_operation_t operation, vg_lite_rectangle_t* rect);
DLLEXPORT void FUNC_DUMP(vg_lite_render_masklayer)(vg_lite_buffer_t* masklayer, vg_lite_mask_operation_t operation, vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_color_t color, vg_lite_matrix_t* matrix);
DLLEXPORT void FUNC_DUMP(vg_lite_scissor_rects)(vg_lite_buffer_t* target, vg_lite_uint32_t nums, vg_lite_rectangle_t rect[]);
DLLEXPORT void FUNC_DUMP(vg_lite_set_mirror)(vg_lite_orientation_t orientation);
DLLEXPORT void FUNC_DUMP(vg_lite_set_gamma)(vg_lite_gamma_conversion_t gamma_value);
DLLEXPORT void FUNC_DUMP(vg_lite_enable_color_transform)();
DLLEXPORT void FUNC_DUMP(vg_lite_disable_color_transform)();
DLLEXPORT void FUNC_DUMP(vg_lite_set_color_transform)(vg_lite_color_transform_t* values);
DLLEXPORT void FUNC_DUMP(vg_lite_flexa_set_stream)(vg_lite_uint8_t stream_id);
DLLEXPORT void FUNC_DUMP(vg_lite_flexa_bg_buffer)(vg_lite_uint8_t stream_id, vg_lite_buffer_t* buffer, vg_lite_uint32_t bg_seg_count, vg_lite_uint32_t bg_seg_size);
DLLEXPORT void FUNC_DUMP(vg_lite_flexa_enable)();
DLLEXPORT void FUNC_DUMP(vg_lite_flexa_disable)();
DLLEXPORT void FUNC_DUMP(vg_lite_flexa_stop_frame)();
DLLEXPORT void FUNC_DUMP(vg_lite_dump_command_buffer)();
DLLEXPORT void FUNC_DUMP(vg_lite_save_png)(const char* name, vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_load_png)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_fb_open)(void);
DLLEXPORT void FUNC_DUMP(vg_lite_fb_close)(vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_load_raw_yuv)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_yv12toyv24)(vg_lite_buffer_t* buffer1, vg_lite_buffer_t* buffer2);
DLLEXPORT void FUNC_DUMP(vg_lite_yv12toyv16)(vg_lite_buffer_t* buffer1, vg_lite_buffer_t* buffer2);
DLLEXPORT void FUNC_DUMP(vg_lite_load_raw)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_load_raw_byline)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_save_raw)(const char* name, vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_save_raw_byline)(const char* name, vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_load_pkm)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_load_pkm_info_to_buffer)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_load_dev_info_to_buffer)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_load_decnano_compressd_data)(vg_lite_buffer_t* buffer, const char* name);
DLLEXPORT void FUNC_DUMP(vg_lite_save_decnano_compressd_data)(const char* name, vg_lite_buffer_t* buffer);
DLLEXPORT void FUNC_DUMP(vg_lite_set_command_buffer_size)(vg_lite_uint32_t size);
DLLEXPORT void FUNC_DUMP(vg_lite_set_memory_pool)(vg_lite_buffer_type_t type, vg_lite_memory_pool_t pool);
DLLEXPORT void FUNC_DUMP(vg_lite_init_grad)(vg_lite_linear_gradient_t* grad);

#endif