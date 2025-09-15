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

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include "vg_lite_context.h"

#define MATRIX_FP_ABS(x)            (((x) < 0) ? -(x) : (x))
#define MATRIX_FP_EPS               2.2204460492503131e-14

extern vg_lite_matrix_t identity_mtx;

/* Get the plane memory pointer and strides info. */
static uint32_t get_buffer_planes(vg_lite_buffer_t * buffer,
                                  uint8_t ** memory,
                                  uint32_t * strides)
{
    uint32_t count = 1;

    switch(buffer->format) {
        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_BGRA5551:
        case VG_LITE_A8:
        case VG_LITE_L8:
        case VG_LITE_A4:
        case VG_LITE_INDEX_1:
        case VG_LITE_INDEX_2:
        case VG_LITE_INDEX_4:
        case VG_LITE_INDEX_8:
        case VG_LITE_YUYV:
        case VG_LITE_YUY2:
        case VG_LITE_RGBA2222:
            count = 1;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = memory[2] = ((uint8_t *)0);
            strides[0] = buffer->stride;
            strides[1] = strides[2] = 0;
            break;

        case VG_LITE_NV12:
        case VG_LITE_NV16:
        case VG_LITE_NV24:
        case VG_LITE_NV24_TILED:
            count = 2;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = 0;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = 0;
            break;

        case VG_LITE_AYUY2:
            count = 2;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = 0;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = 0;
            strides[2] = buffer->yuv.alpha_stride;
            break;

        case VG_LITE_ANV12:
            count = 3;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = buffer->yuv.alpha_stride;
            break;

        case VG_LITE_YV12:
        case VG_LITE_YV24:
        case VG_LITE_YV16:
            count = 3;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = buffer->yuv.v_stride;
            break;

        case VG_LITE_YUY2_TILED:
        case VG_LITE_NV12_TILED:
        case VG_LITE_ANV12_TILED:
        case VG_LITE_AYUY2_TILED:
        default:
            count = 0;

            break;
    }
    return count;
}

vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t * buffer,
                                      vg_lite_uint8_t * data[3],
                                      vg_lite_uint32_t stride[3])
{
#if DUMP_API
    FUNC_DUMP(vg_lite_upload_buffer)(buffer, data, stride);
#endif

    vg_lite_error_t error = VG_LITE_SUCCESS;
    int32_t plane_count;
    uint8_t * buffer_memory[3] = {((uint8_t *)0)};
    uint32_t  buffer_strides[3] = {0};
    uint8_t * pdata;
    int32_t i, j;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_upload_buffer %p %p %p\n", buffer, data, stride);
#endif

    /* Get buffer memory info. */
    plane_count = get_buffer_planes(buffer, buffer_memory, buffer_strides);

    if(plane_count > 0 && plane_count <= 3) {
        /* Copy the data to buffer. */
        for(i = 0; i < plane_count;  i++) {
            pdata = data[i];
            for(j = 0; j < buffer->height; j++) {
                memcpy(buffer_memory[i], pdata, buffer_strides[i]);
                buffer_memory[i] += buffer_strides[i];
                pdata += stride[i];
            }
        }
    }
    else {
        error = VG_LITE_INVALID_ARGUMENT;
    }

    return error;
}

static vg_lite_error_t swap(float * a, float * b)
{
    float temp;
    if(a == NULL || b == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    temp = *a;
    *a = *b;
    *b = temp;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_get_transform_matrix(vg_lite_float_point4_t src, vg_lite_float_point4_t dst,
                                             vg_lite_matrix_t * mat)
{
    float a[8][8], b[9], A[64];
    int i, j, k, m = 8, n = 1;
    int astep = 8, bstep = 1;
    float d;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_get_transform_matrix %p %p %p\n", src, dst, mat);
#endif

    if(src == NULL || dst == NULL || mat == NULL)
        return  VG_LITE_INVALID_ARGUMENT;

    for(i = 0; i < 4; ++i) {
        a[i][0] = a[i + 4][3] = (float)src[i].x;
        a[i][1] = a[i + 4][4] = (float)src[i].y;
        a[i][2] = a[i + 4][5] = 1.0f;
        a[i][3] = a[i][4] = a[i][5] =
                                a[i + 4][0] = a[i + 4][1] = a[i + 4][2] = 0.0f;
        a[i][6] = (float)(-src[i].x * dst[i].x);
        a[i][7] = (float)(-src[i].y * dst[i].x);
        a[i + 4][6] = (float)(-src[i].x * dst[i].y);
        a[i + 4][7] = (float)(-src[i].y * dst[i].y);
        b[i] = (float)dst[i].x;
        b[i + 4] = (float)dst[i].y;
    }
    for(i = 0; i < 8; ++i) {
        for(j = 0; j < 8; ++j) {
            A[8 * i + j] = a[i][j];
        }
    }

    for(i = 0; i < m; i++) {
        k = i;
        for(j = i + 1; j < m; j++)
            if(MATRIX_FP_ABS(A[j * astep + i]) > MATRIX_FP_ABS(A[k * astep + i]))
                k = j;
        if(MATRIX_FP_ABS(A[k * astep + i]) < MATRIX_FP_EPS)
            return VG_LITE_INVALID_ARGUMENT;
        if(k != i) {
            for(j = i; j < m; j++)
                swap(&A[i * astep + j], &A[k * astep + j]);
            for(j = 0; j < n; j++)
                swap(&b[i * bstep + j], &b[k * bstep + j]);
        }
        d = -1 / A[i * astep + i];
        for(j = i + 1; j < m; j++) {
            float alpha = A[j * astep + i] * d;
            for(k = i + 1; k < m; k++)
                A[j * astep + k] += alpha * A[i * astep + k];
            for(k = 0; k < n; k++)
                b[j * bstep + k] += alpha * b[i * bstep + k];
        }
    }

    for(i = m - 1; i >= 0; i--)
        for(j = 0; j < n; j++) {
            float s = b[i * bstep + j];
            for(k = i + 1; k < m; k++)
                s -= A[i * astep + k] * b[k * bstep + j];
            b[i * bstep + j] = s / A[i * astep + i];
        }

    b[8] = 1;

    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            mat->m[i][j] = b[i * 3 + j];
        }
    }

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_scissor(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right, vg_lite_int32_t bottom)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_scissor)(x, y, right, bottom);
#endif

#if gcFEATURE_VG_SCISSOR
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_scissor %d %d %d %d\n", x, y, right, bottom);
#endif

    /* Save scissor Box States. */
    s_context.scissor[0] = x;
    s_context.scissor[1] = y;
    s_context.scissor[2] = right;
    s_context.scissor[3] = bottom;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;
    s_context.scissor_set = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_scissor()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_enable_scissor)();
#endif

#if gcFEATURE_VG_MASK

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_enable_scissor\n");
#endif

    /* Enable scissor Mode. */
    if(!s_context.scissor_enable) {
        s_context.scissor_enable = 1 << 4;
        s_context.scissor_dirty = 1;
    }

    return VG_LITE_SUCCESS;
#else
    /* Noop */
    return VG_LITE_SUCCESS;
