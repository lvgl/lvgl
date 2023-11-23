/**
 * @file lv_templ.h
 *
 */

#ifndef LV_TABVIEW_H
#define LV_TABVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_TABVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    uint32_t tab_cur;
    lv_dir_t tab_pos;
} lv_tabview_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tabview_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_tabview_create(lv_obj_t * parent);

lv_obj_t * lv_tabview_add_tab(lv_obj_t * tv, const char * name);

void lv_tabview_rename_tab(lv_obj_t * obj, uint32_t tab_id, const char * new_name);

void lv_tabview_set_active(lv_obj_t * obj, uint32_t id, lv_anim_enable_t anim_en);

void lv_tabview_set_tab_position(lv_obj_t * obj, lv_dir_t dir);

void lv_tabview_set_tab_bar_size(lv_obj_t * obj, int32_t size);

uint32_t lv_tabview_get_tab_count(lv_obj_t * tv);

uint32_t lv_tabview_get_tab_active(lv_obj_t * tv);

uint32_t lv_tabview_get_tab_count(lv_obj_t * tv);

lv_obj_t * lv_tabview_get_content(lv_obj_t * tv);

lv_obj_t * lv_tabview_get_tab_bar(lv_obj_t * tv);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABVIEW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TABVIEW_H*/
