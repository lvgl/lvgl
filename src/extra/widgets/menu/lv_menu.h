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
    LV_MENU_MODE_HEADER_NOT_FIXED,
    LV_MENU_MODE_HEADER_FIXED
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

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

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
    lv_menu_mode_root_back_btn_t mode_root_back_btn : 1;
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
    lv_obj_t * main_back_btn;
    lv_obj_t * sidebar;
    lv_obj_t * sidebar_content;
    lv_obj_t * sidebar_header;
    lv_obj_t * sidebar_back_btn;
    lv_obj_t * selected_tab;
    lv_ll_t history_ll;
    lv_menu_mode_header_t mode_header : 1;
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

lv_obj_t * lv_menu_create(lv_obj_t * parent);
void lv_menu_set(lv_obj_t * obj, lv_menu_item_t * menu_item);
void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode_header);
void lv_menu_set_mode_root_back_btn(lv_obj_t * obj, lv_menu_mode_root_back_btn_t mode_root_back_btn);
void lv_menu_set_mode_sidebar(lv_obj_t * obj, lv_menu_mode_sidebar_t mode_sidebar);
void lv_menu_set_main_back_btn_text(lv_obj_t * obj, const char * icon, const char * txt);
void lv_menu_set_sidebar_back_btn_text(lv_obj_t * obj, const char * icon, const char * txt);

/* Core menu item functions */
lv_menu_item_t * lv_menu_item_create();
void lv_menu_item_del(lv_obj_t * menu, lv_menu_item_t * menu_item);
void lv_menu_item_del_recursive(lv_obj_t * menu, lv_menu_item_t * menu_item);
lv_obj_t * lv_menu_item_set_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj);
lv_obj_t * lv_menu_item_add_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                lv_obj_t * obj);
bool lv_menu_item_remove_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj);
lv_obj_t * lv_menu_item_add_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 lv_menu_item_t * new_menu_item);
bool lv_menu_item_remove_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_t * menu_item_remove);
void lv_menu_refresh(lv_obj_t * obj);
bool lv_menu_item_back_btn_is_root(lv_obj_t * menu, lv_obj_t * obj);
lv_obj_t * lv_menu_item_create_text(lv_obj_t * parent, const char * icon, const char * txt,
                                        lv_menu_builder_variant_t builder_variant);
lv_obj_t * lv_menu_cont_create(lv_obj_t * parent);

/* Extra convenience menu item functions */
lv_obj_t * lv_menu_item_set_text(lv_obj_t * menu, lv_menu_item_t * menu_item, const char * icon, const  char * txt);
lv_obj_t * lv_menu_item_add_text(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 const char * icon, const char * txt);
lv_obj_t * lv_menu_item_add_btn(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                const char * icon, const char * txt);
lv_obj_t * lv_menu_item_add_seperator(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section);

#if LV_USE_SLIDER
lv_obj_t * lv_menu_item_add_slider(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
#endif

#if LV_USE_SWITCH
lv_obj_t * lv_menu_item_add_switch(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, bool chk);
#endif
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MENU*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MENU_H*/