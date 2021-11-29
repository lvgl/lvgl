/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#include "lv_draw_sw.h"

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
 *  GLOBAL PROTOTYPES
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

void lv_draw_sw_init(void)
{
    static lv_draw_backend_t backend;
    lv_draw_backend_init(&backend);

    backend.draw_arc = lv_draw_sw_arc;
    backend.draw_rect = lv_draw_sw_rect;
    backend.draw_letter = lv_draw_sw_letter;
    backend.draw_img = lv_draw_sw_img;
    backend.draw_line = lv_draw_sw_line;
    backend.draw_polygon = lv_draw_sw_polygon;
    backend.blend_fill = lv_blend_sw_fill;
    backend.blend_map  = lv_blend_sw_map;

    lv_draw_backend_add(&backend);
}

void lv_draw_sw_blend_fill_call_base(lv_draw_backend_class_t * class_p, lv_draw_backend_t * backend)
{
    const lv_draw_backend_class_t * base;
    if(class_p == NULL) base = backend->class_p;
    else base = class_p->base_class;

    /*Find a base in which call the ancestor's event handler_cb if set*/
    while(base && base->event_cb == NULL) base = base->base_class;

    if(base == NULL) return LV_RES_OK;
    if(base->event_cb == NULL) return LV_RES_OK;

    /*Call the actual event callback*/
    e->user_data = NULL;
    base->event_cb(base, e);

    lv_res_t res = LV_RES_OK;
    /*Stop if the object is deleted*/
    if(e->deleted) res = LV_RES_INV;

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
