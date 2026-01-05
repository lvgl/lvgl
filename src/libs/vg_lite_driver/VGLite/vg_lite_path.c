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

/* Path data operations. */
#define CDALIGN(value, by) (((value) + (by) - 1) & ~((by) - 1))
#define CDMIN(x, y) ((x) > (y) ? (y) : (x))
#define CDMAX(x, y) ((x) > (y) ? (x) : (y))


extern uint32_t transform(vg_lite_point_t * result, vg_lite_float_t x, vg_lite_float_t y, vg_lite_matrix_t * matrix);
extern uint32_t convert_blend(vg_lite_blend_t blend);
extern uint32_t inverse(vg_lite_matrix_t * result, vg_lite_matrix_t * matrix);
extern uint32_t convert_yuv2rgb(vg_lite_yuv2rgb_t yuv);
extern uint32_t convert_uv_swizzle(vg_lite_swizzle_t swizzle);
extern uint32_t convert_source_format(vg_lite_buffer_format_t format);
extern vg_lite_error_t check_compress(vg_lite_buffer_format_t format, vg_lite_compress_mode_t compress_mode,
                                      vg_lite_buffer_layout_t tiled, uint32_t width, uint32_t height);
extern void get_format_bytes(vg_lite_buffer_format_t format, uint32_t * mul, uint32_t * div, uint32_t * bytes_align);
extern vg_lite_error_t srcbuf_align_check(vg_lite_buffer_t * source);

extern vg_lite_matrix_t identity_mtx;

/* Convert VGLite data format to HW value. */
static uint32_t convert_path_format(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 0;

        case VG_LITE_S16:
            return 0x100000;

        case VG_LITE_S32:
            return 0x200000;

        case VG_LITE_FP32:
            return 0x300000;

        default:
            return 0;
    }
}

/* Convert VGLite quality enums to HW values. */
static uint32_t convert_path_quality(vg_lite_quality_t quality)
{
    switch(quality) {
        case VG_LITE_HIGH:
            return 0x3;

        case VG_LITE_UPPER:
            return 0x2;

        case VG_LITE_MEDIUM:
            return 0x1;

        default:
            return 0x0;
    }
}

static int32_t get_data_count(uint8_t cmd)
{
    static int32_t count[] = {
        0,
        0,
        2,
        2,
        2,
        2,
        4,
        4,
        6,
        6,
        0,
        1,
        1,
        1,
        1,
        2,
        2,
        4,
        4,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5
    };

    if(cmd > VLC_OP_LCWARC_REL) {
        return -1;
    }
    else {
        return count[cmd];
    }
}

static void compute_pathbounds(float * xmin, float * ymin, float * xmax, float * ymax, float x, float y)
{
    if(xmin != NULL) {
        *xmin = *xmin < x ? *xmin : x;
    }

    if(xmax != NULL) {
        *xmax = *xmax > x ? *xmax : x;
    }

    if(ymin != NULL) {
        *ymin = *ymin < y ? *ymin : y;
    }

    if(ymax != NULL) {
        *ymax = *ymax > y ? *ymax : y;
    }
}

int32_t get_data_size(vg_lite_format_t format)
{
    int32_t data_size = 0;

    switch(format) {
        case VG_LITE_S8:
            data_size = sizeof(int8_t);
            break;

        case VG_LITE_S16:
            data_size = sizeof(int16_t);
            break;

        case VG_LITE_S32:
            data_size = sizeof(int32_t);
            break;

        default:
            data_size = sizeof(vg_lite_float_t);
            break;
    }

    return data_size;
}

vg_lite_error_t vg_lite_init_path(vg_lite_path_t * path,
                                  vg_lite_format_t data_format,
                                  vg_lite_quality_t quality,
                                  vg_lite_uint32_t path_length,
                                  vg_lite_pointer path_data,
                                  vg_lite_float_t min_x, vg_lite_float_t min_y,
                                  vg_lite_float_t max_x, vg_lite_float_t max_y)
{
    int32_t data_size, num = 0;

    if(path == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    memset(path, 0, sizeof(*path));
    path->format = data_format;
    path->quality = quality;
    path->bounding_box[0] = min_x;
    path->bounding_box[1] = min_y;
    path->bounding_box[2] = max_x;
    path->bounding_box[3] = max_y;

    /* Path data cannot end with a CLOSE op. Replace CLOSE with END for path_data */
    data_size = get_data_size(data_format);
    num = path_length / data_size;

    switch(data_format) {
        case VG_LITE_S8:
            if(path_data && (*((char *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((int *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_S16:
            if(path_data && (*(char *)((short *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((short *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_S32:
            if(path_data && (*(char *)((int *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((int *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_FP32:
            if(path_data && (*(char *)((float *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((float *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        default:
            break;
    }

    path->path_length = path_length;
    path->path = path_data;

    path->path_changed = 1;
    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;
    path->pdata_internal = 0;
    s_context.path_lastX = 0;
    s_context.path_lastY = 0;
    /* Default FILL path type*/
    path->path_type = VG_LITE_DRAW_FILL_PATH;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_path_type(vg_lite_path_t * path, vg_lite_path_type_t path_type)
{
    if(!path ||
       (path_type != VG_LITE_DRAW_FILL_PATH &&
        path_type != VG_LITE_DRAW_STROKE_PATH &&
        path_type != VG_LITE_DRAW_FILL_STROKE_PATH)
      )
        return VG_LITE_INVALID_ARGUMENT;

    path->path_type = path_type;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_clear_path(vg_lite_path_t * path)
{
    vg_lite_error_t error;
    if(path->uploaded.handle != NULL) {
        vg_lite_kernel_free_t free_cmd;
        free_cmd.memory_handle = path->uploaded.handle;
        error = vg_lite_kernel(VG_LITE_FREE, &free_cmd);
        if(error != VG_LITE_SUCCESS)
            return error;
    }

#if (CHIPID==0x355)
    if(path->stroke && path->stroke->uploaded.handle != NULL) {
        vg_lite_kernel_free_t free_cmd;
        free_cmd.memory_handle = path->stroke->uploaded.handle;
        error = vg_lite_kernel(VG_LITE_FREE, &free_cmd);
        if(error != VG_LITE_SUCCESS)
            return error;
    }
#endif

    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;

#if (CHIPID==0x355)
    if(path->stroke) {
        path->stroke->uploaded.address = 0;
        path->stroke->uploaded.bytes = 0;
        path->stroke->uploaded.handle = NULL;
        path->stroke->uploaded.memory = NULL;
    }
#endif

    if(path->pdata_internal == 1 && path->path != NULL) {
        vg_lite_os_free(path->path);
    }
    path->path = NULL;

    if(path->stroke_path) {
        vg_lite_os_free(path->stroke_path);
        path->stroke_path = NULL;
    }

#if gcFEATURE_VG_STROKE_PATH
    if(path->stroke) {
        if(path->stroke->path_list_divide) {
            vg_lite_path_list_ptr cur_list;
            while(path->stroke->path_list_divide) {
                cur_list = path->stroke->path_list_divide->next;
                if(path->stroke->path_list_divide->path_points) {
                    vg_lite_path_point_ptr temp_point;
                    while(path->stroke->path_list_divide->path_points) {
                        temp_point = path->stroke->path_list_divide->path_points->next;
                        vg_lite_os_free(path->stroke->path_list_divide->path_points);
                        path->stroke->path_list_divide->path_points = temp_point;
                    }
                    temp_point = NULL;
                }
                vg_lite_os_free(path->stroke->path_list_divide);
                path->stroke->path_list_divide = cur_list;
            }
            cur_list = 0;
        }

        if(path->stroke->stroke_paths) {
            vg_lite_sub_path_ptr temp_sub_path;
            while(path->stroke->stroke_paths) {
                temp_sub_path = path->stroke->stroke_paths->next;
                if(path->stroke->stroke_paths->point_list) {
                    vg_lite_path_point_ptr temp_point;
                    while(path->stroke->stroke_paths->point_list) {
                        temp_point = path->stroke->stroke_paths->point_list->next;
                        vg_lite_os_free(path->stroke->stroke_paths->point_list);
                        path->stroke->stroke_paths->point_list = temp_point;
                    }
                    temp_point = NULL;
                }
                vg_lite_os_free(path->stroke->stroke_paths);
                path->stroke->stroke_paths = temp_sub_path;
            }
            temp_sub_path = NULL;
        }

        if(path->stroke->dash_pattern)
            vg_lite_os_free(path->stroke->dash_pattern);

        vg_lite_os_free(path->stroke);
        path->stroke = NULL;
        path->stroke_valid = 0;


        path->stroke_size = 0;
    }
#endif

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_upload_path(vg_lite_path_t * path)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_upload_path)(path);
#endif

    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t bytes;
    vg_lite_buffer_t Buf, * buffer;

    buffer = &Buf;

    /* Compute the number of bytes required for path + command buffer prefix/postfix. */
    bytes = (8 + path->path_length + 7 + 8) & ~7;

    /* Allocate GPU memory. */
    buffer->width  = bytes;
    buffer->height = 1;
    buffer->stride = 0;
    buffer->format = VG_LITE_A8;
    VG_LITE_RETURN_ERROR(vg_lite_allocate(buffer));

    /* Initialize command buffer prefix. */
    ((uint32_t *) buffer->memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
    ((uint32_t *) buffer->memory)[1] = 0;

    /* Copy the path data. */
    memcpy((uint32_t *) buffer->memory + 2, path->path, path->path_length);

    /* Initialize command buffer postfix. */
    ((uint32_t *) buffer->memory)[(bytes >> 2) - 2] = VG_LITE_RETURN();
    ((uint32_t *) buffer->memory)[(bytes >> 2) - 1] = 0;

    /* Mark path as uploaded. */
    path->path = buffer->memory;
    path->uploaded.handle = buffer->handle;
    path->uploaded.address = buffer->address;
    path->uploaded.memory = buffer->memory;
    path->uploaded.bytes = bytes;
    path->path_changed = 0;
    VLM_PATH_ENABLE_UPLOAD(*path);      /* Implicitly enable path uploading. */

    /* Return pointer to vg_lite_buffer structure. */
    return error;
}

vg_lite_uint32_t vg_lite_get_path_length(vg_lite_uint8_t * cmd, vg_lite_uint32_t count, vg_lite_format_t format)
{
    uint32_t size = 0;
    int32_t dCount = 0;
    uint32_t i = 0;
    int32_t data_size = 0;

    data_size = get_data_size(format);

    for(i = 0; i < count; i++) {
        size++;     /* OP CODE. */

        dCount = get_data_count(cmd[i]);
        size = CDALIGN(size, data_size);
        size += dCount * data_size;

    }
    if(cmd[count - 1] != VLC_OP_END || cmd[count - 1] != VLC_OP_CLOSE) {
        size++;
        size = CDALIGN(size, data_size);
    }

    return size;
}

vg_lite_error_t vg_lite_append_path(vg_lite_path_t * path,
                                    vg_lite_uint8_t * cmd,
                                    vg_lite_pointer data,
                                    vg_lite_uint32_t seg_count)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t i;
    int32_t j;
    int32_t offset = 0;
    int32_t dataCount = 0;
    float * dataf = (float *) data;
    float * pathf = NULL;
    int32_t * data_s32 = (int32_t *) data;
    int32_t * path_s32 = NULL;
    int16_t * data_s16 = (int16_t *) data;
    int16_t * path_s16 = NULL;
    int8_t * data_s8 = (int8_t *) data;
    int8_t * path_s8 = NULL;
    uint8_t * pathc = NULL;
    int32_t data_size;
    uint8_t arc_path = 0;
    uint8_t h_v_path = 0;
    uint8_t smooth_path = 0;
    float px = 0.0f, py = 0.0f, cx = 0.0f, cy = 0.0f;
    int rel = 0;

    if(cmd == NULL || data == NULL || path == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    for(i = 0; i < seg_count; i++) {
        if(cmd[i] > VLC_OP_LCWARC_REL)
            return VG_LITE_INVALID_ARGUMENT;
    }

    /* Support NULL path->path case for OpenVG */
    if(!path->path) {
        data_size = vg_lite_get_path_length(cmd, seg_count, path->format);
        path->path = (vg_lite_pointer)vg_lite_os_malloc(data_size);
        if(!path->path) {
            return VG_LITE_OUT_OF_RESOURCES;
        }
        path->pdata_internal = 1;
        memset(path->path, 0, data_size);
    }

    data_size = get_data_size(path->format);
    path->path_changed = 1;
    pathf = (float *)path->path;
    path_s32 = (int32_t *)path->path;
    path_s16 = (int16_t *)path->path;
    path_s8 = (int8_t *)path->path;
    pathc = (uint8_t *)path->path;
    /* Set bounding box if the first opcode is VLC_OP_MOVE_* */
    if((cmd[0] & 0xfe) == VLC_OP_MOVE) {
        switch(path->format) {
            case VG_LITE_S8:
                cx = (float)data_s8[0];
                cy = (float)data_s8[1];
                break;
            case VG_LITE_S16:
                cx = (float)data_s16[0];
                cy = (float)data_s16[1];
                break;
            case VG_LITE_S32:
                cx = (float)data_s32[0];
                cy = (float)data_s32[1];
                break;
            case VG_LITE_FP32:
                cx = (float)dataf[0];
                cy = (float)dataf[1];
                break;
        }
        path->bounding_box[0] = path->bounding_box[2] = cx;
        path->bounding_box[1] = path->bounding_box[3] = cy;
    }

    /* Loop to fill path data. */
    for(i = 0; i < seg_count; i++) {
#if (CHIPID == 0x355)
        if((i < seg_count) && cmd[i] == VLC_OP_CLOSE && (cmd[i + 1] == VLC_OP_MOVE || cmd[i + 1] == VLC_OP_MOVE_REL)) {
            if(data_size == 1) {
                *(pathc + offset) = cmd[i];
                offset++;
            }
            else if(data_size == 2) {
                *(uint16_t *)(pathc + offset) = 0x0101;
                offset += 2;
            }
            else if(data_size == 4) {
                *(uint32_t *)(pathc + offset) = 0x01010101;
                offset += 4;
            }
        }
        else
#endif
        {
            *(pathc + offset) = cmd[i];
            offset++;
        }

        dataCount = get_data_count(cmd[i]);
        /* compute the bounding_box. */
        if(dataCount >= 0) {
            offset = CDALIGN(offset, data_size);
            if((cmd[i] > VLC_OP_CLOSE) &&
               (cmd[i] < VLC_OP_HLINE) &&
               ((cmd[i] & 0x01) == 1)) {
                rel = 1;
            }
            else if((cmd[i] >= VLC_OP_HLINE) &&
                    ((cmd[i] & 0x01) == 0)) {
                rel = 1;
            }
            else {
                rel = 0;
            }
            if(cmd[i] >= VLC_OP_HLINE && cmd[i] <= VLC_OP_VLINE_REL) {
                switch(path->format) {
                    case VG_LITE_S8:
                        path_s8 = (int8_t *)(pathc + offset);
                        path_s8[0] = *data_s8;
                        data_s8++;
                        if(rel) {
                            cx = px + (float)path_s8[0];
                            cy = py + (float)path_s8[1];
                        }
                        else {
                            cx = (float)path_s8[0];
                            cy = (float)path_s8[1];
                        }
                        break;

                    case VG_LITE_S16:
                        path_s16 = (int16_t *)(pathc + offset);
                        path_s16[0] = *data_s16;
                        data_s16++;
                        if(rel) {
                            cx = px + (float)path_s16[0];
                            cy = py + (float)path_s16[1];
                        }
                        else {
                            cx = (float)path_s16[0];
                            cy = (float)path_s16[1];
                        }
                        break;

                    case VG_LITE_S32:
                        path_s32 = (int32_t *)(pathc + offset);
                        path_s32[0] = *data_s32;
                        data_s32++;
                        if(rel) {
                            cx = px + (float)path_s32[0];
                            cy = py + (float)path_s32[1];
                        }
                        else {
                            cx = (float)path_s32[0];
                            cy = (float)path_s32[1];
                        }
                        break;

                    case VG_LITE_FP32:
                        pathf = (float *)(pathc + offset);
                        pathf[0] = *dataf;
                        dataf++;
                        if(rel) {
                            cx = px + (float)pathf[0];
                            cy = py + (float)pathf[1];
                        }
                        else {
                            cx = (float)pathf[0];
                            cy = (float)pathf[1];
                        }
                        break;
                }
                h_v_path = 1;
                /* Update path bounds. */
                path->bounding_box[0] = CDMIN(path->bounding_box[0], cx);
                path->bounding_box[2] = CDMAX(path->bounding_box[2], cx);
                path->bounding_box[1] = CDMIN(path->bounding_box[1], cy);
                path->bounding_box[3] = CDMAX(path->bounding_box[3], cy);
            }
            else if(cmd[i] < VLC_OP_SCCWARC) {
                /* Mark smooth path,convert it in next step. */
                if(cmd[i] <= VLC_OP_SCUBIC_REL && cmd[i] >= VLC_OP_SQUAD) {
                    smooth_path = 1;
                }
                for(j = 0; j < dataCount / 2; j++) {
                    switch(path->format) {
                        case VG_LITE_S8:
                            path_s8 = (int8_t *)(pathc + offset);
                            path_s8[j * 2] = *data_s8;
                            data_s8++;
                            path_s8[j * 2 + 1] = *data_s8;
                            data_s8++;

                            if(rel) {
                                cx = px + path_s8[j * 2];
                                cy = py + path_s8[j * 2 + 1];
                            }
                            else {
                                cx = path_s8[j * 2];
                                cy = path_s8[j * 2 + 1];
                            }
                            break;
                        case VG_LITE_S16:
                            path_s16 = (int16_t *)(pathc + offset);
                            path_s16[j * 2] = *data_s16;
                            data_s16++;
                            path_s16[j * 2 + 1] = *data_s16;
                            data_s16++;

                            if(rel) {
                                cx = px + path_s16[j * 2];
                                cy = py + path_s16[j * 2 + 1];
                            }
                            else {
                                cx = path_s16[j * 2];
                                cy = path_s16[j * 2 + 1];
                            }
                            break;
                        case VG_LITE_S32:
                            path_s32 = (int32_t *)(pathc + offset);
                            path_s32[j * 2] = *data_s32;
                            data_s32++;
                            path_s32[j * 2 + 1] = *data_s32;
                            data_s32++;

                            if(rel) {
                                cx = px + path_s32[j * 2];
                                cy = py + path_s32[j * 2 + 1];
                            }
                            else {
                                cx = (float)path_s32[j * 2];
                                cy = (float)path_s32[j * 2 + 1];
                            }
                            break;
                        case VG_LITE_FP32:
                            pathf = (float *)(pathc + offset);
                            pathf[j * 2] = *dataf;
                            dataf++;
                            pathf[j * 2 + 1] = *dataf;
                            dataf++;

                            if(rel) {
                                cx = px + pathf[j * 2];
                                cy = py + pathf[j * 2 + 1];
                            }
                            else {
                                cx = pathf[j * 2];
                                cy = pathf[j * 2 + 1];
                            }
                            break;

                        default:
                            return VG_LITE_INVALID_ARGUMENT;
                    }
                    if(cmd[i] <= VLC_OP_LINE_REL && cmd[i] >= VLC_OP_MOVE) {
                        /* Update move to and line path bounds. */
                        path->bounding_box[0] = CDMIN(path->bounding_box[0], cx);
                        path->bounding_box[2] = CDMAX(path->bounding_box[2], cx);
                        path->bounding_box[1] = CDMIN(path->bounding_box[1], cy);
                        path->bounding_box[3] = CDMAX(path->bounding_box[3], cy);
                    }

                }
            }
#if gcFEATURE_VG_ARC_PATH
            else {
                arc_path = 1;
                switch(path->format) {
                    case VG_LITE_S8:
                        path_s8 = (int8_t *)(pathc + offset);
                        path_s8[0] = *data_s8;
                        data_s8++;
                        path_s8[1] = *data_s8;
                        data_s8++;
                        path_s8[2] = *data_s8;
                        data_s8++;
                        path_s8[3] = *data_s8;
                        data_s8++;
                        path_s8[4] = *data_s8;
                        data_s8++;

                        if(rel) {
                            cx = px + path_s8[3];
                            cy = py + path_s8[4];
                        }
                        else {
                            cx = path_s8[3];
                            cy = path_s8[4];
                        }
                        /* Update path bounds. */
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx + 2 * path_s8[0], cy + 2 * path_s8[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px + 2 * path_s8[1], py + 2 * path_s8[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx - 2 * path_s8[0], cy - 2 * path_s8[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px - 2 * path_s8[1], py - 2 * path_s8[1]);
                        break;

                    case VG_LITE_S16:
                        path_s16 = (int16_t *)(pathc + offset);
                        path_s16[0] = *data_s16;
                        data_s16++;
                        path_s16[1] = *data_s16;
                        data_s16++;
                        path_s16[2] = *data_s16;
                        data_s16++;
                        path_s16[3] = *data_s16;
                        data_s16++;
                        path_s16[4] = *data_s16;
                        data_s16++;

                        if(rel) {
                            cx = px + path_s16[3];
                            cy = py + path_s16[4];
                        }
                        else {
                            cx = path_s16[3];
                            cy = path_s16[4];
                        }
                        /* Update path bounds. */
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx + 2 * path_s16[0], cy + 2 * path_s16[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px + 2 * path_s16[1], py + 2 * path_s16[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx - 2 * path_s16[0], cy - 2 * path_s16[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px - 2 * path_s16[1], py - 2 * path_s16[1]);
                        break;

                    case VG_LITE_S32:
                        path_s32 = (int32_t *)(pathc + offset);
                        path_s32[0] = *data_s32;
                        data_s32++;
                        path_s32[1] = *data_s32;
                        data_s32++;
                        path_s32[2] = *data_s32;
                        data_s32++;
                        path_s32[3] = *data_s32;
                        data_s32++;
                        path_s32[4] = *data_s32;
                        data_s32++;

                        if(rel) {
                            cx = px + path_s32[3];
                            cy = py + path_s32[4];
                        }
                        else {
                            cx = (float)path_s32[3];
                            cy = (float)path_s32[4];
                        }
                        /* Update path bounds. */
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx + 2 * path_s32[0], cy + 2 * path_s32[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px + 2 * path_s32[1], py + 2 * path_s32[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx - 2 * path_s32[0], cy - 2 * path_s32[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px - 2 * path_s32[1], py - 2 * path_s32[1]);
                        break;

                    case VG_LITE_FP32:
                        pathf = (float *)(pathc + offset);
                        pathf[0] = *dataf;
                        dataf++;
                        pathf[1] = *dataf;
                        dataf++;
                        pathf[2] = *dataf;
                        dataf++;
                        pathf[3] = *dataf;
                        dataf++;
                        pathf[4] = *dataf;
                        dataf++;

                        if(rel) {
                            cx = px + pathf[3];
                            cy = py + pathf[4];
                        }
                        else {
                            cx = pathf[3];
                            cy = pathf[4];
                        }
                        /* Update path bounds. */
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx + 2 * pathf[0], cy + 2 * pathf[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px + 2 * pathf[1], py + 2 * pathf[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           cx - 2 * pathf[0], cy - 2 * pathf[1]);
                        compute_pathbounds(&path->bounding_box[0], &path->bounding_box[1], &path->bounding_box[2], &path->bounding_box[3],
                                           px - 2 * pathf[1], py - 2 * pathf[1]);
                        break;
                }

            }
#endif
            px = cx;
            py = cy;

            offset += dataCount * data_size;
        }
    }
    if(cmd[seg_count - 1] == VLC_OP_END
#if gcFEATURE_VG_ARC_PATH
       || (cmd[seg_count - 1] == VLC_OP_CLOSE  && (arc_path | h_v_path | smooth_path))
#endif
      ) {
        path->path_length = offset;
    }
    else {
        path->path_length = offset + data_size;
        path->add_end = 1;
        ((uint8_t *)(path->path))[offset] = 0;
    }

#if gcFEATURE_VG_ARC_PATH
    if(arc_path | h_v_path | smooth_path) {
        error = vg_lite_init_arc_path(path,
                                      path->format,
                                      path->quality,
                                      path->path_length,
                                      path->path,
                                      path->bounding_box[0], path->bounding_box[1],
                                      path->bounding_box[2], path->bounding_box[3]);
    }
#endif
    s_context.path_lastX = cx;
    s_context.path_lastY = cy;
    return error;
}

#if (CHIPID==0x355 || CHIPID==0x255) /* GC355/GC255 vg_lite_draw functions */

#define UPDATE_BOUNDING_BOX(bbx, point)                                 \
    do {                                                                \
        if ((point).x < (bbx).x) {                                      \
            (bbx).width += (bbx).x - (point).x;                         \
            (bbx).x = (point).x;                                        \
        }                                                               \
        if ((point).y < (bbx).y) {                                      \
            (bbx).height += (bbx).y - (point).y;                        \
            (bbx).y = (point).y;                                        \
        }                                                               \
        if ((point).x > (bbx).x + (bbx).width)                          \
            (bbx).width = (point).x - (bbx).x;                          \
        if ((point).y > (bbx).y + (bbx).height)                         \
            (bbx).height = (point).y - (bbx).y;                         \
    } while(0)

static vg_lite_error_t transform_bounding_box(vg_lite_rectangle_t * in_bbx,
                                              vg_lite_matrix_t * matrix,
                                              vg_lite_rectangle_t * clip,
                                              vg_lite_rectangle_t * out_bbx,
                                              vg_lite_point_t * origin)
{
    vg_lite_point_t temp;

    memset(out_bbx, 0, sizeof(vg_lite_rectangle_t));

    /* Transform image point (0, 0). */
    if(!transform(&temp, 0.0f, 0.0f, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    out_bbx->x = temp.x;
    out_bbx->y = temp.y;

    /* Provide position of the new origin to the caller if requested. */
    if(origin != NULL) {
        origin->x = temp.x;
        origin->y = temp.y;
    }

    /* Transform image point (0, height). */
    if(!transform(&temp, 0.0f, (vg_lite_float_t)in_bbx->height, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    UPDATE_BOUNDING_BOX(*out_bbx, temp);

    /* Transform image point (width, height). */
    if(!transform(&temp, (vg_lite_float_t)in_bbx->width, (vg_lite_float_t)in_bbx->height, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    UPDATE_BOUNDING_BOX(*out_bbx, temp);

    /* Transform image point (width, 0). */
    if(!transform(&temp, (vg_lite_float_t)in_bbx->width, 0.0f, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    UPDATE_BOUNDING_BOX(*out_bbx, temp);

    /* Clip is required */
    if(clip) {
        out_bbx->x = MAX(out_bbx->x, clip->x);
        out_bbx->y = MAX(out_bbx->y, clip->y);
        out_bbx->width = MIN((out_bbx->x + out_bbx->width), (clip->x + clip->width)) - out_bbx->x;
        out_bbx->height = MIN((out_bbx->y + out_bbx->height), (clip->y + clip->height)) - out_bbx->y;
    }

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t set_interpolation_steps(vg_lite_buffer_t * target,
                                               vg_lite_int32_t s_width,
                                               vg_lite_int32_t s_height,
                                               vg_lite_matrix_t * matrix)
{
    vg_lite_matrix_t    im;
    vg_lite_rectangle_t src_bbx, bounding_box, clip;
    vg_lite_float_t     xs[3], ys[3], cs[3];
    vg_lite_error_t     error = VG_LITE_SUCCESS;
    float               dx = 0.0f, dy = 0.0f;

#define ERR_LIMIT   0.0000610351562f

    /* Get bounding box. */
    memset(&src_bbx, 0, sizeof(vg_lite_rectangle_t));
    memset(&clip, 0, sizeof(vg_lite_rectangle_t));
    src_bbx.width       = (int32_t)s_width;
    src_bbx.height      = (int32_t)s_height;

    if(s_context.scissor_set) {
        clip.x = s_context.scissor[0];
        clip.y = s_context.scissor[1];
        clip.width  = s_context.scissor[2];
        clip.height = s_context.scissor[3];
    }
    else {
        clip.x = clip.y = 0;
        clip.width  = s_context.rtbuffer->width;
        clip.height = s_context.rtbuffer->height;
    }
    transform_bounding_box(&src_bbx, matrix, &clip, &bounding_box, NULL);
    /* Compute inverse matrix. */
    if(!inverse(&im, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    /* Compute interpolation steps. */
    /* X step */
    xs[0] = im.m[0][0] / s_width;
    xs[1] = im.m[1][0] / s_height;
    xs[2] = im.m[2][0];
    /* Y step */
    ys[0] = im.m[0][1] / s_width;
    ys[1] = im.m[1][1] / s_height;
    ys[2] = im.m[2][1];
    /* C step 2 */
    cs[2] = 0.5f * (im.m[2][0] + im.m[2][1]) + im.m[2][2];

    /* C step 0, 1*/
    cs[0] = (0.5f * (im.m[0][0] + im.m[0][1]) + im.m[0][2] + dx) / s_width;
    cs[1] = (0.5f * (im.m[1][0] + im.m[1][1]) + im.m[1][2] + dy) / s_height;
    /* Set command buffer */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *)&cs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *)&cs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *)&cs[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *)&xs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *)&xs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *)&xs[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *)&ys[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *)&ys[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *)&ys[2]));

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t set_interpolation_steps_draw_paint(vg_lite_buffer_t * target,
                                                          vg_lite_int32_t s_width,
                                                          vg_lite_int32_t s_height,
                                                          vg_lite_matrix_t * matrix)
{
    vg_lite_matrix_t    im;
    vg_lite_rectangle_t src_bbx, bounding_box, clip;
    vg_lite_float_t     xs[3], ys[3], cs[3];
    vg_lite_error_t     error = VG_LITE_SUCCESS;
    float               dx = 0.0f, dy = 0.0f;

#define ERR_LIMIT   0.0000610351562f

    /* Get bounding box. */
    memset(&src_bbx, 0, sizeof(vg_lite_rectangle_t));
    memset(&clip, 0, sizeof(vg_lite_rectangle_t));
    src_bbx.width = (int32_t)s_width;
    src_bbx.height = (int32_t)s_height;

    if(s_context.scissor_set) {
        clip.x = s_context.scissor[0];
        clip.y = s_context.scissor[1];
        clip.width = s_context.scissor[2];
        clip.height = s_context.scissor[3];
    }
    else {
        clip.x = clip.y = 0;
        clip.width = s_context.rtbuffer->width;
        clip.height = s_context.rtbuffer->height;
    }
    transform_bounding_box(&src_bbx, matrix, &clip, &bounding_box, NULL);
    /* Compute inverse matrix. */
    if(!inverse(&im, matrix))
        return VG_LITE_INVALID_ARGUMENT;
    /* Compute interpolation steps. */
    /* X step */
    xs[0] = im.m[0][0] / s_width;
    xs[1] = im.m[1][0] / s_height;
    xs[2] = im.m[2][0];
    /* Y step */
    ys[0] = im.m[0][1] / s_width;
    ys[1] = im.m[1][1] / s_height;
    ys[2] = im.m[2][1];
    /* C step 2 */
    cs[2] = 0.5f * (im.m[2][0] + im.m[2][1]) + im.m[2][2];

    /* C step 0, 1*/
    cs[0] = (0.5f * (im.m[0][0] + im.m[0][1]) + im.m[0][2] + dx) / s_width;
    cs[1] = (0.5f * (im.m[1][0] + im.m[1][1]) + im.m[1][2] + dy) / s_height;

    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A04, (void *)&cs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A05, (void *)&cs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A06, (void *)&xs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A07, (void *)&xs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A08, (void *)&ys[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A09, (void *)&ys[1]));
    /* Set command buffer */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *)&cs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *)&cs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *)&cs[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *)&xs[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *)&xs[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *)&xs[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *)&ys[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *)&ys[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *)&ys[2]));

    return VG_LITE_SUCCESS;
}

/* GC355/GC255 vg_lite_draw API implementation
 */
vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                             vg_lite_path_t * path,
                             vg_lite_fill_t fill_rule,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color)
{
    uint32_t blend_mode;
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;
    vg_lite_error_t error;
    int32_t dst_align_width;
    uint32_t mul, div, align;
    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int x, y, width, height;
    uint8_t ts_is_fullscreen = 0;
    uint32_t in_premult = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
#if(CHIPID == 0x355)
    uint8_t * path_re = NULL;
    uint32_t index = 0;
#endif

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw %p %p %d %p %d 0x%08X\n", target, path, fill_rule, matrix, blend, color);
    VGLITE_LOG("    path_type %d, path_length %d, stroke_size %d\n", path->path_type, path->path_length, path->stroke_size);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#if (CHIPID == 0x355)
    if(target->format == VG_LITE_L8 || target->format == VG_LITE_YUYV ||
       target->format == VG_LITE_BGRA2222 || target->format == VG_LITE_RGBA2222 ||
       target->format == VG_LITE_ABGR2222 || target->format == VG_LITE_ARGB2222) {
        printf("Target format: 0x%x is not supported.\n", target->format);
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!matrix) {
        matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_FALSE);
#endif

    /*blend input into context*/
    s_context.blend_mode = blend;

    /* Adjust premultiply setting according to openvg condition */
    target->apply_premult = 0;
    premul_flag = (s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE);
    if(target->premultiplied == 0 && premul_flag == 0) {
        in_premult = 0x10000000;
        target->apply_premult = 1;
    }
    else if((target->premultiplied == 1) ||
            (target->premultiplied == 0 && premul_flag == 1)) {
        in_premult = 0x00000000;
    }
    if(blend == VG_LITE_BLEND_NORMAL_LVGL) {
        in_premult = 0x00000000;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;
    get_format_bytes(target->format, &mul, &div, &align);
    dst_align_width = target->stride * div / mul;
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((dst_align_width <= width) && (target->height <= height)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = dst_align_width;
        point_max.y = target->height;
    }

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        if(point_min.x < 0) point_min.x = 0;
        if(point_min.y < 0) point_min.y = 0;
        if(point_max.x > dst_align_width) point_max.x = dst_align_width;
        if(point_max.y > target->height) point_max.y = target->height;

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.L2_size ? s_context.tessbuf.L2_size : s_context.tessbuf.L1_size;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    in_premult | s_context.capabilities.cap.tiled | blend_mode | s_context.enable_mask | s_context.scissor_enable |
                                    s_context.color_transform | s_context.matrix_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    /* Program tessellation control: for TS module. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &matrix->m[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &matrix->m[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &matrix->m[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &matrix->m[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &matrix->m[1][2]));

    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO ||
       path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
                }
            }
        }
    }
    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));

                if(VLM_PATH_STROKE_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->stroke->uploaded.address, path->stroke->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
                }
            }
        }
    }
    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));

    return error;
}

/* GC355/GC255 vg_lite_draw_pattern API implementation
 */
vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t * target,
                                     vg_lite_path_t * path,
                                     vg_lite_fill_t fill_rule,
                                     vg_lite_matrix_t * path_matrix,
                                     vg_lite_buffer_t * source,
                                     vg_lite_matrix_t * pattern_matrix,
                                     vg_lite_blend_t blend,
                                     vg_lite_pattern_mode_t pattern_mode,
                                     vg_lite_color_t  pattern_color,
                                     vg_lite_color_t  color,
                                     vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t imageMode;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    int32_t dst_align_width;
    uint32_t mul, div, align;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t matrix;
    uint32_t pattern_tile = 0;
    uint32_t transparency_mode = 0;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int x, y, width, height;
    uint8_t ts_is_fullscreen = 0;
    uint32_t in_premult = 0;
    uint32_t src_premultiply_enable = 0;
    uint32_t paintType = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
    uint8_t  lvgl_sw_blend = 0;

#if(CHIPID == 0X355)
    uint8_t * path_re = NULL;
    uint32_t index = 0;
#endif
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_pattern %p %p %d %p %p %p %d %d 0x%08X %d\n",
               target, path, fill_rule, path_matrix, source, pattern_matrix, blend, pattern_mode, pattern_color, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(source->format == VG_LITE_A4 || source->format == VG_LITE_A8) {
        return VG_LITE_NOT_SUPPORT;
    }
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#if (CHIPID == 0x355)
    if(target->format == VG_LITE_L8 || target->format == VG_LITE_YUYV ||
       target->format == VG_LITE_BGRA2222 || target->format == VG_LITE_RGBA2222 ||
       target->format == VG_LITE_ABGR2222 || target->format == VG_LITE_ARGB2222) {
        printf("Target format: 0x%x is not supported.\n", target->format);
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#endif /* gcFEATURE_VG_ERROR_CHECK */

#if !gcFEATURE_VG_LVGL_SUPPORT
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        if(!source->lvgl_buffer) {
            source->lvgl_buffer = (vg_lite_buffer_t *)vg_lite_os_malloc(sizeof(vg_lite_buffer_t));
            *source->lvgl_buffer = *source;
            source->lvgl_buffer->lvgl_buffer = NULL;
            vg_lite_allocate(source->lvgl_buffer);
        }
        /* Make sure render target is up to date before reading RT. */
        vg_lite_finish();
        setup_lvgl_image(target, source, source->lvgl_buffer, blend);
        blend = VG_LITE_BLEND_SRC_OVER;
        lvgl_sw_blend = 1;
    }
#endif

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }
    if(!pattern_matrix) {
        pattern_matrix = &identity_mtx;
    }

    /* Work on pattern states. */
    matrix = *pattern_matrix;
    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        matrix.m[2][0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][2] = 1;
        source->image_mode = VG_LITE_NONE_IMAGE_MODE;
    }

#if gcFEATURE_VG_GAMMA
    save_st_gamma_src_dest(source, target);
#endif

    /*blend input into context*/
    s_context.blend_mode = blend;
    in_premult = 0x00000000;

    /* Adjust premultiply setting according to openvg condition */
    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
        in_premult = 0x00000000;
    }
    if(blend == VG_LITE_BLEND_NORMAL_LVGL) {
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;
    get_format_bytes(target->format, &mul, &div, &align);
    dst_align_width = target->stride * div / mul;
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((dst_align_width <= width) && (target->height <= height)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = dst_align_width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE) ?
                0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    if(pattern_mode == VG_LITE_PATTERN_COLOR) {
        uint8_t a, r, g, b;
        pattern_tile = 0;
        a = pattern_color >> 24;
        r = pattern_color >> 16;
        g = pattern_color >> 8;
        b = pattern_color;
        pattern_color = (a << 24) | (b << 16) | (g << 8) | r;
    }
    else if(pattern_mode == VG_LITE_PATTERN_PAD) {
        pattern_tile = 0x1000;
    }
#if gcFEATURE_VG_IM_REPEAT_REFLECT
    else if(pattern_mode == VG_LITE_PATTERN_REPEAT) {
        pattern_tile = 0x2000;
    }
    else if(pattern_mode == VG_LITE_PATTERN_REFLECT) {
        pattern_tile = 0x3000;
    }
#endif
    else {
        return VG_LITE_INVALID_ARGUMENT;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        VG_LITE_RETURN_ERROR(set_interpolation_steps_draw_paint(target, source->width, source->height, &matrix));
        /* enable pre-multiplied in image unit */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                        filter_mode | pattern_tile | conversion | src_premultiply_enable));

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, pattern_color));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, source->stride | tiled_source));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width | (source->height << 16)));
    }
    else {
        VG_LITE_RETURN_ERROR(set_interpolation_steps(target, source->width, source->height, &matrix));
        /* enable pre-multiplied in image unit */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25, convert_source_format(source->format) |
                                        filter_mode | pattern_tile | conversion | src_premultiply_enable));

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, pattern_color));

#if !gcFEATURE_VG_LVGL_SUPPORT
        if(lvgl_sw_blend) {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->lvgl_buffer->address));
        }
        else
#endif
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->address));

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source->stride | tiled_source));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, 0));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, source->width | (source->height << 16)));
    }

    /* Work on path states. */
    matrix = *path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], &matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, dst_align_width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.L2_size ? s_context.tessbuf.L2_size : s_context.tessbuf.L1_size;

    /* Setup the command buffer. */
    /* Program color register. */
    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        paintType = 1 << 24 | 1 << 25;
    }
    /* enable pre-multiplied from VG to VGPE */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x2 | in_premult | paintType  | s_context.capabilities.cap.tiled | imageMode | blend_mode | transparency_mode |
                                    s_context.enable_mask | s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000400 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &matrix.m[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &matrix.m[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &matrix.m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &matrix.m[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &matrix.m[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &matrix.m[1][2]));

    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO ||
       path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
                }
            }
        }
    }
    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));

                if(VLM_PATH_STROKE_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->stroke->uploaded.address, path->stroke->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
                }
            }
        }
    }

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0, (source->stride) * (source->height));

    return error;
}

