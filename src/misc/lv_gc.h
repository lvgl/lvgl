/**
 * @file lv_gc.h
 *
 */

#ifndef LV_GC_H
#define LV_GC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lv_mem.h"
#include "lv_ll.h"
#include "lv_timer.h"
#include "../draw/lv_img_cache.h"
#include "../draw/lv_draw_mask.h"
#include "../core/lv_obj_pos.h"

/*********************
 *      DEFINES
 *********************/
#if LV_IMG_CACHE_DEF_SIZE
#    define LV_IMG_CACHE_DEF            1
#else
#    define LV_IMG_CACHE_DEF            0
#endif

#define LV_DISPATCH(f, t, n)            f(t, n)
#define LV_DISPATCH_COND(f, t, n, m, v) LV_CONCAT3(LV_DISPATCH, m, v)(f, t, n)

#define LV_DISPATCH00(f, t, n)          LV_DISPATCH(f, t, n)
#define LV_DISPATCH01(f, t, n)
#define LV_DISPATCH10(f, t, n)
#define LV_DISPATCH11(f, t, n)          LV_DISPATCH(f, t, n)

#define LV_ITERATE_ROOTS(f)                                                                 \
    LV_DISPATCH(f, lv_ll_t, _lv_timer_ll) /*Linked list to store the lv_timers*/            \
    LV_DISPATCH(f, lv_ll_t, _lv_disp_ll)  /*Linked list of display device*/                 \
    LV_DISPATCH(f, lv_ll_t, _lv_indev_ll) /*Linked list of input device*/                   \
    LV_DISPATCH(f, lv_ll_t, _lv_fsdrv_ll)                                                   \
    LV_DISPATCH(f, lv_ll_t, _lv_anim_ll)                                                    \
    LV_DISPATCH(f, lv_ll_t, _lv_group_ll)                                                   \
    LV_DISPATCH(f, lv_ll_t, _lv_img_decoder_ll)                                             \
    LV_DISPATCH(f, lv_ll_t, _lv_obj_style_trans_ll)                                         \
    LV_DISPATCH(f, lv_layout_dsc_t *, _lv_layout_list)      \
    LV_DISPATCH_COND(f, _lv_img_cache_entry_t*, _lv_img_cache_array, LV_IMG_CACHE_DEF, 1)    \
    LV_DISPATCH_COND(f, _lv_img_cache_entry_t, _lv_img_cache_single, LV_IMG_CACHE_DEF, 0)    \
    LV_DISPATCH(f, lv_timer_t*, _lv_timer_act)                                              \
    LV_DISPATCH(f, lv_mem_buf_arr_t , lv_mem_buf)                                           \
    LV_DISPATCH_COND(f, _lv_draw_mask_saved_arr_t , _lv_draw_mask_list, LV_DRAW_COMPLEX, 1) \
    LV_DISPATCH(f, void * , _lv_theme_default_styles)                                       \
    LV_DISPATCH_COND(f, uint8_t *, _lv_font_decompr_buf, LV_USE_FONT_COMPRESSED, 1)

#define LV_DEFINE_ROOT(root_type, root_name) root_type root_name;
#define LV_ROOTS LV_ITERATE_ROOTS(LV_DEFINE_ROOT)

#if LV_ENABLE_GC == 1
#if LV_MEM_CUSTOM != 1
#error "GC requires CUSTOM_MEM"
#endif /*LV_MEM_CUSTOM*/
#include LV_GC_INCLUDE
#else  /*LV_ENABLE_GC*/
#define LV_GC_ROOT(x) x
#define LV_EXTERN_ROOT(root_type, root_name) extern root_type root_name;
LV_ITERATE_ROOTS(LV_EXTERN_ROOT)
#endif /*LV_ENABLE_GC*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_gc_clear_roots(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GC_H*/
