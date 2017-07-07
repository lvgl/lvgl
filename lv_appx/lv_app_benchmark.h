/**
 * @file lv_app_benchmark.h
 *
 */

#ifndef LV_APP_BENCHMARK_H
#define LV_APP_BENCHMARK_H

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{

}lv_app_benchmark_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_benchmark_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0*/

#endif /* LV_APP_BENCHMARK_H */
