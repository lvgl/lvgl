/**
 * @file lv_api_map_v9_4.h
 *
 */

#ifndef LV_API_MAP_V9_4_H
#define LV_API_MAP_V9_4_H

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define lv_tabview_rename_tab lv_tabview_set_tab_text
#define lv_wayland_timer_handler    lv_timer_handler
#define lv_wayland_display_close_f_t    lv_wayland_display_close_cb_t
#define lv_xml_component_unregister lv_xml_unregister_component
#define lv_xml_test_unregister lv_xml_unregister_test

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_4_H */
