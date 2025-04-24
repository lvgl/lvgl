/**
 * @file lv_draw_sw_private.h
 *
 */

#ifndef LV_DRAW_SW_PRIVATE_H
#define LV_DRAW_SW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sw.h"
#include "../lv_draw_private.h"

#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_task_t * task_act;
    lv_thread_t thread;
    lv_thread_sync_t sync;
    lv_draw_unit_t * draw_unit;
    uint32_t idx;
    volatile bool inited;
    volatile bool exit_status;
} lv_draw_sw_thread_dsc_t;

struct _lv_draw_sw_unit_t {
    lv_draw_unit_t base_unit;
#if LV_USE_OS
    lv_draw_sw_thread_dsc_t thread_dscs[LV_DRAW_SW_DRAW_UNIT_CNT];
#else
    lv_draw_task_t * task_act;
#endif
};

#if LV_DRAW_SW_SHADOW_CACHE_SIZE
typedef struct {
    uint8_t cache[LV_DRAW_SW_SHADOW_CACHE_SIZE * LV_DRAW_SW_SHADOW_CACHE_SIZE];
    int32_t cache_size;
    int32_t cache_r;
} lv_draw_sw_shadow_cache_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_PRIVATE_H*/
