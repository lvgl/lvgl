/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_image_private.h"
#include "../misc/lv_area_private.h"
#include "lv_image_decoder_private.h"
#include "lv_draw_private.h"
#include "../display/lv_display.h"
#include "../misc/lv_log.h"
#include "../misc/lv_math.h"
#include "../core/lv_refr.h"
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

static void img_decode_and_draw(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                lv_image_decoder_dsc_t * decoder_dsc, lv_area_t * relative_decoded_area,
                                const lv_area_t * img_area, const lv_area_t * clipped_img_area,
                                lv_draw_image_core_cb draw_core_cb);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_image_dsc_init(lv_draw_image_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_image_dsc_t));
    dsc->recolor = lv_color_black();
    dsc->opa = LV_OPA_COVER;
    dsc->scale_x = LV_SCALE_NONE;
    dsc->scale_y = LV_SCALE_NONE;
    dsc->antialias = LV_COLOR_DEPTH > 8 ? 1 : 0;
    dsc->image_area.x2 = LV_COORD_MIN;   /*Indicate invalid area by default by setting a negative size*/
    dsc->base.dsc_size = sizeof(lv_draw_image_dsc_t);
}

lv_draw_image_dsc_t * lv_draw_task_get_image_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_IMAGE ? (lv_draw_image_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_layer(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->scale_x <= 0 || dsc->scale_y <= 0) {
        /* NOT draw if scale is negative or zero */
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    LV_ASSERT_MALLOC(t->draw_dsc);
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_LAYER;
    t->state = LV_DRAW_TASK_STATE_WAITING;

    lv_image_buf_get_transformed_area(&t->_real_area, lv_area_get_width(coords), lv_area_get_height(coords),
                                      dsc->rotation, dsc->scale_x, dsc->scale_y, &dsc->pivot);
    lv_area_move(&t->_real_area, coords->x1, coords->y1);

    lv_layer_t * layer_to_draw = (lv_layer_t *)dsc->src;
    layer_to_draw->all_tasks_added = true;

    lv_draw_finalize_task_creation(layer, t);

    LV_PROFILER_DRAW_END;
}

void lv_draw_image(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        return;
    }
    if(dsc->opa <= LV_OPA_MIN) return;

    if(dsc->scale_x <= 0 || dsc->scale_y <= 0) {
        /* NOT draw if scale is negative or zero */
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_image_dsc_t * new_image_dsc = lv_malloc(sizeof(*dsc));
    LV_ASSERT_MALLOC(new_image_dsc);
    lv_memcpy(new_image_dsc, dsc, sizeof(*dsc));
    lv_result_t res = lv_image_decoder_get_info(new_image_dsc->src, &new_image_dsc->header);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Couldn't get info about the image");
        lv_free(new_image_dsc);
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_task_t * t = lv_draw_add_task(layer, coords);
    t->draw_dsc = new_image_dsc;
    t->type = LV_DRAW_TASK_TYPE_IMAGE;

    lv_image_buf_get_transformed_area(&t->_real_area, lv_area_get_width(coords), lv_area_get_height(coords),
                                      dsc->rotation, dsc->scale_x, dsc->scale_y, &dsc->pivot);
    lv_area_move(&t->_real_area, coords->x1, coords->y1);

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

lv_image_src_t lv_image_src_get_type(const void * src)
{
    if(src == NULL) return LV_IMAGE_SRC_UNKNOWN;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) {
        return LV_IMAGE_SRC_FILE; /*If it's an ASCII character then it's file name*/
    }
    else if(u8_p[0] >= 0x80) {
        return LV_IMAGE_SRC_SYMBOL; /*Symbols begins after 0x7F*/
    }
    else {
        return LV_IMAGE_SRC_VARIABLE; /*`lv_image_dsc_t` is draw to the first byte < 0x20*/
    }
}

void lv_draw_image_normal_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb)
{
    if(draw_core_cb == NULL) {
        LV_LOG_WARN("draw_core_cb is NULL");
        return;
    }

    lv_area_t draw_area;
    lv_area_copy(&draw_area, coords);
    if(draw_dsc->rotation || draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        lv_image_buf_get_transformed_area(&draw_area, w, h, draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y,
                                          &draw_dsc->pivot);

        draw_area.x1 += coords->x1;
        draw_area.y1 += coords->y1;
        draw_area.x2 += coords->x1;
        draw_area.y2 += coords->y1;
    }

    lv_area_t clipped_img_area;
    if(!lv_area_intersect(&clipped_img_area, &draw_area, draw_unit->clip_area)) {
        return;
    }

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    img_decode_and_draw(draw_unit, draw_dsc, &decoder_dsc, NULL, coords, &clipped_img_area, draw_core_cb);

    lv_image_decoder_close(&decoder_dsc);
}

