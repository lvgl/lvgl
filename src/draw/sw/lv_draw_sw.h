/**
 * @file lv_draw_sw.h
 *
 */

#ifndef LV_DRAW_SW_H
#define LV_DRAW_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend.h"
#if LV_USE_DRAW_SW

#include "../lv_draw.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../core/lv_disp.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_mutex_t mutex;
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
    uint32_t idx;
} lv_draw_sw_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_draw_ctx_t * lv_draw_sw_init_ctx(lv_disp_t * disp);

void lv_draw_sw_deinit_ctx(struct _lv_disp_t * disp, lv_draw_ctx_t * draw_ctx);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
