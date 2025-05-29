/**
 * @file lv_draw_ppa.h
 *
 */

#ifndef LV_DRAW_PPA_H
#define LV_DRAW_PPA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_PPA

#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_ppa_init(void);
void lv_draw_ppa_deinit(void);
void lv_draw_buf_ppa_init_handlers(void);

void lv_draw_ppa_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords);

void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_PPA */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_H */
