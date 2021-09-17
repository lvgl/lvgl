/**
 * @file lv_dialog.h
 *
 */

#ifndef LV_DIALOG_H
#define LV_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_DIALOG

/*Testing of dependencies*/
#if LV_USE_BTNMATRIX == 0
#error "lv_dialog: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNMATRIX  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_dialog: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_obj_t * title;
    lv_obj_t * close_btn;
    lv_obj_t * content;
    lv_obj_t * btns;
} lv_dialog_t;

extern const lv_obj_class_t lv_dialog_class;

extern const lv_obj_class_t lv_dialog_backdrop_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a dialog object
 * @param parent        pointer to parent or NULL to create a full screen modal dialog
 * @param title         the title of the dialog
 * @param btn_txts      the buttons as an array of texts terminated by an "" element. E.g. {"btn1", "btn2", ""}
 * @param add_close_btn true: add a close button
 * @return              pointer to the dialog object
 */
lv_obj_t * lv_dialog_create(lv_obj_t * parent, const char * title, const char * btn_txts[], bool add_close_btn);

lv_obj_t * lv_dialog_get_title(lv_obj_t * obj);

lv_obj_t * lv_dialog_get_close_btn(lv_obj_t * obj);

lv_obj_t * lv_dialog_get_content(lv_obj_t * obj);

lv_obj_t * lv_dialog_get_btns(lv_obj_t * obj);

/**
 * Get the index of the selected button
 * @param obj dialog object
 * @return     index of the button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_dialog_get_active_btn(lv_obj_t * obj);

const char * lv_dialog_get_active_btn_text(lv_obj_t * obj);

void lv_dialog_close(lv_obj_t * obj);

void lv_dialog_close_async(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DIALOG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DIALOG_H*/
