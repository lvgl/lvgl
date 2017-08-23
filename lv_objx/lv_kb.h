/**
 * @file lv_kb.h
 * 
 */

#ifndef LV_KB_H
#define LV_KB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_KB != 0

#include "../lv_obj/lv_obj.h"
#include "lv_btnm.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_KB_MODE_TXT,
    LV_KB_MODE_NUM,
}lv_kb_mode_t;


/*Data of keyboard*/
typedef struct {
    lv_btnm_ext_t btnm;     /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * ta;
    lv_kb_mode_t mode;
    uint8_t cur_mng    :1;
    lv_action_t     ok_action;
    lv_action_t     close_action;
}lv_kb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param);


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_KB_H*/
