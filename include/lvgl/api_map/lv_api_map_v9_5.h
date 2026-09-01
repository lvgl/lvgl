/**
 * @file lv_api_map_v9_5.h
 *
 */

#ifndef LV_API_MAP_V9_5_H
#define LV_API_MAP_V9_5_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Shipped in v9.5.0 through lv_api_map_v8.h, where it was an incorrect spelling of v8's
 * `lv_disp_rot_t`. Correcting that in #10095 also removed the only definition of this
 * name, so code written against v9.5.0 no longer compiles. Kept here for compatibility. */
typedef lv_display_rotation_t lv_disp_rotation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define lv_display_delete_event lv_display_remove_event
#define lv_observer_remove lv_observer_delete
#define lv_style_get_prop_inlined lv_style_get_prop
#define lv_obj_is_valid lv_obj_is_in_widget_tree

#define lv_draw_sw_i1_to_argb8888 lv_draw_i1_to_argb8888
#define lv_draw_sw_rgb565_swap lv_draw_rgb565_swap
#define lv_draw_sw_rotate lv_draw_rotate
#define lv_draw_sw_i1_invert lv_draw_i1_invert
#define lv_draw_sw_i1_convert_to_vtiled lv_draw_i1_convert_to_vtiled

#define LV_COLOR_FORMAT_NATIVE LV_COLOR_FORMAT_DEFAULT

#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_I1                       LV_COLOR_FORMAT_I1
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_L8                       LV_COLOR_FORMAT_AL88
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_RGB565                   LV_COLOR_FORMAT_RGB565A8
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_RGB565_SWAPPED           LV_COLOR_FORMAT_RGB565A8
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_RGB888                   LV_COLOR_FORMAT_ARGB8888
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_XRGB8888                 LV_COLOR_FORMAT_ARGB8888
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_ARGB8888                 LV_COLOR_FORMAT_ARGB8888
#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED   LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED

#define LV_COLOR_FORMAT_NATIVE_WITH_ALPHA \
    LV_CONF_PASTE(LV_COLOR_FORMAT_NATIVE_WITH_ALPHA_OF_, LV_COLOR_FORMAT_DEFAULT)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_5_H */
