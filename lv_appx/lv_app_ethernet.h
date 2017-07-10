/**
 * @file lv_app_ethernet.h
 *
 */

#ifndef LV_APP_ETHERNET_H
#define LV_APP_ETHERNET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_ETHERNET != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{

}lv_app_ethernet_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_ethernet_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_ETHERNET != 0*/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_APP_ETHERNET_H */
