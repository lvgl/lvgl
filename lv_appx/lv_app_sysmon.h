/**
 * @file lv_app_example.h
 *
 */

#ifndef LV_APP_SYSMON_H
#define LV_APP_SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_SYSMON != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{

}lv_app_sysmon_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_sysmon_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_SYSMON != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_APP_SYSMON_H */
