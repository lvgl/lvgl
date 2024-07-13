/**
 * @file lv_qnx_window.h
 *
 */

#ifndef LV_QNX_DISP_H
#define LV_QNX_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_QNX

#include <stdbool.h>
#include <screen/screen.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * lv_qnx_window_create(int32_t hor_res, int32_t ver_res);
void lv_qnx_window_set_title(lv_display_t * disp, const char * title);
bool lv_qnx_inputs_create(lv_display_t * disp);
int lv_qnx_event_loop(lv_display_t * disp);
void lv_qnx_quit();

/**********************
 *      MACROS
 **********************/

#endif /* LV_DRV_QNX */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_QNX_DISP_H */
