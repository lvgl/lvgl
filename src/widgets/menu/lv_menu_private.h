/**
 * @file lv_menu_private.h
 *
 */

#ifndef LV_MENU_PRIVATE_H
#define LV_MENU_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_menu.h"

#if LV_USE_MENU

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_menu_load_page_event_data_t {
    lv_obj_t * menu;
    lv_obj_t * page;
};

struct lv_menu_history_t {
    lv_obj_t * page;
};

struct lv_menu_t {
    lv_obj_t obj;
    lv_obj_t * storage; /**<  a pointer to obj that is the parent of all pages not displayed */
    lv_obj_t * main;
    lv_obj_t * main_page;
    lv_obj_t * main_header;
    lv_obj_t *
    main_header_back_btn; /**<  a pointer to obj that on click triggers back btn event handler, can be same as 'main_header' */
    lv_obj_t * main_header_title;
    lv_obj_t * sidebar;
    lv_obj_t * sidebar_page;
    lv_obj_t * sidebar_header;
    lv_obj_t *
    sidebar_header_back_btn; /**<  a pointer to obj that on click triggers back btn event handler, can be same as 'sidebar_header' */
    lv_obj_t * sidebar_header_title;
    lv_obj_t * selected_tab;
    lv_ll_t history_ll;
    uint8_t cur_depth;
    uint8_t prev_depth;
    uint8_t sidebar_generated : 1;
    lv_menu_mode_header_t mode_header : 2;
    lv_menu_mode_root_back_button_t mode_root_back_btn : 1;
};

struct lv_menu_page_t {
    lv_obj_t obj;
    char  *  title;
    bool     static_title;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_MENU */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MENU_PRIVATE_H*/
