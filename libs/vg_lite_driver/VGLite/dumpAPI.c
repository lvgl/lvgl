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

#if 0 /*VGLite dump API is not supported yet */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"dumpAPI.h"
#include"vg_lite_context.h"

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

#define IMAGE_CONTROL   0
#define RESOLUTION_2K_WIDTH 2048
#define RESOLUTION_2K_HEIGHT 1024
#define BYTE_PER_PIXEL 4

#if DUMP_API

DumpVGLHandler handler;

static int createVgl = 1;
static int lgradEX_count = 0;

static void dump_vgl_serialize(const void* ptr, size_t bytes, size_t alignto) {
    FILE* file = handler.file;
    const uint8_t dummy = 0;

    for (size_t i = handler.offset; i < ALIGN(handler.offset, alignto); ++i)
        fwrite(&dummy, 1, 1, file);
    ALIGN_TO(handler.offset, alignto);
    fwrite(ptr, 1, bytes, file);
    handler.offset += bytes;
}

static int dump_vgl_flush() {
    return fflush(handler.file);
}

static void dump_vgl_allocate_buffer(vg_lite_buffer_t* buf) {
    vg_lite_buffer_t srb;
    
    handler.buffer[handler.bufferNum++] = buf;

    memcpy(&srb, buf, sizeof(vg_lite_buffer_t));
    srb.handle = srb.memory =srb.lvgl_buffer=
    srb.yuv.uv_handle = srb.yuv.uv_memory =
    srb.yuv.v_handle = srb.yuv.v_memory =
    srb.fc_buffer[0].handle = srb.fc_buffer[0].memory =
    srb.fc_buffer[1].handle = srb.fc_buffer[1].memory =
    srb.fc_buffer[2].handle = srb.fc_buffer[2].memory =
    NULL;
    srb.address =
    srb.fc_buffer[0].address =
    srb.fc_buffer[1].address =
    srb.fc_buffer[2].address =
    0;

    dump_vgl_serialize(&srb, sizeof(srb), alignof(vg_lite_buffer_t));
}

static int dump_vgl_refer_to_buffer(vg_lite_buffer_t* buf) {
    for (int i = 0; i < handler.bufferNum; ++i) {
        if (handler.buffer[i] == buf) return i;
    }
    /* Error: not found buffer. */
    return -1;
}

static int dump_vgl_free_buffer(vg_lite_buffer_t* buf) {
    int i;
    i = dump_vgl_refer_to_buffer(buf);
    if (i == -1)
        return -1;
    else {
        handler.buffer[i] = NULL;
    }
    return i;
}

static void dump_vgl_allocate_lgradEX(vg_lite_ext_linear_gradient_t* lgradEX) {
    vg_lite_ext_linear_gradient_t srlg;
    
    handler.lgradEX[handler.lgradEXNum++] = lgradEX;

    memcpy(&srlg, lgradEX, sizeof(vg_lite_ext_linear_gradient_t));
    srlg.image.handle = srlg.image.memory =
    srlg.image.yuv.uv_handle = srlg.image.yuv.uv_memory = 
    srlg.image.yuv.v_handle = srlg.image.yuv.v_memory =
    srlg.image.fc_buffer[0].handle = srlg.image.fc_buffer[0].memory =
    srlg.image.fc_buffer[1].handle = srlg.image.fc_buffer[1].memory =
    srlg.image.fc_buffer[2].handle = srlg.image.fc_buffer[2].memory =
    NULL;
    srlg.image.address =
    srlg.image.fc_buffer[0].address =
    srlg.image.fc_buffer[1].address =
    srlg.image.fc_buffer[2].address =
    0;

    dump_vgl_serialize(&srlg, sizeof(srlg), alignof(vg_lite_ext_linear_gradient_t));
}

static void dump_vgl_allocate_lgrad(vg_lite_linear_gradient_t* lgrad) {
    vg_lite_linear_gradient_t srlg;
    
    handler.lgrad[handler.lgradNum++] = lgrad;

    memcpy(&srlg, lgrad, sizeof(vg_lite_linear_gradient_t));
    srlg.image.handle = srlg.image.memory =
    srlg.image.yuv.uv_handle = srlg.image.yuv.uv_memory = 
    srlg.image.yuv.v_handle = srlg.image.yuv.v_memory =
    srlg.image.fc_buffer[0].handle = srlg.image.fc_buffer[0].memory =
    srlg.image.fc_buffer[1].handle = srlg.image.fc_buffer[1].memory =
    srlg.image.fc_buffer[2].handle = srlg.image.fc_buffer[2].memory =
    NULL;
    srlg.image.address =
    srlg.image.fc_buffer[0].address =
    srlg.image.fc_buffer[1].address =
    srlg.image.fc_buffer[2].address =
    0;

    dump_vgl_serialize(&srlg, sizeof(srlg), alignof(vg_lite_linear_gradient_t));
}

