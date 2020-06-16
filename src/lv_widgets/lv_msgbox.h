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
#include "../lv_conf_internal.h"

#if LV_USE_MSGBOX != 0

/*Testing of dependencies*/
#if LV_USE_CONT == 0
#error "lv_mbox: lv_cont is required. Enable it in lv_conf.h (LV_USE_CONT  1) "
#endif

#if LV_USE_BTNMATRIX == 0
#error "lv_mbox: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNMATRIX  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_mbox: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_cont.h"
#include "lv_btnmatrix.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of message box*/
typedef struct {
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * text; /*Text of the message box*/
    lv_obj_t * btnm; /*Button matrix for the buttons*/
#if LV_USE_ANIMATION
    uint16_t anim_time; /*Duration of close animation [ms] (0: no animation)*/
#endif
} lv_msgbox_ext_t;

/** Message box styles. */
enum {
    LV_MSGBOX_PART_BG = LV_CONT_PART_MAIN,

    LV_MSGBOX_PART_BTN_BG = _LV_CONT_PART_REAL_LAST,
    LV_MSGBOX_PART_BTN,
};
typedef uint8_t lv_msgbox_style_t;

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
lv_obj_t * lv_msgbox_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 */
void lv_msgbox_add_btns(lv_obj_t * mbox, const char * btn_mapaction[]);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_msgbox_set_text(lv_obj_t * mbox, const char * txt);

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void lv_msgbox_set_anim_time(lv_obj_t * mbox, uint16_t anim_time);

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_msgbox_start_auto_close(lv_obj_t * mbox, uint16_t delay);

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_msgbox_stop_auto_close(lv_obj_t * mbox);

/**
 * Set whether recoloring is enabled. Must be called after `lv_msgbox_add_btns`.
 * @param mbox pointer to message box object
 * @param en whether recoloring is enabled
 */
void lv_msgbox_set_recolor(lv_obj_t * mbox, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_msgbox_get_text(const lv_obj_t * mbox);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param mbox pointer to message box object
 * @return  index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_msgbox_get_active_btn(lv_obj_t * mbox);

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param mbox pointer to message box object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox);

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_msgbox_get_anim_time(const lv_obj_t * mbox);

/**
 * Get whether recoloring is enabled
 * @param mbox pointer to a message box object
 * @return whether recoloring is enabled
 */
bool lv_msgbox_get_recolor(const lv_obj_t * mbox);

/**
 * Get message box button matrix
 * @param mbox pointer to a message box object
 * @return pointer to button matrix object
 * @remarks return value will be NULL unless `lv_msgbox_add_btns` has been already called
 */
lv_obj_t * lv_msgbox_get_btnmatrix(lv_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSGBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MSGBOX_H*/