/* GC355/GC255 vg_lite_draw_linear_grad API implementation
 */
vg_lite_error_t vg_lite_draw_linear_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_linear_gradient_ext_t * grad,
                                         vg_lite_color_t paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t image_mode;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    int32_t dst_align_width;
    uint32_t mul, div, align;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_buffer_t * source = &grad->image;
    vg_lite_matrix_t * matrix = &grad->matrix;
    uint32_t linear_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t in_premult = 0;
    uint32_t src_premultiply_enable = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
    void * data;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int x, y, width, height;
    uint8_t ts_is_fullscreen = 0;

    vg_lite_float_t dx, dy, dxdx_dydy;
    vg_lite_float_t lg_step_x_lin, lg_step_y_lin, lg_constant_lin;

#if(CHIPID == 0X355)
    uint8_t * path_re = NULL;
    uint32_t index = 0;
#endif
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_linear_grad %p %p %d %p %p 0x%08X %d %d\n",
               target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_LVGL_SUPPORT
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(source->format == VG_LITE_A4 || source->format == VG_LITE_A8) {
        return VG_LITE_NOT_SUPPORT;
    }
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#if (CHIPID == 0x355)
    if(target->format == VG_LITE_L8 || target->format == VG_LITE_YUYV ||
       target->format == VG_LITE_BGRA2222 || target->format == VG_LITE_RGBA2222 ||
       target->format == VG_LITE_ABGR2222 || target->format == VG_LITE_ARGB2222) {
        printf("Target format: 0x%x is not supported.\n", target->format);
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_TRUE);
#endif

    /*blend input into context*/
    s_context.blend_mode = blend;

    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
#if (CHIPID==0x255)
        src_premultiply_enable = 0x00000000;