#endif
}

vg_lite_error_t vg_lite_disable_scissor()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_disable_scissor)();
#endif

#if gcFEATURE_VG_MASK

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_disable_scissor\n");
#endif

    /* Disable scissor Mode. */
    if(s_context.scissor_enable) {
        s_context.scissor_enable = 0;
        s_context.scissor_dirty = 1;
    }

    return VG_LITE_SUCCESS;
#else
    /* Noop */
    return VG_LITE_SUCCESS;
#endif
}

vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count, vg_lite_uint32_t * colors)
{
#if gcFEATURE_VG_IM_INDEX_FORMAT
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t addr = 0x0B00;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_CLUT %d %p\n", count, colors);
#endif

#if gcFEATURE_VG_NEW_IMAGE_INDEX
    {
        switch(count) {
            case 256:
            case 16:
            case 4:
            case 2:
                addr = 0x0B00;
                break;
            default:
                error = VG_LITE_INVALID_ARGUMENT;
                return error;
                break;
        }
    }
#else
    {
        switch(count) {
            case 256:
                addr = 0x0B00;
                break;
            case 16:
                addr = 0x0AA0;
                break;
            case 4:
                addr = 0x0A9C;
                break;
            case 2:
                addr = 0x0A98;
                break;
            default:
                error = VG_LITE_INVALID_ARGUMENT;
                return error;
                break;
        }
    }
#endif

    VG_LITE_RETURN_ERROR(push_clut(&s_context, addr, count, (uint32_t *)colors));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_source_global_alpha)(alpha_mode, alpha_value);
#endif

#if gcFEATURE_VG_GLOBAL_ALPHA
    uint32_t image_alpha_mode;
    uint32_t image_alpha_value;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_source_global_alpha %d %d\n", alpha_mode, alpha_value);
#endif

    image_alpha_mode = (uint8_t)alpha_mode;
    image_alpha_value = alpha_value << 2;

    s_context.src_alpha_mode = image_alpha_mode;
    s_context.src_alpha_value = image_alpha_value;

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_dest_global_alpha)(alpha_mode, alpha_value);
#endif

#if gcFEATURE_VG_GLOBAL_ALPHA
    uint32_t dest_alpha_mode;
    uint32_t dest_alpha_value;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_dest_global_alpha %d %d\n", alpha_mode, alpha_value);
#endif

    dest_alpha_mode = (alpha_mode == VG_LITE_NORMAL) ? 0 : (alpha_mode == VG_LITE_GLOBAL) ? 0x00000400 : 0x00000800;
    dest_alpha_value = alpha_value << 12;

    s_context.dst_alpha_mode = dest_alpha_mode;
    s_context.dst_alpha_value = dest_alpha_value;

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_color_key)(colorkey);
#endif

#if gcFEATURE_VG_COLOR_KEY
    uint8_t i;
    uint32_t value_low = 0;
    uint32_t value_high = 0;
    uint8_t r, g, b, a, e;
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_color_key %p\n", colorkey);
#endif

    /* Set color key states. */
    for(i = 0; i < 4; i++) {
        /* Set gcregVGPEColorKeyLow. Layout "E/R/G/B". */
        r = colorkey[i].low_r;
        g = colorkey[i].low_g;
        b = colorkey[i].low_b;
        e = colorkey[i].enable;
        value_low = (e << 24) | (r << 16) | (g << 8) | b;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A90 + i, value_low));

        /* Set gcregVGPEColorKeyHigh. Layout "A/R/G/B". */
        r = colorkey[i].hign_r;
        g = colorkey[i].hign_g;
        b = colorkey[i].hign_b;
        a = colorkey[i].alpha;
        value_high = (a << 24) | (r << 16) | (g << 8) | b;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A94 + i, value_high));
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_dither()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_enable_dither)();
#endif

#if gcFEATURE_VG_DITHER
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t table_low = 0x7B48F3C0;
    uint32_t table_high = 0x596AD1E2;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_enable_dither\n");
#endif

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, table_low));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, table_high));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_dither()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_disable_dither)();
#endif

#if gcFEATURE_VG_DITHER
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_disable_dither\n");
#endif

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, 0xFFFFFFFF));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, 0xFFFFFFFF));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_masklayer()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_enable_masklayer)();
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_enable_masklayer\n");
#endif

    s_context.enable_mask = (1 << 20);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_masklayer()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_disable_masklayer)();
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_disable_masklayer\n");
#endif

    s_context.enable_mask = 0;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_create_masklayer(vg_lite_buffer_t * masklayer, vg_lite_uint32_t width, vg_lite_uint32_t height)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_create_masklayer)(masklayer, width, height);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_create_masklayer %p %d %d\n", masklayer, width, height);
#endif

    memset(masklayer, 0, sizeof(vg_lite_buffer_t));
    masklayer->width = width;
    masklayer->height = height;
    masklayer->format = VG_LITE_A8;
    VG_LITE_RETURN_ERROR(vg_lite_allocate(masklayer));

    VG_LITE_RETURN_ERROR(vg_lite_clear(masklayer, NULL, 0xFF << 24));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_fill_masklayer(vg_lite_buffer_t * masklayer, vg_lite_rectangle_t * rect, vg_lite_uint8_t value)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_fill_masklayer)(masklayer, rect, value);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_fill_masklayer %p %p %d\n", masklayer, rect, value);
#endif

    error = vg_lite_clear(masklayer, rect, value << 24);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_blend_masklayer(
    vg_lite_buffer_t * dst_masklayer,
    vg_lite_buffer_t * src_masklayer,
    vg_lite_mask_operation_t operation,
    vg_lite_rectangle_t * rect
)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_blend_masklayer)(dst_masklayer, src_masklayer, operation, rect);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    vg_lite_rectangle_t area = *rect;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_blend_masklayer %p %p %d %p\n", dst_masklayer, src_masklayer, operation, rect);
#endif

    vg_lite_identity(&matrix);
    vg_lite_translate((vg_lite_float_t)rect->x, (vg_lite_float_t)rect->y, &matrix);

    switch(operation) {
        case VG_LITE_CLEAR_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_clear(dst_masklayer, &area, 0x0));
            break;
        case VG_LITE_FILL_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_clear(dst_masklayer, &area, 0xFF << 24));
            break;
        case VG_LITE_SET_MASK:
            area.x = 0;
            area.y = 0;
            VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, &area, &matrix, VG_LITE_BLEND_NONE, 0, filter));
            break;
        case VG_LITE_UNION_MASK:
            area.x = 0;
            area.y = 0;
            VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, &area, &matrix, VG_LITE_BLEND_SCREEN, 0, filter));
            break;
        case VG_LITE_INTERSECT_MASK:
            area.x = 0;
            area.y = 0;
            VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, &area, &matrix, VG_LITE_BLEND_DST_IN, 0, filter));
            break;
        case VG_LITE_SUBTRACT_MASK:
            area.x = 0;
            area.y = 0;
            VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, &area, &matrix, VG_LITE_BLEND_SUBTRACT, 0,
                                                   filter));
            break;
        default:
            break;
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_masklayer(vg_lite_buffer_t * masklayer)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_masklayer)(masklayer);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_masklayer %p\n", masklayer);
#endif

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A14, masklayer->address));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A15, masklayer->stride));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000010));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_render_masklayer(
    vg_lite_buffer_t * masklayer,
    vg_lite_mask_operation_t operation,
    vg_lite_path_t * path,
    vg_lite_fill_t fill_rule,
    vg_lite_color_t color,
    vg_lite_matrix_t * matrix
)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_render_masklayer)(masklayer, operation, path, fill_rule, color, matrix);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_render_masklayer %p %d %p %d %d %p\n", masklayer, operation, path, fill_rule, color, matrix);
#endif

    if(!matrix) {
        matrix = &identity_mtx;
    }

    switch(operation) {
        case VG_LITE_CLEAR_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, 0));
            break;
        case VG_LITE_FILL_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, 0xFF << 24));
            break;
        case VG_LITE_SET_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, color << 24));
            break;
        case VG_LITE_UNION_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_SCREEN, color << 24));
            break;
        case VG_LITE_INTERSECT_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_DST_IN, color << 24));
            break;
        case VG_LITE_SUBTRACT_MASK:
            VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_SUBTRACT, color << 24));
            break;
        default:
            break;
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_destroy_masklayer(vg_lite_buffer_t * masklayer)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_destroy_masklayer)(masklayer);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_destroy_masklayer %p\n", masklayer);
#endif

    VG_LITE_RETURN_ERROR(vg_lite_free(masklayer));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_pixel_matrix(vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t * channel)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_pixel_matrix)(matrix, channel);
