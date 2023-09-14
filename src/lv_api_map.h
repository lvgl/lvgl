/**
 * @file lv_api_map.h
 *
 */

#ifndef LV_API_MAP_H
#define LV_API_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define LV_BTNMATRIX_BTN_NONE   LV_BUTTONMATRIX_BUTTON_NONE

#define LV_BTNMATRIX_CTRL_HIDDEN       LV_BUTTONMATRIX_CTRL_HIDDEN
#define LV_BTNMATRIX_CTRL_NO_REPEAT    LV_BUTTONMATRIX_CTRL_NO_REPEAT
#define LV_BTNMATRIX_CTRL_DISABLED     LV_BUTTONMATRIX_CTRL_DISABLED
#define LV_BTNMATRIX_CTRL_CHECKABLE    LV_BUTTONMATRIX_CTRL_CHECKABLE
#define LV_BTNMATRIX_CTRL_CHECKED      LV_BUTTONMATRIX_CTRL_CHECKED
#define LV_BTNMATRIX_CTRL_CLICK_TRIG   LV_BUTTONMATRIX_CTRL_CLICK_TRIG
#define LV_BTNMATRIX_CTRL_POPOVER      LV_BUTTONMATRIX_CTRL_POPOVER
#define LV_BTNMATRIX_CTRL_RECOLOR      LV_BUTTONMATRIX_CTRL_RECOLOR
#define LV_BTNMATRIX_CTRL_CUSTOM_1     LV_BUTTONMATRIX_CTRL_CUSTOM_1
#define LV_BTNMATRIX_CTRL_CUSTOM_2     LV_BUTTONMATRIX_CTRL_CUSTOM_2


/**********************
 *      TYPEDEFS
 **********************/

typedef lv_image_dsc_t lv_img_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_task_handler(void)
{
    return lv_timer_handler();
}


/**
 * Move the object to the foreground.
 * It will look like if it was created as the last child of its parent.
 * It also means it can cover any of the siblings.
 * @param obj       pointer to an object
 */
static inline void lv_obj_move_foreground(lv_obj_t * obj)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(!parent) {
        LV_LOG_WARN("parent is NULL");
        return;
    }

    lv_obj_move_to_index(obj, lv_obj_get_child_cnt(parent) - 1);
}

/**
 * Move the object to the background.
 * It will look like if it was created as the first child of its parent.
 * It also means any of the siblings can cover the object.
 * @param obj       pointer to an object
 */
static inline void lv_obj_move_background(lv_obj_t * obj)
{
    lv_obj_move_to_index(obj, 0);
}


/**********************
 *      MACROS
 **********************/
#define lv_img_create         lv_image_create
#define lv_img_set_src        lv_image_set_src
#define lv_img_set_offset_x   lv_image_set_offset_x
#define lv_img_set_offset_y   lv_image_set_offset_y
#define lv_img_set_angle      lv_image_set_angle
#define lv_img_set_pivot      lv_image_set_pivot
#define lv_img_set_zoom       lv_image_set_zoom
#define lv_img_set_antialias  lv_image_set_antialias
#define lv_img_set_size_mode  lv_image_set_size_mode
#define lv_img_get_src        lv_image_get_src
#define lv_img_get_offset_x   lv_image_get_offset_x
#define lv_img_get_offset_y   lv_image_get_offset_y
#define lv_img_get_angle      lv_image_get_angle
#define lv_img_get_pivot      lv_image_get_pivot
#define lv_img_get_zoom       lv_image_get_zoom
#define lv_img_get_antialias  lv_image_get_antialias
#define lv_img_get_size_mode  lv_image_get_size_mode

#define lv_list_set_btn_text lv_list_set_button_text
#define lv_list_get_btn_text lv_list_get_button_text
#define lv_list_add_btn      lv_list_add_button

#define lv_btn_create        lv_button_create

#define lv_btnmatrix_create                  lv_buttonmatrix_create
#define lv_btnmatrix_set_map                 lv_buttonmatrix_set_map
#define lv_btnmatrix_set_ctrl_map            lv_buttonmatrix_set_ctrl_map
#define lv_btnmatrix_set_selected_btn        lv_buttonmatrix_set_selected_button
#define lv_btnmatrix_set_btn_ctrl            lv_buttonmatrix_set_button_ctrl
#define lv_btnmatrix_clear_btn_ctrl          lv_buttonmatrix_clear_button_ctrl
#define lv_btnmatrix_set_btn_ctrl_all        lv_buttonmatrix_set_button_ctrl_all
#define lv_btnmatrix_clear_btn_ctrl_all      lv_buttonmatrix_clear_button_ctrl_all
#define lv_btnmatrix_set_btn_width           lv_buttonmatrix_set_button_width
#define lv_btnmatrix_set_one_checked         lv_buttonmatrix_set_one_checked
#define lv_btnmatrix_get_map                 lv_buttonmatrix_get_map
#define lv_btnmatrix_get_selected_btn        lv_buttonmatrix_get_selected_button
#define lv_btnmatrix_get_button_text         lv_buttonmatrix_get_button_text
#define lv_btnmatrix_has_button_ctrl         lv_buttonmatrix_has_button_ctrl
#define lv_btnmatrix_get_one_checked         lv_buttonmatrix_get_one_checked

#define lv_tabview_get_tab_btns              lv_tabview_get_tab_buttons

#define lv_msgbox_get_btns                   lv_msgbox_get_buttons

/**********************
 *      MACROS
 **********************/
/** Use this macro to declare an image in a C file*/
#define LV_IMG_DECLARE(var_name) extern const lv_image_dsc_t var_name;

/**********************
 * DEPRECATED FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_API_MAP_H*/
