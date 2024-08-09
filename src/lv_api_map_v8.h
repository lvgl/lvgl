/**
 * @file lv_api_map_v8.h
 *
 */

#ifndef LV_API_MAP_V8_H
#define LV_API_MAP_V8_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "misc/lv_types.h"

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

#if LV_USE_BUTTONMATRIX
#define LV_BTNMATRIX_BTN_NONE   LV_BUTTONMATRIX_BUTTON_NONE

#define LV_BTNMATRIX_CTRL_HIDDEN       LV_BUTTONMATRIX_CTRL_HIDDEN
#define LV_BTNMATRIX_CTRL_NO_REPEAT    LV_BUTTONMATRIX_CTRL_NO_REPEAT
#define LV_BTNMATRIX_CTRL_DISABLED     LV_BUTTONMATRIX_CTRL_DISABLED
#define LV_BTNMATRIX_CTRL_CHECKABLE    LV_BUTTONMATRIX_CTRL_CHECKABLE
#define LV_BTNMATRIX_CTRL_CHECKED      LV_BUTTONMATRIX_CTRL_CHECKED
#define LV_BTNMATRIX_CTRL_CLICK_TRIG   LV_BUTTONMATRIX_CTRL_CLICK_TRIG
#define LV_BTNMATRIX_CTRL_POPOVER      LV_BUTTONMATRIX_CTRL_POPOVER
#define LV_BTNMATRIX_CTRL_CUSTOM_1     LV_BUTTONMATRIX_CTRL_CUSTOM_1
#define LV_BTNMATRIX_CTRL_CUSTOM_2     LV_BUTTONMATRIX_CTRL_CUSTOM_2
#endif /* LV_USE_BUTTONMATRIX */

/**********************
 *      TYPEDEFS
 **********************/
typedef int32_t                     lv_coord_t;
typedef lv_result_t                 lv_res_t;
typedef lv_image_dsc_t              lv_img_dsc_t;
typedef lv_display_t                lv_disp_t;
typedef lv_display_rotation_t       lv_disp_rotation_t;
typedef lv_display_render_mode_t    lv_disp_render_t;
typedef lv_anim_completed_cb_t      lv_anim_ready_cb_t;
typedef lv_screen_load_anim_t       lv_scr_load_anim_t;

#if LV_USE_BUTTONMATRIX
typedef lv_buttonmatrix_ctrl_t      lv_btnmatrix_ctrl_t;
#endif /* LV_USE_BUTTONMATRIX */

#if LV_USE_IMAGEBUTTON
#define LV_IMGBTN_STATE_RELEASED         LV_IMAGEBUTTON_STATE_RELEASED
#define LV_IMGBTN_STATE_PRESSED          LV_IMAGEBUTTON_STATE_PRESSED
#define LV_IMGBTN_STATE_DISABLED         LV_IMAGEBUTTON_STATE_DISABLED
#define LV_IMGBTN_STATE_CHECKED_RELEASED LV_IMAGEBUTTON_STATE_CHECKED_RELEASED
#define LV_IMGBTN_STATE_CHECKED_PRESSED  LV_IMAGEBUTTON_STATE_CHECKED_PRESSED
#define LV_IMGBTN_STATE_CHECKED_DISABLED LV_IMAGEBUTTON_STATE_CHECKED_DISABLED
#endif /* LV_USE_IMAGEBUTTON */

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

    lv_obj_move_to_index(obj, lv_obj_get_child_count(parent) - 1);
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

#define LV_INDEV_STATE_PR   LV_INDEV_STATE_PRESSED
#define LV_INDEV_STATE_REL  LV_INDEV_STATE_RELEASED

#define lv_obj_del                       lv_obj_delete
#define lv_obj_del_async                 lv_obj_delete_async
#define lv_obj_clear_flag                lv_obj_remove_flag
#define lv_obj_clear_state               lv_obj_remove_state

