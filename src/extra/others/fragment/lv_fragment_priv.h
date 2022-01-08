/**
 * @file lv_fragment_priv.h
 *
 */

#ifndef LV_FRAGMENT_PRIV_H
#define LV_FRAGMENT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_FRAGMENT
#include "lv_fragment.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct internal_states_t {
    const lv_fragment_class_t * cls;
    lv_fragment_t * instance;
    lv_obj_t * const * container;
    bool obj_created;
    bool destroying_obj;
    bool is_msgbox;
    bool in_stack;
    struct internal_states_t * prev;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FRAGMENT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FRAGMENT_PRIV_H*/
