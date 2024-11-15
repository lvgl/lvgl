/**
 * @file lv_demo_high_res.h
 *
 */

#ifndef LV_DEMO_HIGH_RES_H
#define LV_DEMO_HIGH_RES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../src/lv_conf_internal.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/others/observer/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* input subjects */
    lv_subject_t logo;
    lv_subject_t logo_dark;
    lv_subject_t hour;
    lv_subject_t minute;
    /* output subjects */
    /* ... TODO */
} lv_demo_high_res_subjects_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_demo_high_res_subjects_t * lv_demo_high_res(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_HIGH_RES_H*/
