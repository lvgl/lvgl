/**
 * @file lv_obj_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_draw.h"
#include "lv_obj.h"
#include "lv_disp.h"
#include "lv_indev.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, uint32_t part, lv_draw_rect_dsc_t * draw_dsc)
{

#if LV_DRAW_COMPLEX
    draw_dsc->radius = lv_obj_get_style_radius(obj, part);

    lv_opa_t main_opa = part != LV_PART_MAIN ? lv_obj_get_style_opa(obj, part) : LV_OPA_COVER;
    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa <= LV_OPA_MIN || main_opa <= LV_OPA_MIN) {
        draw_dsc->bg_opa = LV_OPA_TRANSP;
        draw_dsc->border_opa = LV_OPA_TRANSP;
        draw_dsc->shadow_opa = LV_OPA_TRANSP;
        draw_dsc->outline_opa = LV_OPA_TRANSP;
        return;
    }

    draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);

    if(draw_dsc->bg_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_opa = lv_obj_get_style_bg_opa(obj, part);
        if(draw_dsc->bg_opa > LV_OPA_MIN) {
            draw_dsc->bg_color = lv_obj_get_style_bg_color_filtered(obj, part);
            const lv_grad_dsc_t * grad = lv_obj_get_style_bg_grad(obj, part);
            if(grad && grad->dir != LV_GRAD_DIR_NONE) {
                lv_memcpy(&draw_dsc->bg_grad, grad, sizeof(*grad));
            }
            else {
                draw_dsc->bg_grad.dir = lv_obj_get_style_bg_grad_dir(obj, part);
                if(draw_dsc->bg_grad.dir != LV_GRAD_DIR_NONE) {
                    draw_dsc->bg_grad.stops[0].color = lv_obj_get_style_bg_color_filtered(obj, part);
                    draw_dsc->bg_grad.stops[1].color = lv_obj_get_style_bg_grad_color_filtered(obj, part);
                    draw_dsc->bg_grad.stops[0].frac = lv_obj_get_style_bg_main_stop(obj, part);
                    draw_dsc->bg_grad.stops[1].frac = lv_obj_get_style_bg_grad_stop(obj, part);
                }
                draw_dsc->bg_grad.dither = lv_obj_get_style_bg_dither_mode(obj, part);
            }
        }
    }

    draw_dsc->border_width = lv_obj_get_style_border_width(obj, part);
    if(draw_dsc->border_width) {
        if(draw_dsc->border_opa != LV_OPA_TRANSP) {
            draw_dsc->border_opa = lv_obj_get_style_border_opa(obj, part);
            if(draw_dsc->border_opa > LV_OPA_MIN) {
                draw_dsc->border_side = lv_obj_get_style_border_side(obj, part);
                draw_dsc->border_color = lv_obj_get_style_border_color_filtered(obj, part);
            }
        }
    }

    draw_dsc->outline_width = lv_obj_get_style_outline_width(obj, part);
    if(draw_dsc->outline_width) {
        if(draw_dsc->outline_opa != LV_OPA_TRANSP) {
            draw_dsc->outline_opa = lv_obj_get_style_outline_opa(obj, part);
            if(draw_dsc->outline_opa > LV_OPA_MIN) {
                draw_dsc->outline_pad = lv_obj_get_style_outline_pad(obj, part);
                draw_dsc->outline_color = lv_obj_get_style_outline_color_filtered(obj, part);
            }
        }
    }

    if(draw_dsc->bg_img_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_img_src = lv_obj_get_style_bg_img_src(obj, part);
        if(draw_dsc->bg_img_src) {
            draw_dsc->bg_img_opa = lv_obj_get_style_bg_img_opa(obj, part);
            if(draw_dsc->bg_img_opa > LV_OPA_MIN) {
                if(lv_img_src_get_type(draw_dsc->bg_img_src) == LV_IMG_SRC_SYMBOL) {
                    draw_dsc->bg_img_symbol_font = lv_obj_get_style_text_font(obj, part);
                    draw_dsc->bg_img_recolor = lv_obj_get_style_text_color_filtered(obj, part);
                }
                else {
                    draw_dsc->bg_img_recolor = lv_obj_get_style_bg_img_recolor_filtered(obj, part);
                    draw_dsc->bg_img_recolor_opa = lv_obj_get_style_bg_img_recolor_opa(obj, part);
                    draw_dsc->bg_img_tiled = lv_obj_get_style_bg_img_tiled(obj, part);
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
                    draw_dsc->shadow_ofs_x = lv_obj_get_style_shadow_ofs_x(obj, part);
                    draw_dsc->shadow_ofs_y = lv_obj_get_style_shadow_ofs_y(obj, part);
                    draw_dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, part);
                    draw_dsc->shadow_color = lv_obj_get_style_shadow_color_filtered(obj, part);
                }
            }
        }
    }

    if(main_opa < LV_OPA_MAX) {
        opa = (uint16_t)((uint16_t) main_opa * opa) >> 8;
    }

    if(opa < LV_OPA_MAX) {
        draw_dsc->bg_opa = (uint16_t)((uint16_t)draw_dsc->bg_opa * opa) >> 8;
        draw_dsc->bg_img_opa = (uint16_t)((uint16_t)draw_dsc->bg_img_opa * opa) >> 8;
        draw_dsc->border_opa = (uint16_t)((uint16_t)draw_dsc->border_opa * opa) >> 8;
        draw_dsc->shadow_opa = (uint16_t)((uint16_t)draw_dsc->shadow_opa * opa) >> 8;
        draw_dsc->outline_opa = (uint16_t)((uint16_t)draw_dsc->outline_opa * opa) >> 8;
    }
#else /*LV_DRAW_COMPLEX*/
    if(draw_dsc->bg_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_opa = lv_obj_get_style_bg_opa(obj, part);
        if(draw_dsc->bg_opa > LV_OPA_MIN) {
            draw_dsc->bg_color = lv_obj_get_style_bg_color_filtered(obj, part);
        }
    }

    draw_dsc->border_width = lv_obj_get_style_border_width(obj, part);
    if(draw_dsc->border_width) {
        if(draw_dsc->border_opa != LV_OPA_TRANSP) {
            draw_dsc->border_opa = lv_obj_get_style_border_opa(obj, part);
            if(draw_dsc->border_opa > LV_OPA_MIN) {
                draw_dsc->border_color = lv_obj_get_style_border_color_filtered(obj, part);
                draw_dsc->border_side = lv_obj_get_style_border_side(obj, part);
            }
        }
    }

    draw_dsc->outline_width = lv_obj_get_style_outline_width(obj, part);
    if(draw_dsc->outline_width) {
        if(draw_dsc->outline_opa != LV_OPA_TRANSP) {
            draw_dsc->outline_opa = lv_obj_get_style_outline_opa(obj, part);
            if(draw_dsc->outline_opa > LV_OPA_MIN) {
                draw_dsc->outline_pad = lv_obj_get_style_outline_pad(obj, part);
                draw_dsc->outline_color = lv_obj_get_style_outline_color_filtered(obj, part);
            }
        }
    }

    if(draw_dsc->bg_img_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_img_src = lv_obj_get_style_bg_img_src(obj, part);
        if(draw_dsc->bg_img_src) {
            draw_dsc->bg_img_opa = lv_obj_get_style_bg_img_opa(obj, part);
            if(draw_dsc->bg_img_opa > LV_OPA_MIN) {
                if(lv_img_src_get_type(draw_dsc->bg_img_src) == LV_IMG_SRC_SYMBOL) {
                    draw_dsc->bg_img_symbol_font = lv_obj_get_style_text_font(obj, part);
                    draw_dsc->bg_img_recolor = lv_obj_get_style_text_color_filtered(obj, part);
                }
                else {
                    draw_dsc->bg_img_recolor = lv_obj_get_style_bg_img_recolor_filtered(obj, part);
                    draw_dsc->bg_img_recolor_opa = lv_obj_get_style_bg_img_recolor_opa(obj, part);
                    draw_dsc->bg_img_tiled = lv_obj_get_style_bg_img_tiled(obj, part);
                }
            }
        }
    }
