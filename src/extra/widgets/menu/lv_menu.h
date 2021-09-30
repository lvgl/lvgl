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
#include "../../../core/lv_obj.h"

#define LV_USE_MENU 1
#if LV_USE_MENU

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_MENU_SECTION_OFF,
    LV_MENU_SECTION_ON
};
typedef uint8_t lv_menu_item_section_type_t;

enum {
    LV_MENU_MODE_HEADER_TOP_FIXED, /* Header is positioned at the top */
    LV_MENU_MODE_HEADER_TOP_UNFIXED, /* Header is positioned at the top and can be scrolled out of view*/
    LV_MENU_MODE_HEADER_BOTTOM_FIXED /* Header is positioned at the bottom */
};
typedef uint8_t lv_menu_mode_header_t;

enum {
    LV_MENU_MODE_ROOT_BACK_BTN_DISABLED,
    LV_MENU_MODE_ROOT_BACK_BTN_ENABLED
};
typedef uint8_t lv_menu_mode_root_back_btn_t;

enum {
    LV_MENU_MODE_SIDEBAR_DISABLED,
    LV_MENU_MODE_SIDEBAR_ENABLED
};
typedef uint8_t lv_menu_mode_sidebar_t;

typedef struct lv_menu_item_t lv_menu_item_t;

typedef struct lv_menu_item_t {
    lv_obj_t * obj;
    lv_menu_item_t ** items;
    uint32_t child_cnt;
    lv_menu_item_section_type_t section : 1;
} lv_menu_item_t;

typedef struct {
    lv_obj_t obj;
    lv_obj_t * menu;
    lv_menu_item_t * menu_item;
    lv_menu_mode_sidebar_t mode_sidebar : 1;
} lv_menu_cont_t;

typedef struct {
    lv_menu_item_t * menu_item;
} lv_menu_history_t;

typedef struct {
    lv_obj_t obj;
    lv_obj_t * storage;
    lv_obj_t * main;
    lv_obj_t * main_content;
    lv_obj_t * main_header;
    lv_obj_t * main_header_back_btn; /* a pointer to obj that on click triggers back btn event handler, can be same as 'main_header' */
    lv_obj_t * sidebar;
    lv_obj_t * sidebar_content;
    lv_obj_t * sidebar_header;
    lv_obj_t * sidebar_header_back_btn; /* a pointer to obj that on click triggers back btn event handler, can be same as 'sidebar_header' */
    lv_obj_t * selected_tab;
    lv_ll_t history_ll;
    uint8_t cur_depth;
    uint8_t prev_depth;
    uint8_t sidebar_generated : 1;
    lv_menu_mode_header_t mode_header : 2;
    lv_menu_mode_root_back_btn_t mode_root_back_btn : 1;
    lv_menu_mode_sidebar_t mode_sidebar : 1;
} lv_menu_t;

extern const lv_obj_class_t lv_menu_class;
extern const lv_obj_class_t lv_menu_cont_class;
extern const lv_obj_class_t lv_menu_section_class;
extern const lv_obj_class_t lv_menu_seperator_class;
extern const lv_obj_class_t lv_menu_sidebar_cont_class;
extern const lv_obj_class_t lv_menu_main_cont_class;
extern const lv_obj_class_t lv_menu_sidebar_header_cont_class;
extern const lv_obj_class_t lv_menu_main_header_cont_class;
extern const lv_obj_class_t lv_menu_sidebar_content_cont_class;
extern const lv_obj_class_t lv_menu_main_content_cont_class;
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
 * Create a menu cont object
 * @param parent pointer to an object, it will be the parent of the new menu cont object
 * @return pointer to the created menu cont object
 */
lv_obj_t * lv_menu_cont_create(lv_obj_t * parent);

/**
 * Create a menu item
 * @return pointer to the created menu item
 */
lv_menu_item_t * lv_menu_item_create();

/**
 * Refresh menu after adding/removing menu items
 * @param obj pointer to the menu
 */
void lv_menu_refr(lv_obj_t * obj);

/*=====================
 * Setter functions
 *====================*/
/**
 * Set menu item to display
 * @param obj pointer to the menu
 * @param menu_item pointer to the menu item to set
 */
void lv_menu_set(lv_obj_t * obj, lv_menu_item_t * menu_item);

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
 * Set whether to display sidebar
 * @param obj pointer to a menu
 * @param mode_sidebar
 */
void lv_menu_set_mode_sidebar(lv_obj_t * obj, lv_menu_mode_sidebar_t mode_sidebar);

/**
 * Set main header back btn obj that on click triggers back btn event handler
 * @param menu pointer to the menu
 * @param obj pointer to the obj
 */
void lv_menu_set_main_header_back_btn(lv_obj_t * menu, lv_obj_t * obj);

/**
 * Set sidebar header back btn obj that on click triggers back btn event handler
 * @param menu pointer to the menu
 * @param obj pointer to the obj
 */
void lv_menu_set_sidebar_header_back_btn(lv_obj_t * menu, lv_obj_t * obj);

/**
 * Deletes the provided menu item
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 */
void lv_menu_item_del(lv_obj_t * menu, lv_menu_item_t * menu_item);

/**
 * Deletes the provided menu item to delete recursively
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 */
void lv_menu_item_del_recursive(lv_obj_t * menu, lv_menu_item_t * menu_item);

/**
 * Remove the provided obj from menu item and deletes it
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param obj pointer to the obj to be removed
 * @return true if obj was removed
 */
bool lv_menu_item_remove_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj);

 /**
 * Remove the provided menu from menu item and the menu item is not freed
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param menu_item_remove pointer to the menu item to be removed
 * @return true if menu item was removed
 */
bool lv_menu_item_remove_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_t * menu_item_remove);

/**
 * Set obj that represents the menu item
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param obj pointer to the obj, must be of type lv_menu_cont_class
 */
void lv_menu_item_set_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj);

/**
 * Add obj to the menu item
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param section
 * @param obj pointer to the new obj to be added
 */
void lv_menu_item_add_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                lv_obj_t * obj);

/**
 * Add menu to the menu item
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param section
 * @param new_menu_item pointer to the new menu to be added
 */
void lv_menu_item_add_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 lv_menu_item_t * new_menu_item);

/**
 * Add seperator obj to the menu item
 * @param menu pointer to the menu
 * @param menu_item pointer to the menu item
 * @param section
 * @return seperator obj
 */
lv_obj_t * lv_menu_item_add_seperator(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section);

/*=====================
 * Getter functions
 *====================*/
 /**
 * Get a pointer to menu item that is currently displayed in main content
 * @param obj pointer to the menu
 * @return pointer to menu_item
 */
lv_menu_item_t * lv_menu_get_main_item(lv_obj_t * obj);

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
 * Check if a obj is a root back btn
 * @param menu pointer to the menu
 * @return true if it is a root back btn
 */
bool lv_menu_item_back_btn_is_root(lv_obj_t * menu, lv_obj_t * obj);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MENU*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MENU_H*/
