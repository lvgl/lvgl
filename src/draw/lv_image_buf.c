/**
 * @file lv_image_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string.h>
#include "lv_image_buf.h"
#include "lv_draw_image.h"
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"

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

void lv_image_buf_set_palette(lv_image_dsc_t * dsc, uint8_t id, lv_color32_t c)
{
    if(dsc->header.cf < LV_COLOR_FORMAT_I1  || dsc->header.cf > LV_COLOR_FORMAT_I8) {
        LV_LOG_WARN("Not indexed color format");
        return;
    }
    uint8_t * buf = (uint8_t *)dsc->data;
    lv_memcpy(&buf[id * sizeof(c)], &c, sizeof(c));
}

void lv_image_buf_free(lv_image_dsc_t * dsc)
{
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_free((void *)dsc->data);

        lv_free(dsc);
    }
}

void _lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle, uint16_t scale_x,
                                        uint16_t scale_y,
                                        const lv_point_t * pivot)
{
    if(angle == 0 && scale_x == LV_SCALE_NONE && scale_y == LV_SCALE_NONE) {
        res->x1 = 0;
        res->y1 = 0;
        res->x2 = w - 1;
        res->y2 = h - 1;
        return;
    }

    lv_point_t p[4] = {
        {0, 0},
        {w, 0},
        {0, h},
        {w, h},
    };
    lv_point_transform(&p[0], angle, scale_x, scale_y, pivot, true);
    lv_point_transform(&p[1], angle, scale_x, scale_y, pivot, true);
    lv_point_transform(&p[2], angle, scale_x, scale_y, pivot, true);
    lv_point_transform(&p[3], angle, scale_x, scale_y, pivot, true);
    res->x1 = LV_MIN4(p[0].x, p[1].x, p[2].x, p[3].x);
    res->x2 = LV_MAX4(p[0].x, p[1].x, p[2].x, p[3].x) - 1;
    res->y1 = LV_MIN4(p[0].y, p[1].y, p[2].y, p[3].y);
    res->y2 = LV_MAX4(p[0].y, p[1].y, p[2].y, p[3].y) - 1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
