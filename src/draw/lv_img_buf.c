/**
 * @file lv_img_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string.h>
#include "lv_img_buf.h"
#include "lv_draw_img.h"
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

void lv_img_buf_set_palette(lv_img_dsc_t * dsc, uint8_t id, lv_color32_t c)
{
    if(dsc->header.cf < LV_COLOR_FORMAT_I1  || dsc->header.cf > LV_COLOR_FORMAT_I8) {
        LV_LOG_WARN("Not indexed color format");
        return;
    }
    uint8_t * buf = (uint8_t *)dsc->data;
    lv_memcpy(&buf[id * sizeof(c)], &c, sizeof(c));
}

void lv_img_buf_free(lv_img_dsc_t * dsc)
{
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_free((void *)dsc->data);

        lv_free(dsc);
    }
}

void _lv_img_buf_get_transformed_area(lv_area_t * res, lv_coord_t w, lv_coord_t h, lv_coord_t angle, uint16_t zoom,
                                      const lv_point_t * pivot)
{
    if(angle == 0 && zoom == LV_ZOOM_NONE) {
        res->x1 = 0;
        res->y1 = 0;
        res->x2 = w - 1;
        res->y2 = h - 1;
        return;
    }

    lv_point_t p[4] = {
        {0, 0},
        {w - 1, 0},
        {0, h - 1},
        {w - 1, h - 1},
    };
    lv_point_transform(&p[0], angle, zoom, pivot);
    lv_point_transform(&p[1], angle, zoom, pivot);
    lv_point_transform(&p[2], angle, zoom, pivot);
    lv_point_transform(&p[3], angle, zoom, pivot);
    res->x1 = LV_MIN4(p[0].x, p[1].x, p[2].x, p[3].x);
    res->x2 = LV_MAX4(p[0].x, p[1].x, p[2].x, p[3].x);
    res->y1 = LV_MIN4(p[0].y, p[1].y, p[2].y, p[3].y);
    res->y2 = LV_MAX4(p[0].y, p[1].y, p[2].y, p[3].y);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