#endif

#if gcFEATURE_VG_PIXEL_MATRIX
    vg_lite_error_t error = VG_LITE_SUCCESS;
    short pix_matrix[20] = { 0 };
    int i = 0;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_pixel_matrix %p (%d %d %d %d)\n", matrix, channel->enable_a, channel->enable_b,
               channel->enable_g, channel->enable_r);
#endif

    s_context.matrix_enable = (channel->enable_a ? (1 << 17) : 0) |
                              (channel->enable_r ? (1 << 23) : 0) |
                              (channel->enable_g ? (1 << 22) : 0) |
                              (channel->enable_b ? (1 << 21) : 0);

    if(s_context.matrix_enable) {
        for(i = 0; i < 20; i++) {
            if(matrix[i] > 127.0f || matrix[i] < -128.0f) {
                return VG_LITE_INVALID_ARGUMENT;
            }
            pix_matrix[i] = (short)(matrix[i] * 256);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0ADE + i, pix_matrix[i]));
        }
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_gaussian_filter(vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_gaussian_filter)(w0, w1, w2);
#endif

#if gcFEATURE_VG_GAUSSIAN_BLUR
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_gaussian_filter %f %f %f\n", w0, w1, w2);
#endif

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD2 + 1, (uint32_t)(w0 * 256)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD4 + 1, (uint32_t)(w1 * 256)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD6 + 1, (uint32_t)(w2 * 256)));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_scissor_rects(vg_lite_buffer_t * target, vg_lite_uint32_t nums, vg_lite_rectangle_t rect[])
{
#if DUMP_API
    FUNC_DUMP(vg_lite_scissor_rects)(target, nums, rect);
#endif

#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_rectangle_t rect_clamp, rect_draw;
    vg_lite_int32_t left_x, right_x, left_len, middle_len, right_len, stride, j, max_x, max_y;
    vg_lite_uint8_t alpha;
    vg_lite_uint32_t i;
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_scissor_rects %d %p\n", nums, rect);
    for(i = 0; i < nums; i++) {
        VGLITE_LOG("    Rect(%d, %d, %d, %d)\n", rect[i].x, rect[i].y, rect[i].width, rect[i].height);
    }
#endif

    /* Record scissor enable flag and disable scissor. */
    vg_lite_uint8_t enable = s_context.scissor_enable;
    s_context.scissor_enable = 0;

    /* Free the old scissor layer if its size is too small for target */
    if(s_context.scissor_layer &&
       (s_context.scissor_layer->width < ((target->width + 7) / 8) || s_context.scissor_layer->height < target->height)) {
        vg_lite_free(s_context.scissor_layer);
        vg_lite_os_free(s_context.scissor_layer);
        s_context.scissor_layer = NULL;
    }

    /* Allocate if scissor layer is NULL */
    if(s_context.scissor_layer == NULL) {
        s_context.scissor_layer = (vg_lite_buffer_t *)vg_lite_os_malloc(sizeof(vg_lite_buffer_t));
        if(!s_context.scissor_layer) {
            return VG_LITE_OUT_OF_RESOURCES;
        }

        memset(s_context.scissor_layer, 0, sizeof(vg_lite_buffer_t));
        s_context.scissor_layer->scissor_buffer = 1;
        s_context.scissor_layer->width = (target->width + 7) / 8;
        s_context.scissor_layer->height = target->height;
        s_context.scissor_layer->format = VG_LITE_A8;
        VG_LITE_RETURN_ERROR(vg_lite_allocate(s_context.scissor_layer));
    }
    s_context.scissor_layer->scissor_buffer = 1;

    /* Clear scissor layer*/
    VG_LITE_RETURN_ERROR(vg_lite_clear(s_context.scissor_layer, NULL, 0x00000000));
    vg_lite_finish();

    max_x = s_context.scissor_layer->width * 8;
    max_y = s_context.scissor_layer->height;

    /* Draw rectangle to scissor layer, one bit data of scissor layer corresponds to one pixel. */
    for(i = 0; i < nums; ++i) {
        /* Clamp the rect */
        memcpy(&rect_clamp, &rect[i], sizeof(vg_lite_rectangle_t));
        {
            if(rect_clamp.x < 0 || rect_clamp.y < 0) {
                rect_clamp.width += rect_clamp.x;
                rect_clamp.height += rect_clamp.y;
                rect_clamp.x = rect_clamp.y = 0;
            }
            if(rect_clamp.x >= max_x || rect_clamp.y >= max_y || rect_clamp.width <= 0 || rect_clamp.height <= 0) {
                rect_clamp.x = rect_clamp.y = rect_clamp.width = rect_clamp.height = 0;
            }
            if(rect_clamp.x + rect_clamp.width > max_x) {
                rect_clamp.width = max_x - rect_clamp.x;
            }
            if(rect_clamp.y + rect_clamp.height > max_y) {
                rect_clamp.height = max_y - rect_clamp.y;
            }
        }

        if(((rect_clamp.x + rect_clamp.width) >> 3) == (rect_clamp.x >> 3)) {
            rect_draw.x = rect_clamp.x / 8;
            rect_draw.y = rect_clamp.y;
            rect_draw.width = 1;
            rect_draw.height = rect_clamp.height;
            alpha = (uint8_t)(((uint8_t)(0xff >> (8 - rect_clamp.width))) << (rect_clamp.x % 8));
            stride = s_context.scissor_layer->stride;
            for(j = rect_draw.y; j < rect_draw.height + rect_draw.y; ++j) {
                ((vg_lite_uint8_t *)s_context.scissor_layer->memory)[j * stride + rect_draw.x] |= alpha;
            }
        }
        else {
            /* Split the rect */
            left_x = (rect_clamp.x % 8 == 0) ? rect_clamp.x : ((rect_clamp.x + 7) & 0xFFFFFFF8);
            right_x = (rect_clamp.x + rect_clamp.width) & 0xFFFFFFF8;
            middle_len = right_x - left_x;
            left_len = left_x - rect_clamp.x;
            right_len = rect_clamp.x + rect_clamp.width - right_x;

            /* Draw left rect */
            if(left_len) {
                rect_draw.x = rect_clamp.x / 8;
                rect_draw.y = rect_clamp.y;
                rect_draw.width = 1;
                rect_draw.height = rect_clamp.height;
                alpha = (uint8_t)(0xff << (8 - left_len));
                stride = s_context.scissor_layer->stride;
                for(j = rect_draw.y; j < rect_draw.height + rect_draw.y; ++j) {
                    ((vg_lite_uint8_t *)s_context.scissor_layer->memory)[j * stride + rect_draw.x] |= alpha;
                }
            }

            /* Draw middle rect */
            if(middle_len) {
                rect_draw.x = left_x / 8;
                rect_draw.y = rect_clamp.y;
                rect_draw.width = middle_len / 8;
                rect_draw.height = rect_clamp.height;
                VG_LITE_RETURN_ERROR(vg_lite_clear(s_context.scissor_layer, &rect_draw, 0xFFFFFFFF));
                vg_lite_finish();
            }

            /* Draw right rect */
            if(right_len) {
                rect_draw.x = (rect_clamp.x + rect_clamp.width - right_len) / 8;
                rect_draw.y = rect_clamp.y;
                rect_draw.width = 1;
                rect_draw.height = rect_clamp.height;
                alpha = (uint8_t)(0xff >> (8 - right_len));
                stride = s_context.scissor_layer->stride;
                for(j = rect_draw.y; j < rect_draw.height + rect_draw.y; ++j) {
                    ((vg_lite_uint8_t *)s_context.scissor_layer->memory)[j * stride + rect_draw.x] |= alpha;
                }
            }
        }
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A16, s_context.scissor_layer->address));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A17, s_context.scissor_layer->stride));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000100));
    vg_lite_finish();
    s_context.scissor_enable = enable;
    s_context.scissor_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_mirror(vg_lite_orientation_t orientation)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_mirror)(orientation);
