/**
 * @file lv_app_fsel.h
 *
 */

#ifndef LV_APP_FSEL_H
#define LV_APP_FSEL_H

/*********************
 *      INCLUDES
 *********************/
#include "../lv_app.h"
#if LV_APP_ENABLE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the File selector utility
 */
void lv_app_fsel_init(void);

/**
 * Open the File selector
 * @param path start path
 * @param filter show only files with a specific extension, e.g. "wav".
 *               "/" means filter to folders.
 * @param param a free parameter which will be added to 'ok_action'
 * @param ok_action an action to call when a file or folder is chosen (give 'param' and the path as parameters)
 */
void lv_app_fsel_open(const char * path, const char * filter, void * param,
                       void (*ok_action)(void *, const char *));

/**
 * Close the File selector
 */
void lv_app_fsel_close(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_APP_FSEL_H*/