#endif
}

void lv_obj_init_draw_label_dsc(lv_obj_t * obj, uint32_t part, lv_draw_label_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_text_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN) return;

    draw_dsc->color = lv_obj_get_style_text_color_filtered(obj, part);
    draw_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, part);
    draw_dsc->line_space = lv_obj_get_style_text_line_space(obj, part);
    draw_dsc->decor = lv_obj_get_style_text_decor(obj, part);
#if LV_DRAW_COMPLEX
    draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);
#endif

    draw_dsc->font = lv_obj_get_style_text_font(obj, part);

#if LV_USE_BIDI
    draw_dsc->bidi_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
#endif

    draw_dsc->align = lv_obj_get_style_text_align(obj, part);
}

void lv_obj_init_draw_img_dsc(lv_obj_t * obj, uint32_t part, lv_draw_img_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_img_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    lv_opa_t opa_scale = lv_obj_get_style_opa(obj, part);
    if(opa_scale < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->angle = 0;
    draw_dsc->zoom = LV_IMG_ZOOM_NONE;
    draw_dsc->pivot.x = lv_area_get_width(&obj->coords) / 2;
    draw_dsc->pivot.y = lv_area_get_height(&obj->coords) / 2;

    draw_dsc->recolor_opa = lv_obj_get_style_img_recolor_opa(obj, part);
    if(draw_dsc->recolor_opa > 0) {
        draw_dsc->recolor = lv_obj_get_style_img_recolor_filtered(obj, part);
    }
#if LV_DRAW_COMPLEX
    draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);
#endif
}

