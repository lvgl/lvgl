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
#include "lv_task.h"
#include "../lv_draw/lv_img_cache.h"
#include "../lv_draw/lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/

#define LV_ITERATE_ROOTS(f) \
    f(lv_ll_t, _lv_task_ll)  /*Linked list to store the lv_tasks*/ \
    f(lv_ll_t, _lv_disp_ll)  /*Linked list of screens*/            \
    f(lv_ll_t, _lv_indev_ll) /*Linked list of screens*/            \
    f(lv_ll_t, _lv_drv_ll)                                         \
    f(lv_ll_t, _lv_file_ll)                                        \
    f(lv_ll_t, _lv_anim_ll)                                        \
    f(lv_ll_t, _lv_group_ll)                                       \
    f(lv_ll_t, _lv_img_defoder_ll)                                 \
    f(lv_ll_t, _lv_obj_style_trans_ll)                             \
    f(lv_img_cache_entry_t*, _lv_img_cache_array)                  \
    f(lv_task_t*, _lv_task_act)                                    \
    f(lv_mem_buf_arr_t , _lv_mem_buf)                              \
    f(_lv_draw_mask_saved_arr_t , _lv_draw_mask_list)              \
    f(void * , _lv_theme_material_styles)                          \
    f(void * , _lv_theme_template_styles)                          \
    f(void * , _lv_theme_mono_styles)                              \
    f(void * , _lv_theme_empty_styles)                             \
    f(uint8_t *, _lv_font_decompr_buf)                             \


#define LV_DEFINE_ROOT(root_type, root_name) root_type root_name;
#define LV_ROOTS LV_ITERATE_ROOTS(LV_DEFINE_ROOT)

#if LV_ENABLE_GC == 1
#if LV_MEM_CUSTOM != 1
#error "GC requires CUSTOM_MEM"
#endif /* LV_MEM_CUSTOM */
#else  /* LV_ENABLE_GC */
#define LV_GC_ROOT(x) x
#define LV_EXTERN_ROOT(root_type, root_name) extern root_type root_name;
LV_ITERATE_ROOTS(LV_EXTERN_ROOT)
#endif /* LV_ENABLE_GC */

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
} /* extern "C" */
#endif

#endif /*LV_GC_H*/
