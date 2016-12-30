/**
 * @file lv_app_example.h
 *
 */

#ifndef LV_APP_EXAMPLE_H
#define LV_APP_EXAMPLE_H

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{

}lv_app_example_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_example_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0*/

#endif /* LV_APP_EXAMPLE_H */
