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


#define LV_DISP_ROTATION_0       LV_DISPLAY_ROTATION_0
#define LV_DISP_ROTATION_90      LV_DISPLAY_ROTATION_90
#define LV_DISP_ROTATION_180     LV_DISPLAY_ROTATION_180
#define LV_DISP_ROTATION_270     LV_DISPLAY_ROTATION_270

#define LV_DISP_RENDER_MODE_PARTIAL LV_DISPLAY_RENDER_MODE_PARTIAL
#define LV_DISP_RENDER_MODE_DIRECT  LV_DISPLAY_RENDER_MODE_DIRECT
#define LV_DISP_RENDER_MODE_FULL    LV_DISPLAY_RENDER_MODE_FULL

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
typedef lv_result_t lv_res_t ;
typedef lv_image_dsc_t lv_img_dsc_t;
typedef lv_display_rotation_t lv_disp_rotation_t;
typedef lv_display_render_mode_t lv_disp_render_t;

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
#define LV_RES_OK         LV_RESULT_OK
#define LV_RES_INV        LV_RESULT_INVALID


#define lv_disp_create                   lv_display_create
#define lv_disp_remove                   lv_display_remove
#define lv_disp_set_default              lv_display_set_default
#define lv_disp_get_default              lv_display_get_default
#define lv_disp_get_next                 lv_display_get_next
#define lv_disp_set_res                  lv_display_set_resolution
#define lv_disp_set_physical_res         lv_display_set_physical_res
#define lv_disp_set_offset               lv_display_set_offset
#define lv_disp_set_rotation             lv_display_set_rotation
#define lv_disp_set_dpi                  lv_display_set_dpi
#define lv_disp_get_hor_res              lv_display_get_horizontal_resolution
#define lv_disp_get_ver_res              lv_display_get_vertical_resolution
#define lv_disp_get_physical_hor_res     lv_display_get_physical_horizontal_resolution
#define lv_disp_get_physical_ver_res     lv_display_get_physical_vertical_resolution
#define lv_disp_get_offset_x             lv_display_get_offset_x
#define lv_disp_get_offset_y             lv_display_get_offset_y
#define lv_disp_get_rotation             lv_display_get_rotation
#define lv_disp_get_dpi                  lv_display_get_dpi
#define lv_disp_set_draw_buffers         lv_display_set_draw_buffers
#define lv_disp_set_flush_cb             lv_display_set_flush_cb
#define lv_disp_set_color_format         lv_display_set_color_format
#define lv_disp_get_color_format         lv_display_get_color_format
#define lv_disp_set_antialiasing         lv_display_set_antialiasing
#define lv_disp_get_antialiasing         lv_display_get_antialiasing
#define lv_disp_flush_ready              lv_display_flush_ready
#define lv_disp_flush_is_last            lv_display_flush_is_last
#define lv_disp_is_double_buffered       lv_display_is_double_buffered
#define lv_disp_get_scr_act              lv_display_get_scr_act
#define lv_disp_get_scr_prev             lv_display_get_scr_prev
#define lv_disp_load_scr                 lv_display_load_scr
#define lv_disp_get_layer_top            lv_display_get_layer_top
#define lv_disp_get_layer_sys            lv_display_get_layer_sys
#define lv_disp_get_layer_bottom         lv_display_get_layer_bottom
#define lv_disp_add_event                lv_display_add_event
#define lv_disp_get_event_count          lv_display_get_event_count
#define lv_disp_get_event_dsc            lv_display_get_event_dsc
#define lv_disp_remove_event             lv_display_remove_event
#define lv_disp_send_event               lv_display_send_event
#define lv_disp_set_theme                lv_display_set_theme
#define lv_disp_get_theme                lv_display_get_theme
#define lv_disp_get_inactive_time        lv_display_get_inactive_time
#define lv_disp_trig_activity            lv_display_trig_activity
#define lv_disp_enable_invalidation      lv_display_enable_invalidation
#define lv_disp_is_invalidation_enabled  lv_display_is_invalidation_enabled
#define lv_disp_set_user_data            lv_display_set_user_data
#define lv_disp_set_driver_data          lv_display_set_driver_data
#define lv_disp_get_user_data            lv_display_get_user_data
#define lv_disp_get_driver_data          lv_display_get_driver_data

#define lv_txt_get_size       lv_text_get_size
#define lv_txt_get_width      lv_text_get_width

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

#define lv_image_set_angle                  lv_image_set_rotation
#define lv_image_get_angle                  lv_image_get_rotation
#define lv_image_set_zoom                   lv_image_set_scale
#define lv_image_get_zoom                   lv_image_get_scale

#define lv_obj_get_style_transform_zoom     lv_obj_get_style_transform_scale
#define lv_obj_get_style_transform_angle    lv_obj_get_style_transform_rotation
#define lv_obj_set_style_transform_zoom     lv_obj_set_style_transform_scale
#define lv_obj_set_style_transform_angle    lv_obj_set_style_transform_rotation
#define lv_style_set_transform_angle        lv_style_set_transform_rotation
#define lv_style_set_transform_zoom         lv_style_set_transform_scale
#define LV_ZOOM_NONE             LV_SCALE_NONE

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
