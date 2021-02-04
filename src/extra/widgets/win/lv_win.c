/**
 * @file lv_win.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_win.h"
#if LV_USE_WIN

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
static bool inited = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_win_create(lv_obj_t * parent, lv_coord_t header_height)
{
    static lv_style_t style_plain;
    if(!inited) {
        lv_style_init(&style_plain);
        lv_style_set_radius(&style_plain, LV_STATE_DEFAULT, 0);
        inited = true;
    }

    lv_obj_t * win = lv_obj_create(parent, NULL);
    lv_obj_set_size(win, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_flex_dir(win, LV_FLEX_DIR_COLUMN);
    lv_obj_reset_style_list(win, LV_OBJ_PART_MAIN);

    lv_obj_t * header = lv_obj_create(win, NULL);
    lv_obj_set_flex_item(header, true);
    lv_obj_set_size(header, LV_COORD_PCT(100), header_height);
    lv_obj_set_flex_dir(header, LV_FLEX_DIR_ROW);
    lv_obj_set_flex_place(header, LV_FLEX_PLACE_START, LV_FLEX_PLACE_CENTER);
//    lv_obj_set_flex_gap(header, LV_DPX(2));
    lv_obj_add_style(header, LV_OBJ_PART_MAIN, &style_plain);

    lv_obj_t * cont = lv_obj_create(win, NULL);
    lv_obj_set_flex_item(cont, true);
    lv_obj_set_size(cont, LV_COORD_PCT(100), LV_FLEX_GROW(1));
    lv_obj_add_style(cont, LV_OBJ_PART_MAIN, &style_plain);

    return win;
}

lv_obj_t * lv_win_add_title(lv_obj_t * win, const char * txt)
{
    lv_obj_t * header = lv_win_get_header(win);
    lv_obj_t * title = lv_label_create(header, NULL);
    lv_obj_set_flex_item(title, LV_FLEX_PLACE_CENTER);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_label_set_text(title, txt);
    lv_obj_set_width(title, LV_FLEX_GROW(1));
    return title;
}

lv_obj_t * lv_win_add_btn(lv_obj_t * win, const void * icon, lv_coord_t btn_w, lv_coord_t btn_h, lv_event_cb_t event_cb)
{
    lv_obj_t * header = lv_win_get_header(win);
    lv_obj_t * btn = lv_btn_create(header, NULL);
    lv_obj_set_size(btn, btn_w, btn_h);
    lv_obj_set_flex_item(btn, LV_FLEX_PLACE_CENTER);
    lv_obj_set_event_cb(btn, event_cb);

    lv_obj_t * img = lv_img_create(btn, NULL);
    lv_img_set_src(img, icon);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    return btn;
}

lv_obj_t * lv_win_get_header(lv_obj_t * win)
{
    return lv_obj_get_child_back(win, NULL);
}

lv_obj_t * lv_win_get_content(lv_obj_t * win)
{
    return lv_obj_get_child(win, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif

