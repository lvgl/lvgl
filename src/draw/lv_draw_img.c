/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_img.h"
#include "lv_img_cache.h"
#include "../disp/lv_disp.h"
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_img_dsc_init(lv_draw_img_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_img_dsc_t));
    dsc->recolor = lv_color_black();
    dsc->opa = LV_OPA_COVER;
    dsc->zoom = LV_ZOOM_NONE;
    dsc->antialias = LV_COLOR_DEPTH > 8 ? 1 : 0;
}


void lv_draw_layer(lv_layer_t * layer, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_LAYER;
    t->state = LV_DRAW_TASK_STATE_WAITING;

    lv_layer_t * layer_to_draw = (lv_layer_t *)dsc->src;
    layer_to_draw->all_tasks_added = true;

    lv_draw_finalize_task_creation(layer, t);
}


void lv_draw_img(lv_layer_t * layer, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        return;
    }
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_BEGIN;
    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_IMAGE;

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_END;
}

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
lv_img_src_t lv_img_src_get_type(const void * src)
{
    lv_img_src_t img_src_type = LV_IMG_SRC_UNKNOWN;

    if(src == NULL) return img_src_type;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) {
        img_src_type = LV_IMG_SRC_FILE; /*If it's an ASCII character then it's file name*/
    }
    else if(u8_p[0] >= 0x80) {
        img_src_type = LV_IMG_SRC_SYMBOL; /*Symbols begins after 0x7F*/
    }
    else {
        img_src_type = LV_IMG_SRC_VARIABLE; /*`lv_img_dsc_t` is draw to the first byte < 0x20*/
    }

    if(LV_IMG_SRC_UNKNOWN == img_src_type) {
        LV_LOG_WARN("unknown image type");
    }

    return img_src_type;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