#endif

#if gcFEATURE_VG_MIRROR
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_mirror %d\n", orientation);
#endif

    s_context.mirror_orient = orientation;
    s_context.mirror_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_gamma(vg_lite_gamma_conversion_t gamma_value)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_set_gamma)(gamma_value);
#endif

#if gcFEATURE_VG_GAMMA
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_gamma %d\n", gamma_value);
#endif

    s_context.gamma_value = gamma_value << 12;
    s_context.gamma_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/* Set s_context.gamma_value base on target buffer */
vg_lite_void set_gamma_dest_only(vg_lite_buffer_t * target, vg_lite_int32_t stencil)
{
    uint32_t gamma_value = 0;

    /* Set gamma configuration of source buffer */
    /* Openvg paintcolor defaults to SRGB */
    s_context.gamma_src = 1;

    /* Set gamma configuration of dst buffer */
    if((target->format >= OPENVG_lRGBX_8888 && target->format <= OPENVG_A_4) ||
       (target->format >= OPENVG_lXRGB_8888 && target->format <= OPENVG_lARGB_8888_PRE) ||
       (target->format >= OPENVG_lBGRX_8888 && target->format <= OPENVG_lBGRA_8888_PRE) ||
       (target->format >= OPENVG_lXBGR_8888 && target->format <= OPENVG_lABGR_8888_PRE) ||
       (target->format >= OPENVG_lRGBX_8888_PRE && target->format <= OPENVG_lRGBA_4444_PRE)) {
        s_context.gamma_dst = 0;
    }
    else {
        s_context.gamma_dst = 1;
    }

    if(s_context.gamma_src == 0 && s_context.gamma_dst == 1) {
        gamma_value = 0x00002000;
    }
    else if(s_context.gamma_src == 1 && s_context.gamma_dst == 0) {
        gamma_value = 0x00001000;
    }
    else {
        gamma_value = 0x00000000;
    }

    if(stencil && target->image_mode == VG_LITE_STENCIL_MODE) {
        s_context.gamma_stencil = gamma_value;
        gamma_value = 0x00000000;
    }

    if(s_context.gamma_dirty == 0 && gamma_value != s_context.gamma_value) {
        s_context.gamma_value = gamma_value;
        s_context.gamma_dirty = 1;
    }
}

/* Set s_context.gamma_value base on source and target buffers */
vg_lite_void get_st_gamma_src_dest(vg_lite_buffer_t * source, vg_lite_buffer_t * target)
{
    uint32_t gamma_value = 0;

    /* Set gamma configuration of source buffer */
    if((source->format >= OPENVG_lRGBX_8888 && source->format <= OPENVG_A_4) ||
       (source->format >= OPENVG_lXRGB_8888 && source->format <= OPENVG_lARGB_8888_PRE) ||
       (source->format >= OPENVG_lBGRX_8888 && source->format <= OPENVG_lBGRA_8888_PRE) ||
       (source->format >= OPENVG_lXBGR_8888 && source->format <= OPENVG_lABGR_8888_PRE) ||
       (source->format >= OPENVG_lRGBX_8888_PRE && source->format <= OPENVG_lRGBA_4444_PRE)) {
        s_context.gamma_src = 0;
    }
    else {
        s_context.gamma_src = 1;
    }
    /* Set gamma configuration of dst buffer */
    if((target->format >= OPENVG_lRGBX_8888 && target->format <= OPENVG_A_4) ||
       (target->format >= OPENVG_lXRGB_8888 && target->format <= OPENVG_lARGB_8888_PRE) ||
       (target->format >= OPENVG_lBGRX_8888 && target->format <= OPENVG_lBGRA_8888_PRE) ||
       (target->format >= OPENVG_lXBGR_8888 && target->format <= OPENVG_lABGR_8888_PRE) ||
       (target->format >= OPENVG_lRGBX_8888_PRE && target->format <= OPENVG_lRGBA_4444_PRE)) {
        s_context.gamma_dst = 0;
    }
    else {
        s_context.gamma_dst = 1;
    }

    if(s_context.gamma_src == 0 && s_context.gamma_dst == 1) {
        gamma_value = 0x00002000;
    }
    else if(s_context.gamma_src == 1 && s_context.gamma_dst == 0) {
        gamma_value = 0x00001000;
    }
    else {
        gamma_value = 0x00000000;
    }

    if(source->image_mode == VG_LITE_STENCIL_MODE) {
        if(source->paintType == VG_LITE_PAINT_PATTERN
           || source->paintType == VG_LITE_PAINT_RADIAL_GRADIENT
           || source->paintType == VG_LITE_PAINT_LINEAR_GRADIENT) {
            gamma_value = s_context.gamma_stencil;
        }
        else if(source->paintType == VG_LITE_PAINT_COLOR && s_context.gamma_dst == 0) {
            gamma_value = 0x00001000;
        }
        else {
            gamma_value = 0x00000000;
        }
    }

    if(s_context.gamma_dirty == 0 && gamma_value != s_context.gamma_value) {
        s_context.gamma_value = gamma_value;
        s_context.gamma_dirty = 1;
    }
}