#endif
        in_premult = 0x00000000;
    }
    if(blend == VG_LITE_BLEND_NORMAL_LVGL) {
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;
    get_format_bytes(target->format, &mul, &div, &align);
    dst_align_width = target->stride * div / mul;
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((dst_align_width <= width) && (target->height <= height)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = dst_align_width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    image_mode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE) ?
                 0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    switch(grad->spread_mode) {
        case VG_LITE_GRADIENT_SPREAD_FILL:
            linear_tile = 0x0;
            break;

        case VG_LITE_GRADIENT_SPREAD_PAD:
            linear_tile = 0x1000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REPEAT:
            linear_tile = 0x2000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REFLECT:
            linear_tile = 0x3000;
            break;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_FILL) {
        uint8_t a, r, g, b;
        a = paint_color >> 24;
        r = paint_color >> 16;
        g = paint_color >> 8;
        b = paint_color;
        paint_color = (a << 24) | (b << 16) | (g << 8) | r;
    }

    /* compute radial gradient paremeters */

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

    dx = grad->linear_grad.X1 - grad->linear_grad.X0;
    dy = grad->linear_grad.Y1 - grad->linear_grad.Y0;
    dxdx_dydy = dx * dx + dy * dy;

    /*
    **      dx (T(x) - x0) + dy (T(y) - y0)
    **  g = -------------------------------
    **                dx^2 + dy^2
    **
    **  where
    **
    **      dx := x1 - x0
    **      dy := y1 - y1
    **      T(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **            = x m00 + y m01 + 0.5 (m00 + m01) + m02
    **      T(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **            = x m10 + y m11 + 0.5 (m10 + m11) + m12.
    **
    **  We can factor the top line into:
    **
    **      = dx (x m00 + y m01 + 0.5 (m00 + m01) + m02 - x0)
    **      + dy (x m10 + y m11 + 0.5 (m10 + m11) + m12 - y0)
    **
    **      = x (dx m00 + dy m10)
    **      + y (dx m01 + dy m11)
    **      + dx (0.5 (m00 + m01) + m02 - x0)
    **      + dy (0.5 (m10 + m11) + m12 - y0).
    */

    lg_step_x_lin
        = (dx * MAT(&inverse_matrix, 0, 0) + dy * MAT(&inverse_matrix, 1, 0))
          / dxdx_dydy;

    lg_step_y_lin
        = (dx * MAT(&inverse_matrix, 0, 1) + dy * MAT(&inverse_matrix, 1, 1))
          / dxdx_dydy;

    lg_constant_lin =
        (
            (
                0.5f * (MAT(&inverse_matrix, 0, 0) + MAT(&inverse_matrix, 0, 1))
                + MAT(&inverse_matrix, 0, 2) - grad->linear_grad.X0
            ) * dx

            +

            (
                0.5f * (MAT(&inverse_matrix, 1, 0) + MAT(&inverse_matrix, 1, 1))
                + MAT(&inverse_matrix, 1, 2) - grad->linear_grad.Y0
            ) * dy
        )
        / dxdx_dydy;

    /* Setup the command buffer. */

    /* linear gradient parameters*/
    data = &lg_constant_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &lg_step_x_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &lg_step_y_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));

    VG_LITE_RETURN_ERROR(set_interpolation_steps(target, source->width, source->height, matrix));

    /* enable pre-multiplied in image unit */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter_mode | linear_tile | conversion | src_premultiply_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width));

    /* Work on path states. */
    matrix = path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, dst_align_width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.L2_size ? s_context.tessbuf.L2_size : s_context.tessbuf.L1_size;

    /* Setup the command buffer. */
    /* Program color register. */

    /* enable pre-multiplied from VG to VGPE */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x01000002 | s_context.capabilities.cap.tiled | in_premult | image_mode | blend_mode | transparency_mode |
                                    s_context.enable_mask | s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000400 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &matrix->m[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &matrix->m[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &matrix->m[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &matrix->m[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &matrix->m[1][2]));

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
                }
            }
        }
    }
    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));

                if(VLM_PATH_STROKE_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->stroke->uploaded.address, path->stroke->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
                }
            }
        }
    }

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));

    return error;
}

/* GC355/GC255 vg_lite_draw_radial_grad API implementation
 */
vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_radial_gradient_t * grad,
                                         vg_lite_color_t paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t imageMode;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    int32_t dst_align_width;
    uint32_t mul, div, align;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_buffer_t * source = &grad->image;
    vg_lite_matrix_t * matrix = &grad->matrix;
    uint32_t rad_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t in_premult = 0;
    uint32_t src_premultiply_enable = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
    void * data;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int x, y, width, height;
    uint8_t ts_is_fullscreen = 0;

    vg_lite_float_t radius;

    vg_lite_float_t centerX, centerY;
    vg_lite_float_t focalX, focalY;
    vg_lite_float_t fx, fy;
    vg_lite_float_t fxfy_2;
    vg_lite_float_t radius2;
    vg_lite_float_t r2_fx2, r2_fy2;
    vg_lite_float_t r2_fx2_2, r2_fy2_2;
    vg_lite_float_t r2_fx2_fy2;
    vg_lite_float_t r2_fx2_fy2sq;
    vg_lite_float_t cx, cy;

    vg_lite_float_t rgConstantLin, rgStepXLin, rgStepYLin;
    vg_lite_float_t rgConstantRad, rgStepXRad, rgStepYRad;
    vg_lite_float_t rgStepXXRad, rgStepYYRad, rgStepXYRad;
#if(CHIPID == 0X355)
    uint8_t * path_re = NULL;
    uint32_t index = 0;
#endif
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_radial_grad %p %p %d %p %p 0x%08X %d %d\n",
               target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_LVGL_SUPPORT
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(source->format == VG_LITE_A4 || source->format == VG_LITE_A8) {
        return VG_LITE_NOT_SUPPORT;
    }
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#if (CHIPID == 0x355)
    if(target->format == VG_LITE_L8 || target->format == VG_LITE_YUYV ||
       target->format == VG_LITE_BGRA2222 || target->format == VG_LITE_RGBA2222 ||
       target->format == VG_LITE_ABGR2222 || target->format == VG_LITE_ARGB2222) {
        printf("Target format: 0x%x is not supported.\n", target->format);
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    radius = grad->radial_grad.r;
    if(radius < 0) {
        return VG_LITE_INVALID_ARGUMENT;
    }
    VG_LITE_RETURN_ERROR(check_compress(source->format, source->compress_mode, source->tiled, source->width,
                                        source->height));
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_TRUE);
#endif

    /*blend input into context*/
    s_context.blend_mode = blend;

    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
#if (CHIPID==0x255)
        src_premultiply_enable = 0x00000000;
#endif
        in_premult = 0x00000000;
    }
    if(blend == VG_LITE_BLEND_NORMAL_LVGL) {
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;
    get_format_bytes(target->format, &mul, &div, &align);
    dst_align_width = target->stride * div / mul;
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((dst_align_width <= width) && (target->height <= height)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = dst_align_width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE) ?
                0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    switch(grad->spread_mode) {
        case VG_LITE_GRADIENT_SPREAD_FILL:
            rad_tile = 0x0;
            break;

        case VG_LITE_GRADIENT_SPREAD_PAD:
            rad_tile = 0x1000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REPEAT:
            rad_tile = 0x2000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REFLECT:
            rad_tile = 0x3000;
            break;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_FILL) {
        uint8_t a, r, g, b;
        a = paint_color >> 24;
        r = paint_color >> 16;
        g = paint_color >> 8;
        b = paint_color;
        paint_color = (a << 24) | (b << 16) | (g << 8) | r;
    }

    /* compute radial gradient paremeters */

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

    /* Make shortcuts to the gradient information. */
    centerX = grad->radial_grad.cx;
    centerY = grad->radial_grad.cy;
    focalX  = grad->radial_grad.fx;
    focalY  = grad->radial_grad.fy;

    /* Compute constants of the equation. */
    fx           = focalX - centerX;
    fy           = focalY - centerY;
    radius2      = radius * radius;
    if(fx * fx + fy * fy > radius2) {
        /* If the focal point is outside the circle, let's move it
            to inside the circle. Per vg11 spec pg125 "If (fx, fy) lies outside ...
            For here, we set it at 0.9 ratio to the center.
        */
        vg_lite_float_t fr = (vg_lite_float_t)sqrt(fx * fx + fy * fy);
        fx = radius * fx / fr * 0.9f;
        fy = radius * fy / fr * 0.9f;
        focalX = grad->radial_grad.fx + fx;
        focalY = grad->radial_grad.fy + fy;
    }

    fxfy_2       = 2.0f * fx * fy;
    r2_fx2       = radius2 - fx * fx;
    r2_fy2       = radius2 - fy * fy;
    r2_fx2_2     = 2.0f * r2_fx2;
    r2_fy2_2     = 2.0f * r2_fy2;
    r2_fx2_fy2   = r2_fx2  - fy * fy;
    r2_fx2_fy2sq = r2_fx2_fy2 * r2_fx2_fy2;

    /*                        _____________________________________
    **      dx fx + dy fy + \/r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = -------------------------------------------------------
    **                         r^2 - fx^2 - fy^2
    **
    **  Where
    **
    **      dx := F(x) - focalX
    **      dy := F(y) - focalY
    **      fx := focalX - centerX
    **      fy := focalX - centerY
    **
    **  and
    **
    **      F(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **      F(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **
    **  So, dx can be factored into
    **
    **      dx = (x + 0.5) m00 + (y + 0.5) m01 + m02 - focalX
    **         = x m00 + y m01 + 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **         = x m00 + y m01 + cx
    **
    **  where
    **
    **      cx := 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **  The same way we can factor dy into
    **
    **      dy = x m10 + y m11 + cy
    **
    **  where
    **
    **      cy := 0.5 m10 + 0.5 m11 + m12 - focalY.
    **
    **  Now we can rewrite g as
    **                               ______________________________________
    **        dx fx + dy fy         / r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = ----------------- + \  /  -------------------------------------
    **      r^2 - fx^2 - fy^2    \/           (r^2 - fx^2 - fy^2)^2
    **               ____
    **    = gLin + \/gRad
    **
    **  where
    **
    **                dx fx + dy fy
    **      gLin := -----------------
    **              r^2 - fx^2 - fy^2
    **
    **              r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **      gRad := -------------------------------------
    **                      (r^2 - fx^2 - fy^2)^2
    */

    cx
        = 0.5f * (MAT(&inverse_matrix, 0, 0) + MAT(&inverse_matrix, 0, 1))
          + MAT(&inverse_matrix, 0, 2)
          - focalX;

    cy
        = 0.5f * (MAT(&inverse_matrix, 1, 0) + MAT(&inverse_matrix, 1, 1))
          + MAT(&inverse_matrix, 1, 2)
          - focalY;

    /*
    **            dx fx + dy fy
    **  gLin := -----------------
    **          r^2 - fx^2 - fy^2
    **
    **  We can factor the top half into
    **
    **      = (x m00 + y m01 + cx) fx + (x m10 + y m11 + cy) fy
    **
    **      = x (m00 fx + m10 fy)
    **      + y (m01 fx + m11 fy)
    **      + cx fx + cy fy.
    */

    rgStepXLin
        = (MAT(&inverse_matrix, 0, 0) * fx + MAT(&inverse_matrix, 1, 0) * fy)
          / r2_fx2_fy2;

    rgStepYLin
        = (MAT(&inverse_matrix, 0, 1) * fx + MAT(&inverse_matrix, 1, 1) * fy)
          / r2_fx2_fy2;

    rgConstantLin = (cx * fx  + cy * fy) / r2_fx2_fy2;

    /*
    **          r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  gRad := -------------------------------------
    **                  (r^2 - fx^2 - fy^2)^2
    **
    **          r^2 (dx^2 + dy^2) - dx^2 fy^2 - dy^2 fx^2 + 2 dx dy fx fy
    **       := ---------------------------------------------------------
    **                            (r^2 - fx^2 - fy^2)^2
    **
    **          dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **       := -----------------------------------------------------
    **                          (r^2 - fx^2 - fy^2)^2
    **
    **  First, lets factor dx^2 into
    **
    **      dx^2 = (x m00 + y m01 + cx)^2
    **           = x^2 m00^2 + y^2 m01^2 + 2 x y m00 m01
    **           + 2 x m00 cx + 2 y m01 cx + cx^2
    **
    **           = x^2 (m00^2)
    **           + y^2 (m01^2)
    **           + x y (2 m00 m01)
    **           + x (2 m00 cx)
    **           + y (2 m01 cx)
    **           + cx^2.
    **
    **  The same can be done for dy^2:
    **
    **      dy^2 = x^2 (m10^2)
    **           + y^2 (m11^2)
    **           + x y (2 m10 m11)
    **           + x (2 m10 cy)
    **           + y (2 m11 cy)
    **           + cy^2.
    **
    **  Let's also factor dx dy into
    **
    **      dx dy = (x m00 + y m01 + cx) (x m10 + y m11 + cy)
    **            = x^2 m00 m10 + y^2 m01 m11 + x y m00 m11 + x y m01 m10
    **            + x m00 cy + x m10 cx + y m01 cy + y m11 cx + cx cy
    **
    **            = x^2 (m00 m10)
    **            + y^2 (m01 m11)
    **            + x y (m00 m11 + m01 m10)
    **            + x (m00 cy + m10 cx)
    **            + y (m01 cy + m11 cx)
    **            + cx cy.
    **
    **  Now that we have all this, lets look at the top of gRad.
    **
    **      = dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **      = x^2 m00^2 (r^2 - fy^2) + y^2 m01^2 (r^2 - fy^2)
    **      + x y 2 m00 m01 (r^2 - fy^2) + x 2 m00 cx (r^2 - fy^2)
    **      + y 2 m01 cx (r^2 - fy^2) + cx^2 (r^2 - fy^2)
    **      + x^2 m10^2 (r^2 - fx^2) + y^2 m11^2 (r^2 - fx^2)
    **      + x y 2 m10 m11 (r^2 - fx^2) + x 2 m10 cy (r^2 - fx^2)
    **      + y 2 m11 cy (r^2 - fx^2) + cy^2 (r^2 - fx^2)
    **      + x^2 m00 m10 2 fx fy + y^2 m01 m11 2 fx fy
    **      + x y (m00 m11 + m01 m10) 2 fx fy
    **      + x (m00 cy + m10 cx) 2 fx fy + y (m01 cy + m11 cx) 2 fx fy
    **      + cx cy 2 fx fy
    **
    **      = x^2 ( m00^2 (r^2 - fy^2)
    **            + m10^2 (r^2 - fx^2)
    **            + m00 m10 2 fx fy
    **            )
    **      + y^2 ( m01^2 (r^2 - fy^2)
    **            + m11^2 (r^2 - fx^2)
    **            + m01 m11 2 fx fy
    **            )
    **      + x y ( 2 m00 m01 (r^2 - fy^2)
    **            + 2 m10 m11 (r^2 - fx^2)
    **            + (m00 m11 + m01 m10) 2 fx fy
    **            )
    **      + x ( 2 m00 cx (r^2 - fy^2)
    **          + 2 m10 cy (r^2 - fx^2)
    **          + (m00 cy + m10 cx) 2 fx fy
    **          )
    **      + y ( 2 m01 cx (r^2 - fy^2)
    **          + 2 m11 cy (r^2 - fx^2)
    **          + (m01 cy + m11 cx) 2 fx fy
    **          )
    **      + cx^2 (r^2 - fy^2) + cy^2 (r^2 - fx^2) + cx cy 2 fx fy.
    */

    rgStepXXRad =
        (
            MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 0, 0) * r2_fy2
            + MAT(&inverse_matrix, 1, 0) * MAT(&inverse_matrix, 1, 0) * r2_fx2
            + MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 1, 0) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYYRad =
        (
            MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 0, 1) * r2_fy2
            + MAT(&inverse_matrix, 1, 1) * MAT(&inverse_matrix, 1, 1) * r2_fx2
            + MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 1, 1) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXYRad =
        (
            MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 0, 1) * r2_fy2_2
            + MAT(&inverse_matrix, 1, 0) * MAT(&inverse_matrix, 1, 1) * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 1, 1)
                + MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 1, 0)
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXRad =
        (
            MAT(&inverse_matrix, 0, 0) * cx * r2_fy2_2
            + MAT(&inverse_matrix, 1, 0) * cy * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 0) * cy
                + MAT(&inverse_matrix, 1, 0) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYRad =
        (
            MAT(&inverse_matrix, 0, 1) * cx * r2_fy2_2
            + MAT(&inverse_matrix, 1, 1) * cy * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 1) * cy
                + MAT(&inverse_matrix, 1, 1) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgConstantRad =
        (
            cx * cx * r2_fy2
            + cy * cy * r2_fx2
            + cx * cy * fxfy_2
        )
        / r2_fx2_fy2sq;

    /* Setup the command buffer. */
    data = &rgConstantLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &rgStepXLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &rgStepYLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));
    data = &rgConstantRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A05, *(uint32_t *) data));
    data = &rgStepXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A07, *(uint32_t *) data));
    data = &rgStepYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A09, *(uint32_t *) data));
    data = &rgStepXXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A03, *(uint32_t *) data));
    data = &rgStepYYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0A, *(uint32_t *) data));
    data = &rgStepXYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0B, *(uint32_t *) data));
    VG_LITE_RETURN_ERROR(set_interpolation_steps(target, source->width, source->height, matrix));

    /* enable pre-multiplied in image unit */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter_mode | rad_tile | conversion | src_premultiply_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width));

    /* Work on path states. */
    matrix = path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, dst_align_width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.L2_size ? s_context.tessbuf.L2_size : s_context.tessbuf.L1_size;

    /* Setup the command buffer. */
    /* Program color register. */

    /* enable pre-multiplied from VG to VGPE */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x02000002 | s_context.capabilities.cap.tiled | in_premult | imageMode | blend_mode | transparency_mode |
                                    s_context.enable_mask | s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000400 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &matrix->m[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &matrix->m[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &matrix->m[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &matrix->m[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &matrix->m[1][2]));

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO ||
       path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
                }
            }
        }
    }
    /* Setup tessellation loop. */
    if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
        for(y = point_min.y; y < point_max.y; y += height) {
            for(x = point_min.x; x < point_max.x; x += width) {
                /* Tessellate path. */
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 15));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A01, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, x | (y << 16)));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));

                if(VLM_PATH_STROKE_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->stroke->uploaded.address, path->stroke->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
                }
            }
        }
    }

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));

    return error;
}

