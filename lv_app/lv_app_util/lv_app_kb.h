/**
 * @file lv_app_kb.h
 *
 */

#ifndef LV_APP_KB_H
#define LV_APP_KB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_app.h"
#if USE_LV_APP_KB != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_APP_KB_MODE_TXT =            0x0001,
	LV_APP_KB_MODE_NUM =            0x0002,
    LV_APP_KB_MODE_WIN_RESIZE =     0x0004,
    LV_APP_KB_MODE_CUR_MANAGE =     0x0008,
    LV_APP_KB_MODE_ANIM_IN =        0x0010,
    LV_APP_KB_MODE_ANIM_OUT =       0x0020,
}lv_app_kb_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the application keyboard
 */
void lv_app_kb_init(void);

/**
 * Open a keyboard for a text area object
 * @param ta pointer to a text area object
 * @param mode 'OR'd values of 'lv_app_kb_mode_t' enum
 * @param close a function to call when the keyboard is closed
 * @param ok a function to called when the "Ok" button is pressed
 * @return the created button matrix objects
 */
lv_obj_t * lv_app_kb_open(lv_obj_t * ta, lv_app_kb_mode_t mode, void (*close)(lv_obj_t *), void (*ok)(lv_obj_t *));

/**
 * Close the keyboard
 * @param ok true: call the ok function, false: call the close function
 */
void lv_app_kb_close(bool ok);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_APP_KB_H*/