/* Set s_context.gamma_value base on source and target buffers */
vg_lite_void save_st_gamma_src_dest(vg_lite_buffer_t * source, vg_lite_buffer_t * target)
{
    uint32_t gamma_value = 0;

    /* Set gamma configuration of source buffer */
    if((source->format >= OPENVG_lRGBX_8888 && source->format <= OPENVG_A_4) ||
       (source->format >= OPENVG_lXRGB_8888 && source->format <= OPENVG_lARGB_8888_PRE) ||
       (source->format >= OPENVG_lBGRX_8888 && source->format <= OPENVG_lBGRA_8888_PRE) ||
       (source->format >= OPENVG_lXBGR_8888 && source->format <= OPENVG_lABGR_8888_PRE) ||
       (source->format >= OPENVG_lRGBX_8888_PRE && source->format <= OPENVG_lRGBA_4444_PRE)) {
        s_context.gamma_src = 0;
    }
    else {
        s_context.gamma_src = 1;
    }
    /* Set gamma configuration of dst buffer */
    if((target->format >= OPENVG_lRGBX_8888 && target->format <= OPENVG_A_4) ||
       (target->format >= OPENVG_lXRGB_8888 && target->format <= OPENVG_lARGB_8888_PRE) ||
       (target->format >= OPENVG_lBGRX_8888 && target->format <= OPENVG_lBGRA_8888_PRE) ||
       (target->format >= OPENVG_lXBGR_8888 && target->format <= OPENVG_lABGR_8888_PRE) ||
       (target->format >= OPENVG_lRGBX_8888_PRE && target->format <= OPENVG_lRGBA_4444_PRE)) {
        s_context.gamma_dst = 0;
    }
    else {
        s_context.gamma_dst = 1;
    }

    if(s_context.gamma_src == 0 && s_context.gamma_dst == 1) {
        gamma_value = 0x00002000;
    }
    else if(s_context.gamma_src == 1 && s_context.gamma_dst == 0) {
        gamma_value = 0x00001000;
    }
    else {
        gamma_value = 0x00000000;
    }

    if(target->image_mode == VG_LITE_STENCIL_MODE) {
        s_context.gamma_stencil = gamma_value;
        gamma_value = 0x00000000;
    }

    if(s_context.gamma_dirty == 0 && gamma_value != s_context.gamma_value) {
        s_context.gamma_value = gamma_value;
        s_context.gamma_dirty = 1;
    }
}

vg_lite_error_t vg_lite_enable_color_transform()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_enable_color_transform)();
#endif

#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_enable_color_transform\n");
#endif

    s_context.color_transform = (1 << 16);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_color_transform()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_disable_color_transform)();
#endif

#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_disable_color_transform\n");
#endif

    s_context.color_transform = 0;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_color_transform(vg_lite_color_transform_t * values)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_enable_color_transform)();
#endif

#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_float_t * color_transformations = (vg_lite_float_t *)values;
    int color_elements = 0;
    short temp_transform[8] = { 0 };
    uint32_t final_transform[8] = { 0 };

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_color_transform %p\n", values);
#endif

    for(color_elements = 0; color_elements < 8; color_elements++) {
        if(color_elements % 2) {
            color_transformations[color_elements] = CLAMP(color_transformations[color_elements], -1.0f, 1.0f);
        }
        else {
            color_transformations[color_elements] = CLAMP(color_transformations[color_elements], -127.0f, 127.0f);
        }
        temp_transform[color_elements] = (short)(color_transformations[color_elements] * 256);
        final_transform[color_elements] = (uint32_t)temp_transform[color_elements] & 0x0000FFFF;
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0C, final_transform[2] | final_transform[3] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0D, final_transform[4] | final_transform[5] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0E, final_transform[6] | final_transform[7] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0F, final_transform[0] | final_transform[1] << 16));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

#if !gcFEATURE_VG_LVGL_SUPPORT

typedef struct {
    float                 r;
    float                 g;
    float                 b;
    float                 a;
} Color;

int colorToInt(float c, int maxc)
{
    return MIN(MAX((int)floor(c * (float)maxc + 0.5f), 0), maxc);
}

float intToColor(unsigned int i, unsigned int maxi)
{
    return (float)(i & maxi) / (float)maxi;
}

Color readPixel(vg_lite_buffer_t * src, int x, int y)
{
    unsigned int p = 0;
    Color c;
    uint8_t * scanline = (uint8_t *)src->memory + y * src->stride;

    uint8_t bitsPerPixel = 0;
    int rb = 0;
    int gb = 0;
    int bb = 0;
    int ab = 0;
    int rs = 0;
    int gs = 0;
    int bs = 0;
    int as = 0;
    switch(src->format) {
        case VG_LITE_A8:
        case VG_LITE_L8:
            ab = 8;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ABGR4444:
            rs = 12;
            gs = 8;
            bs = 4;
            as = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ARGB4444:
            bs = 12;
            gs = 8;
            rs = 4;
            as = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGBA4444:
            as = 12;
            bs = 8;
            gs = 4;
            rs = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA4444:
            as = 12;
            rs = 8;
            gs = 4;
            bs = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGB565:
            rs = 0;
            gs = 5;
            bs = 11;
            as = 0;
            rb = 5;
            gb = 6;
            bb = 5;
            ab = 0;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGR565:
            rs = 11;
            gs = 5;
            bs = 0;
            as = 0;
            rb = 5;
            gb = 6;
            bb = 5;
            ab = 0;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ABGR8888:
        case VG_LITE_XBGR8888:
            rs = 24;
            gs = 16;
            bs = 8;
            as = 0;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_ARGB8888:
        case VG_LITE_XRGB8888:
            rs = 8;
            gs = 16;
            bs = 24;
            as = 0;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_RGBA8888:
        case VG_LITE_RGBX8888:
            rs = 0;
            gs = 8;
            bs = 16;
            as = 24;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_BGRA8888:
        case VG_LITE_BGRX8888:
            rs = 16;
            gs = 8;
            bs = 0;
            as = 24;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_ABGR1555:
            rs = 11;
            gs = 6;
            bs = 1;
            as = 0;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGBA5551:
            rs = 0;
            gs = 5;
            bs = 10;
            as = 15;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ARGB1555:
            rs = 1;
            gs = 6;
            bs = 11;
            as = 0;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA5551:
            rs = 10;
            gs = 5;
            bs = 0;
            as = 15;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA2222:
            rs = 4;
            gs = 2;
            bs = 0;
            as = 6;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_RGBA2222:
            rs = 0;
            gs = 2;
            bs = 4;
            as = 6;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ABGR2222:
            rs = 6;
            gs = 4;
            bs = 2;
            as = 0;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ARGB2222:
            rs = 2;
            gs = 4;
            bs = 6;
            as = 0;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        default:
            break;
    }

    switch(bitsPerPixel) {
        case 32: {
                uint32_t * s = (((uint32_t *)scanline) + x);
                p = (unsigned int) * s;
                break;
            }

        case 16: {
                uint16_t * s = ((uint16_t *)scanline) + x;
                p = (unsigned int) * s;
                break;
            }

        case 8: {
                uint8_t * s = ((uint8_t *)scanline) + x;
                p = (unsigned int) * s;
                break;
            }
        case 4: {
                uint8_t * s = ((uint8_t *)scanline) + (x >> 1);
                p = (unsigned int)(*s >> ((x & 1) << 2)) & 0xf;
                break;
            }
        case 2: {
                uint8_t * s = ((uint8_t *)scanline) + (x >> 2);
                p = (unsigned int)(*s >> ((x & 3) << 1)) & 0x3;
                break;
            }
        default: {
                uint8_t * s = ((uint8_t *)scanline) + (x >> 3);
                p = (unsigned int)(*s >> (x & 7)) & 0x1;
                break;
            }
    }

    //rgba
    c.r = rb ? intToColor(p >> rs, (1 << rb) - 1) : (float)1.0f;
    c.g = gb ? intToColor(p >> gs, (1 << gb) - 1) : (float)1.0f;
    c.b = bb ? intToColor(p >> bs, (1 << bb) - 1) : (float)1.0f;
    c.a = ab ? intToColor(p >> as, (1 << ab) - 1) : (float)1.0f;

    return c;
}

