/**
 * @file lv_app_phantom.h
 *
 */

#ifndef LV_APP_PHANTOM_H
#define LV_APP_PHANTOM_H

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_PHANTOM != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    void (*com_listen)(lv_app_inst_t * app_send,
                       lv_app_inst_t * app_rec,
                       lv_app_com_type_t type ,
                       const void * data, uint32_t size);
}lv_app_phantom_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_phantom_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_PHANTOM != 0*/

#endif /* LV_APP_PHANTOM_H */
