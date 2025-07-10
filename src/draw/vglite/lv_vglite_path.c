/**
 * @file lv_vglite_path.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_path.h"

#if LV_USE_DRAW_VGLITE

/*********************
 *      DEFINES
 *********************/
#define VLC_OP_ARG_LEN(OP, LEN) \
    case VLC_OP_##OP:           \
    return (LEN)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void vglite_create_rect_path_data(int32_t * path_data, uint32_t * path_data_size,
                                  int32_t radius,
                                  const lv_area_t * coords)
{
    int32_t rect_width = lv_area_get_width(coords);
    int32_t rect_height = lv_area_get_height(coords);

    /* Get the final radius. Can't be larger than the half of the shortest side */
    int32_t shortest_side = LV_MIN(rect_width, rect_height);
    int32_t final_radius = LV_MIN(radius, shortest_side / 2);

    /* Path data element index */
    uint8_t pidx = 0;

    if((radius == (int32_t)LV_RADIUS_CIRCLE) && (rect_width == rect_height)) {

        /* Get the control point offset for rounded cases */
        int32_t cpoff = (int32_t)((float)final_radius * BEZIER_OPTIM_CIRCLE);

        /* Circle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y1;

        /* Top-right arc */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-right arc*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-left arc */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Top-left arc*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }
    else if(radius > 0) {
        /* Get the control point offset for rounded cases */
        int32_t cpoff = (int32_t)((float)final_radius * BEZIER_OPTIM_CIRCLE);

        /* Rounded rectangle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y1;

        /* Top side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 - final_radius + 1;  /*Extended for VGLite*/
        path_data[pidx++] = coords->y1;

        /* Top-right corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius;

        /* Right side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1;                 /*Extended for VGLite*/
        path_data[pidx++] = coords->y2 - final_radius + 1;  /*Extended for VGLite*/

        /* Bottom-right corner*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y2 + 1;                 /*Extended for VGLite*/

        /* Bottom-left corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Left side*/
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1 + final_radius;

        /* Top-left corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }
    else {
        /* Non-rounded rectangle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1;

        /* Top side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1; /*Extended for VGLite*/
        path_data[pidx++] = coords->y1;

        /* Right side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1; /*Extended for VGLite*/
        path_data[pidx++] = coords->y2 + 1; /*Extended for VGLite*/

        /* Bottom side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y2 + 1; /*Extended for VGLite*/

        /* Left side*/
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }

    /* Resulting path size */
    *path_data_size = pidx * sizeof(int32_t);
}

#if LV_USE_VGLITE_DEBUG
uint8_t lv_vglite_vlc_op_arg_len(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ARG_LEN(END, 0);
            VLC_OP_ARG_LEN(CLOSE, 0);
            VLC_OP_ARG_LEN(MOVE, 2);
            VLC_OP_ARG_LEN(MOVE_REL, 2);
            VLC_OP_ARG_LEN(LINE, 2);
            VLC_OP_ARG_LEN(LINE_REL, 2);
            VLC_OP_ARG_LEN(QUAD, 4);
            VLC_OP_ARG_LEN(QUAD_REL, 4);
            VLC_OP_ARG_LEN(CUBIC, 6);
            VLC_OP_ARG_LEN(CUBIC_REL, 6);
            VLC_OP_ARG_LEN(SCCWARC, 5);
            VLC_OP_ARG_LEN(SCCWARC_REL, 5);
            VLC_OP_ARG_LEN(SCWARC, 5);
            VLC_OP_ARG_LEN(SCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCCWARC, 5);
            VLC_OP_ARG_LEN(LCCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCWARC, 5);
            VLC_OP_ARG_LEN(LCWARC_REL, 5);
        default:
            break;
    }

    LV_LOG_ERROR("UNKNOW_VLC_OP: 0x%x", vlc_op);
    LV_ASSERT(false);
    return 0;
}


uint8_t lv_vglite_path_format_len(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 1;
        case VG_LITE_S16:
            return 2;
        case VG_LITE_S32:
            return 4;
        case VG_LITE_FP32:
            return 4;
        default:
            break;
    }

    LV_LOG_ERROR("UNKNOW_FORMAT: %d", format);
    LV_ASSERT(false);
    return 0;
}

void lv_vglite_path_for_each_data(const vg_lite_path_t * path, lv_vglite_path_iter_cb_t cb, void * user_data)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(cb);

    uint8_t fmt_len = lv_vglite_path_format_len(path->format);
    uint8_t * cur = path->path;
    uint8_t * end = cur + path->path_length;
    float tmp_data[8];

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* get arguments length */
        uint8_t arg_len = lv_vglite_vlc_op_arg_len(op_code);

        /* skip op code */
        cur += fmt_len;

        /* print arguments */
        for(uint8_t i = 0; i < arg_len; i++) {
            switch(path->format) {
                case VG_LITE_S8:
                    tmp_data[i] = *((int8_t *)cur);
                    break;
                case VG_LITE_S16:
                    tmp_data[i] = *((int16_t *)cur);
                    break;
                case VG_LITE_S32:
                    tmp_data[i] = *((int32_t *)cur);
                    break;
                case VG_LITE_FP32:
                    tmp_data[i] = *((float *)cur);
                    break;
                default:
                    LV_LOG_ERROR("UNKNOW_FORMAT(%d)", path->format);
                    LV_ASSERT(false);
                    break;
            }

            cur += fmt_len;
        }

        cb(user_data, op_code, tmp_data, arg_len);
    }
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/