void writePixel(vg_lite_buffer_t * temp, int x, int y, Color * c)
{
    uint8_t bitsPerPixel = 0;
    int rb = 0;
    int gb = 0;
    int bb = 0;
    int ab = 0;
    int rs = 0;
    int gs = 0;
    int bs = 0;
    int as = 0;
    switch(temp->format) {
        case VG_LITE_A8:
        case VG_LITE_L8:
            ab = 8;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ABGR4444:
            rs = 12;
            gs = 8;
            bs = 4;
            as = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ARGB4444:
            bs = 12;
            gs = 8;
            rs = 4;
            as = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGBA4444:
            as = 12;
            bs = 8;
            gs = 4;
            rs = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA4444:
            as = 12;
            rs = 8;
            gs = 4;
            bs = 0;
            rb = 4;
            gb = 4;
            bb = 4;
            ab = 4;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGB565:
            rs = 0;
            gs = 5;
            bs = 11;
            as = 0;
            rb = 5;
            gb = 6;
            bb = 5;
            ab = 0;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGR565:
            rs = 11;
            gs = 5;
            bs = 0;
            as = 0;
            rb = 5;
            gb = 6;
            bb = 5;
            ab = 0;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ABGR8888:
        case VG_LITE_XBGR8888:
            rs = 24;
            gs = 16;
            bs = 8;
            as = 0;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_ARGB8888:
        case VG_LITE_XRGB8888:
            rs = 8;
            gs = 16;
            bs = 24;
            as = 0;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_RGBA8888:
        case VG_LITE_RGBX8888:
            rs = 0;
            gs = 8;
            bs = 16;
            as = 24;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_BGRA8888:
        case VG_LITE_BGRX8888:
            rs = 16;
            gs = 8;
            bs = 0;
            as = 24;
            rb = 8;
            gb = 8;
            bb = 8;
            ab = 8;
            bitsPerPixel = 32;
            break;
        case VG_LITE_ABGR1555:
            rs = 11;
            gs = 6;
            bs = 1;
            as = 0;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_RGBA5551:
            rs = 0;
            gs = 5;
            bs = 10;
            as = 15;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_ARGB1555:
            rs = 1;
            gs = 6;
            bs = 11;
            as = 0;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA5551:
            rs = 10;
            gs = 5;
            bs = 0;
            as = 15;
            rb = 5;
            gb = 5;
            bb = 5;
            ab = 1;
            bitsPerPixel = 16;
            break;
        case VG_LITE_BGRA2222:
            rs = 4;
            gs = 2;
            bs = 0;
            as = 6;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_RGBA2222:
            rs = 0;
            gs = 2;
            bs = 4;
            as = 6;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ABGR2222:
            rs = 6;
            gs = 4;
            bs = 2;
            as = 0;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        case VG_LITE_ARGB2222:
            rs = 2;
            gs = 4;
            bs = 6;
            as = 0;
            rb = 2;
            gb = 2;
            bb = 2;
            ab = 2;
            bitsPerPixel = 8;
            break;
        default:
            break;
    }

    unsigned int cr = rb ? colorToInt(c->r, (1 << rb) - 1) : 0;
    unsigned int cg = gb ? colorToInt(c->g, (1 << gb) - 1) : 0;
    unsigned int cb = bb ? colorToInt(c->b, (1 << bb) - 1) : 0;
    unsigned int ca = ab ? colorToInt(c->a, (1 << ab) - 1) : 0;

    unsigned int p = (cr << rs) | (cg << gs) | (cb << bs) | (ca << as);
    char * scanline = (char *)temp->memory + y * temp->stride;
    switch(bitsPerPixel) {
        case 32: {
                uint32_t * s = ((uint32_t *)scanline) + x;
                *s = (uint32_t)p;
                break;
            }

        case 16: {
                uint16_t * s = ((uint16_t *)scanline) + x;
                *s = (uint16_t)p;
                break;
            }

        case 8: {
                char * s = ((char *)scanline) + x;
                *s = (char)p;
                break;
            }
        case 4: {
                char * s = ((char *)scanline) + (x >> 1);
                *s = (char)((p << ((x & 1) << 2)) | ((unsigned int) * s & ~(0xf << ((x & 1) << 2))));
                break;
            }

        case 2: {
                char * s = ((char *)scanline) + (x >> 2);
                *s = (char)((p << ((x & 3) << 1)) | ((unsigned int) * s & ~(0x3 << ((x & 3) << 1))));
                break;
            }

        default: {
                break;
            }
    }
}

