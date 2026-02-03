/**
 * @file lv_draw_eve_private.h
 *
 */

/*  Author: juanj
 *
 *  Modified by LVGL
 */

#ifndef LV_DRAW_EVE_PRIVATE_H
#define LV_DRAW_EVE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE

#include "lv_draw_eve_target.h"
#include "lv_draw_eve_ram_g.h"
#include "../lv_draw_private.h"
#include "../../misc/lv_types.h"
#include "../../core/lv_global.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_line.h"
#include "../lv_draw_label.h"
#include "../../font/fmt_txt/lv_font_fmt_txt.h"
#include "../lv_draw_arc.h"

#if LV_DRAW_EVE_WRITE_BUFFER_SIZE != 0 && LV_DRAW_EVE_WRITE_BUFFER_SIZE < 4
#warning LV_DRAW_EVE_WRITE_BUFFER_SIZE cannot be less than 4. Using 0 (buffering disabled).
#define LV_DRAW_EVE_WRITE_BUFFER_SIZE_INTERNAL 0
#else
#define LV_DRAW_EVE_WRITE_BUFFER_SIZE_INTERNAL LV_DRAW_EVE_WRITE_BUFFER_SIZE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uintptr_t key;
    uint32_t addr;
} lv_draw_eve_ramg_hash_table_cell_t;

typedef struct {
    uint32_t ramg_addr_end;
    uint32_t hash_table_cell_count;
    uint32_t hash_table_cells_occupied;
    lv_draw_eve_ramg_hash_table_cell_t * hash_table;
} lv_draw_eve_ramg_t;

struct _lv_draw_eve_unit_t {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    lv_display_t * disp;
    lv_draw_eve_ramg_t ramg;
    lv_draw_eve_parameters_t params;
    lv_draw_eve_operation_cb_t op_cb;
#if LV_DRAW_EVE_WRITE_BUFFER_SIZE_INTERNAL
    uint32_t lv_eve_write_buf_len;
    uint8_t lv_eve_write_buf[LV_DRAW_EVE_WRITE_BUFFER_SIZE_INTERNAL];
#endif
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_eve_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords);
bool lv_draw_eve_image_src_check(const void * src);
uint32_t lv_draw_eve_image_upload_image(bool burst_is_active, const lv_image_dsc_t * img_dsc);

void lv_draw_eve_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_eve_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_eve_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

void lv_draw_eve_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                       const lv_area_t * coords);
bool lv_draw_eve_label_font_check(const lv_font_t * font);
uint32_t lv_draw_eve_label_upload_glyph(bool burst_is_active, const lv_font_fmt_txt_dsc_t * font_dsc,
                                        uint32_t gid_index);

void lv_draw_eve_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_eve_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#define DEGREES(n) ((65536UL * (n)) / 3600)
#define F16(x)     ((int32_t)((x) * 65536L))

#define lv_draw_eve_unit_g (LV_GLOBAL_DEFAULT()->draw_eve_unit)

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_PRIVATE_H*/
