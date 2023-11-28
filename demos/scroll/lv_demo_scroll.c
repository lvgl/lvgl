/**
 * @file lv_demo_scroll.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_scroll.h"

#if LV_USE_DEMO_SCROLL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * switch_create(lv_obj_t * parent, const char * title, lv_obj_flag_t flag, bool en);
static void generic_switch_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * list;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_demo_scroll(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_style_shadow_width(panel, 16, 0);
    lv_obj_set_style_shadow_offset_y(panel, 8, 0);
    lv_obj_set_style_shadow_offset_x(panel, 4, 0);
    lv_obj_set_style_shadow_opa(panel, LV_OPA_40, 0);

    lv_obj_set_size(panel, lv_pct(70), lv_pct(90));
    lv_obj_center(panel);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    list = lv_list_create(panel);
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image1.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image2.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image3.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image4.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image5.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image6.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image7.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image8.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image9.png");
    lv_list_add_button(list, LV_SYMBOL_IMAGE, "Image10.png");

    switch_create(panel, "Scrollable", LV_OBJ_FLAG_SCROLLABLE, true);
    switch_create(panel, "Scroll chain", LV_OBJ_FLAG_SCROLL_CHAIN, true);
    switch_create(panel, "Elastic scroll", LV_OBJ_FLAG_SCROLL_ELASTIC, true);
    switch_create(panel, "Add scroll momentum", LV_OBJ_FLAG_SCROLL_MOMENTUM, true);

    /*Show the switches first*/
    lv_obj_move_foreground(list);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * switch_create(lv_obj_t * parent, const char * title, lv_obj_flag_t flag, bool en)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, title);
    lv_obj_set_flex_grow(label, 1);

    lv_obj_t * sw = lv_switch_create(cont);
    lv_obj_add_event_cb(sw, generic_switch_event_cb, LV_EVENT_VALUE_CHANGED, (void *)((lv_uintptr_t) flag));
    if(en) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
        lv_obj_add_flag(list, flag);
    }
    else {
        lv_obj_remove_state(sw, LV_STATE_CHECKED);
        lv_obj_remove_flag(list, flag);
    }

    return cont;
}

static void generic_switch_event_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);
    lv_obj_flag_t flag = (lv_obj_flag_t)((lv_uintptr_t)lv_event_get_user_data(e));

    if(lv_obj_has_state(sw, LV_STATE_CHECKED)) lv_obj_add_flag(list, flag);
    else lv_obj_remove_flag(list, flag);
}

#endif