static void dump_vgl_allocate_rgrad(vg_lite_radial_gradient_t* rgrad) {
    vg_lite_radial_gradient_t srrg;

    handler.rgrad[handler.rgradNum++] = rgrad;

    memcpy(&srrg, rgrad, sizeof(vg_lite_radial_gradient_t));
    srrg.image.handle = srrg.image.memory =
    srrg.image.yuv.uv_handle = srrg.image.yuv.uv_memory = 
    srrg.image.yuv.v_handle = srrg.image.yuv.v_memory =
    srrg.image.fc_buffer[0].handle = srrg.image.fc_buffer[0].memory =
    srrg.image.fc_buffer[1].handle = srrg.image.fc_buffer[1].memory =
    srrg.image.fc_buffer[2].handle = srrg.image.fc_buffer[2].memory =
    NULL;
    srrg.image.address =
    srrg.image.fc_buffer[0].address =
    srrg.image.fc_buffer[1].address =
    srrg.image.fc_buffer[2].address =
    0;

    dump_vgl_serialize(&srrg, sizeof(srrg), alignof(vg_lite_radial_gradient_t));
}

static int dump_vgl_refer_to_rgrad(vg_lite_radial_gradient_t* rgrad) {
    for (int i = 0; i < handler.rgradNum; ++i) {
        if (handler.rgrad[i] == rgrad) return i;
    }
    /* Error: not found rgrad. */
    return -1;
}

static void dump_vgl_serialize_path(vg_lite_path_t* path) {
    vg_lite_path_t srp;

    memcpy(&srp, path, sizeof(vg_lite_path_t));
    srp.path = srp.stroke = srp.stroke_path = 0;

    dump_vgl_serialize(&srp, sizeof(vg_lite_path_t), alignof(vg_lite_path_t));
    dump_vgl_serialize(path->path, path->path_length, 4);
    dump_vgl_serialize(path->stroke_path, path->stroke_size, 4);
}

