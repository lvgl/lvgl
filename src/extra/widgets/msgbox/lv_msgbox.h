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
extern const lv_obj_class_t lv_msgbox;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created message box
 */
lv_obj_t * lv_msgbox_create(const char * title, const char * txt, const char * btn_txts[], bool add_close_btn);

lv_obj_t * lv_msgbox_get_title(lv_obj_t * mbox);

lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * mbox);

lv_obj_t * lv_msgbox_get_text(lv_obj_t * mbox);

lv_obj_t * lv_msgbox_get_btns(lv_obj_t * mbox);

const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox);

void lv_msgbox_close(lv_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSGBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MSGBOX_H*/