vg_lite_void setup_lvgl_image(vg_lite_buffer_t * dst, vg_lite_buffer_t * src, vg_lite_buffer_t * lvgl_buf,
                              vg_lite_blend_t operation)
{
    Color c_src, c_dst, c_temp;
    /* copy source region to tmp dst */
    for(int j = 0; j < src->height; j++) {
        for(int i = 0; i < src->width; i++) {
            c_src = readPixel(src, i, j);
            c_dst = readPixel(dst, i, j);

            switch(operation) {
                case VG_LITE_BLEND_NORMAL_LVGL:
                    c_temp.a = c_src.a;
                    c_temp.r = c_src.a * c_src.r;
                    c_temp.g = c_src.a * c_src.g;
                    c_temp.b = c_src.a * c_src.b;
                    break;
                case VG_LITE_BLEND_ADDITIVE_LVGL:
                    c_temp.a = c_src.a;
                    c_temp.r = (c_src.r + c_dst.r) * c_src.a;
                    c_temp.g = (c_src.g + c_dst.g) * c_src.a;
                    c_temp.b = (c_src.b + c_dst.b) * c_src.a;
                    break;
                case VG_LITE_BLEND_SUBTRACT_LVGL:
                    c_temp.a = c_src.a;
                    c_temp.r = c_src.r < c_dst.r ? (c_dst.r - c_src.r) * c_src.a : 0;
                    c_temp.g = c_src.g < c_dst.g ? (c_dst.g - c_src.g) * c_src.a : 0;
                    c_temp.b = c_src.b < c_dst.b ? (c_dst.b - c_src.b) * c_src.a : 0;
                    break;
                case VG_LITE_BLEND_MULTIPLY_LVGL:
                    c_temp.a = c_src.a;
                    c_temp.r = c_src.r * c_dst.r * c_src.a;
                    c_temp.g = c_src.g * c_dst.g * c_src.a;
                    c_temp.b = c_src.b * c_dst.b * c_src.a;
                    break;
                default:
                    break;
            }
            if(lvgl_buf) {
                writePixel(lvgl_buf, i, j, &c_temp);
            }
#if !gcFEATURE_VG_GLOBAL_ALPHA
            c_dst.a = 1.0;
            writePixel(dst, i, j, &c_dst);
#endif
        }
    }
    return;
}

#endif  /* !gcFEATURE_VG_LVGL_SUPPORT */

/****************** FLEXA Support ***************/

vg_lite_error_t vg_lite_flexa_enable()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_flexa_enable)();
#endif

#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;
    vg_lite_kernel_flexa_info_t flexa_data;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_flexa_enable\n");
#endif

    flexa_data.sbi_mode = s_context.sbi_mode = 0x1;
    flexa_data.sync_mode = s_context.sync_mode = BIT(14);
    flexa_data.flexa_mode = s_context.flexa_mode = 0x1;
    flexa_data.segment_address = s_context.segment_address;
    flexa_data.segment_count = s_context.segment_count;
    flexa_data.segment_size = s_context.segment_size;
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.start_flag = s_context.start_flag = BIT(9);
    flexa_data.stop_flag = s_context.stop_flag = BIT(11);
    flexa_data.reset_flag = s_context.reset_flag = BIT(10);
    /* set sync mode */
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_ENABLE, &flexa_data));

    /* check if reset is complete */
    data.addr = 0x03600;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_CHECK, &data));
    reset_bit = data.reg;

    if(reset_bit == 1) {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_set_stream(vg_lite_uint8_t stream_id)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_flexa_set_stream)(stream_id);
#endif

#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_flexa_set_stream %d\n", stream_id);
#endif

    s_context.stream_id = stream_id << 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_bg_buffer(vg_lite_uint8_t stream_id, vg_lite_buffer_t * buffer,
                                        vg_lite_uint32_t bg_seg_count, vg_lite_uint32_t bg_seg_size)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_flexa_bg_buffer)(stream_id, buffer, bg_seg_count, bg_seg_size);
#endif

#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_flexa_bg_buffer %d %p %d %d\n", stream_id, buffer, bg_seg_count, bg_seg_size);
#endif

    flexa_data.sbi_mode = s_context.sbi_mode;
    flexa_data.segment_address = s_context.segment_address = buffer->address;
    flexa_data.segment_count = s_context.segment_count = bg_seg_count;
    flexa_data.segment_size = s_context.segment_size = bg_seg_size;
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.start_flag = s_context.start_flag;
    flexa_data.stop_flag = s_context.stop_flag;
    flexa_data.reset_flag = s_context.reset_flag;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS, &flexa_data));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_stop_frame()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_flexa_stop_frame)();
#endif

#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_flexa_stop_frame\n");
#endif

    flexa_data.stop_flag = s_context.stop_flag = BIT(11);
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.sbi_mode = s_context.sbi_mode;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_STOP_FRAME, &flexa_data));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_disable()
{
#if DUMP_API
    FUNC_DUMP(vg_lite_flexa_disable)();
#endif

#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_flexa_disable\n");
#endif

    flexa_data.flexa_mode = s_context.flexa_mode = 0x0;
    flexa_data.sync_mode = s_context.sync_mode = BIT(2);
    flexa_data.stream_id = s_context.stream_id = 0;
    flexa_data.sbi_mode = s_context.sbi_mode = 0x0;
    flexa_data.start_flag = s_context.start_flag = 0x0;
    flexa_data.stop_flag = s_context.stop_flag = 0x0;
    flexa_data.reset_flag = s_context.reset_flag = 0x0;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_DISABLE, &flexa_data));

    /* check if reset is complete */
    data.addr = 0x03600;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_CHECK, &data));
    reset_bit = data.reg;
    if(reset_bit == 1) {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/****************** FAST_CLEAR implementation. ***************/
#if gcFEATURE_VG_IM_FASTCLEAR
static vg_lite_error_t _free_fc_buffer(vg_lite_fc_buffer_t * buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_free_t free;

    if(buffer == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    /* Make sure we have a valid memory handle. */
    if(buffer->handle) {
        /* Free the buffer. */
        free.memory_handle = buffer->handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));

        /* Mark the buffer as freed. */
        buffer->handle = NULL;
        buffer->memory = NULL;
    }

    return VG_LITE_SUCCESS;
}
static vg_lite_error_t convert_color(vg_lite_buffer_format_t format, uint32_t value, uint32_t * result, int * bpp)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t r, g, b, a;
    int Bpp = 0;

    r = B(value);
    g = G(value);
    b = R(value);
    a = A(value);

    do {
        switch(format) {
            case VG_LITE_RGBA8888:
                *result = ARGB(a, b, g, r);
                Bpp = 32;
                break;

            case VG_LITE_BGRA8888:
                *result = ARGB(a, r, g, b);
                Bpp = 32;
                break;

            case VG_LITE_RGBX8888:
                *result = ARGB(0xff, b, g, r);
                Bpp = 32;
                break;

            case VG_LITE_BGRX8888:
                *result = ARGB(0xff, r, g, b);
                Bpp = 32;
                break;


            case VG_LITE_RGBA4444:
                *result = ARGB4(a, b, g, r);
                Bpp = 16;
                break;

            case VG_LITE_BGRA4444:
                *result = ARGB4(a, r, g, b);
                Bpp = 16;
                break;

            case VG_LITE_RGB565:
                *result = ((b & 0xf8) << 8) |
                          ((g & 0xfc) << 3) |
                          ((r & 0xf8) >> 3);
                Bpp = 16;
                break;

            case VG_LITE_BGR565:
                *result = ((r & 0xf8) << 8) |
                          ((g & 0xfc) << 3) |
                          ((b & 0xf8) >> 3);
                Bpp = 16;
                break;

            case VG_LITE_BGRA5551:
                *result = ((b & 0xf8) << 8) |
                          ((g & 0xf8) << 3) |
                          ((r & 0xf8) >> 2) |
                          ((a & 0x80) >> 7);
                Bpp = 16;
                break;

            case VG_LITE_A8:
                *result = ARGB(a, a, a, a);
                Bpp = 8;
                break;

            case VG_LITE_L8:
                *result = ARGB(r, r, r, r);
                Bpp = 8;
                break;

            default:
                error = VG_LITE_NOT_SUPPORT;
                break;
        }
    } while(0);

    if(bpp != NULL) {
        *bpp = Bpp;
    }

    if(Bpp == 16) {
        *result = ((*result) << 16) | (*result);
    }
    return error;
}