typedef int FunctionIdx_t;
#define FUNC_IDX(func)  _##func##_dump_idx
#define _vg_lite_init_dump_idx                        (FunctionIdx_t)1
#define _vg_lite_close_dump_idx                       (FunctionIdx_t)2
#define _vg_lite_finish_dump_idx                      (FunctionIdx_t)3
#define _vg_lite_flush_dump_idx                       (FunctionIdx_t)4
#define _vg_lite_allocate_dump_idx                    (FunctionIdx_t)5
#define _vg_lite_free_dump_idx                        (FunctionIdx_t)6
#define _vg_lite_upload_buffer_dump_idx               (FunctionIdx_t)7
#define _vg_lite_map_dump_idx                         (FunctionIdx_t)8
#define _vg_lite_unmap_dump_idx                       (FunctionIdx_t)9
#define _vg_lite_flush_mapped_buffer_dump_idx         (FunctionIdx_t)10
#define _vg_lite_clear_dump_idx                       (FunctionIdx_t)11
#define _vg_lite_blit_dump_idx                        (FunctionIdx_t)12
#define _vg_lite_blit_rect_dump_idx                   (FunctionIdx_t)13
#define _vg_lite_blit2_dump_idx                       (FunctionIdx_t)14
#define _vg_lite_draw_dump_idx                        (FunctionIdx_t)15
#define _vg_lite_upload_path_dump_idx                 (FunctionIdx_t)16
#define _vg_lite_draw_pattern_dump_idx                (FunctionIdx_t)17
#define _vg_lite_draw_grad_dump_idx                   (FunctionIdx_t)18
#define _vg_lite_draw_linear_grad_dump_idx            (FunctionIdx_t)19 
#define _vg_lite_draw_radial_grad_dump_idx            (FunctionIdx_t)20 
#define _vg_lite_set_scissor_dump_idx                 (FunctionIdx_t)21 
#define _vg_lite_enable_scissor_dump_idx              (FunctionIdx_t)22 
#define _vg_lite_disable_scissor_dump_idx             (FunctionIdx_t)23
#define _vg_lite_source_global_alpha_dump_idx         (FunctionIdx_t)24 
#define _vg_lite_dest_global_alpha_dump_idx           (FunctionIdx_t)25
#define _vg_lite_set_color_key_dump_idx               (FunctionIdx_t)26
#define _vg_lite_enable_dither_dump_idx               (FunctionIdx_t)27
#define _vg_lite_disable_dither_dump_idx              (FunctionIdx_t)28
#define _vg_lite_set_tess_buffer_dump_idx             (FunctionIdx_t)29
#define _vg_lite_set_command_buffer_size_dump_idx     (FunctionIdx_t)30
#define _vg_lite_set_command_buffer_dump_idx          (FunctionIdx_t)31
#define _vg_lite_set_pixel_matrix_dump_idx            (FunctionIdx_t)32
#define _vg_lite_gaussian_filter_dump_idx             (FunctionIdx_t)33
#define _vg_lite_enable_masklayer_dump_idx            (FunctionIdx_t)34
#define _vg_lite_disable_masklayer_dump_idx           (FunctionIdx_t)35
#define _vg_lite_set_masklayer_dump_idx               (FunctionIdx_t)36
#define _vg_lite_destroy_masklayer_dump_idx           (FunctionIdx_t)37
#define _vg_lite_create_masklayer_dump_idx            (FunctionIdx_t)38
#define _vg_lite_fill_masklayer_dump_idx              (FunctionIdx_t)39
#define _vg_lite_blend_masklayer_dump_idx             (FunctionIdx_t)40
#define _vg_lite_render_masklayer_dump_idx            (FunctionIdx_t)41
#define _vg_lite_scissor_rects_dump_idx               (FunctionIdx_t)50
#define _vg_lite_set_mirror_dump_idx                  (FunctionIdx_t)51
#define _vg_lite_set_gamma_dump_idx                   (FunctionIdx_t)52
#define _vg_lite_init_grad_dump_idx                   (FunctionIdx_t)53
#define _vg_lite_enable_color_transform_dump_idx      (FunctionIdx_t)54
#define _vg_lite_disable_color_transform_dump_idx     (FunctionIdx_t)55
#define _vg_lite_set_color_transform_dump_idx         (FunctionIdx_t)56
#define _vg_lite_flexa_set_stream_dump_idx            (FunctionIdx_t)57
#define _vg_lite_flexa_bg_buffer_dump_idx             (FunctionIdx_t)58
#define _vg_lite_flexa_enable_dump_idx                (FunctionIdx_t)59
#define _vg_lite_flexa_disable_dump_idx               (FunctionIdx_t)60
#define _vg_lite_flexa_stop_frame_dump_idx            (FunctionIdx_t)61
#define _vg_lite_dump_command_buffer_dump_idx         (FunctionIdx_t)62
#define _vg_lite_update_linear_grad_dump_idx          (FunctionIdx_t)63
#define _vg_lite_clear_linear_grad_dump_idx           (FunctionIdx_t)64
#define _vg_lite_update_grad_dump_idx                 (FunctionIdx_t)65
#define _vg_lite_clear_grad_dump_idx                  (FunctionIdx_t)66
#define _vg_lite_update_radial_grad_dump_idx          (FunctionIdx_t)67
#define _vg_lite_clear_radial_grad_dump_idx           (FunctionIdx_t)68
#define _vg_lite_save_png_dump_idx                    (FunctionIdx_t)69
#define _vg_lite_load_png_dump_idx                    (FunctionIdx_t)70
#define _vg_lite_fb_open_dump_idx                     (FunctionIdx_t)71
#define _vg_lite_fb_close_dump_idx                    (FunctionIdx_t)72
#define _vg_lite_load_raw_yuv_dump_idx                (FunctionIdx_t)73
#define _vg_lite_yv12toyv24_dump_idx                  (FunctionIdx_t)74
#define _vg_lite_yv12toyv16_dump_idx                  (FunctionIdx_t)75
#define _vg_lite_load_raw_dump_idx                    (FunctionIdx_t)76
#define _vg_lite_load_raw_byline_dump_idx             (FunctionIdx_t)77
#define _vg_lite_save_raw_dump_idx                    (FunctionIdx_t)78
#define _vg_lite_save_raw_byline_dump_idx             (FunctionIdx_t)79
#define _vg_lite_load_pkm_dump_idx                    (FunctionIdx_t)80
#define _vg_lite_load_pkm_info_to_buffer_dump_idx     (FunctionIdx_t)81
#define _vg_lite_load_dev_info_to_buffer_dump_idx     (FunctionIdx_t)82
#define _vg_lite_load_decnano_compressd_data_dump_idx (FunctionIdx_t)83
#define _vg_lite_save_decnano_compressd_data_dump_idx (FunctionIdx_t)84
#define _vg_lite_set_memory_pool_dump_idx             (FunctionIdx_t)85

#define DUMP_PARAM(param, type)\
    dump_vgl_serialize(&param, sizeof(param), alignof(type))
#define DUMP_BYTES(ptr, bytes, alignto)\
    dump_vgl_serialize(ptr, bytes, alignto)
