/**
 * @file lv_mbox.h
 *
 */

#ifndef LV_MSGBOX_H
#define LV_MSGBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_MSGBOX

/*Testing of dependencies*/
#if LV_USE_BTNMATRIX == 0
#error "lv_mbox: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNMATRIX  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_mbox: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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
    lv_obj_t * text;
    lv_obj_t * btns;
} lv_msgbox_t;

extern const lv_obj_class_t lv_msgbox_class;

extern const lv_obj_class_t lv_msgbox_backdrop_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a message box object
 * @param parent        pointer to parent or NULL to create a full screen modal message box
 * @param title         the title of the message box
 * @param txt           the text of the message box
 * @param btn_txts      the buttons as an array of texts terminated by an "" element. E.g. {"btn1", "btn2", ""}
 * @param add_close_btn true: add a close button
 * @return              pointer to the message box object
 */
lv_obj_t * lv_msgbox_create(lv_obj_t * parent, const char * title, const char * txt, const char * btn_txts[],
                            bool add_close_btn);

lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj);

lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * obj);

lv_obj_t * lv_msgbox_get_text(lv_obj_t * obj);

lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj);

lv_obj_t * lv_msgbox_get_btns(lv_obj_t * obj);

/**
 * Get the index of the selected button
 * @param mbox message box object
 * @return     index of the button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_msgbox_get_active_btn(lv_obj_t * mbox);

const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox);

void lv_msgbox_close(lv_obj_t * mbox);

void lv_msgbox_close_async(lv_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSGBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MSGBOX_H*/