#define lv_indev_set_disp                lv_indev_set_display
#define lv_indev_get_act                 lv_indev_active
#define lv_scr_act                       lv_screen_active
#define lv_disp_remove                   lv_display_delete
#define lv_disp_set_default              lv_display_set_default
#define lv_disp_get_default              lv_display_get_default
#define lv_disp_get_next                 lv_display_get_next
#define lv_disp_set_rotation             lv_display_set_rotation
#define lv_disp_get_hor_res              lv_display_get_horizontal_resolution
#define lv_disp_get_ver_res              lv_display_get_vertical_resolution
#define lv_disp_get_physical_hor_res     lv_display_get_physical_horizontal_resolution
#define lv_disp_get_physical_ver_res     lv_display_get_physical_vertical_resolution
#define lv_disp_get_offset_x             lv_display_get_offset_x
#define lv_disp_get_offset_y             lv_display_get_offset_y
#define lv_disp_get_rotation             lv_display_get_rotation
#define lv_disp_get_dpi                  lv_display_get_dpi
#define lv_disp_get_antialiasing         lv_display_get_antialiasing
#define lv_disp_flush_ready              lv_display_flush_ready
#define lv_disp_flush_is_last            lv_display_flush_is_last
#define lv_disp_get_scr_act              lv_display_get_screen_active
#define lv_disp_get_scr_prev             lv_display_get_screen_prev
#define lv_disp_load_scr                 lv_screen_load
#define lv_scr_load                      lv_screen_load
#define lv_scr_load_anim                 lv_screen_load_anim
#define lv_disp_get_layer_top            lv_display_get_layer_top
#define lv_disp_get_layer_sys            lv_display_get_layer_sys
#define lv_disp_send_event               lv_display_send_event
#define lv_disp_set_theme                lv_display_set_theme
#define lv_disp_get_theme                lv_display_get_theme
#define lv_disp_get_inactive_time        lv_display_get_inactive_time
#define lv_disp_trig_activity            lv_display_trigger_activity
#define lv_disp_enable_invalidation      lv_display_enable_invalidation
#define lv_disp_is_invalidation_enabled  lv_display_is_invalidation_enabled
#define lv_disp_refr_timer              lv_display_refr_timer
#define lv_disp_get_refr_timer          lv_display_get_refr_timer

#define lv_timer_del                    lv_timer_delete

#define lv_anim_del                     lv_anim_delete
#define lv_anim_del_all                 lv_anim_delete_all
#define lv_anim_set_ready_cb            lv_anim_set_completed_cb

#define lv_group_del                    lv_group_delete

#if LV_USE_TEXTAREA
#define lv_txt_get_size       lv_text_get_size
#define lv_txt_get_width      lv_text_get_width
#endif /* LV_USE_TEXTAREA */

#if LV_USE_IMAGE
#define lv_img_create         lv_image_create
#define lv_img_set_src        lv_image_set_src
#define lv_img_set_offset_x   lv_image_set_offset_x
#define lv_img_set_offset_y   lv_image_set_offset_y
#define lv_img_set_angle      lv_image_set_rotation
#define lv_img_set_pivot      lv_image_set_pivot
#define lv_img_set_zoom       lv_image_set_scale
#define lv_img_set_antialias  lv_image_set_antialias
#define lv_img_get_src        lv_image_get_src
#define lv_img_get_offset_x   lv_image_get_offset_x
#define lv_img_get_offset_y   lv_image_get_offset_y
#define lv_img_get_angle      lv_image_get_rotation
#define lv_img_get_pivot      lv_image_get_pivot
#define lv_img_get_zoom       lv_image_get_scale
#define lv_img_get_antialias  lv_image_get_antialias
#endif /* LV_USE_IMAGE */

#if LV_USE_IMAGEBUTTON
#define lv_imgbtn_create         lv_imagebutton_create
#define lv_imgbtn_set_src        lv_imagebutton_set_src
#define lv_imgbtn_set_state      lv_imagebutton_set_state
#define lv_imgbtn_get_src_left   lv_imagebutton_get_src_left
#define lv_imgbtn_get_src_middle lv_imagebutton_get_src_middle
#define lv_imgbtn_get_src_right  lv_imagebutton_get_src_right
#endif /* LV_USE_IMAGEBUTTON */

#if LV_USE_LIST
#define lv_list_set_btn_text lv_list_set_button_text
#define lv_list_get_btn_text lv_list_get_button_text
#define lv_list_add_btn      lv_list_add_button
#endif /* LV_USE_LIST */

#if LV_USE_BUTTON
#define lv_btn_create        lv_button_create
#endif /* LV_USE_BUTTON */

#if LV_USE_BUTTONMATRIX
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
#define lv_btnmatrix_get_btn_text            lv_buttonmatrix_get_button_text
#define lv_btnmatrix_has_button_ctrl         lv_buttonmatrix_has_button_ctrl
#define lv_btnmatrix_get_one_checked         lv_buttonmatrix_get_one_checked
#endif /* LV_USE_BUTTONMATRIX */

#if LV_USE_TABVIEW
#define lv_tabview_get_tab_btns              lv_tabview_get_tab_bar
#define lv_tabview_get_tab_act               lv_tabview_get_tab_active
#define lv_tabview_set_act                   lv_tabview_set_active
#endif /* LV_USE_TABVIEW */

#if LV_USE_TILEVIEW
#define lv_tileview_get_tile_act             lv_tileview_get_tile_active
#define lv_obj_set_tile_id                   lv_tileview_set_tile_by_index
#define lv_obj_set_tile                      lv_tileview_set_tile
#endif /* LV_USE_TILEVIEW */

#if LV_USE_ROLLER
#define lv_roller_set_visible_row_cnt       lv_roller_set_visible_row_count
#define lv_roller_get_option_cnt            lv_roller_get_option_count
#endif /* LV_USE_ROLLER */