#define DUMP_FUNCTION(func)\
    do{\
        FunctionIdx_t _idx = FUNC_IDX(func);\
        dump_vgl_serialize(&_idx, sizeof(_idx), alignof(FunctionIdx_t));\
    }while(0)
#define DUMP_ALLOC(buf)         dump_vgl_allocate_buffer(buf)
#define DUMP_BUFFER(buf)\
    do{\
        int _idx = dump_vgl_refer_to_buffer(buf);\
        if(_idx == -1) return;\
        dump_vgl_serialize(&_idx, sizeof(_idx), alignof(int));\
    }while(0)
#define DUMP_PATH(path)         dump_vgl_serialize_path(path)
#define DUMP_FLUSH()            dump_vgl_flush()
#define DUMP_FREE(buf)          dump_vgl_free_buffer(buf)

DLLEXPORT void FUNC_DUMP(vg_lite_init)(
    vg_lite_int32_t tess_width, 
    vg_lite_int32_t tess_height) {
    if (createVgl) {
        const char fp_vgl[] = "./dump.vgl";
        memset(&handler, 0, sizeof(DumpVGLHandler));
        handler.file = fopen(fp_vgl, "wb");
        createVgl = 0;
    }

    DUMP_FUNCTION(vg_lite_init);
    DUMP_PARAM(tess_width, vg_lite_int32_t);
    DUMP_PARAM(tess_height, vg_lite_int32_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_memory_pool)(
    vg_lite_buffer_type_t type, 
    vg_lite_memory_pool_t pool) {
    if (createVgl) {
        const char fp_vgl[] = "./dump.vgl";
        memset(&handler, 0, sizeof(DumpVGLHandler));
        handler.file = fopen(fp_vgl, "wb");
        createVgl = 0;
    }

    DUMP_FUNCTION(vg_lite_set_memory_pool);
    DUMP_PARAM(type, vg_lite_buffer_type_t);
    DUMP_PARAM(pool, vg_lite_memory_pool_t);

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_close)(void) {
    DUMP_FUNCTION(vg_lite_close);
    fclose(handler.file);

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_finish)(void) {
    DUMP_FUNCTION(vg_lite_finish);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flush)(void) {
    DUMP_FUNCTION(vg_lite_flush);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_allocate)(
    vg_lite_buffer_t* buffer) {
    DUMP_FUNCTION(vg_lite_allocate);
    DUMP_ALLOC(buffer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_free)(vg_lite_buffer_t* buffer) {
    DUMP_FUNCTION(vg_lite_free);
    DUMP_BUFFER(buffer);
    DUMP_FLUSH();
    DUMP_FREE(buffer);

    return ;
}

DLLEXPORT void FUNC_DUMP(vg_lite_upload_buffer)(
    vg_lite_buffer_t* buffer, 
    vg_lite_uint8_t* data[3], 
    vg_lite_uint32_t stride[3]){
    DUMP_FUNCTION(vg_lite_upload_buffer);
    DUMP_BUFFER(buffer);
    DUMP_BYTES(stride, sizeof(vg_lite_uint32_t[3]), 
        alignof(vg_lite_uint32_t[3]));
    DUMP_BYTES(data[0], stride[0], alignof(vg_lite_uint8_t));
    DUMP_BYTES(data[1], stride[1], alignof(vg_lite_uint8_t));
    DUMP_BYTES(data[2], stride[2], alignof(vg_lite_uint8_t));
    DUMP_FLUSH();

    return ;
}

DLLEXPORT void FUNC_DUMP(vg_lite_map)(
    vg_lite_buffer_t* buffer, 
    vg_lite_map_flag_t flag, 
    int32_t fd) {
    DUMP_FUNCTION(vg_lite_map);
    {   // Dump CPU buffer
        vg_lite_buffer_t srb;
        memcpy(&srb, buffer, sizeof(vg_lite_buffer_t));
        srb.memory = NULL;
        DUMP_PARAM(srb, vg_lite_buffer_t);
        /* Refer to warp_user_memory case, buffer memory align to 64 Bytes. */
        DUMP_BYTES(buffer->memory, buffer->stride * buffer->height, 64);
    }
    DUMP_ALLOC(buffer);
    DUMP_PARAM(flag, vg_lite_map_flag_t);
    DUMP_PARAM(fd, int32_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_unmap)(vg_lite_buffer_t* buffer){
    DUMP_FUNCTION(vg_lite_unmap);
    DUMP_BUFFER(buffer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flush_mapped_buffer)(
    vg_lite_buffer_t* buffer) {
    DUMP_FUNCTION(vg_lite_flush_mapped_buffer);
    DUMP_BUFFER(buffer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_clear)(vg_lite_buffer_t* target,
    vg_lite_rectangle_t* rect,
    vg_lite_color_t color) {
    char hasRect = rect != NULL;

    DUMP_FUNCTION(vg_lite_clear);
    DUMP_BUFFER(target);
    DUMP_PARAM(hasRect, char);
    if(hasRect)
        DUMP_BYTES(rect, sizeof(vg_lite_rectangle_t), 
            alignof(vg_lite_rectangle_t));
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_blit)(vg_lite_buffer_t* target, 
    vg_lite_buffer_t* source, vg_lite_matrix_t* matrix, vg_lite_blend_t blend, 
    vg_lite_color_t color, vg_lite_filter_t filter){
    char hasMatrix = matrix != NULL;

    DUMP_FUNCTION(vg_lite_blit);
    DUMP_BUFFER(target);

    /* Dump source buffer*/
    DUMP_BUFFER(source);
    DUMP_BYTES(source, sizeof(vg_lite_buffer_t), alignof(vg_lite_buffer_t));
    DUMP_BYTES(source->memory, source->stride * source->height, 1);

    DUMP_PARAM(hasMatrix, char);
    if (hasMatrix)
        DUMP_BYTES(matrix, sizeof(vg_lite_matrix_t), alignof(vg_lite_matrix_t));

    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_blit_rect)(vg_lite_buffer_t* target,
    vg_lite_buffer_t* source, vg_lite_rectangle_t* rect, 
    vg_lite_matrix_t* matrix, vg_lite_blend_t blend, vg_lite_color_t color, 
    vg_lite_filter_t filter) {
    char hasRect = rect != NULL;
    char hasMatrix = matrix != NULL;

    DUMP_FUNCTION(vg_lite_blit_rect);
    DUMP_BUFFER(target);
    DUMP_BUFFER(source);
    DUMP_PARAM(hasRect, char);
    if (hasRect)
        DUMP_BYTES(rect, sizeof(vg_lite_rectangle_t), 
            alignof(vg_lite_rectangle_t));
    DUMP_PARAM(hasMatrix, char);
    if (hasMatrix)
        DUMP_BYTES(matrix, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_blit2)(vg_lite_buffer_t* target, 
    vg_lite_buffer_t* source0, vg_lite_buffer_t* source1, 
    vg_lite_matrix_t* matrix0, vg_lite_matrix_t* matrix1, vg_lite_blend_t blend,
    vg_lite_filter_t filter) {
    char hasMatrix0 = matrix0 != NULL;
    char hasMatrix1 = matrix1 != NULL;

    DUMP_FUNCTION(vg_lite_blit2);
    DUMP_BUFFER(target);
    DUMP_BUFFER(source0);
    DUMP_BUFFER(source1);
    DUMP_PARAM(hasMatrix0, char);
    if (hasMatrix0)
        DUMP_BYTES(matrix0, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_PARAM(hasMatrix1, char);
    if (hasMatrix1)
        DUMP_BYTES(matrix1, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_draw)(vg_lite_buffer_t* target,
    vg_lite_path_t* path,
    vg_lite_fill_t fill_rule,
    vg_lite_matrix_t* matrix,
    vg_lite_blend_t blend,
    vg_lite_color_t color) {
    DUMP_FUNCTION(vg_lite_draw);
    DUMP_BUFFER(target);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_BYTES(matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_upload_path)(vg_lite_path_t* path) {
    DUMP_FUNCTION(vg_lite_upload_path);
    DUMP_PATH(path);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_draw_pattern)(
    vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule, 
    vg_lite_matrix_t* matrix0, vg_lite_buffer_t* source, 
    vg_lite_matrix_t* matrix1, vg_lite_blend_t blend, 
    vg_lite_pattern_mode_t pattern_mode, vg_lite_color_t  pattern_color, 
    vg_lite_color_t color, vg_lite_filter_t filter) {
    char hasMatrix0 = matrix0 != NULL;
    char hasMatrix1 = matrix1 != NULL;

    DUMP_FUNCTION(vg_lite_draw_pattern);
    DUMP_BUFFER(target);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_PARAM(hasMatrix0, char);
    if (hasMatrix0)
        DUMP_BYTES(matrix0, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_BUFFER(source);
    DUMP_PARAM(hasMatrix1, char);
    if (hasMatrix1)
        DUMP_BYTES(matrix1, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(pattern_mode, vg_lite_pattern_mode_t);
    DUMP_PARAM(pattern_color, vg_lite_color_t);
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_update_linear_grad)(
    vg_lite_ext_linear_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_update_linear_grad);
    dump_vgl_allocate_lgradEX(grad);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_draw_linear_grad)(
    vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule,
    vg_lite_matrix_t* path_matrix, vg_lite_ext_linear_gradient_t* grad, 
    vg_lite_color_t paint_color, vg_lite_blend_t blend, 
    vg_lite_filter_t filter) {

    DUMP_FUNCTION(vg_lite_draw_linear_grad);
    DUMP_BUFFER(target);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_BYTES(path_matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    {   /* Dump lgradEX. */
        int idx = lgradEX_count;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
        lgradEX_count++;
    }
    DUMP_BYTES(&grad->matrix, sizeof(vg_lite_matrix_t), 
        alignof(vg_lite_matrix_t));
    DUMP_PARAM(paint_color, vg_lite_color_t);
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_clear_linear_grad)(
    vg_lite_ext_linear_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_clear_linear_grad);
    {   /* Dump lgradEX. */
        int idx = lgradEX_count - 1;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return ;
}

DLLEXPORT void FUNC_DUMP(vg_lite_update_grad)(
    vg_lite_linear_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_update_grad);
    dump_vgl_allocate_lgrad(grad);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_draw_grad)(
    vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule,
    vg_lite_matrix_t* matrix, vg_lite_linear_gradient_t* grad, 
    vg_lite_blend_t blend) {
    DUMP_FUNCTION(vg_lite_draw_grad);
    DUMP_BUFFER(target);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_BYTES(matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    {   /* Dump lgrad. */
        int idx = lgradEX_count;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
        lgradEX_count++;
    }
    DUMP_BYTES(&grad->matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_clear_grad)(
    vg_lite_linear_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_clear_grad);
    {   /* Dump lgrad. */
        int idx = lgradEX_count - 1;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_update_radial_grad)(
    vg_lite_radial_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_update_radial_grad);
    dump_vgl_allocate_rgrad(grad);
    DUMP_FLUSH();

    return ;
}

DLLEXPORT void FUNC_DUMP(vg_lite_draw_radial_grad)(
    vg_lite_buffer_t* target, vg_lite_path_t* path, vg_lite_fill_t fill_rule,
    vg_lite_matrix_t* path_matrix, vg_lite_radial_gradient_t* grad, 
    vg_lite_color_t  paint_color, vg_lite_blend_t blend, 
    vg_lite_filter_t filter) {
    DUMP_FUNCTION(vg_lite_draw_radial_grad);
    DUMP_BUFFER(target);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_BYTES(path_matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    {   /* Dump rgrad. */
        int idx = dump_vgl_refer_to_rgrad(grad);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_BYTES(&grad->matrix, sizeof(vg_lite_matrix_t),
        alignof(vg_lite_matrix_t));
    DUMP_PARAM(paint_color, vg_lite_color_t);
    DUMP_PARAM(blend, vg_lite_blend_t);
    DUMP_PARAM(filter, vg_lite_filter_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_clear_radial_grad)(
    vg_lite_radial_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_clear_radial_grad);
    {   /* Dump lgrad. */
        int idx = dump_vgl_refer_to_rgrad(grad);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_scissor)(
    vg_lite_int32_t x, vg_lite_int32_t y, 
    vg_lite_int32_t right, vg_lite_int32_t bottom) {
    DUMP_FUNCTION(vg_lite_set_scissor);
    DUMP_PARAM(x, vg_lite_int32_t);
    DUMP_PARAM(y, vg_lite_int32_t);
    DUMP_PARAM(right, vg_lite_int32_t);
    DUMP_PARAM(bottom, vg_lite_int32_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_enable_scissor)() {
    DUMP_FUNCTION(vg_lite_enable_scissor);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_disable_scissor)() {
    DUMP_FUNCTION(vg_lite_disable_scissor);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_source_global_alpha)(
    vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value) {
    DUMP_FUNCTION(vg_lite_source_global_alpha);
    DUMP_PARAM(alpha_mode, vg_lite_global_alpha_t);
    DUMP_PARAM(alpha_value, vg_lite_uint8_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_dest_global_alpha)(
    vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value) {
    DUMP_FUNCTION(vg_lite_dest_global_alpha);
    DUMP_PARAM(alpha_mode, vg_lite_global_alpha_t);
    DUMP_PARAM(alpha_value, vg_lite_uint8_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_color_key)(
    vg_lite_color_key4_t colorkey) {
    DUMP_FUNCTION(vg_lite_set_color_key);
    DUMP_BYTES(colorkey, sizeof(vg_lite_color_key4_t),
        alignof(vg_lite_color_key4_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_enable_dither)() {
    DUMP_FUNCTION(vg_lite_enable_dither);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_disable_dither)() {
    DUMP_FUNCTION(vg_lite_disable_dither);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_pixel_matrix)(
    vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t* channel) {
    DUMP_FUNCTION(vg_lite_set_pixel_matrix);
    DUMP_BYTES(matrix, sizeof(vg_lite_pixel_matrix_t),
        alignof(vg_lite_pixel_matrix_t));
    DUMP_BYTES(channel, sizeof(vg_lite_pixel_channel_enable_t),
        alignof(vg_lite_pixel_channel_enable_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_gaussian_filter)(
    vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2) {
    DUMP_FUNCTION(vg_lite_gaussian_filter);
    DUMP_PARAM(w0, vg_lite_float_t);
    DUMP_PARAM(w1, vg_lite_float_t);
    DUMP_PARAM(w2, vg_lite_float_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_enable_masklayer)() {
    DUMP_FUNCTION(vg_lite_enable_masklayer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_disable_masklayer)() {
    DUMP_FUNCTION(vg_lite_disable_masklayer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_masklayer)(
    vg_lite_buffer_t* masklayer) {
    DUMP_FUNCTION(vg_lite_set_masklayer);
    DUMP_BUFFER(masklayer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_destroy_masklayer)(
    vg_lite_buffer_t* masklayer) {
    DUMP_FUNCTION(vg_lite_destroy_masklayer);
    DUMP_BUFFER(masklayer);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_create_masklayer)(
    vg_lite_buffer_t* masklayer, 
    vg_lite_uint32_t width, vg_lite_uint32_t height) {
    DUMP_FUNCTION(vg_lite_create_masklayer);
    DUMP_ALLOC(masklayer);
    DUMP_PARAM(width, vg_lite_uint32_t);
    DUMP_PARAM(height, vg_lite_uint32_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_fill_masklayer)(
    vg_lite_buffer_t* masklayer, vg_lite_rectangle_t* rect, 
    vg_lite_uint8_t value) {
    char hasRect = rect != NULL;

    DUMP_FUNCTION(vg_lite_fill_masklayer);
    DUMP_BUFFER(masklayer);
    DUMP_PARAM(hasRect, char);
    if (hasRect)
        DUMP_BYTES(rect, sizeof(vg_lite_rectangle_t),
            alignof(vg_lite_rectangle_t));
    DUMP_PARAM(value, char);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_blend_masklayer)(
    vg_lite_buffer_t* dst_masklayer, vg_lite_buffer_t* src_masklayer, 
    vg_lite_mask_operation_t operation, vg_lite_rectangle_t* rect) {
    char hasRect = rect != NULL;
    
    DUMP_FUNCTION(vg_lite_blend_masklayer);
    DUMP_BUFFER(dst_masklayer);
    DUMP_BUFFER(src_masklayer);
    DUMP_PARAM(operation, vg_lite_mask_operation_t);
    if (hasRect)
        DUMP_BYTES(rect, sizeof(vg_lite_rectangle_t),
            alignof(vg_lite_rectangle_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_render_masklayer)(
    vg_lite_buffer_t* masklayer, vg_lite_mask_operation_t operation, 
    vg_lite_path_t* path, vg_lite_fill_t fill_rule, vg_lite_color_t color, 
    vg_lite_matrix_t* matrix) {
    char hasMatrix = matrix != NULL;

    DUMP_FUNCTION(vg_lite_render_masklayer);
    DUMP_BUFFER(masklayer);
    DUMP_PARAM(operation, vg_lite_mask_operation_t);
    DUMP_PATH(path);
    DUMP_PARAM(fill_rule, vg_lite_fill_t);
    DUMP_PARAM(color, vg_lite_color_t);
    DUMP_PARAM(hasMatrix, vg_lite_matrix_t);
    if (hasMatrix)
        DUMP_BYTES(matrix, sizeof(vg_lite_matrix_t),
            alignof(vg_lite_matrix_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_scissor_rects)(
    vg_lite_buffer_t* target, vg_lite_uint32_t nums, vg_lite_rectangle_t rect[]) {
    DUMP_FUNCTION(vg_lite_scissor_rects);
    DUMP_BUFFER(target);
    DUMP_PARAM(nums, vg_lite_uint32_t);
    DUMP_BYTES(rect, sizeof(vg_lite_rectangle_t) * nums,
        alignof(vg_lite_rectangle_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_mirror)(
    vg_lite_orientation_t orientation) {
    DUMP_FUNCTION(vg_lite_set_mirror);
    DUMP_PARAM(orientation, vg_lite_orientation_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_gamma)(
    vg_lite_gamma_conversion_t gamma_value) {
    DUMP_FUNCTION(vg_lite_set_gamma);
    DUMP_PARAM(gamma_value, vg_lite_gamma_conversion_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_enable_color_transform)() {
    DUMP_FUNCTION(vg_lite_enable_color_transform);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_disable_color_transform)() {
    DUMP_FUNCTION(vg_lite_disable_color_transform);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_color_transform)(
    vg_lite_color_transform_t* values) {
    DUMP_FUNCTION(vg_lite_set_color_transform);
    DUMP_BYTES(values, sizeof(vg_lite_color_transform_t),
        alignof(vg_lite_color_transform_t));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flexa_set_stream)(
    vg_lite_uint8_t stream_id) {
    DUMP_FUNCTION(vg_lite_flexa_set_stream);
    DUMP_PARAM(stream_id, vg_lite_uint8_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flexa_bg_buffer)(
    vg_lite_uint8_t stream_id, vg_lite_buffer_t* buffer, 
    vg_lite_uint32_t bg_seg_count, vg_lite_uint32_t bg_seg_size) {
    DUMP_FUNCTION(vg_lite_flexa_bg_buffer);
    DUMP_PARAM(stream_id, vg_lite_uint8_t);
    DUMP_BUFFER(buffer);
    DUMP_PARAM(bg_seg_count, vg_lite_uint32_t);
    DUMP_PARAM(bg_seg_size, vg_lite_uint32_t);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flexa_enable)() {
    DUMP_FUNCTION(vg_lite_flexa_enable);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flexa_disable)() {
    DUMP_FUNCTION(vg_lite_flexa_disable);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_flexa_stop_frame)() {
    DUMP_FUNCTION(vg_lite_flexa_stop_frame);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_dump_command_buffer)() {
    DUMP_FUNCTION(vg_lite_dump_command_buffer);
    DUMP_FLUSH();

    return;
}


DLLEXPORT void FUNC_DUMP(vg_lite_save_png)(const char* name,
    vg_lite_buffer_t* buffer) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_save_png);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    {   // Dump buffer and return 0 if failure.
        int idx = dump_vgl_refer_to_buffer(buffer);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_png)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_png);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_fb_open)(void) {
    DUMP_FUNCTION(vg_lite_fb_open);
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_fb_close)(vg_lite_buffer_t* buffer) {
    DUMP_FUNCTION(vg_lite_fb_close);
    {   // Dump buffer and no return
        int idx = dump_vgl_refer_to_buffer(buffer);
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_raw_yuv)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_raw_yuv);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_yv12toyv24)(
    vg_lite_buffer_t* buffer1, vg_lite_buffer_t* buffer2) {
    DUMP_FUNCTION(vg_lite_yv12toyv24);
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer1);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer2);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_yv12toyv16)(
    vg_lite_buffer_t* buffer1, vg_lite_buffer_t* buffer2) {
    DUMP_FUNCTION(vg_lite_yv12toyv16);
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer1);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer2);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_raw)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_raw);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_raw_byline)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_raw_byline);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_save_raw)(
    const char* name, vg_lite_buffer_t* buffer) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_save_raw);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_save_raw_byline)(
    const char* name, vg_lite_buffer_t* buffer) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_save_raw_byline);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_pkm)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_pkm);

    DUMP_BUFFER(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_pkm_info_to_buffer)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_pkm_info_to_buffer);
    DUMP_BYTES(buffer, sizeof(vg_lite_buffer_t), alignof(vg_lite_buffer_t));
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_dev_info_to_buffer)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_dev_info_to_buffer);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_load_decnano_compressd_data)(
    vg_lite_buffer_t* buffer, const char* name) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_load_decnano_compressd_data);
    DUMP_ALLOC(buffer);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_save_decnano_compressd_data)(
    const char* name, vg_lite_buffer_t* buffer) {
    size_t bytes;

    bytes = strlen(name) + 1;

    DUMP_FUNCTION(vg_lite_save_decnano_compressd_data);
    DUMP_PARAM(bytes, size_t);
    DUMP_BYTES(name, bytes, alignof(char));
    {   // Dump buffer and return -1 if failed.
        int idx = dump_vgl_refer_to_buffer(buffer);
        if (idx == -1) return;
        dump_vgl_serialize(&idx, sizeof(idx), alignof(int));
    }
    DUMP_FLUSH();

    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_set_command_buffer_size)(
    vg_lite_uint32_t size) {
    if (createVgl) {
        const char fp_vgl[] = "./dump.vgl";
        memset(&handler, 0, sizeof(DumpVGLHandler));
        handler.file = fopen(fp_vgl, "wb");
        createVgl = 0;
    }

    DUMP_FUNCTION(vg_lite_set_command_buffer_size);
    DUMP_PARAM(size, vg_lite_uint32_t);
    DUMP_FLUSH();
        
    return;
}

DLLEXPORT void FUNC_DUMP(vg_lite_init_grad)(vg_lite_linear_gradient_t* grad) {
    DUMP_FUNCTION(vg_lite_init_grad);
}

#endif /* DUMP_API */
#endif