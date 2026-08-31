/**
 * @file lv_obj_style_internal.h
 *
 */

#ifndef LV_OBJ_STYLE_INTERNAL_H
#define LV_OBJ_STYLE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl_public.h"
#include "lv_obj_style_internal_gen.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

static inline lv_text_align_t lv_obj_calculate_style_text_align_internal(const lv_obj_t * obj, lv_part_t part,
                                                                         const char * txt)
{
    LV_ASSERT(obj != NULL);
    LV_ASSERT(txt != NULL);

    lv_text_align_t align = lv_obj_get_style_text_align_internal(obj, part);
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir_internal(obj, part);
    lv_bidi_calculate_align(&align, &base_dir, txt);
    return align;
}

static inline int32_t lv_obj_get_style_space_left_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t padding = lv_obj_get_style_pad_left(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_LEFT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_right_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t padding = lv_obj_get_style_pad_right(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_RIGHT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_top_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t padding = lv_obj_get_style_pad_top(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_TOP) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_bottom_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t padding = lv_obj_get_style_pad_bottom(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_BOTTOM) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_transform_scale_x_safe_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t scale = lv_obj_get_style_transform_scale_x(obj, part);
    return scale > 0 ? scale : 1;
}

static inline int32_t lv_obj_get_style_transform_scale_y_safe_internal(const lv_obj_t * obj, lv_part_t part)
{
    LV_ASSERT(obj != NULL);
    int32_t scale = lv_obj_get_style_transform_scale_y(obj, part);
    return scale > 0 ? scale : 1;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_STYLE_INTERNAL_H*/