#if LV_USE_TABLE
#define lv_table_set_col_cnt                lv_table_set_column_count
#define lv_table_set_row_cnt                lv_table_set_row_count
#define lv_table_get_col_cnt                lv_table_get_column_count
#define lv_table_get_row_cnt                lv_table_get_row_count
#define lv_table_set_col_width              lv_table_set_column_width
#define lv_table_get_col_width              lv_table_get_column_width
#endif /* LV_USE_TABLE */

#if LV_USE_DROPDOWN
#define lv_dropdown_get_option_cnt          lv_dropdown_get_option_count
#endif /* LV_USE_DROPDOWN */

#define lv_obj_get_child_cnt                lv_obj_get_child_count
#define lv_obj_get_disp                     lv_obj_get_display
#define lv_obj_delete_anim_ready_cb         lv_obj_delete_anim_completed_cb

#define LV_STYLE_ANIM_TIME            LV_STYLE_ANIM_DURATION
#define LV_STYLE_IMG_OPA              LV_STYLE_IMAGE_OPA
#define LV_STYLE_IMG_RECOLOR          LV_STYLE_IMAGE_RECOLOR
#define LV_STYLE_IMG_RECOLOR_OPA      LV_STYLE_IMAGE_RECOLOR_OPA
#define LV_STYLE_SHADOW_OFS_X         LV_STYLE_SHADOW_OFFSET_X
#define LV_STYLE_SHADOW_OFS_Y         LV_STYLE_SHADOW_OFFSET_Y
#define LV_STYLE_TRANSFORM_ANGLE      LV_STYLE_TRANSFORM_ROTATION

#define lv_obj_get_style_anim_time               lv_obj_get_style_anim_duration
#define lv_obj_get_style_img_opa                 lv_obj_get_style_image_opa
#define lv_obj_get_style_img_recolor             lv_obj_get_style_image_recolor
#define lv_obj_get_style_img_recolor_filtered    lv_obj_get_style_image_recolor_filtered
#define lv_obj_get_style_img_recolor_opa         lv_obj_get_style_image_recolor_opa
#define lv_obj_get_style_shadow_ofs_x            lv_obj_get_style_shadow_offset_x
#define lv_obj_get_style_shadow_ofs_y            lv_obj_get_style_shadow_offset_y
#define lv_obj_get_style_transform_angle         lv_obj_get_style_transform_rotation
#define lv_obj_get_style_bg_img_src              lv_obj_get_style_bg_image_src
#define lv_obj_get_style_bg_img_recolor          lv_obj_get_style_bg_image_recolor
#define lv_obj_get_style_bg_img_recolor_opa      lv_obj_get_style_bg_image_recolor_opa

#define lv_obj_set_style_anim_time               lv_obj_set_style_anim_duration
#define lv_obj_set_style_img_opa                 lv_obj_set_style_image_opa
#define lv_obj_set_style_img_recolor             lv_obj_set_style_image_recolor
#define lv_obj_set_style_img_recolor_opa         lv_obj_set_style_image_recolor_opa
#define lv_obj_set_style_shadow_ofs_x            lv_obj_set_style_shadow_offset_x
#define lv_obj_set_style_shadow_ofs_y            lv_obj_set_style_shadow_offset_y
#define lv_obj_set_style_transform_zoom          lv_obj_set_style_transform_scale
#define lv_obj_set_style_transform_angle         lv_obj_set_style_transform_rotation
#define lv_obj_set_style_bg_img_src              lv_obj_set_style_bg_image_src
#define lv_obj_set_style_bg_img_recolor          lv_obj_set_style_bg_image_recolor
#define lv_obj_set_style_bg_img_recolor_opa      lv_obj_set_style_bg_image_recolor_opa

#define lv_style_set_anim_time                   lv_style_set_anim_duration
#define lv_style_set_img_opa                     lv_style_set_image_opa
#define lv_style_set_img_recolor                 lv_style_set_image_recolor
#define lv_style_set_img_recolor_opa             lv_style_set_image_recolor_opa
#define lv_style_set_shadow_ofs_x                lv_style_set_shadow_offset_x
#define lv_style_set_shadow_ofs_y                lv_style_set_shadow_offset_y
#define lv_style_set_transform_angle             lv_style_set_transform_rotation
#define lv_style_set_transform_zoom              lv_style_set_transform_scale
#define lv_style_set_bg_img_src                  lv_style_set_bg_image_src
#define lv_style_set_bg_img_recolor              lv_style_set_bg_image_recolor
#define lv_style_set_bg_img_recolor_opa          lv_style_set_bg_image_recolor_opa

#if LV_USE_KEYBOARD
#define lv_keyboard_get_selected_btn             lv_keyboard_get_selected_button
#define lv_keyboard_get_btn_text                 lv_keyboard_get_button_text
#endif /* LV_USE_KEYBOARD */

#define LV_ZOOM_NONE                        LV_SCALE_NONE

#define lv_image_decoder_built_in_open      lv_bin_decoder_open
#define lv_image_decoder_built_in_close     lv_bin_decoder_close

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

#endif /*LV_API_MAP_V8_H*/
