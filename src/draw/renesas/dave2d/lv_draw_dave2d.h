/**
 * @file lv_draw_dave2d.h
 *
 */

#ifndef LV_DRAW_DAVE2D_H
#define LV_DRAW_DAVE2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"
#if LV_USE_DRAW_DAVE2D
#include "../../lv_draw.h"
#include "../../lv_draw_private.h"
#include "bsp_api.h"
#include "dave_driver.h"

#if LV_USE_FLOAT

/* We need to redefine some of D2 fixed point math macros to deal with lv_precise_t being float now */
#undef D2_FIX4
#undef D2_INT4
#undef D2_FLOOR4
#undef D2_CEIL4
#undef D2_FRAC4
#undef D2_FIX16
#undef D2_INT16
#undef D2_FLOOR16
#undef D2_CEIL16
#undef D2_FRAC16

#define D2_FIX4(x)      (((int32_t)(x)) << 4)
#define D2_INT4(x)      (((int32_t)(x))(x) >> 4)
#define D2_FLOOR4(x)    (((int32_t)(x))((d2_u32)(x)) & ~15u)
#define D2_CEIL4(x)     ((((d2_u32)(x)) + 15u) & ~15u)
#define D2_FRAC4(x)     (((d2_u32)(x)) & 15u)
#define D2_FIX16(x)     (((int32_t)(x)) << 16)
#define D2_INT16(x)     (((int32_t)(x)) >> 16)
#define D2_FLOOR16(x)   (((d2_u32)(x)) & ~65535u)
#define D2_CEIL16(x)    ((((d2_u32)(x)) + 65535u) & ~65535u)
#define D2_FRAC16(x)    (((d2_u32)(x)) & 65535u)

/* It also should be included here before the other LVGL Dave2D files */
#endif

#include "lv_draw_dave2d_utils.h"
#include "../../lv_draw_rect.h"
#include "../../lv_draw_line.h"
#include "../../lv_draw_arc.h"
#include "../../lv_draw_label.h"
#include "../../lv_draw_image.h"
#include "../../lv_draw_triangle.h"
#include "../../lv_draw_buf.h"



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
    uint32_t idx;
    d2_device * d2_handle;
    d2_renderbuffer * renderbuffer;
    d2_renderbuffer * label_renderbuffer;

#if LV_USE_OS
    lv_mutex_t * pd2Mutex;
#endif
} lv_draw_dave2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_dave2d_init(void);

void lv_draw_dave2d_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_dave2d_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords);

void lv_draw_dave2d_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc,
                               const lv_area_t * coords);

void lv_draw_dave2d_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

void lv_draw_dave2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_dave2d_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

void lv_draw_dave2d_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc,
                              const lv_area_t * coords);

void lv_draw_dave2d_transform(lv_draw_task_t * t, const lv_area_t * dest_area, const void * src_buf,
                              int32_t src_w, int32_t src_h, int32_t src_stride,
                              const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t cf, void * dest_buf);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_DAVE2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DAVE2D_H*/
