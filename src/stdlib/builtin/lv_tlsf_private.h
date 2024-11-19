/**
 * @file lv_tlsf_private.h
 *
 */

#ifndef LV_TLSF_PRIVATE_H
#define LV_TLSF_PRIVATE_H

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_tlsf.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
#if LV_USE_OS
    lv_mutex_t mutex;
#endif
    lv_tlsf_t tlsf;
    size_t cur_used;
    size_t max_used;
    lv_ll_t  pool_ll;
} lv_tlsf_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

#endif /*LV_TLSF_PRIVATE_H*/
