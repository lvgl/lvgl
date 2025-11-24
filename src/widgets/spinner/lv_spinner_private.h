/**
 * @file lv_spinner_private.h
 *
 */

#ifndef LV_SPINNER_PRIVATE_H
#define LV_SPINNER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_spinner.h"
#if LV_USE_SPINNER

#include "../arc/lv_arc_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_spinner_t {
    lv_arc_t arc;                 /**< Add the ancestor's type first */
    uint32_t duration;            /**< Anim duration in ms */
    uint32_t angle;               /**< Anim angle in degrees */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_SPINNER_PRIVATE_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SPINNER_PRIVATE_H*/
