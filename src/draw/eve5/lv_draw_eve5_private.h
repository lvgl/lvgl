/**
 * @file lv_draw_eve5_private.h
 *
 */

#ifndef LV_DRAW_EVE5_PRIVATE_H
#define LV_DRAW_EVE5_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/
#include "lv_draw_eve5.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw_private.h"
#include "../lv_draw_label_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../font/fmt_txt/lv_font_fmt_txt.h"

/*********************
 * DEFINES
 *********************/
#define ALIGN_UP(x, align) (((x) + ((align)-1)) & ~((align)-1))

/* Fixed point helpers */
#define F16(x)              ((int32_t)((x) * 65536L))
#define F16_SCALE_DIV_256(x) ((int32_t)(((x) / 256.0f) * 65536L))
#define DEGREES(a)          ((uint16_t)(((a) % 360) * 65536L / 360))

/* Nibble extraction for font bitmaps */
#define GET_NIBBLE_HI(w) ((uint8_t)((w) >> 4))
#define GET_NIBBLE_LO(w) ((uint8_t)((w) & 0x0F))

/**********************
 * TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    EVE_HalContext *hal;
    Esd_GpuAlloc *allocator;
    lv_cache_t *texture_cache;
	bool rendering_in_progress;  /* Re-entrancy guard for SW fallback */
} lv_draw_eve5_unit_t;

typedef struct {
    lv_draw_dsc_base_t *draw_dsc;
    int32_t w;
    int32_t h;
    Esd_GpuHandle texture_handle;
} lv_draw_eve5_cache_data_t;

/**********************
 * PROTOTYPES (HAL)
 **********************/

/* Shared utilities */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t *u, const lv_area_t *clip, const lv_area_t *layer_area);

/* Layer management */
void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen);
void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen);

/* Child layer compositing */
void lv_draw_eve5_hal_render_child(lv_draw_eve5_unit_t *u, lv_draw_task_t *t, lv_layer_t *parent_layer);

/* SW fallback buffer compositing */
void lv_draw_eve5_hal_composite_buffer(lv_draw_eve5_unit_t *u, 
                                        const lv_draw_task_t *t,
                                        const uint8_t *buf_data,
                                        int32_t buf_w, int32_t buf_h,
                                        const lv_area_t *buf_area);

/* Primitive drawing */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Bitmap/image drawing */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Label/text drawing */
void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);

/* Utilities */
bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle);

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_PRIVATE_H */
