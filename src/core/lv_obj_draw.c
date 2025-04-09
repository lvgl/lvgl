/**
 * @file lv_obj_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_draw_private.h"
#include "lv_obj_private.h"
#include "lv_obj_style.h"
#include "../display/lv_display.h"
#include "../indev/lv_indev.h"
#include "../stdlib/lv_string.h"
#include "../draw/lv_draw_arc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline lv_opa_t get_layer_opa(const lv_obj_t * obj, lv_part_t part, const lv_draw_dsc_base_t * base_dsc);
static lv_color_t normal_apply_layer_recolor(const lv_obj_t * obj, lv_part_t part,  const lv_draw_dsc_base_t * base_dsc,
                                             lv_color_t color);
static lv_color32_t image_apply_layer_recolor(const lv_obj_t * obj, lv_part_t part,
                                              const lv_draw_dsc_base_t * base_dsc, lv_color_t color, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_rect_dsc_t * draw_dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_dsc->base.obj = obj;
    draw_dsc->base.part = part;

    lv_opa_t opa = get_layer_opa(obj, part, &draw_dsc->base);
    if(part != LV_PART_MAIN) {
        if(opa <= LV_OPA_MIN) {
            draw_dsc->bg_opa = LV_OPA_TRANSP;
            draw_dsc->bg_image_opa = LV_OPA_TRANSP;
            draw_dsc->border_opa = LV_OPA_TRANSP;
            draw_dsc->outline_opa = LV_OPA_TRANSP;
            draw_dsc->shadow_opa = LV_OPA_TRANSP;
            LV_PROFILER_DRAW_END;
            return;
        }
    }

    draw_dsc->radius = lv_obj_get_style_radius(obj, part);

    if(draw_dsc->bg_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_opa = lv_obj_get_style_bg_opa(obj, part);
        if(draw_dsc->bg_opa > LV_OPA_MIN) {
            lv_color_t bg_color = lv_obj_get_style_bg_color_filtered(obj, part);
            draw_dsc->bg_color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, bg_color);
            const lv_grad_dsc_t * grad = lv_obj_get_style_bg_grad(obj, part);
            if(grad && grad->dir != LV_GRAD_DIR_NONE) {
                lv_memcpy(&draw_dsc->bg_grad, grad, sizeof(*grad));
            }
            else {
                draw_dsc->bg_grad.dir = lv_obj_get_style_bg_grad_dir(obj, part);
                if(draw_dsc->bg_grad.dir != LV_GRAD_DIR_NONE) {
                    draw_dsc->bg_grad.stops[0].color = draw_dsc->bg_color;
                    lv_color_t bg_grad_color = lv_obj_get_style_bg_grad_color_filtered(obj, part);
                    draw_dsc->bg_grad.stops[1].color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, bg_grad_color);
                    draw_dsc->bg_grad.stops[0].frac = lv_obj_get_style_bg_main_stop(obj, part);
                    draw_dsc->bg_grad.stops[1].frac = lv_obj_get_style_bg_grad_stop(obj, part);
                    draw_dsc->bg_grad.stops[0].opa = lv_obj_get_style_bg_main_opa(obj, part);
                    draw_dsc->bg_grad.stops[1].opa = lv_obj_get_style_bg_grad_opa(obj, part);
                }
            }
        }
    }

    if(draw_dsc->border_opa != LV_OPA_TRANSP) {
        draw_dsc->border_width = lv_obj_get_style_border_width(obj, part);
        if(draw_dsc->border_width) {
            draw_dsc->border_opa = lv_obj_get_style_border_opa(obj, part);
            if(draw_dsc->border_opa > LV_OPA_MIN) {
                draw_dsc->border_side = lv_obj_get_style_border_side(obj, part);
                lv_color_t border_color = lv_obj_get_style_border_color_filtered(obj, part);
                draw_dsc->border_color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, border_color);
            }
        }
    }

    if(draw_dsc->outline_opa != LV_OPA_TRANSP) {
        draw_dsc->outline_width = lv_obj_get_style_outline_width(obj, part);
        if(draw_dsc->outline_width) {
            draw_dsc->outline_opa = lv_obj_get_style_outline_opa(obj, part);
            if(draw_dsc->outline_opa > LV_OPA_MIN) {
                draw_dsc->outline_pad = lv_obj_get_style_outline_pad(obj, part);
                lv_color_t outline_color = lv_obj_get_style_outline_color_filtered(obj, part);
                draw_dsc->outline_color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, outline_color);
            }
        }
    }

    if(draw_dsc->bg_image_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_image_src = lv_obj_get_style_bg_image_src(obj, part);
        if(draw_dsc->bg_image_src) {
            draw_dsc->bg_image_opa = lv_obj_get_style_bg_image_opa(obj, part);
            if(draw_dsc->bg_image_opa > LV_OPA_MIN) {
                if(lv_image_src_get_type(draw_dsc->bg_image_src) == LV_IMAGE_SRC_SYMBOL) {
                    draw_dsc->bg_image_symbol_font = lv_obj_get_style_text_font(obj, part);
                    lv_color_t text_color = lv_obj_get_style_text_color_filtered(obj, part);
                    draw_dsc->bg_image_recolor = normal_apply_layer_recolor(obj, part, &draw_dsc->base, text_color);
                }
                else {
                    lv_color_t bg_image_recolor = lv_obj_get_style_bg_image_recolor_filtered(obj, part);
                    lv_opa_t bg_image_recolor_opa = lv_obj_get_style_bg_image_recolor_opa(obj, part);
                    lv_color32_t result = image_apply_layer_recolor(obj, part, &draw_dsc->base, bg_image_recolor, bg_image_recolor_opa);
                    draw_dsc->bg_image_recolor_opa = result.alpha;
                    draw_dsc->bg_image_recolor = lv_color_make(result.red, result.green, result.blue);
                    draw_dsc->bg_image_tiled = lv_obj_get_style_bg_image_tiled(obj, part);
                }
            }
        }
    }

    if(draw_dsc->shadow_opa) {
        draw_dsc->shadow_width = lv_obj_get_style_shadow_width(obj, part);
        if(draw_dsc->shadow_width) {
            if(draw_dsc->shadow_opa > LV_OPA_MIN) {
                draw_dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, part);
                if(draw_dsc->shadow_opa > LV_OPA_MIN) {
                    draw_dsc->shadow_offset_x = lv_obj_get_style_shadow_offset_x(obj, part);
                    draw_dsc->shadow_offset_y = lv_obj_get_style_shadow_offset_y(obj, part);
                    draw_dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, part);
                    lv_color_t shadow_color = lv_obj_get_style_shadow_color_filtered(obj, part);
                    draw_dsc->shadow_color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, shadow_color);
                }
            }
        }
    }

    if(opa < LV_OPA_MAX) {
        draw_dsc->bg_opa = LV_OPA_MIX2(draw_dsc->bg_opa, opa);
        draw_dsc->bg_image_opa = LV_OPA_MIX2(draw_dsc->bg_image_opa, opa);
        draw_dsc->border_opa = LV_OPA_MIX2(draw_dsc->border_opa, opa);
        draw_dsc->shadow_opa = LV_OPA_MIX2(draw_dsc->shadow_opa, opa);
        draw_dsc->outline_opa = LV_OPA_MIX2(draw_dsc->outline_opa, opa);
    }

    LV_PROFILER_DRAW_END;
}

void lv_obj_init_draw_label_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_label_dsc_t * draw_dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_dsc->base.obj = obj;
    draw_dsc->base.part = part;

    draw_dsc->opa = lv_obj_get_style_text_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_opa_t opa = get_layer_opa(obj, part, &draw_dsc->base);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = LV_OPA_MIX2(draw_dsc->opa, opa);
    }
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_color_t text_color = lv_obj_get_style_text_color_filtered(obj, part);
    draw_dsc->color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, text_color);
    draw_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, part);
    draw_dsc->line_space = lv_obj_get_style_text_line_space(obj, part);
    draw_dsc->decor = lv_obj_get_style_text_decor(obj, part);

    draw_dsc->font = lv_obj_get_style_text_font(obj, part);

#if LV_USE_BIDI
    draw_dsc->bidi_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
#endif

    draw_dsc->align = lv_obj_get_style_text_align(obj, part);

    LV_PROFILER_DRAW_END;
}

void lv_obj_init_draw_image_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_image_dsc_t * draw_dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_dsc->base.obj = obj;
    draw_dsc->base.part = part;

    draw_dsc->opa = lv_obj_get_style_image_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_opa_t opa = get_layer_opa(obj, part, &draw_dsc->base);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = LV_OPA_MIX2(draw_dsc->opa, opa);
    }
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    draw_dsc->rotation = 0;
    draw_dsc->scale_x = LV_SCALE_NONE;
    draw_dsc->scale_y = LV_SCALE_NONE;
    draw_dsc->pivot.x = lv_area_get_width(&obj->coords) / 2;
    draw_dsc->pivot.y = lv_area_get_height(&obj->coords) / 2;

    lv_color_t recolor = lv_obj_get_style_image_recolor_filtered(obj, part);
    lv_opa_t recolor_opa = lv_obj_get_style_image_recolor_opa(obj, part);
    lv_color32_t result = image_apply_layer_recolor(obj, part, &draw_dsc->base, recolor, recolor_opa);
    draw_dsc->recolor_opa = result.alpha;
    draw_dsc->recolor = lv_color_make(result.red, result.green, result.blue);

    if(part != LV_PART_MAIN) draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);

    LV_PROFILER_DRAW_END;
}

void lv_obj_init_draw_line_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_line_dsc_t * draw_dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_dsc->base.obj = obj;
    draw_dsc->base.part = part;

    draw_dsc->opa = lv_obj_get_style_line_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_opa_t opa = get_layer_opa(obj, part, &draw_dsc->base);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = LV_OPA_MIX2(draw_dsc->opa, opa);
    }
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    draw_dsc->width = lv_obj_get_style_line_width(obj, part);
    if(draw_dsc->width == 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_color_t line_color = lv_obj_get_style_line_color_filtered(obj, part);
    draw_dsc->color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, line_color);

    draw_dsc->dash_width = lv_obj_get_style_line_dash_width(obj, part);
    if(draw_dsc->dash_width) {
        draw_dsc->dash_gap = lv_obj_get_style_line_dash_gap(obj, part);
    }

    draw_dsc->round_start = lv_obj_get_style_line_rounded(obj, part);
    draw_dsc->round_end = draw_dsc->round_start;

    LV_PROFILER_DRAW_END;
}

void lv_obj_init_draw_arc_dsc(lv_obj_t * obj, lv_part_t part, lv_draw_arc_dsc_t * draw_dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_dsc->base.obj = obj;
    draw_dsc->base.part = part;

    draw_dsc->width = lv_obj_get_style_arc_width(obj, part);
    if(draw_dsc->width == 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    draw_dsc->opa = lv_obj_get_style_arc_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_opa_t opa = get_layer_opa(obj, part, &draw_dsc->base);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = LV_OPA_MIX2(draw_dsc->opa, opa);
    }
    if(draw_dsc->opa <= LV_OPA_MIN) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_color_t arc_color = lv_obj_get_style_arc_color_filtered(obj, part);
    draw_dsc->color = normal_apply_layer_recolor(obj, part, &draw_dsc->base, arc_color);
    draw_dsc->img_src = lv_obj_get_style_arc_image_src(obj, part);

    draw_dsc->rounded = lv_obj_get_style_arc_rounded(obj, part);
    LV_PROFILER_DRAW_END;
}

int32_t lv_obj_calculate_ext_draw_size(lv_obj_t * obj, lv_part_t part)
{
    LV_PROFILER_DRAW_BEGIN;
    int32_t s = 0;

    int32_t sh_width = lv_obj_get_style_shadow_width(obj, part);
    if(sh_width) {
        lv_opa_t sh_opa = lv_obj_get_style_shadow_opa(obj, part);
        if(sh_opa > LV_OPA_MIN) {
            sh_width = sh_width / 2 + 1;    /*The blur adds only half width*/
            sh_width += lv_obj_get_style_shadow_spread(obj, part);
            int32_t sh_ofs_x = lv_obj_get_style_shadow_offset_x(obj, part);
            int32_t sh_ofs_y = lv_obj_get_style_shadow_offset_y(obj, part);
            sh_width += LV_MAX(LV_ABS(sh_ofs_x), LV_ABS(sh_ofs_y));
            s = LV_MAX(s, sh_width);
        }
    }

    int32_t outline_width = lv_obj_get_style_outline_width(obj, part);
    if(outline_width) {
        lv_opa_t outline_opa = lv_obj_get_style_outline_opa(obj, part);
        if(outline_opa > LV_OPA_MIN) {
            int32_t outline_pad = lv_obj_get_style_outline_pad(obj, part);
            s = LV_MAX(s, outline_pad + outline_width);
        }
    }

    int32_t w = lv_obj_get_style_transform_width(obj, part);
    int32_t h = lv_obj_get_style_transform_height(obj, part);
    int32_t wh = LV_MAX(w, h);
    if(wh > 0) s += wh;

    LV_PROFILER_DRAW_END;
    return s;
}