#else /* (CHIPID==0x355 || CHIPID==0x255) */

/* GC555 vg_lite_draw API implementation
 */
vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                             vg_lite_path_t * path,
                             vg_lite_fill_t fill_rule,
                             vg_lite_matrix_t * matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_draw)(target, path, fill_rule, matrix, blend, color);
#endif

    uint32_t blend_mode;
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;
    vg_lite_error_t error;
    vg_lite_point_t point_min = { 0 }, point_max = { 0 }, temp = { 0 };
    int width, height;
    uint8_t ts_is_fullscreen = 0;
    uint32_t return_offset = 0;
    vg_lite_kernel_free_t free_memory;
    vg_lite_kernel_allocate_t memory;
    float new_matrix[6];
    float scale, bias;
    uint32_t tile_setting = 0;
    uint32_t in_premult = 0;
    uint32_t premul_flag = 0;
#if (!gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_512_PARALLEL_PATHS)
    uint32_t parallel_workpaths1 = 2;
    uint32_t parallel_workpaths2 = 2;
#endif
#if (!gcFEATURE_VG_SPLIT_PATH || !gcFEATURE_VG_PARALLEL_PATHS || !gcFEATURE_VG_512_PARALLEL_PATHS)
    int32_t y = 0;
    uint32_t par_height = 0;
    int32_t next_boundary = 0;
#endif

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw %p %p %d %p %d 0x%08X\n", target, path, fill_rule, matrix, blend, color);
    VGLITE_LOG("    path_type %d, path_length %d, stroke_size %d\n", path->path_type, path->path_length, path->stroke_size);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_24BIT
    if(target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_NEW_BLEND_MODE
    if(blend == VG_LITE_BLEND_DARKEN || blend == VG_LITE_BLEND_LIGHTEN) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!matrix) {
        matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_FALSE);
#endif
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_GLOBAL, 0xff));
    }
#endif
    /*blend input into context*/
    s_context.blend_mode = blend;

    /* Adjust premultiply setting according to openvg condition */
    target->apply_premult = 0;
    premul_flag = (s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE)
                  || (s_context.blend_mode >= VG_LITE_BLEND_NORMAL_LVGL && s_context.blend_mode <= VG_LITE_BLEND_MULTIPLY_LVGL);

    if(target->premultiplied == 0 && premul_flag == 0) {
        in_premult = 0x10000000;
        target->apply_premult = 1;
    }
    else if((target->premultiplied == 1) ||
            (target->premultiplied == 0 && premul_flag == 1)) {
        in_premult = 0x00000000;
    }
    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }

    if((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 || target->format == VG_LITE_YUY2_TILED
        || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
       && path->quality != VG_LITE_LOW) {
        path->quality = VG_LITE_LOW;
        printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    width = target->width;
    height = target->height;
    if(s_context.scissor_set) {
        width = s_context.scissor[2] - s_context.scissor[0];
        height = s_context.scissor[3] - s_context.scissor[1];
    }
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((target->width <= width) && (target->height <= height) && (!s_context.scissor_set)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        if(point_min.x < 0) point_min.x = 0;
        if(point_min.y < 0) point_min.y = 0;
        if(point_max.x > target->width) point_max.x = target->width;
        if(point_max.y > target->height) point_max.y = target->height;

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    width = point_max.x - point_min.x;
    height = point_max.y - point_min.y;
    scale = 1.0f;
    bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * scale;
    new_matrix[1] = matrix->m[0][1] * scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * scale;
    new_matrix[4] = matrix->m[1][1] * scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.tessbuf_size;
#if gcFEATURE_VG_TESSELLATION_TILED_OUT
    tile_setting = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;
#endif

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    in_premult | s_context.capabilities.cap.tiled | blend_mode | tile_setting | s_context.enable_mask |
                                    s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    /* Program tessellation control: for TS module. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *)&new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *)&new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *)&new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *)&new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *)&new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *)&new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *)&matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *)&matrix->m[1][2]));

    /* DDRLess does not support uploading path data. */
    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free_memory));
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *)memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *)memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *)memory.memory)[1] = 0;
            memcpy((uint8_t *)memory.memory + 8, path->path, path->path_length);
            ((uint32_t *)memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *)memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes = memory.bytes;
            path->path_changed = 0;
        }
    }

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0, path->uploaded.bytes);
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tessbuf.physical_addr, s_context.tessbuf.tessbuf_size);
#endif

    if(width + point_min.x > target->width) {
        width = target->width - point_min.x;
    }

#if (!gcFEATURE_VG_SPLIT_PATH || !gcFEATURE_VG_PARALLEL_PATHS || !gcFEATURE_VG_512_PARALLEL_PATHS)
    s_context.tessbuf.tess_w_h = width | (height << 16);
    if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO ||
       path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
#if !gcFEATURE_VG_PARALLEL_PATHS
        if(height <= 128)
            parallel_workpaths1 = 4;
        else
            parallel_workpaths1 = height * 128 / 4096 - 1;

        if(parallel_workpaths1 > parallel_workpaths2)
            parallel_workpaths1 = parallel_workpaths2;
#endif
        for(y = point_min.y; y < point_max.y; y += par_height) {
#if !gcFEATURE_VG_512_PARALLEL_PATHS
            next_boundary = (y + 512) & 0xfffffe00;
#elif (!gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_SPLIT_PATH)
            next_boundary = (y + 32) & 0xffffffe0;
#else
            next_boundary = (y + 16) & 0xfffffff0;
#endif
            par_height = ((next_boundary < point_max.y) ? next_boundary - y : (point_max.y - y));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                            in_premult | s_context.capabilities.cap.tiled | blend_mode | tile_setting | s_context.enable_mask |
                                            s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
            /* Program tessellation control: for TS module. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (par_height << 16)));

            if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
            }
            else {
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000101));
#if !gcFEATURE_VG_PARALLEL_PATHS
                s_context.path_counter++;
                if(parallel_workpaths1 == s_context.path_counter) {
                    VG_LITE_RETURN_ERROR(push_stall(&s_context, 7));
                    s_context.path_counter = 0;
                }
#endif
            }
        }
    }
    if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
#if !gcFEATURE_VG_PARALLEL_PATHS
        if(height <= 128)
            parallel_workpaths1 = 4;
        else
            parallel_workpaths1 = height * 128 / 4096 - 1;

        if(parallel_workpaths1 > parallel_workpaths2)
            parallel_workpaths1 = parallel_workpaths2;
#endif
        for(y = point_min.y; y < point_max.y; y += par_height) {
#if !gcFEATURE_VG_512_PARALLEL_PATHS
            next_boundary = (y + 512) & 0xfffffe00;
#elif (!gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_SPLIT_PATH)
            next_boundary = (y + 32) & 0xffffffe0;
#else
            next_boundary = (y + 16) & 0xfffffff0;
#endif
            par_height = ((next_boundary < point_max.y) ? next_boundary - y : (point_max.y - y));

            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                            in_premult | s_context.capabilities.cap.tiled | blend_mode | tile_setting | s_context.enable_mask |
                                            s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));

            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (par_height << 16)));

            if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
            }
            else {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000101));
#if !gcFEATURE_VG_PARALLEL_PATHS
                s_context.path_counter++;
                if(parallel_workpaths1 == s_context.path_counter) {
                    VG_LITE_RETURN_ERROR(push_stall(&s_context, 7));
                    s_context.path_counter = 0;
                }
#endif
            }
        }
    }
#else
    {
        s_context.tessbuf.tess_w_h = width | (height << 16);
        if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO ||
           path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
            /* Tessellate path. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tessbuf.tess_w_h));

            if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
            }
            else {
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
            }
        }
        if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
            /* Tessellate path. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tessbuf.tess_w_h));

            if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
            }
            else {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
            }
        }
    }
#endif
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_NORMAL, 0xFF));
    }
#endif
    return error;
}

/* GC555 vg_lite_draw_pattern API implementation
 */
vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t * target,
                                     vg_lite_path_t * path,
                                     vg_lite_fill_t fill_rule,
                                     vg_lite_matrix_t * path_matrix,
                                     vg_lite_buffer_t * source,
                                     vg_lite_matrix_t * pattern_matrix,
                                     vg_lite_blend_t blend,
                                     vg_lite_pattern_mode_t pattern_mode,
                                     vg_lite_color_t  pattern_color,
                                     vg_lite_color_t  color,
                                     vg_lite_filter_t filter)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_draw_pattern)(target, path, fill_rule, path_matrix, source, pattern_matrix, blend, pattern_mode,
                                    pattern_color, color, filter);
#endif

#if gcFEATURE_VG_IM_INPUT
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    uint32_t imageMode = 0;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    int32_t stride;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t matrix;
    uint32_t pattern_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t tile_setting = 0;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = { 0 }, point_max = { 0 }, temp = { 0 };
    int width, height;
    uint8_t ts_is_fullscreen = 0;

    float new_matrix[6];
    float Scale, Bias;

    uint32_t compress_mode;
    uint32_t src_premultiply_enable = 0;
    uint32_t index_endian = 0;
    uint32_t in_premult = 0;
    uint32_t paintType = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
    uint8_t  lvgl_sw_blend = 0;
#if (!gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_512_PARALLEL_PATHS)
    uint32_t parallel_workpaths1 = 2;
    uint32_t parallel_workpaths2 = 2;
#endif
#if (!gcFEATURE_VG_SPLIT_PATH || !gcFEATURE_VG_PARALLEL_PATHS || !gcFEATURE_VG_512_PARALLEL_PATHS)
    int32_t y = 0;
    uint32_t par_height = 0;
    int32_t next_boundary = 0;
#endif

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_pattern %p %p %d %p %p %p %d %d 0x%08X %d\n",
               target, path, fill_rule, path_matrix, source, pattern_matrix, blend, pattern_mode, pattern_color, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_INDEX_ENDIAN
    if((source->format >= VG_LITE_INDEX_1) && (source->format <= VG_LITE_INDEX_4) && source->index_endian) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_RGBA8_ETC2_EAC
    if(source->format == VG_LITE_RGBA8888_ETC2_EAC) {
        return VG_LITE_NOT_SUPPORT;
    }
#else
    if((source->format == VG_LITE_RGBA8888_ETC2_EAC) && (source->width % 16 || source->height % 4)) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#endif
#if !gcFEATURE_VG_YUY2_INPUT
    if(source->format == VG_LITE_YUYV || source->format == VG_LITE_YUY2) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_YUV_INPUT
    if((source->format >= VG_LITE_NV12 && source->format <= VG_LITE_NV16) || source->format == VG_LITE_NV24) {
        return VG_LITE_NOT_SUPPORT;
    }
#elif !gcFEATURE_VG_NV24_INPUT
    if(source->format == VG_LITE_NV24) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_AYUV_INPUT
    if(source->format == VG_LITE_ANV12 || source->format == VG_LITE_AYUY2) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_YUV_TILED_INPUT
    if((source->format >= VG_LITE_YUY2_TILED && source->format <= VG_LITE_AYUY2_TILED) ||
       (source->format == VG_LITE_NV24_TILED)) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_24BIT
    if((target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) ||
       (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658)) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_24BIT_PLANAR
    if(source->format >= VG_LITE_ABGR8565_PLANAR && source->format <= VG_LITE_RGBA5658_PLANAR) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif

#if !gcFEATURE_VG_STENCIL
    if(source->image_mode == VG_LITE_STENCIL_MODE) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_NEW_BLEND_MODE
    if(blend == VG_LITE_BLEND_DARKEN || blend == VG_LITE_BLEND_LIGHTEN) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }

    VG_LITE_RETURN_ERROR(srcbuf_align_check(source));
    VG_LITE_RETURN_ERROR(check_compress(source->format, source->compress_mode, source->tiled, source->width,
                                        source->height));
#endif /* gcFEATURE_VG_ERROR_CHECK */

