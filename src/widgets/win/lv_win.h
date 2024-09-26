/**
 * @file lv_win.h
 *
 */

#ifndef LV_WIN_H
#define LV_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"
#if LV_USE_WIN
/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_win_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a window widget
 * @param parent    pointer to a parent widget
 * @return          the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * parent);

/**
 * Add a title to the window
 * @param obj       pointer to a window widget
 * @param txt       the text of the title
 * @return          the widget where the content of the title can be created
 */
lv_obj_t * lv_win_add_title(lv_obj_t * win, const char * txt);

/**
 * Add a button to the window
 * @param obj       pointer to a window widget
 * @param icon      an icon to be displayed on the button
 * @param btn_w     width of the button
 * @return          the widget where the content of the button can be created
 */
lv_obj_t * lv_win_add_button(lv_obj_t * win, const void * icon, int32_t btn_w);

/**
 * Get the header of the window
 * @param win       pointer to a window widget
 * @return          the header of the window
 */
lv_obj_t * lv_win_get_header(lv_obj_t * win);

/**
 * Get the content of the window
 * @param win       pointer to a window widget
 * @return          the content of the window
 */
lv_obj_t * lv_win_get_content(lv_obj_t * win);
/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_WIN*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WIN_H*/