void lv_obj_refresh_ext_draw_size(lv_obj_t * obj)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t s_old = lv_obj_get_ext_draw_size(obj);
    int32_t s_new = 0;
    lv_obj_send_event(obj, LV_EVENT_REFR_EXT_DRAW_SIZE, &s_new);

    /*Store the result if the special attrs already allocated*/
    if(obj->spec_attr) {
        obj->spec_attr->ext_draw_size = s_new;
    }
    /*Allocate spec. attrs. only if the result is not zero.
     *Zero is the default value if the spec. attr. are not defined.*/
    else if(s_new != 0) {
        lv_obj_allocate_spec_attr(obj);
        obj->spec_attr->ext_draw_size = s_new;
    }

    if(s_new != s_old) lv_obj_invalidate(obj);
    LV_PROFILER_DRAW_END;
}

int32_t lv_obj_get_ext_draw_size(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->ext_draw_size;
    else return 0;
}

lv_layer_type_t lv_obj_get_layer_type(const lv_obj_t * obj)
{

    if(obj->spec_attr) return (lv_layer_type_t)obj->spec_attr->layer_type;
    else return LV_LAYER_TYPE_NONE;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline lv_opa_t get_layer_opa(const lv_obj_t * obj, lv_part_t part, const lv_draw_dsc_base_t * base_dsc)
{
    if(base_dsc->layer) {
        /* Accessing the layer opa directly is faster than using get style opa recursive */
        if(part == LV_PART_MAIN) {
            return base_dsc->layer->opa;
        }

        return LV_OPA_MIX2(base_dsc->layer->opa, lv_obj_get_style_opa(obj, part));
    }

    /* fallback to old recursive style opa */
    return lv_obj_get_style_opa_recursive(obj, part);
}


static lv_color_t normal_apply_layer_recolor(const lv_obj_t * obj, lv_part_t part,  const lv_draw_dsc_base_t * base_dsc,
                                             lv_color_t color)
{
    lv_color32_t recolor;

    if(base_dsc->layer) {
        recolor = base_dsc->layer->recolor;
        if(part != LV_PART_MAIN) {
            recolor = lv_obj_style_apply_recolor(obj, part, recolor);
        }
    }
    else {
        recolor = lv_obj_get_style_recolor_recursive(obj, part);
    }

    return lv_color_mix(lv_color_make(recolor.red, recolor.green, recolor.blue), color, recolor.alpha);
}


static lv_color32_t image_apply_layer_recolor(const lv_obj_t * obj, lv_part_t part,
                                              const lv_draw_dsc_base_t * base_dsc, lv_color_t color, lv_opa_t opa)
{
    lv_color32_t recolor;

    if(base_dsc->layer) {
        recolor = base_dsc->layer->recolor;
        if(part != LV_PART_MAIN) {
            recolor = lv_obj_style_apply_recolor(obj, part, recolor);
        }
    }
    else {
        recolor = lv_obj_get_style_recolor_recursive(obj, part);
    }

    if(opa > LV_OPA_TRANSP && recolor.alpha > LV_OPA_TRANSP) {
        return lv_color_over32(recolor, lv_color_to_32(color, opa));
    }
    else if(recolor.alpha > LV_OPA_TRANSP) {
        return recolor;
    }
    else {
        return lv_color_to_32(color, opa);
    }
}