#if !gcFEATURE_VG_LVGL_SUPPORT
    if((blend >= VG_LITE_BLEND_ADDITIVE_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) ||
       (blend == VG_LITE_BLEND_NORMAL_LVGL && gcFEATURE_VG_SRC_PREMULTIPLIED)) {
        if(!source->lvgl_buffer) {
            source->lvgl_buffer = (vg_lite_buffer_t *)vg_lite_os_malloc(sizeof(vg_lite_buffer_t));
            *source->lvgl_buffer = *source;
            source->lvgl_buffer->lvgl_buffer = NULL;
            vg_lite_allocate(source->lvgl_buffer);
        }
        /* Make sure render target is up to date before reading RT. */
        vg_lite_finish();
        setup_lvgl_image(target, source, source->lvgl_buffer, blend);
        blend = VG_LITE_BLEND_SRC_OVER;
        lvgl_sw_blend = 1;
    }
#endif

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }
    if(!pattern_matrix) {
        pattern_matrix = &identity_mtx;
    }

    /* Work on pattern states. */
    matrix = *pattern_matrix;
    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        matrix.m[2][0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][2] = 1;
        source->image_mode = VG_LITE_NONE_IMAGE_MODE;
    }

#if gcFEATURE_VG_INDEX_ENDIAN
    if((source->format >= VG_LITE_INDEX_1) && (source->format <= VG_LITE_INDEX_4) && source->index_endian) {
        index_endian = 1 << 14;
    }
#endif

#if gcFEATURE_VG_GAMMA
    save_st_gamma_src_dest(source, target);
#endif

#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_GLOBAL, 0xff));
    }
#endif
    /*blend input into context*/
    s_context.blend_mode = blend;
    in_premult = 0x00000000;

    /* Adjust premultiply setting according to openvg condition */
    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE
       || (s_context.blend_mode >= VG_LITE_BLEND_NORMAL_LVGL && s_context.blend_mode <= VG_LITE_BLEND_MULTIPLY_LVGL)) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }
#if (gcFEATURE_VG_SRC_PREMULTIPLIED == 0)
    if(blend == VG_LITE_BLEND_NORMAL_LVGL)
        in_premult = 0x00000000;
#endif
    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }

    if((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 || target->format == VG_LITE_YUY2_TILED
        || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
       && path->quality != VG_LITE_LOW) {
        path->quality = VG_LITE_LOW;
        printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = target->width;
    height = target->height;

    if(s_context.scissor_set) {
        width = s_context.scissor[2] - s_context.scissor[0];
        height = s_context.scissor[3] - s_context.scissor[1];
    }
    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((target->width <= width) && (target->height <= height) && (!s_context.scissor_set)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, &matrix))
        return VG_LITE_INVALID_ARGUMENT;

#if gcFEATURE_VG_MATH_PRECISION_FIX
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0];
    x_step[1] = inverse_matrix.m[1][0];
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1];
    y_step[1] = inverse_matrix.m[1][1];
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]);
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]);
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#else
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) / source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) / source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#endif

    /* Determine image mode (NORMAL, NONE , MULTIPLY or STENCIL) depending on the color. */
    switch(source->image_mode) {
        case VG_LITE_NONE_IMAGE_MODE:
            imageMode = 0x0;
            break;

        case VG_LITE_MULTIPLY_IMAGE_MODE:
            imageMode = 0x00002000;
            break;

        case VG_LITE_NORMAL_IMAGE_MODE:
        case VG_LITE_ZERO:
            imageMode = 0x00001000;
            break;

        case VG_LITE_STENCIL_MODE:
            imageMode = 0x00003000;
            break;

        case VG_LITE_RECOLOR_MODE:
            imageMode = 0x00006000;
            break;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;
    compress_mode = (uint32_t)source->compress_mode << 25;

    if(pattern_mode == VG_LITE_PATTERN_COLOR) {
        uint8_t a, r, g, b;
        pattern_tile = 0;
        a = pattern_color >> 24;
        r = pattern_color >> 16;
        g = pattern_color >> 8;
        b = pattern_color;
        pattern_color = (a << 24) | (b << 16) | (g << 8) | r;
    }
    else if(pattern_mode == VG_LITE_PATTERN_PAD) {
        pattern_tile = 0x1000;
    }
#if gcFEATURE_VG_IM_REPEAT_REFLECT
    else if(pattern_mode == VG_LITE_PATTERN_REPEAT) {
        pattern_tile = 0x2000;
    }
    else if(pattern_mode == VG_LITE_PATTERN_REFLECT) {
        pattern_tile = 0x3000;
    }
#endif
    else {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A04, (void *) &c_step[0]));
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A05, (void *) &c_step[1]));
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A06, (void *) &x_step[0]));
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A07, (void *) &x_step[1]));
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A08, (void *) &y_step[0]));
        VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A09, (void *) &y_step[1]));
    }

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if(((source->format >= VG_LITE_YUY2) &&
        (source->format <= VG_LITE_AYUY2)) ||
       ((source->format >= VG_LITE_YUY2_TILED) &&
        (source->format <= VG_LITE_AYUY2_TILED))) {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }
    blend_mode = convert_blend(blend);

    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24,
                                        convert_source_format(source->format) | filter_mode | pattern_tile | uv_swiz | yuv2rgb | conversion | compress_mode |
                                        src_premultiply_enable | index_endian));

        if(source->yuv.uv_planar) {
            /* Program u plane address if necessary. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A50, source->yuv.uv_planar));
        }
        if(source->yuv.v_planar) {
            /* Program v plane address if necessary. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A52, source->yuv.v_planar));
        }

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, pattern_color));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));
        /* 24bit format stride configured to 4bpp. */
        if(source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658) {
            stride = source->stride / 3 * 4;
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, stride | tiled_source));
        }
        else {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, source->stride | tiled_source));
        }
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width | (source->height << 16)));
    }
    else {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                        convert_source_format(source->format) | filter_mode | pattern_tile | uv_swiz | yuv2rgb | conversion | compress_mode |
                                        src_premultiply_enable | index_endian));

        if(source->yuv.uv_planar) {
            /* Program u plane address if necessary. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
        }
        if(source->yuv.v_planar) {
            /* Program v plane address if necessary. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
        }

        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, pattern_color));

#if !gcFEATURE_VG_LVGL_SUPPORT
        if(lvgl_sw_blend) {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->lvgl_buffer->address));
        }
        else
#endif
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->address));

        /* 24bit format stride configured to 4bpp. */
        if(source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658) {
            stride = source->stride / 3 * 4;
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, stride | tiled_source));
        }
        else {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source->stride | tiled_source));
        }
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, 0));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, source->width | (source->height << 16)));
    }

    /* Work on path states. */
    matrix = *path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], &matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], &matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    width = point_max.x - point_min.x;
    height = point_max.y - point_min.y;
    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix.m[0][0] * Scale;
    new_matrix[1] = matrix.m[0][1] * Scale;
    new_matrix[2] = (matrix.m[0][0] + matrix.m[0][1]) * Bias + matrix.m[0][2];
    new_matrix[3] = matrix.m[1][0] * Scale;
    new_matrix[4] = matrix.m[1][1] * Scale;
    new_matrix[5] = (matrix.m[1][0] + matrix.m[1][1]) * Bias + matrix.m[1][2];

    /* Convert states into hardware values. */
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.tessbuf_size;
#if gcFEATURE_VG_TESSELLATION_TILED_OUT
    tile_setting = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;
#endif

    if(source->paintType == VG_LITE_PAINT_PATTERN) {
        paintType = 1 << 24 | 1 << 25;
    }

    /* Setup the command buffer. */
#if gcFEATURE_VG_GLOBAL_ALPHA
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD1,
                                    s_context.dst_alpha_mode | s_context.dst_alpha_value | s_context.src_alpha_mode | s_context.src_alpha_value));
#endif
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    in_premult | paintType | s_context.capabilities.cap.tiled | imageMode | blend_mode | transparency_mode | tile_setting |
                                    s_context.enable_mask | s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable | 0x2));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix.m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix.m[1][2]));

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0, path->uploaded.bytes);
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tessbuf.physical_addr, s_context.tessbuf.tessbuf_size);
#endif

    if(width + point_min.x > target->width) {
        width = target->width - point_min.x;
    }

#if (!gcFEATURE_VG_SPLIT_PATH || !gcFEATURE_VG_PARALLEL_PATHS || !gcFEATURE_VG_512_PARALLEL_PATHS)
    s_context.tessbuf.tess_w_h = width | (height << 16);

#if !gcFEATURE_VG_PARALLEL_PATHS
    if(height <= 128)
        parallel_workpaths1 = 4;
    else
        parallel_workpaths1 = height * 128 / 4096 - 1;

    if(parallel_workpaths1 > parallel_workpaths2)
        parallel_workpaths1 = parallel_workpaths2;
#endif
    for(y = point_min.y; y < point_max.y; y += par_height) {
#if !gcFEATURE_VG_512_PARALLEL_PATHS
        next_boundary = (y + 512) & 0xfffffe00;
#elif (!gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_SPLIT_PATH)
        next_boundary = (y + 32) & 0xffffffe0;
#else
        next_boundary = (y + 16) & 0xfffffff0;
#endif
        par_height = ((next_boundary < point_max.y) ? next_boundary - y : (point_max.y - y));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                        in_premult | paintType | s_context.capabilities.cap.tiled | imageMode | blend_mode | transparency_mode | tile_setting |
                                        s_context.enable_mask | s_context.scissor_enable | s_context.color_transform | s_context.matrix_enable | 0x2));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (par_height << 16)));

        if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
            VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
        }
        else {
            if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO)
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
            if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
            }
#if !gcFEATURE_VG_PARALLEL_PATHS
            s_context.path_counter++;
            if(parallel_workpaths1 == s_context.path_counter) {
                VG_LITE_RETURN_ERROR(push_stall(&s_context, 7));
                s_context.path_counter = 0;
            }
#endif
        }
    }
#else
    {
        /* Tessellate path. */
        s_context.tessbuf.tess_w_h = width | (height << 16);
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tessbuf.tess_w_h));

        if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
            VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
        }
        else {
            if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO)
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
            if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
            }
        }
    }
#endif
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_NORMAL, 0xFF));
    }
#endif
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0, (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif
    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/* GC555 vg_lite_draw_linear_grad API implementation
 */
vg_lite_error_t vg_lite_draw_linear_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_ext_linear_gradient_t * grad,
                                         vg_lite_color_t paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_draw_linear_grad)(target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_LINEAR_GRADIENT_EXT && gcFEATURE_VG_IM_INPUT
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t image_mode = 0;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    vg_lite_buffer_t * source = &grad->image;
    vg_lite_matrix_t * matrix = &grad->matrix;
    uint32_t linear_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    uint32_t in_premult = 0;
    uint32_t src_premultiply_enable = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;
    void * data;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = { 0 }, point_max = { 0 }, temp = { 0 };
    int width, height;
    uint8_t ts_is_fullscreen = 0;
    float new_matrix[6];
    float Scale, Bias;

    vg_lite_float_t dx, dy, dxdx_dydy;
    vg_lite_float_t lg_step_x_lin, lg_step_y_lin, lg_constant_lin;

#if !gcFEATURE_VG_PARALLEL_PATHS
    uint32_t parallel_workpaths1 = 2;
    uint32_t parallel_workpaths2 = 2;

#endif

    int y;
    int temp_height = 0;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_linear_grad %p %p %d %p %p 0x%08X %d %d\n",
               target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_LVGL_SUPPORT
    if((blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL)) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_24BIT
    if(target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif

#if !gcFEATURE_VG_STENCIL
    if(source->image_mode == VG_LITE_STENCIL_MODE) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_NEW_BLEND_MODE
    if(blend == VG_LITE_BLEND_DARKEN || blend == VG_LITE_BLEND_LIGHTEN) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_IM_REPEAT_REFLECT
    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_REPEAT || grad->spread_mode == VG_LITE_GRADIENT_SPREAD_REFLECT) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(source->format == VG_LITE_A4 || source->format == VG_LITE_A8) {
        return VG_LITE_NOT_SUPPORT;
    }
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_TRUE);
#endif
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_GLOBAL, 0xff));
    }
#endif
    /*blend input into context*/
    s_context.blend_mode = blend;

    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE
       || (s_context.blend_mode >= VG_LITE_BLEND_NORMAL_LVGL && s_context.blend_mode <= VG_LITE_BLEND_MULTIPLY_LVGL)) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
#if (CHIPID==0x255)
        src_premultiply_enable = 0x00000000;
#endif
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;

    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((target->width <= width) && (target->height <= height) && (!s_context.scissor_set)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL, NONE , MULTIPLY or STENCIL) depending on the color. */
    switch(source->image_mode) {
        case VG_LITE_NONE_IMAGE_MODE:
            image_mode = 0x0;
            break;

        case VG_LITE_MULTIPLY_IMAGE_MODE:
            return VG_LITE_INVALID_ARGUMENT;

        case VG_LITE_NORMAL_IMAGE_MODE:
        case VG_LITE_ZERO:
            image_mode = 0x00001000;
            break;

        case VG_LITE_STENCIL_MODE:
            image_mode = 0x00003000;
            break;

        case VG_LITE_RECOLOR_MODE:
            image_mode = 0x00006000;
            break;
    }
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    switch(grad->spread_mode) {
        case VG_LITE_GRADIENT_SPREAD_FILL:
            linear_tile = 0x0;
            break;

        case VG_LITE_GRADIENT_SPREAD_PAD:
            linear_tile = 0x1000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REPEAT:
            linear_tile = 0x2000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REFLECT:
            linear_tile = 0x3000;
            break;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_FILL) {
        uint8_t a, r, g, b;
        a = paint_color >> 24;
        r = paint_color >> 16;
        g = paint_color >> 8;
        b = paint_color;
        paint_color = (a << 24) | (b << 16) | (g << 8) | r;
    }

    /* compute linear gradient paremeters */

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

    dx = grad->linear_grad.X1 - grad->linear_grad.X0;
    dy = grad->linear_grad.Y1 - grad->linear_grad.Y0;
#if gcFEATURE_VG_MATH_PRECISION_FIX
    dxdx_dydy = (vg_lite_float_t)((dx * dx + dy * dy) / sqrt((dx + 1) * (dx + 1) + (dy + 1) * (dy + 1)));
#else
    dxdx_dydy = dx * dx + dy * dy;
