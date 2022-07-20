/**
 * @file lv_menu.h
 *
 */

#ifndef LV_MENU_H
#define LV_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_MENU

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_MENU_HEADER_TOP_FIXED, /* Header is positioned at the top */
    LV_MENU_HEADER_TOP_UNFIXED, /* Header is positioned at the top and can be scrolled out of view*/
    LV_MENU_HEADER_BOTTOM_FIXED /* Header is positioned at the bottom */
};
typedef uint8_t lv_menu_mode_header_t;

enum {
    LV_MENU_ROOT_BACK_BTN_DISABLED,
    LV_MENU_ROOT_BACK_BTN_ENABLED
};
typedef uint8_t lv_menu_mode_root_back_btn_t;

typedef struct lv_menu_load_page_event_data_t {
    lv_obj_t * menu;
    lv_obj_t * page;
} lv_menu_load_page_event_data_t;

typedef struct {
    lv_obj_t * page;
} lv_menu_history_t;

typedef struct {
    lv_obj_t obj;
    lv_obj_t * storage; /* a pointer to obj that is the parent of all pages not displayed */
    lv_obj_t * main;
    lv_obj_t * main_page;
    lv_obj_t * main_header;
    lv_obj_t *
    main_header_back_btn; /* a pointer to obj that on click triggers back btn event handler, can be same as 'main_header' */
    lv_obj_t * main_header_title;
    lv_obj_t * sidebar;
    lv_obj_t * sidebar_page;
    lv_obj_t * sidebar_header;
    lv_obj_t *
    sidebar_header_back_btn; /* a pointer to obj that on click triggers back btn event handler, can be same as 'sidebar_header' */
    lv_obj_t * sidebar_header_title;
    lv_obj_t * selected_tab;
    lv_ll_t history_ll;
    uint8_t cur_depth;
    uint8_t prev_depth;
    uint8_t sidebar_generated : 1;
    lv_menu_mode_header_t mode_header : 2;
    lv_menu_mode_root_back_btn_t mode_root_back_btn : 1;
} lv_menu_t;

typedef struct {
    lv_obj_t obj;
    char * title;
} lv_menu_page_t;

extern const lv_obj_class_t lv_menu_class;
extern const lv_obj_class_t lv_menu_page_class;
extern const lv_obj_class_t lv_menu_cont_class;
extern const lv_obj_class_t lv_menu_section_class;
extern const lv_obj_class_t lv_menu_separator_class;
extern const lv_obj_class_t lv_menu_sidebar_cont_class;
extern const lv_obj_class_t lv_menu_main_cont_class;
extern const lv_obj_class_t lv_menu_sidebar_header_cont_class;
extern const lv_obj_class_t lv_menu_main_header_cont_class;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a menu object
 * @param parent pointer to an object, it will be the parent of the new menu
 * @return pointer to the created menu
 */
lv_obj_t * lv_menu_create(lv_obj_t * parent);

/**
 * Create a menu page object
 * @param parent pointer to menu object
 * @param title pointer to text for title in header (NULL to not display title)
 * @return pointer to the created menu page
 */
lv_obj_t * lv_menu_page_create(lv_obj_t * parent, char * title);

/**
 * Create a menu cont object
 * @param parent pointer to an object, it will be the parent of the new menu cont object
 * @return pointer to the created menu cont
 */
lv_obj_t * lv_menu_cont_create(lv_obj_t * parent);

/**
 * Create a menu section object
 * @param parent pointer to an object, it will be the parent of the new menu section object
 * @return pointer to the created menu section
 */
lv_obj_t * lv_menu_section_create(lv_obj_t * parent);

/**
 * Create a menu separator object
 * @param parent pointer to an object, it will be the parent of the new menu separator object
 * @return pointer to the created menu separator
 */
lv_obj_t * lv_menu_separator_create(lv_obj_t * parent);
/*=====================
 * Setter functions
 *====================*/
/**
 * Set menu page to display in main
 * @param obj pointer to the menu
 * @param page pointer to the menu page to set (NULL to clear main and clear menu history)
 */
void lv_menu_set_page(lv_obj_t * obj, lv_obj_t * page);

/**
 * Set menu page to display in sidebar
 * @param obj pointer to the menu
 * @param page pointer to the menu page to set (NULL to clear sidebar)
 */
void lv_menu_set_sidebar_page(lv_obj_t * obj, lv_obj_t * page);

/**
 * Set the how the header should behave and its position
 * @param obj pointer to a menu
 * @param mode_header
 */
void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode_header);

/**
 * Set whether back button should appear at root
 * @param obj pointer to a menu
 * @param mode_root_back_btn
 */
void lv_menu_set_mode_root_back_btn(lv_obj_t * obj, lv_menu_mode_root_back_btn_t mode_root_back_btn);

/**
 * Add menu to the menu item
 * @param menu pointer to the menu
 * @param obj pointer to the obj
 * @param page pointer to the page to load when obj is clicked
 */
void lv_menu_set_load_page_event(lv_obj_t * menu, lv_obj_t * obj, lv_obj_t * page);

/*=====================
 * Getter functions
 *====================*/
/**
* Get a pointer to menu page that is currently displayed in main
* @param obj pointer to the menu
* @return pointer to current page
*/
lv_obj_t * lv_menu_get_cur_main_page(lv_obj_t * obj);

/**
* Get a pointer to menu page that is currently displayed in sidebar
* @param obj pointer to the menu
* @return pointer to current page
*/
lv_obj_t * lv_menu_get_cur_sidebar_page(lv_obj_t * obj);

/**
* Get a pointer to main header obj
* @param obj pointer to the menu
* @return pointer to main header obj
*/
lv_obj_t * lv_menu_get_main_header(lv_obj_t * obj);

/**
* Get a pointer to main header back btn obj
* @param obj pointer to the menu
* @return pointer to main header back btn obj
*/
lv_obj_t * lv_menu_get_main_header_back_btn(lv_obj_t * obj);

/**
* Get a pointer to sidebar header obj
* @param obj pointer to the menu
* @return pointer to sidebar header obj
*/
lv_obj_t * lv_menu_get_sidebar_header(lv_obj_t * obj);

/**
* Get a pointer to sidebar header obj
* @param obj pointer to the menu
* @return pointer to sidebar header back btn obj
*/
lv_obj_t * lv_menu_get_sidebar_header_back_btn(lv_obj_t * obj);

/**
 * Check if an obj is a root back btn
 * @param menu pointer to the menu
 * @return true if it is a root back btn
 */
bool lv_menu_back_btn_is_root(lv_obj_t * menu, lv_obj_t * obj);

/**
 * Clear menu history
 * @param obj pointer to the menu
 */
void lv_menu_clear_history(lv_obj_t * obj);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MENU*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MENU_H*/
