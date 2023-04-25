/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_draw_rect.h"
#include "../misc/lv_assert.h"
#include "../core/lv_obj_event.h"

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

LV_ATTRIBUTE_FAST_MEM void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_rect_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad.stops[0].color = lv_color_white();
    dsc->bg_grad.stops[1].color = lv_color_black();
    dsc->bg_grad.stops[1].frac = 0xFF;
    dsc->bg_grad.stops_count = 2;
    dsc->border_color = lv_color_black();
    dsc->shadow_color = lv_color_black();
    dsc->bg_img_symbol_font = LV_FONT_DEFAULT;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->bg_img_opa = LV_OPA_COVER;
    dsc->outline_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
    dsc->shadow_opa = LV_OPA_COVER;
    dsc->border_side = LV_BORDER_SIDE_FULL;
}

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->bg_opa <= LV_OPA_MIN &&
       (dsc->border_opa <= LV_OPA_MIN || dsc->border_width == 0) &&
       (dsc->outline_opa <= LV_OPA_MIN || dsc->outline_width == 0) &&
       (dsc->shadow_opa <= LV_OPA_MIN || dsc->shadow_width == 0)) {
        return;
    }

    lv_draw_task_t * t = lv_draw_add_task(layer, coords);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_RECTANGLE;

    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
        /*Disable sending LV_EVENT_DRAW_TASK_ADDED first to avoid triggering recursive
         *event calls due draw task adds in the event*/
        lv_obj_clear_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        lv_obj_send_event(dsc->base.obj, LV_EVENT_DRAW_TASK_ADDED, t);
        lv_obj_add_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    }

    lv_draw_dispatch();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
