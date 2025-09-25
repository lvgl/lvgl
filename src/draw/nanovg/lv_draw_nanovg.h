/**
 * @file lv_draw_nanovg.h
 *
 */

#ifndef LV_DRAW_NANOVG_H
#define LV_DRAW_NANOVG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_DRAW_NANOVG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * fb;
    int32_t width;
    int32_t height;
} lv_draw_nanovg_event_param_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize NanoVG rendering
 */
void lv_draw_nanovg_init(void);

/**
 * Initialize event parameter for NanoVG rendering
 * @param param pointer to an initialized `lv_draw_nanovg_event_param_t` struct
 */
void lv_draw_nanovg_event_param_init(lv_draw_nanovg_event_param_t * param);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NANOVG_H*/