#endif

    /*
    **      dx (T(x) - x0) + dy (T(y) - y0)
    **  g = -------------------------------
    **                dx^2 + dy^2
    **
    **  where
    **
    **      dx := x1 - x0
    **      dy := y1 - y0
    **      T(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **            = x m00 + y m01 + 0.5 (m00 + m01) + m02
    **      T(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **            = x m10 + y m11 + 0.5 (m10 + m11) + m12.
    **
    **  We can factor the top line into:
    **
    **      = dx (x m00 + y m01 + 0.5 (m00 + m01) + m02 - x0)
    **      + dy (x m10 + y m11 + 0.5 (m10 + m11) + m12 - y0)
    **
    **      = x (dx m00 + dy m10)
    **      + y (dx m01 + dy m11)
    **      + dx (0.5 (m00 + m01) + m02 - x0)
    **      + dy (0.5 (m10 + m11) + m12 - y0).
    */

    lg_step_x_lin
        = (dx * MAT(&inverse_matrix, 0, 0) + dy * MAT(&inverse_matrix, 1, 0))
          / dxdx_dydy;

    lg_step_y_lin
        = (dx * MAT(&inverse_matrix, 0, 1) + dy * MAT(&inverse_matrix, 1, 1))
          / dxdx_dydy;

    lg_constant_lin =
        (
            (
                0.5f * (MAT(&inverse_matrix, 0, 0) + MAT(&inverse_matrix, 0, 1))
                + MAT(&inverse_matrix, 0, 2) - grad->linear_grad.X0
            ) * dx

            +

            (
                0.5f * (MAT(&inverse_matrix, 1, 0) + MAT(&inverse_matrix, 1, 1))
                + MAT(&inverse_matrix, 1, 2) - grad->linear_grad.Y0
            ) * dy
        )
        / dxdx_dydy;

    /* Setup the command buffer. */

    /* linear gradient parameters*/
    data = &lg_constant_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &lg_step_x_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &lg_step_y_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

#if gcFEATURE_VG_MATH_PRECISION_FIX
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0];
    x_step[1] = inverse_matrix.m[1][0];
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1];
    y_step[1] = inverse_matrix.m[1][1];
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]);
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]);
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#else
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) / source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) / source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#endif

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if(((source->format >= VG_LITE_YUY2) &&
        (source->format <= VG_LITE_AYUY2)) ||
       ((source->format >= VG_LITE_YUY2_TILED) &&
        (source->format <= VG_LITE_AYUY2_TILED))) {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if(source->yuv.uv_planar) {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if(source->yuv.v_planar) {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter_mode | uv_swiz | yuv2rgb | linear_tile | conversion | src_premultiply_enable));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width | (source->height << 16)));

    /* Work on path states. */
    matrix = path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * Scale;
    new_matrix[1] = matrix->m[0][1] * Scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * Bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * Scale;
    new_matrix[4] = matrix->m[1][1] * Scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * Bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.tessbuf_size;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x01000002 | s_context.capabilities.cap.tiled | in_premult | image_mode | blend_mode | transparency_mode |
                                    s_context.enable_mask | s_context.color_transform | s_context.matrix_enable | s_context.scissor_enable));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000400 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0, path->uploaded.bytes);
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tessbuf.physical_addr, s_context.tessbuf.tessbuf_size);
#endif

    if(width + point_min.x > target->width) {
        width = target->width - point_min.x;
    }

#if (gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_512_PARALLEL_PATHS)
    {
        /* Tessellate path. */
        s_context.tessbuf.tess_w_h = width | (height << 16);
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tessbuf.tess_w_h));

        if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
            VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
        }
        else {
            if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO)
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
            if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
            }
        }
    }
#else
    {
        height = s_context.tessbuf.tess_w_h >> 16;
        if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO) {
#if gcFEATURE_VG_512_PARALLEL_PATHS
            if(height <= 128)
                parallel_workpaths1 = 4;
            else
                parallel_workpaths1 = height * 128 / 4096 - 1;

            if(parallel_workpaths1 > parallel_workpaths2)
                parallel_workpaths1 = parallel_workpaths2;
#endif
            for(y = point_min.y; y < point_max.y; y += height) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
                if(y + height > target->height) {
                    temp_height = target->height - y;
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (temp_height << 16)));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (height << 16)));
                }

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
#if gcFEATURE_VG_512_PARALLEL_PATHS
                    s_context.path_counter ++;
                    if(parallel_workpaths1 == s_context.path_counter) {
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0E02, 0x10 | (0x7 << 8)));
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0F00, 0x10 | (0x7 << 8)));
                        s_context.path_counter = 0;
                    }
#endif
                }
            }
        }

        if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
            for(y = point_min.y; y < point_max.y; y += height) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
                if(y + height > target->height) {
                    temp_height = target->height - y;
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (temp_height << 16)));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (height << 16)));
                }

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    format = convert_path_format(VG_LITE_FP32);
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
#if gcFEATURE_VG_512_PARALLEL_PATHS
                    s_context.path_counter ++;
                    if(parallel_workpaths1 == s_context.path_counter) {
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0E02, 0x10 | (0x7 << 8)));
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0F00, 0x10 | (0x7 << 8)));
                        s_context.path_counter = 0;
                    }
#endif
                }
            }
        }
    }
#endif

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_NORMAL, 0xFF));
    }
#endif
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0, (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/* GC555 vg_lite_draw_radial_grad API implementation
 */
vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_radial_gradient_t * grad,
                                         vg_lite_color_t paint_color,
                                         vg_lite_blend_t blend,
                                         vg_lite_filter_t filter)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_draw_radial_grad)(target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_RADIAL_GRADIENT && gcFEATURE_VG_IM_INPUT
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t imageMode = 0;
    uint32_t blend_mode;
    uint32_t filter_mode = 0;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    vg_lite_buffer_t * source = &grad->image;
    vg_lite_matrix_t * matrix = &grad->matrix;
    uint32_t rad_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    void * data;
    uint32_t compress_mode;
    uint32_t in_premult = 0;
    uint32_t src_premultiply_enable = 0;
    uint32_t premul_flag = 0;
    uint32_t prediv_flag = 0;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = { 0 }, point_max = { 0 }, temp = { 0 };
    int width, height;
    uint8_t ts_is_fullscreen = 0;
    float new_matrix[6];
    float Scale, Bias;

    vg_lite_float_t radius;

    vg_lite_float_t centerX, centerY;
    vg_lite_float_t focalX, focalY;
    vg_lite_float_t fx, fy;
    vg_lite_float_t fxfy_2;
    vg_lite_float_t radius2;
    vg_lite_float_t r2_fx2, r2_fy2;
    vg_lite_float_t r2_fx2_2, r2_fy2_2;
    vg_lite_float_t r2_fx2_fy2;
    vg_lite_float_t r2_fx2_fy2sq;
    vg_lite_float_t cx, cy;

    vg_lite_float_t rgConstantLin, rgStepXLin, rgStepYLin;
    vg_lite_float_t rgConstantRad, rgStepXRad, rgStepYRad;
    vg_lite_float_t rgStepXXRad, rgStepYYRad, rgStepXYRad;

    int y;
    int temp_height = 0;

#if !gcFEATURE_VG_PARALLEL_PATHS
    uint32_t parallel_workpaths1 = 2;
    uint32_t parallel_workpaths2 = 2;
#endif

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_draw_radial_grad %p %p %d %p %p 0x%08X %d %d\n",
               target, path, fill_rule, path_matrix, grad, paint_color, blend, filter);
#endif

#if gcFEATURE_VG_ERROR_CHECK
#if !gcFEATURE_VG_QUALITY_8X
    if(path->quality == VG_LITE_UPPER) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_LVGL_SUPPORT
    if((blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL)) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_24BIT
    if(target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif

#if !gcFEATURE_VG_STENCIL
    if(source->image_mode == VG_LITE_STENCIL_MODE) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_NEW_BLEND_MODE
    if(blend == VG_LITE_BLEND_DARKEN || blend == VG_LITE_BLEND_LIGHTEN) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
#if !gcFEATURE_VG_IM_REPEAT_REFLECT
    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_REPEAT || grad->spread_mode == VG_LITE_GRADIENT_SPREAD_REFLECT) {
        return VG_LITE_NOT_SUPPORT;
    }
#endif
    if(source->format == VG_LITE_A4 || source->format == VG_LITE_A8) {
        return VG_LITE_NOT_SUPPORT;
    }
    if(!path || !path->path) {
        return VG_LITE_INVALID_ARGUMENT;
    }
    radius = grad->radial_grad.r;
    if(radius < 0) {
        return VG_LITE_INVALID_ARGUMENT;
    }
    VG_LITE_RETURN_ERROR(check_compress(source->format, source->compress_mode, source->tiled, source->width,
                                        source->height));
#endif /* gcFEATURE_VG_ERROR_CHECK */

    if(!path->path_length) {
        return VG_LITE_SUCCESS;
    }

    if(!path_matrix) {
        path_matrix = &identity_mtx;
    }

#if gcFEATURE_VG_GAMMA
    set_gamma_dest_only(target, VGL_TRUE);
#endif
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_GLOBAL, 0xff));
    }