void lv_draw_image_tiled_helper(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb)
{
    if(draw_core_cb == NULL) {
        LV_LOG_WARN("draw_core_cb is NULL");
        return;
    }

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    int32_t img_w = draw_dsc->header.w;
    int32_t img_h = draw_dsc->header.h;

    lv_area_t tile_area;
    if(lv_area_get_width(&draw_dsc->image_area) >= 0) {
        tile_area = draw_dsc->image_area;
    }
    else {
        tile_area = *coords;
    }
    lv_area_set_width(&tile_area, img_w);
    lv_area_set_height(&tile_area, img_h);

    int32_t tile_x_start = tile_area.x1;

    lv_area_t relative_decoded_area = {
        .x1 = LV_COORD_MIN,
        .y1 = LV_COORD_MIN,
        .x2 = LV_COORD_MIN,
        .y2 = LV_COORD_MIN,
    };

    while(tile_area.y1 <= coords->y2) {
        while(tile_area.x1 <= coords->x2) {

            lv_area_t clipped_img_area;
            if(lv_area_intersect(&clipped_img_area, &tile_area, coords)) {
                img_decode_and_draw(draw_unit, draw_dsc, &decoder_dsc, &relative_decoded_area, &tile_area, &clipped_img_area,
                                    draw_core_cb);
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }

    lv_image_decoder_close(&decoder_dsc);
}

void lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle,
                                       uint16_t scale_x, uint16_t scale_y, const lv_point_t * pivot)
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

static void img_decode_and_draw(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                lv_image_decoder_dsc_t * decoder_dsc, lv_area_t * relative_decoded_area,
                                const lv_area_t * img_area, const lv_area_t * clipped_img_area,
                                lv_draw_image_core_cb draw_core_cb)
{
    lv_draw_image_sup_t sup;
    sup.alpha_color = draw_dsc->recolor;
    sup.palette = decoder_dsc->palette;
    sup.palette_size = decoder_dsc->palette_size;

    /*The whole image is available, just draw it*/
    if(decoder_dsc->decoded && (relative_decoded_area == NULL || relative_decoded_area->x1 == LV_COORD_MIN)) {
        draw_core_cb(draw_unit, draw_dsc, decoder_dsc, &sup, img_area, clipped_img_area);
    }
    /*Draw in smaller pieces*/
    else {
        lv_area_t relative_full_area_to_decode = *clipped_img_area;
        lv_area_move(&relative_full_area_to_decode, -img_area->x1, -img_area->y1);
        lv_area_t tmp;
        if(relative_decoded_area == NULL) relative_decoded_area = &tmp;
        relative_decoded_area->x1 = LV_COORD_MIN;
        relative_decoded_area->y1 = LV_COORD_MIN;
        relative_decoded_area->x2 = LV_COORD_MIN;
        relative_decoded_area->y2 = LV_COORD_MIN;
        lv_result_t res = LV_RESULT_OK;

        while(res == LV_RESULT_OK) {
            res = lv_image_decoder_get_area(decoder_dsc, &relative_full_area_to_decode, relative_decoded_area);

            lv_area_t absolute_decoded_area = *relative_decoded_area;
            lv_area_move(&absolute_decoded_area, img_area->x1, img_area->y1);
            if(res == LV_RESULT_OK) {
                /*Limit draw area to the current decoded area and draw the image*/
                lv_area_t clipped_img_area_sub;
                if(lv_area_intersect(&clipped_img_area_sub, clipped_img_area, &absolute_decoded_area)) {
                    draw_core_cb(draw_unit, draw_dsc, decoder_dsc, &sup,
                                 &absolute_decoded_area, &clipped_img_area_sub);
                }
            }
        }
    }
}