void lv_obj_init_draw_line_dsc(lv_obj_t * obj, uint32_t part, lv_draw_line_dsc_t * draw_dsc)
{
    draw_dsc->width = lv_obj_get_style_line_width(obj, part);
    if(draw_dsc->width == 0) return;

    draw_dsc->opa = lv_obj_get_style_line_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->color = lv_obj_get_style_line_color_filtered(obj, part);

    draw_dsc->dash_width = lv_obj_get_style_line_dash_width(obj, part);
    if(draw_dsc->dash_width) {
        draw_dsc->dash_gap = lv_obj_get_style_line_dash_gap(obj, part);
    }

    draw_dsc->round_start = lv_obj_get_style_line_rounded(obj, part);
    draw_dsc->round_end = draw_dsc->round_start;

#if LV_DRAW_COMPLEX
    draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);
#endif
}

void lv_obj_init_draw_arc_dsc(lv_obj_t * obj, uint32_t part, lv_draw_arc_dsc_t * draw_dsc)
{
    draw_dsc->width = lv_obj_get_style_arc_width(obj, part);
    if(draw_dsc->width == 0) return;

    draw_dsc->opa = lv_obj_get_style_arc_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

    draw_dsc->color = lv_obj_get_style_arc_color_filtered(obj, part);
    draw_dsc->img_src = lv_obj_get_style_arc_img_src(obj, part);

    draw_dsc->rounded = lv_obj_get_style_arc_rounded(obj, part);

#if LV_DRAW_COMPLEX
    draw_dsc->blend_mode = lv_obj_get_style_blend_mode(obj, part);
#endif
}

lv_coord_t lv_obj_calculate_ext_draw_size(lv_obj_t * obj, uint32_t part)
{
    lv_coord_t s = 0;

    lv_coord_t sh_width = lv_obj_get_style_shadow_width(obj, part);
    if(sh_width) {
        lv_opa_t sh_opa = lv_obj_get_style_shadow_opa(obj, part);
        if(sh_opa > LV_OPA_MIN) {
            sh_width = sh_width / 2 + 1;    /*The blur adds only half width*/
            sh_width += lv_obj_get_style_shadow_spread(obj, part);
            lv_coord_t sh_ofs_x = lv_obj_get_style_shadow_ofs_x(obj, part);
            lv_coord_t sh_ofs_y = lv_obj_get_style_shadow_ofs_y(obj, part);
            sh_width += LV_MAX(LV_ABS(sh_ofs_x), LV_ABS(sh_ofs_y));
            s = LV_MAX(s, sh_width);
        }
    }

    lv_coord_t outline_width = lv_obj_get_style_outline_width(obj, part);
    if(outline_width) {
        lv_opa_t outline_opa = lv_obj_get_style_outline_opa(obj, part);
        if(outline_opa > LV_OPA_MIN) {
            lv_coord_t outline_pad = lv_obj_get_style_outline_pad(obj, part);
            s = LV_MAX(s, outline_pad + outline_width);
        }
    }

    lv_coord_t w = lv_obj_get_style_transform_width(obj, part);
    lv_coord_t h = lv_obj_get_style_transform_height(obj, part);
    lv_coord_t wh = LV_MAX(w, h);
    if(wh > 0) s += wh;

    return s;
}

void lv_obj_draw_dsc_init(lv_obj_draw_part_dsc_t * dsc, lv_draw_ctx_t * draw_ctx)
{
    lv_memset_00(dsc, sizeof(lv_obj_draw_part_dsc_t));
    dsc->draw_ctx = draw_ctx;
}

bool lv_obj_draw_part_check_type(lv_obj_draw_part_dsc_t * dsc, const lv_obj_class_t * class_p, uint32_t type)
{
    if(dsc->class_p == class_p && dsc->type == type) return true;
    else return false;
}

void lv_obj_refresh_ext_draw_size(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t s_old = _lv_obj_get_ext_draw_size(obj);
    lv_coord_t s_new = 0;
    lv_event_send(obj, LV_EVENT_REFR_EXT_DRAW_SIZE, &s_new);

    if(s_new != s_old) lv_obj_invalidate(obj);

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
}

lv_coord_t _lv_obj_get_ext_draw_size(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->ext_draw_size;
    else return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
