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
    lv_draw_task_t * volatile task_act;
    lv_thread_t thread;
    lv_thread_sync_t new_task_sync;
    lv_thread_sync_t task_done_sync;
    lv_draw_unit_t * draw_unit;
    uint32_t idx;
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

/**
 * Draw a decoded image.
 *
 * @param layer             the target layer
 * @param draw_dsc          the draw descriptor of the image
 * @param decoder_dsc       pointer to the decoded image's descriptor
 * @param sup               supplementary data
 * @param img_coords        the absolute coordinates of the image
 * @param clipped_img_area  the absolute coordinates of the image area to draw (already clipped)
 * @param user_data         unused
 */
void lv_draw_sw_image_core(lv_layer_t * layer, const lv_draw_image_dsc_t * draw_dsc,
                           const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                           const lv_area_t * img_coords, const lv_area_t * clipped_img_area, void * user_data);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_PRIVATE_H*/
