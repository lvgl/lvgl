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
#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

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

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/