/* Fill Target buffer by FC buffer. Only used in cmodel/fpga for verification. */
#if defined(DEBUG) || defined(_DEBUG)
static vg_lite_error_t fill_fc_target(vg_lite_buffer_t * target, vg_lite_buffer_t * fcb)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint8_t * fc = (uint8_t *)fcb->memory;
    uint16_t * target16;
    uint32_t * target32;
    uint8_t * target8;
    uint32_t  clear32;
    int byte_done = 0;
    int i, j, k;
    int bpp;

    do {
        convert_color(target->format, target->fc_buffer[0].color, &clear32, &bpp);

        if(bpp == 32) {
            target32 = (uint32_t *)target->memory;
            for(i = 0; i < fcb->width; i++) {

                for(j = 0; j < 8; j++) {    /* Loop the bits*/

                    if(!(((*fc) >> j) & 1)) {
                        for(k = 0; k < 64 / 4; k++) {
                            target32[k] = clear32;
                            byte_done += 4;
                            if(byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target32 += 64 / 4;
                }

                fc++;
            }
        }
        else if(bpp == 16) {
            target16 = (uint16_t *)target->memory;
            for(i = 0; i < fcb->width; i++) {

                for(j = 0; j < 8; j++) {    /* Loop the bits*/

                    if(!(((*fc) >> j) & 1)) {
                        for(k = 0; k < 64 / 2; k++) {
                            target16[k] = (uint16_t)clear32;
                            byte_done += 2;
                            if(byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target16 += 64 / 2;
                }

                fc++;
            }
        }
        else if(bpp == 8) {
            target8 = (uint8_t *)target->memory;
            for(i = 0; i < fcb->width; i++) {

                for(j = 0; j < 8; j++) {    /* Loop the bits*/

                    if(!(((*fc) >> j) & 1)) {
                        for(k = 0; k < 64; k++) {
                            target8[k] = (uint8_t)clear32;
                            byte_done++;
                            if(byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target8 += 64;
                }

                fc++;
            }
        }
    } while(0);

    return error;
}
#endif

/* Update the fast_clear buffer when render target switched. */
static vg_lite_error_t update_fc_buffer(vg_lite_buffer_t * target)
{
    int rt_bytes;
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t * context = GET_CONTEXT();
    vg_lite_kernel_allocate_t allocate;

    do {
        if(target == NULL) {
            error = VG_LITE_INVALID_ARGUMENT;
            break;
        }

        rt_bytes = target->stride * target->height;
        rt_bytes = VG_LITE_ALIGN(rt_bytes, (FC_BIT_TO_BYTES * 2));
        rt_bytes = rt_bytes / FC_BIT_TO_BYTES / 2;
        /* Only allocate new buffer when the allocated is not big enough. Yes*/
        if(rt_bytes > target->fc_buffer[0].stride) {
            _free_fc_buffer(&target->fc_buffer[0]);

            target->fc_buffer[0].width = rt_bytes;         /* The actually used bytes. */
            rt_bytes = VG_LITE_ALIGN(rt_bytes, FC_BURST_BYTES);     /* The allocated aligned bytes. */
            target->fc_buffer[0].stride = rt_bytes;
            target->fc_buffer[0].height = 1;
            allocate.bytes = rt_bytes;
            allocate.contiguous = 1;

            VG_LITE_BREAK_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &allocate));
            target->fc_buffer[0].handle = allocate.memory_handle;
            target->fc_buffer[0].memory = allocate.memory;
            target->fc_buffer[0].address = allocate.memory_gpu;
        }
        else {
            /* Just update the fc buffer size. */
            target->fc_buffer[0].width = rt_bytes;
        }
        memset(target->fc_buffer[0].memory, 0xff, target->fc_buffer[0].stride);
        VG_LITE_RETURN_ERROR(push_state(context, 0x0A9A, target->fc_buffer[0].address));   /* FC buffer address. */
    } while(0);

    return error;
}

/* Update FC registers and clear FC buffer. */
static vg_lite_error_t clear_fc(vg_lite_fc_buffer_t * buffer, uint32_t value)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t * context = GET_CONTEXT();
    uint32_t bytes_to_clear;

    if(buffer == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    bytes_to_clear = buffer->stride / FC_BURST_BYTES;
    buffer->color = value;

    do {
        VG_LITE_BREAK_ERROR(push_state(context, 0x0A9B, value));                       /* FC clear value. */
        VG_LITE_BREAK_ERROR(push_state(context, 0x0AB0, 0x80000000 | bytes_to_clear));   /* FC clear command. */
    } while(0);

    return error;
}

#if VG_TARGET_FC_DUMP
static int fc_buf_dump(vg_lite_buffer_t * target, vg_lite_buffer_t * fcb)
{
    int error = VG_LITE_SUCCESS;
    uint8_t * fc = (uint8_t *)fcb->memory;
    uint8_t * target8;
    int byte_done = 0;
    int target_bytes;
    int i, j;

    static unsigned s_cnt;
    unsigned cnt = s_cnt;

    FILE * fpFCBuf;
    FILE * fpTargetBuf;
    FILE * fpTargetBufInfo;
    char buf[256];

    s_cnt++;

    sprintf(buf, "gc555.fc_buf.f%04d.txt", cnt);
    fpFCBuf = fopen(buf, "wt");
    if(NULL == fpFCBuf) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        return -1;
    }

    sprintf(buf, "gc555.target_buf_info.f%04d.txt", cnt);
    fpTargetBufInfo = fopen(buf, "wt");
    if(NULL == fpTargetBufInfo) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }
    else {
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "format", target->format);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "tiled", target->tiled);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "width", target->width);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "height", target->height);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "stride", target->stride);

        fclose(fpTargetBufInfo);
    }

    sprintf(buf, "gc555.target_buf.f%04d.txt", cnt);
    fpTargetBuf = fopen(buf, "wt");
    if(NULL == fpTargetBuf) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }

    /* Dump FC buffer & Dump target buffer */
    target8 = (uint8_t *)target->memory;
    target_bytes = target->stride * target->height;

    for(i = 0; i < fcb->width; ++i) {
        fprintf(fpFCBuf, "%02x\n", fc[i]);
        /* 1 byte of fc related with 512 bytes of target buffer */
        for(j = 0; j < 128; ++j) {
            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[0] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[1] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[2] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x\n", byte_done < target_bytes ? target8[3] : 0);
            byte_done++;

            target8 += 4;
        }
    }

    fclose(fpFCBuf);
    fclose(fpTargetBuf);

    return error;
}
#endif /* VG_TARGET_FC_DUMP */
#endif /* gcFEATURE_VG_IM_FASTCLEAR */

#endif /* LV_USE_VG_LITE_DRIVER */