#endif
    /*blend input into context*/
    s_context.blend_mode = blend;

    src_premultiply_enable = 0x01000100;
    if(s_context.color_transform == 0 && s_context.gamma_dst == s_context.gamma_src && s_context.matrix_enable == 0 &&
       s_context.dst_alpha_mode == 0 && s_context.src_alpha_mode == 0 &&
       (source->image_mode == VG_LITE_NORMAL_IMAGE_MODE || source->image_mode == 0)) {
        prediv_flag = 0;
    }
    else {
        prediv_flag = 1;
    }
    if((s_context.blend_mode >= OPENVG_BLEND_SRC_OVER && s_context.blend_mode <= OPENVG_BLEND_ADDITIVE) ||
       source->image_mode == VG_LITE_STENCIL_MODE
       || (s_context.blend_mode >= VG_LITE_BLEND_NORMAL_LVGL && s_context.blend_mode <= VG_LITE_BLEND_MULTIPLY_LVGL)) {
        premul_flag = 1;
    }
    else {
        premul_flag = 0;
    }

    if((source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 0) ||
       (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 0)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x10000000;
    }
    /* when src and dst all pre format, im pre_out set to 0 to perform data truncation to prevent data overflow */
    else if(source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 0) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 0 && target->premultiplied == 1) ||
            (source->premultiplied == 0 && target->premultiplied == 0 && premul_flag == 1)) {
        src_premultiply_enable = 0x01000100;
        in_premult = 0x00000000;
    }
    else if((source->premultiplied == 1 && target->premultiplied == 1 && prediv_flag == 1) ||
            (source->premultiplied == 1 && target->premultiplied == 0 && prediv_flag == 1)) {
        src_premultiply_enable = 0x00000100;
        in_premult = 0x00000000;
    }
    if((source->format == VG_LITE_A4 || source->format == VG_LITE_A8) && blend >= VG_LITE_BLEND_SRC_OVER &&
       blend <= VG_LITE_BLEND_SUBTRACT) {
#if (CHIPID==0x255)
        src_premultiply_enable = 0x00000000;
#endif
        in_premult = 0x00000000;
    }
    if(source->premultiplied == target->premultiplied && premul_flag == 0) {
        target->apply_premult = 1;
    }
    else {
        target->apply_premult = 0;
    }

    error = set_render_target(target);
    if(error != VG_LITE_SUCCESS) {
        return error;
    }
    else if(error == VG_LITE_NO_CONTEXT) {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    if((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 || target->format == VG_LITE_YUY2_TILED
        || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
       && path->quality != VG_LITE_LOW) {
        path->quality = VG_LITE_LOW;
        printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tessbuf.tess_w_h & 0xFFFF;
    height = s_context.tessbuf.tess_w_h >> 16;

    if(width == 0 || height == 0)
        return VG_LITE_NO_CONTEXT;
    if((target->width <= width) && (target->height <= height) && (!s_context.scissor_set)) {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) && (source->format != VG_LITE_A8))) {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL, NONE , MULTIPLY or STENCIL) depending on the color. */
    switch(source->image_mode) {
        case VG_LITE_NONE_IMAGE_MODE:
            imageMode = 0x0;
            break;

        case VG_LITE_MULTIPLY_IMAGE_MODE:
            return VG_LITE_INVALID_ARGUMENT;

        case VG_LITE_NORMAL_IMAGE_MODE:
        case VG_LITE_ZERO:
            imageMode = 0x00001000;
            break;

        case VG_LITE_STENCIL_MODE:
            imageMode = 0x00003000;
            break;

        case VG_LITE_RECOLOR_MODE:
            imageMode = 0x00006000;
            break;
    }

    switch(filter) {
        case VG_LITE_FILTER_POINT:
            filter_mode = 0;
            break;

        case VG_LITE_FILTER_LINEAR:
            filter_mode = 0x10000;
            break;

        case VG_LITE_FILTER_BI_LINEAR:
            filter_mode = 0x20000;
            break;

        case VG_LITE_FILTER_GAUSSIAN:
            filter_mode = 0x30000;
            break;
    }

    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    switch(grad->spread_mode) {
        case VG_LITE_GRADIENT_SPREAD_FILL:
            rad_tile = 0x0;
            break;

        case VG_LITE_GRADIENT_SPREAD_PAD:
            rad_tile = 0x1000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REPEAT:
            rad_tile = 0x2000;
            break;

        case VG_LITE_GRADIENT_SPREAD_REFLECT:
            rad_tile = 0x3000;
            break;
    }

    compress_mode = (uint32_t)source->compress_mode << 25;

    if(grad->spread_mode == VG_LITE_GRADIENT_SPREAD_FILL) {
        uint8_t a, r, g, b;
        a = paint_color >> 24;
        r = paint_color >> 16;
        g = paint_color >> 8;
        b = paint_color;
        paint_color = (a << 24) | (b << 16) | (g << 8) | r;
    }

    /* compute radial gradient paremeters */

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

    /* Make shortcuts to the gradient information. */
    centerX = grad->radial_grad.cx;
    centerY = grad->radial_grad.cy;
    focalX  = grad->radial_grad.fx;
    focalY  = grad->radial_grad.fy;

    /* Compute constants of the equation. */
    fx           = focalX - centerX;
    fy           = focalY - centerY;
    radius2      = radius * radius;
    if(fx * fx + fy * fy > radius2) {
        /* If the focal point is outside the circle, let's move it
            to inside the circle. Per vg11 spec pg125 "If (fx, fy) lies outside ...
            For here, we set it at 0.9 ratio to the center.
        */
        vg_lite_float_t fr = (vg_lite_float_t)sqrt(fx * fx + fy * fy);
        fx = radius * fx / fr * 0.9f;
        fy = radius * fy / fr * 0.9f;
        focalX = grad->radial_grad.fx + fx;
        focalY = grad->radial_grad.fy + fy;
    }

    fxfy_2       = 2.0f * fx * fy;
    r2_fx2       = radius2 - fx * fx;
    r2_fy2       = radius2 - fy * fy;
    r2_fx2_2     = 2.0f * r2_fx2;
    r2_fy2_2     = 2.0f * r2_fy2;
#if gcFEATURE_VG_MATH_PRECISION_FIX
    r2_fx2_fy2   = (r2_fx2  - fy * fy) / source->width;
    r2_fx2_fy2sq = (r2_fx2_fy2 * r2_fx2_fy2);
#else
    r2_fx2_fy2   = r2_fx2  - fy * fy;
    r2_fx2_fy2sq = r2_fx2_fy2 * r2_fx2_fy2;
#endif

    /*                        _____________________________________
    **      dx fx + dy fy + \/r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = -------------------------------------------------------
    **                         r^2 - fx^2 - fy^2
    **
    **  Where
    **
    **      dx := F(x) - focalX
    **      dy := F(y) - focalY
    **      fx := focalX - centerX
    **      fy := focalX - centerY
    **
    **  and
    **
    **      F(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **      F(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **
    **  So, dx can be factored into
    **
    **      dx = (x + 0.5) m00 + (y + 0.5) m01 + m02 - focalX
    **         = x m00 + y m01 + 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **         = x m00 + y m01 + cx
    **
    **  where
    **
    **      cx := 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **  The same way we can factor dy into
    **
    **      dy = x m10 + y m11 + cy
    **
    **  where
    **
    **      cy := 0.5 m10 + 0.5 m11 + m12 - focalY.
    **
    **  Now we can rewrite g as
    **                               ______________________________________
    **        dx fx + dy fy         / r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = ----------------- + \  /  -------------------------------------
    **      r^2 - fx^2 - fy^2    \/           (r^2 - fx^2 - fy^2)^2
    **               ____
    **    = gLin + \/gRad
    **
    **  where
    **
    **                dx fx + dy fy
    **      gLin := -----------------
    **              r^2 - fx^2 - fy^2
    **
    **              r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **      gRad := -------------------------------------
    **                      (r^2 - fx^2 - fy^2)^2
    */

    cx
        = 0.5f * (MAT(&inverse_matrix, 0, 0) + MAT(&inverse_matrix, 0, 1))
          + MAT(&inverse_matrix, 0, 2)
          - focalX;

    cy
        = 0.5f * (MAT(&inverse_matrix, 1, 0) + MAT(&inverse_matrix, 1, 1))
          + MAT(&inverse_matrix, 1, 2)
          - focalY;

    /*
    **            dx fx + dy fy
    **  gLin := -----------------
    **          r^2 - fx^2 - fy^2
    **
    **  We can factor the top half into
    **
    **      = (x m00 + y m01 + cx) fx + (x m10 + y m11 + cy) fy
    **
    **      = x (m00 fx + m10 fy)
    **      + y (m01 fx + m11 fy)
    **      + cx fx + cy fy.
    */

    rgStepXLin
        = (MAT(&inverse_matrix, 0, 0) * fx + MAT(&inverse_matrix, 1, 0) * fy)
          / r2_fx2_fy2;

    rgStepYLin
        = (MAT(&inverse_matrix, 0, 1) * fx + MAT(&inverse_matrix, 1, 1) * fy)
          / r2_fx2_fy2;

    rgConstantLin = (cx * fx  + cy * fy) / r2_fx2_fy2;

    /*
    **          r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  gRad := -------------------------------------
    **                  (r^2 - fx^2 - fy^2)^2
    **
    **          r^2 (dx^2 + dy^2) - dx^2 fy^2 - dy^2 fx^2 + 2 dx dy fx fy
    **       := ---------------------------------------------------------
    **                            (r^2 - fx^2 - fy^2)^2
    **
    **          dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **       := -----------------------------------------------------
    **                          (r^2 - fx^2 - fy^2)^2
    **
    **  First, lets factor dx^2 into
    **
    **      dx^2 = (x m00 + y m01 + cx)^2
    **           = x^2 m00^2 + y^2 m01^2 + 2 x y m00 m01
    **           + 2 x m00 cx + 2 y m01 cx + cx^2
    **
    **           = x^2 (m00^2)
    **           + y^2 (m01^2)
    **           + x y (2 m00 m01)
    **           + x (2 m00 cx)
    **           + y (2 m01 cx)
    **           + cx^2.
    **
    **  The same can be done for dy^2:
    **
    **      dy^2 = x^2 (m10^2)
    **           + y^2 (m11^2)
    **           + x y (2 m10 m11)
    **           + x (2 m10 cy)
    **           + y (2 m11 cy)
    **           + cy^2.
    **
    **  Let's also factor dx dy into
    **
    **      dx dy = (x m00 + y m01 + cx) (x m10 + y m11 + cy)
    **            = x^2 m00 m10 + y^2 m01 m11 + x y m00 m11 + x y m01 m10
    **            + x m00 cy + x m10 cx + y m01 cy + y m11 cx + cx cy
    **
    **            = x^2 (m00 m10)
    **            + y^2 (m01 m11)
    **            + x y (m00 m11 + m01 m10)
    **            + x (m00 cy + m10 cx)
    **            + y (m01 cy + m11 cx)
    **            + cx cy.
    **
    **  Now that we have all this, lets look at the top of gRad.
    **
    **      = dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **      = x^2 m00^2 (r^2 - fy^2) + y^2 m01^2 (r^2 - fy^2)
    **      + x y 2 m00 m01 (r^2 - fy^2) + x 2 m00 cx (r^2 - fy^2)
    **      + y 2 m01 cx (r^2 - fy^2) + cx^2 (r^2 - fy^2)
    **      + x^2 m10^2 (r^2 - fx^2) + y^2 m11^2 (r^2 - fx^2)
    **      + x y 2 m10 m11 (r^2 - fx^2) + x 2 m10 cy (r^2 - fx^2)
    **      + y 2 m11 cy (r^2 - fx^2) + cy^2 (r^2 - fx^2)
    **      + x^2 m00 m10 2 fx fy + y^2 m01 m11 2 fx fy
    **      + x y (m00 m11 + m01 m10) 2 fx fy
    **      + x (m00 cy + m10 cx) 2 fx fy + y (m01 cy + m11 cx) 2 fx fy
    **      + cx cy 2 fx fy
    **
    **      = x^2 ( m00^2 (r^2 - fy^2)
    **            + m10^2 (r^2 - fx^2)
    **            + m00 m10 2 fx fy
    **            )
    **      + y^2 ( m01^2 (r^2 - fy^2)
    **            + m11^2 (r^2 - fx^2)
    **            + m01 m11 2 fx fy
    **            )
    **      + x y ( 2 m00 m01 (r^2 - fy^2)
    **            + 2 m10 m11 (r^2 - fx^2)
    **            + (m00 m11 + m01 m10) 2 fx fy
    **            )
    **      + x ( 2 m00 cx (r^2 - fy^2)
    **          + 2 m10 cy (r^2 - fx^2)
    **          + (m00 cy + m10 cx) 2 fx fy
    **          )
    **      + y ( 2 m01 cx (r^2 - fy^2)
    **          + 2 m11 cy (r^2 - fx^2)
    **          + (m01 cy + m11 cx) 2 fx fy
    **          )
    **      + cx^2 (r^2 - fy^2) + cy^2 (r^2 - fx^2) + cx cy 2 fx fy.
    */

    rgStepXXRad =
        (
            MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 0, 0) * r2_fy2
            + MAT(&inverse_matrix, 1, 0) * MAT(&inverse_matrix, 1, 0) * r2_fx2
            + MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 1, 0) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYYRad =
        (
            MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 0, 1) * r2_fy2
            + MAT(&inverse_matrix, 1, 1) * MAT(&inverse_matrix, 1, 1) * r2_fx2
            + MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 1, 1) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXYRad =
        (
            MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 0, 1) * r2_fy2_2
            + MAT(&inverse_matrix, 1, 0) * MAT(&inverse_matrix, 1, 1) * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 0) * MAT(&inverse_matrix, 1, 1)
                + MAT(&inverse_matrix, 0, 1) * MAT(&inverse_matrix, 1, 0)
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXRad =
        (
            MAT(&inverse_matrix, 0, 0) * cx * r2_fy2_2
            + MAT(&inverse_matrix, 1, 0) * cy * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 0) * cy
                + MAT(&inverse_matrix, 1, 0) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYRad =
        (
            MAT(&inverse_matrix, 0, 1) * cx * r2_fy2_2
            + MAT(&inverse_matrix, 1, 1) * cy * r2_fx2_2
            + (
                MAT(&inverse_matrix, 0, 1) * cy
                + MAT(&inverse_matrix, 1, 1) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgConstantRad =
        (
            cx * cx * r2_fy2
            + cy * cy * r2_fx2
            + cx * cy * fxfy_2
        )
        / r2_fx2_fy2sq;

    /* Setup the command buffer. */

    /* rad gradient parameters*/
    data = &rgConstantLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &rgStepXLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &rgStepYLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));
    data = &rgConstantRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A05, *(uint32_t *) data));
    data = &rgStepXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A07, *(uint32_t *) data));
    data = &rgStepYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A09, *(uint32_t *) data));
    data = &rgStepXXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A03, *(uint32_t *) data));
    data = &rgStepYYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0A, *(uint32_t *) data));
    data = &rgStepXYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0B, *(uint32_t *) data));

    /* Compute inverse matrix. */
    if(!inverse(&inverse_matrix, matrix))
        return VG_LITE_INVALID_ARGUMENT;

#if gcFEATURE_VG_MATH_PRECISION_FIX
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0];
    x_step[1] = inverse_matrix.m[1][0];
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1];
    y_step[1] = inverse_matrix.m[1][1];
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]);
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]);
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#else
    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) / source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) / source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];
#endif

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if(((source->format >= VG_LITE_YUY2) &&
        (source->format <= VG_LITE_AYUY2)) ||
       ((source->format >= VG_LITE_YUY2_TILED) &&
        (source->format <= VG_LITE_AYUY2_TILED))) {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if(source->yuv.uv_planar) {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if(source->yuv.v_planar) {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter_mode | uv_swiz | yuv2rgb | rad_tile | conversion | src_premultiply_enable | compress_mode));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width | (source->height << 16)));

    /* Work on path states. */
    matrix = path_matrix;

    if(ts_is_fullscreen == 0) {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1], matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3], matrix);
        if(temp.x < point_min.x) point_min.x = temp.x;
        if(temp.y < point_min.y) point_min.y = temp.y;
        if(temp.x > point_max.x) point_max.x = temp.x;
        if(temp.y > point_max.y) point_max.y = temp.y;

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if(s_context.scissor_set) {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * Scale;
    new_matrix[1] = matrix->m[0][1] * Scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * Bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * Scale;
    new_matrix[4] = matrix->m[1][1] * Scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * Bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tessbuf.tessbuf_size;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x02000002 | s_context.capabilities.cap.tiled | in_premult | imageMode | blend_mode | transparency_mode |
                                    s_context.enable_mask | s_context.color_transform | s_context.matrix_enable | s_context.scissor_enable));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B, 0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C, 0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        if(path->path_changed != 0) {
            if(path->uploaded.handle != NULL) {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0, path->uploaded.bytes);
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tessbuf.physical_addr, s_context.tessbuf.tessbuf_size);
#endif

    if(width + point_min.x > target->width) {
        width = target->width - point_min.x;
    }

#if (gcFEATURE_VG_PARALLEL_PATHS && gcFEATURE_VG_512_PARALLEL_PATHS)
    {
        /* Tessellate path. */
        s_context.tessbuf.tess_w_h = width | (height << 16);
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tessbuf.tess_w_h));

        if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
            VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
        }
        else {
            if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO)
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
            if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
                format = convert_path_format(VG_LITE_FP32);
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
            }
        }
    }
#else
    {
        height = s_context.tessbuf.tess_w_h >> 16;
        if(path->path_type == VG_LITE_DRAW_FILL_PATH || path->path_type == VG_LITE_DRAW_ZERO) {
#if gcFEATURE_VG_512_PARALLEL_PATHS
            if(height <= 128)
                parallel_workpaths1 = 4;
            else
                parallel_workpaths1 = height * 128 / 4096 - 1;

            if(parallel_workpaths1 > parallel_workpaths2)
                parallel_workpaths1 = parallel_workpaths2;
#endif

            for(y = point_min.y; y < point_max.y; y += height) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
                if(y + height > target->height) {
                    temp_height = target->height - y;
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (temp_height << 16)));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (height << 16)));
                }

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->path_length, path->path));
#if gcFEATURE_VG_512_PARALLEL_PATHS
                    s_context.path_counter ++;
                    if(parallel_workpaths1 == s_context.path_counter) {
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0E02, 0x10 | (0x7 << 8)));
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0F00, 0x10 | (0x7 << 8)));
                        s_context.path_counter = 0;
                    }
#endif
                }
            }
        }
        if(path->path_type == VG_LITE_DRAW_STROKE_PATH || path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH) {
            for(y = point_min.y; y < point_max.y; y += height) {
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
                VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (y << 16)));
                if(y + height > target->height) {
                    temp_height = target->height - y;
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (temp_height << 16)));
                }
                else {
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, width | (height << 16)));
                }

                if(VLM_PATH_GET_UPLOAD_BIT(*path) == 1) {
                    VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
                }
                else {
                    format = convert_path_format(VG_LITE_FP32);
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
                    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, path->stroke_color));
                    VG_LITE_RETURN_ERROR(push_data(&s_context, path->stroke_size, path->stroke_path));
#if gcFEATURE_VG_512_PARALLEL_PATHS
                    s_context.path_counter ++;
                    if(parallel_workpaths1 == s_context.path_counter) {
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0E02, 0x10 | (0x7 << 8)));
                        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0F00, 0x10 | (0x7 << 8)));
                        s_context.path_counter = 0;
                    }
#endif
                }
            }
        }
    }
#endif

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
#if gcFEATURE_VG_GLOBAL_ALPHA
    if(blend >= VG_LITE_BLEND_NORMAL_LVGL && blend <= VG_LITE_BLEND_MULTIPLY_LVGL) {
        VG_LITE_RETURN_ERROR(vg_lite_dest_global_alpha(VG_LITE_NORMAL, 0xFF));
    }
#endif
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0, (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

#endif /* (CHIPID==0x355 || CHIPID==0x255) */

/* GC555/GC355/GC255 vg_lite_draw_grad API implementation
 */
vg_lite_error_t vg_lite_draw_grad(vg_lite_buffer_t * target,
                                  vg_lite_path_t * path,
                                  vg_lite_fill_t fill_rule,
                                  vg_lite_matrix_t * matrix,
                                  vg_lite_linear_gradient_t * grad,
                                  vg_lite_blend_t blend)
{
#if DUMP_API
    FUNC_DUMP(vg_lite_draw_grad)(target, path, fill_rule, matrix, grad, blend);
#endif

    return vg_lite_draw_pattern(target, path, fill_rule, matrix,
                                &grad->image, &grad->matrix, blend, VG_LITE_PATTERN_PAD, 0, 0, VG_LITE_FILTER_LINEAR);
}

#endif /* LV_USE_VG_LITE_DRIVER */

