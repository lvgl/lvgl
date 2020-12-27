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
#define LV_OBJX_NAME "lv_obj"

#define SCROLLBAR_MIN_SIZE (LV_DPX(10))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a rectangle draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_PART_SCROLLBAR`, `LV_BTN_PART_REL` or `LV_SLIDER_PART_KNOB`
 * @param draw_dsc the descriptor the initialize.
 *                 If an `*_opa` filed is set to `LV_OPA_TRANSP` the related properties won't be initialized.
 *                 Should be initialized with `lv_draw_rect_dsc_init(draw_dsc)`.
 * @note Only the relevant fields will be set.
 *       E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, uint8_t part, lv_draw_rect_dsc_t * draw_dsc)
{
    draw_dsc->radius = lv_obj_get_style_radius(obj, part);

#if LV_USE_OPA_SCALE
    lv_opa_t main_opa = part != LV_PART_MAIN ? lv_obj_get_style_opa(obj, part) : LV_OPA_COVER;
    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa <= LV_OPA_MIN || main_opa <= LV_OPA_MIN) {
        draw_dsc->bg_opa = LV_OPA_TRANSP;
        draw_dsc->border_opa = LV_OPA_TRANSP;
        draw_dsc->shadow_opa = LV_OPA_TRANSP;
        draw_dsc->outline_opa = LV_OPA_TRANSP;
        return;
    }
#endif

    if(draw_dsc->bg_opa != LV_OPA_TRANSP) {
        draw_dsc->bg_opa = lv_obj_get_style_bg_opa(obj, part);
        if(draw_dsc->bg_opa > LV_OPA_MIN) {
            draw_dsc->bg_color = lv_obj_get_style_bg_color_filtered(obj, part);
            draw_dsc->bg_grad_dir =  lv_obj_get_style_bg_grad_dir(obj, part);
            if(draw_dsc->bg_grad_dir != LV_GRAD_DIR_NONE) {
                draw_dsc->bg_grad_color = lv_obj_get_style_bg_grad_color(obj, part);
                draw_dsc->bg_main_color_stop =  lv_obj_get_style_bg_main_stop(obj, part);
                draw_dsc->bg_grad_color_stop =  lv_obj_get_style_bg_grad_stop(obj, part);
            }

#if LV_USE_BLEND_MODES
            draw_dsc->bg_blend_mode = lv_obj_get_style_bg_blend_mode(obj, part);
#endif
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
#if LV_USE_BLEND_MODES
            draw_dsc->border_blend_mode = lv_obj_get_style_border_blend_mode(obj, part);
#endif
        }
    }

#if LV_USE_OUTLINE
    draw_dsc->outline_width = lv_obj_get_style_outline_width(obj, part);
    if(draw_dsc->outline_width) {
        if(draw_dsc->outline_opa != LV_OPA_TRANSP) {
            draw_dsc->outline_opa = lv_obj_get_style_outline_opa(obj, part);
            if(draw_dsc->outline_opa > LV_OPA_MIN) {
                draw_dsc->outline_pad = lv_obj_get_style_outline_pad(obj, part);
                draw_dsc->outline_color = lv_obj_get_style_outline_color(obj, part);
            }
#if LV_USE_BLEND_MODES
            draw_dsc->outline_blend_mode = lv_obj_get_style_outline_blend_mode(obj, part);
#endif
        }
    }
#endif


#if LV_USE_PATTERN
    if(draw_dsc->content_opa != LV_OPA_TRANSP) {
        draw_dsc->content_src = lv_obj_get_style_content_src(obj, part);
        if(draw_dsc->content_src) {
            draw_dsc->content_ofs_y = lv_obj_get_style_content_ofs_y(obj, part);
            draw_dsc->content_ofs_x = lv_obj_get_style_content_ofs_x(obj, part);
            draw_dsc->content_align = lv_obj_get_style_content_align(obj, part);

            if(lv_img_src_get_type(draw_dsc->content_src) == LV_IMG_SRC_SYMBOL) {
                draw_dsc->content_opa = lv_obj_get_style_text_opa(obj, part);
                if(draw_dsc->content_opa > LV_OPA_MIN) {
                    draw_dsc->content_font = lv_obj_get_style_text_font(obj, part);
                    draw_dsc->content_color = lv_obj_get_style_text_color(obj, part);
                    draw_dsc->content_letter_space = lv_obj_get_style_text_letter_space(obj, part);
                    draw_dsc->content_line_space = lv_obj_get_style_text_line_space(obj, part);
#if LV_USE_BLEND_MODES
                    draw_dsc->content_blend_mode = lv_obj_get_style_text_blend_mode(obj, part);
#endif
                }
            }
            else {
                draw_dsc->content_opa = lv_obj_get_style_img_opa(obj, part);
                if(draw_dsc->content_opa > LV_OPA_MIN) {
                    draw_dsc->content_color = lv_obj_get_style_img_recolor(obj, part);
                    draw_dsc->content_recolor_opa = lv_obj_get_style_img_recolor_opa(obj, part);

#if LV_USE_BLEND_MODES
                    draw_dsc->content_blend_mode = lv_obj_get_style_img_blend_mode(obj, part);
#endif
                }
            }
        }
    }

#endif

#if LV_USE_SHADOW
    if(draw_dsc->shadow_opa) {
        draw_dsc->shadow_width = lv_obj_get_style_shadow_width(obj, part);
        if(draw_dsc->shadow_width) {
            if(draw_dsc->shadow_opa > LV_OPA_MIN) {
                draw_dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, part);
                if(draw_dsc->shadow_opa > LV_OPA_MIN) {
                    draw_dsc->shadow_ofs_x = lv_obj_get_style_shadow_ofs_x(obj, part);
                    draw_dsc->shadow_ofs_y = lv_obj_get_style_shadow_ofs_y(obj, part);
                    draw_dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, part);
                    draw_dsc->shadow_color = lv_obj_get_style_shadow_color(obj, part);
#if LV_USE_BLEND_MODES
                    draw_dsc->shadow_blend_mode = lv_obj_get_style_shadow_blend_mode(obj, part);
#endif
                }
            }
        }
    }
#endif

#if LV_USE_OPA_SCALE
    if(main_opa < LV_OPA_MAX) {
        opa = (uint16_t)((uint16_t) main_opa * opa) >> 8;
    }

    if(opa < LV_OPA_MAX) {
        draw_dsc->bg_opa = (uint16_t)((uint16_t)draw_dsc->bg_opa * opa) >> 8;
        draw_dsc->border_opa = (uint16_t)((uint16_t)draw_dsc->border_opa * opa) >> 8;
        draw_dsc->shadow_opa = (uint16_t)((uint16_t)draw_dsc->shadow_opa * opa) >> 8;
        draw_dsc->content_opa = (uint16_t)((uint16_t)draw_dsc->content_opa * opa) >> 8;
        draw_dsc->outline_opa = (uint16_t)((uint16_t)draw_dsc->outline_opa * opa) >> 8;
    }
#endif
}

/**
 * Initialize a label draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_PART_SCROLLBAR`, `LV_BTN_PART_REL` or `LV_SLIDER_PART_KNOB`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_label_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_label_dsc(lv_obj_t * obj, uint8_t part, lv_draw_label_dsc_t * draw_dsc)
{
    draw_dsc->opa = lv_obj_get_style_text_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN) return;

#if LV_USE_OPA_SCALE
    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN) return;
#endif

    draw_dsc->color = lv_obj_get_style_text_color_filtered(obj, part);
    draw_dsc->letter_space = lv_obj_get_style_text_letter_space(obj, part);
    draw_dsc->line_space = lv_obj_get_style_text_line_space(obj, part);
    draw_dsc->decor = lv_obj_get_style_text_decor(obj, part);
#if LV_USE_BLEND_MODES
    draw_dsc->blend_mode = lv_obj_get_style_text_blend_mode(obj, part);
#endif

    draw_dsc->font = lv_obj_get_style_text_font(obj, part);

//    if(draw_dsc->sel_start != LV_DRAW_LABEL_NO_TXT_SEL && draw_dsc->sel_end != LV_DRAW_LABEL_NO_TXT_SEL) {
//        draw_dsc->color = lv_obj_get_style_text_sel_color(obj, part);
//    }

#if LV_USE_BIDI
    draw_dsc->bidi_dir = lv_obj_get_base_dir(obj);
#endif
}

/**
 * Initialize an image draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_IMG_PART_MAIN`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_image_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_img_dsc(lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc)
{
//    draw_dsc->opa = lv_obj_get_style_image_opa(obj, part);
//    if(draw_dsc->opa <= LV_OPA_MIN)  return;
//
//#if LV_USE_OPA_SCALE
//    lv_opa_t opa_scale = lv_obj_get_style_opa_scale(obj, part);
//    if(opa_scale < LV_OPA_MAX) {
//        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa_scale) >> 8;
//    }
//    if(draw_dsc->opa <= LV_OPA_MIN)  return;
//#endif
//
//    draw_dsc->angle = 0;
//    draw_dsc->zoom = LV_IMG_ZOOM_NONE;
//    draw_dsc->pivot.x = lv_area_get_width(&obj->coords) / 2;
//    draw_dsc->pivot.y = lv_area_get_height(&obj->coords) / 2;
//
//    draw_dsc->recolor_opa = lv_obj_get_style_image_recolor_opa(obj, part);
//    if(draw_dsc->recolor_opa > 0) {
//        draw_dsc->recolor = lv_obj_get_style_image_recolor(obj, part);
//    }
//#if LV_USE_BLEND_MODES
//    draw_dsc->blend_mode = lv_obj_get_style_image_blend_mode(obj, part);
//#endif
}


/**
 * Initialize a line draw descriptor from an object's styles in its current state
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_LINE_PART_MAIN`
 * @param draw_dsc the descriptor the initialize.
 *                 If the `opa` filed is set to or the property is equal to `LV_OPA_TRANSP` the rest won't be initialized.
 *                 Should be initialized with `lv_draw_line_dsc_init(draw_dsc)`.
 */
void lv_obj_init_draw_line_dsc(lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc)
{
    draw_dsc->width = lv_obj_get_style_line_width(obj, part);
    if(draw_dsc->width == 0) return;

    draw_dsc->opa = lv_obj_get_style_line_opa(obj, part);
    if(draw_dsc->opa <= LV_OPA_MIN)  return;

#if LV_USE_OPA_SCALE
    lv_opa_t opa = lv_obj_get_style_opa(obj, part);
    if(opa < LV_OPA_MAX) {
        draw_dsc->opa = (uint16_t)((uint16_t)draw_dsc->opa * opa) >> 8;
    }
    if(draw_dsc->opa <= LV_OPA_MIN)  return;
#endif

    draw_dsc->color = lv_obj_get_style_line_color(obj, part);

    draw_dsc->dash_width = lv_obj_get_style_line_dash_width(obj, part);
    if(draw_dsc->dash_width) {
        draw_dsc->dash_gap = lv_obj_get_style_line_dash_gap(obj, part);
    }

    draw_dsc->round_start = lv_obj_get_style_line_rounded(obj, part);
    draw_dsc->round_end = draw_dsc->round_start;

#if LV_USE_BLEND_MODES
    draw_dsc->blend_mode = lv_obj_get_style_line_blend_mode(obj, part);
#endif
}

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj pointer to an object
 * @param part part of the object
 * @return the extra size required around the object
 */
lv_coord_t _lv_obj_get_draw_rect_ext_pad_size(lv_obj_t * obj, uint8_t part)
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
            sh_width += LV_MATH_MAX(LV_MATH_ABS(sh_ofs_x), LV_MATH_ABS(sh_ofs_y));
            s = LV_MATH_MAX(s, sh_width);
        }
    }

    lv_coord_t outline_width = lv_obj_get_style_outline_width(obj, part);
    if(outline_width) {
        lv_opa_t outline_opa = lv_obj_get_style_outline_opa(obj, part);
        if(outline_opa > LV_OPA_MIN) {
            lv_coord_t outline_pad = lv_obj_get_style_outline_pad(obj, part);
            s = LV_MATH_MAX(s, outline_pad + outline_width);
        }
    }

    const void * content_src = lv_obj_get_style_content_src(obj, part);
    if(content_src) {
        lv_opa_t content_opa;
        lv_point_t content_size;
        if(lv_img_src_get_type(content_src) == LV_IMG_SRC_SYMBOL) {
            content_opa = lv_obj_get_style_text_opa(obj, part);
            if(content_opa > 0) {
                lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, part);
                lv_coord_t line_space = lv_obj_get_style_text_letter_space(obj, part);
                const lv_font_t * font = lv_obj_get_style_text_font(obj, part);
                _lv_txt_get_size(&content_size, content_src, font, letter_space, line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);
            }
        } else {
            content_opa = lv_obj_get_style_img_opa(obj, part);
                if(content_opa > 0) {
                lv_img_header_t header;
                lv_img_decoder_get_info(content_src, &header);
                content_size.x = header.w;
                content_size.y = header.h;
            }
        }

        if(content_opa > 0) {
            lv_area_t content_area;
            content_area.x1 = 0;
            content_area.y1 = 0;
            content_area.x2 = content_size.x - 1;
            content_area.y2 = content_size.y - 1;

            lv_align_t align = lv_obj_get_style_content_align(obj, part);
            lv_coord_t xofs = lv_obj_get_style_content_ofs_x(obj, part);
            lv_coord_t yofs = lv_obj_get_style_content_ofs_y(obj, part);
            lv_point_t p_align;
            _lv_area_align(&obj->coords, &content_area, align, &p_align);

            content_area.x1 += p_align.x + xofs;
            content_area.y1 += p_align.y + yofs;
            content_area.x2 += p_align.x + xofs;
            content_area.y2 += p_align.y + yofs;

            s = LV_MATH_MAX(s, obj->coords.x1 - content_area.x1);
            s = LV_MATH_MAX(s, obj->coords.y1 - content_area.y1);
            s = LV_MATH_MAX(s, content_area.x2 - obj->coords.x2);
            s = LV_MATH_MAX(s, content_area.y2 - obj->coords.y2);
        }
    }

    lv_coord_t w = lv_obj_get_style_transform_width(obj, part);
    lv_coord_t h = lv_obj_get_style_transform_height(obj, part);
    lv_coord_t wh = LV_MATH_MAX(w, h);
    if(wh > 0) s += wh;

    return s;
}

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object to refresh the extended draw area.
 * The result will be written into `obj->ext_draw_pad`
 * @param obj pointer to an object
 */
void _lv_obj_refresh_ext_draw_pad(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t s = 0;
    lv_signal_send(obj, LV_SIGNAL_REFR_EXT_DRAW_PAD, &s);

    /*Store the result if the special attrs already allocated*/
    if(obj->spec_attr) {
        obj->spec_attr->ext_draw_pad = s;
    }
    /* Allocate spec. attrs. only if the result is not zero.
     * Zero is the default value if the spec. attr. are not defined. */
    else if(s != 0) {
        lv_obj_allocate_spec_attr(obj);
        obj->spec_attr->ext_draw_pad = s;
    }
}

/**
 * Draw scrollbars on an object is required
 * @param obj pointer to an object
 * @param clip_area the clip area coming from the design function
 */
void _lv_obj_draw_scrollbar(lv_obj_t * obj, const lv_area_t * clip_area)
{
    return;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE) == false) return;

    lv_scroll_dir_t sm = lv_obj_get_scrollbar_mode(obj);
    if(sm == LV_SCROLLBAR_MODE_OFF)  return;

    /*If there is no indev scrolling this object but `mode==active` return*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    if(sm == LV_SCROLLBAR_MODE_ACTIVE) {
        while(indev) {
            if(lv_indev_get_scroll_obj(indev) == obj) break;
            indev = lv_indev_get_next(indev);
        }
        if(indev == NULL)  return;
    }

    lv_coord_t st = lv_obj_get_scroll_top(obj);
    lv_coord_t sb = lv_obj_get_scroll_bottom(obj);
    lv_coord_t sl = lv_obj_get_scroll_left(obj);
    lv_coord_t sr = lv_obj_get_scroll_right(obj);

    lv_scroll_dir_t dir = lv_obj_get_scroll_dir(obj);

    bool ver_draw = false;
    if((dir & LV_DIR_VER) &&
       ((sm == LV_SCROLLBAR_MODE_ON) ||
        (sm == LV_SCROLLBAR_MODE_AUTO && (st > 0 || sb > 0)) ||
        (sm == LV_SCROLLBAR_MODE_ACTIVE && lv_indev_get_scroll_dir(indev) == LV_SCROLL_DIR_VER))) {
        ver_draw = true;
    }


    bool hor_draw = false;
    if((dir & LV_DIR_HOR) &&
          ((sm == LV_SCROLLBAR_MODE_ON) ||
           (sm == LV_SCROLLBAR_MODE_AUTO && (sl > 0 || sr > 0)) ||
           (sm == LV_SCROLLBAR_MODE_ACTIVE && lv_indev_get_scroll_dir(indev) == LV_SCROLL_DIR_HOR))) {
        hor_draw = true;
    }

    if(!hor_draw && !ver_draw) return;

    lv_coord_t end_space = lv_obj_get_style_margin_top(obj, LV_PART_SCROLLBAR);
    lv_coord_t side_space = lv_obj_get_style_margin_right(obj, LV_PART_SCROLLBAR);
    lv_coord_t tickness = lv_obj_get_style_pad_left(obj, LV_PART_SCROLLBAR);

    lv_coord_t obj_h = lv_obj_get_height(obj);
    lv_coord_t obj_w = lv_obj_get_width(obj);

    lv_coord_t ver_reg_space = ver_draw ? tickness + side_space : 0;
    lv_coord_t hor_req_space = hor_draw ? tickness + side_space : 0;
    lv_coord_t rem;

    lv_draw_rect_dsc_t draw_dsc;
    lv_res_t sb_res = scrollbar_init_draw_dsc(obj, &draw_dsc);
    if(sb_res != LV_RES_OK) return;

    lv_area_t area;
    area.y1 = obj->coords.y1;
    area.y2 = obj->coords.y2;
    area.x2 = obj->coords.x2 - side_space;
    area.x1 = area.x2 - tickness;

    /*Draw horizontal scrollbar if the mode is ON or can be scrolled in this direction*/
    if(ver_draw && _lv_area_is_on(&area, clip_area)) {
        lv_coord_t content_h = obj_h + st + sb;
        lv_coord_t sb_h = ((obj_h - end_space * 2 - hor_req_space) * obj_h) / content_h;
        sb_h = LV_MATH_MAX(sb_h, SCROLLBAR_MIN_SIZE);
        rem = (obj_h - end_space * 2 - hor_req_space) - sb_h;  /*Remaining size from the scrollbar track that is not the scrollbar itself*/
        lv_coord_t scroll_h = content_h - obj_h; /*The size of the content which can be really scrolled*/
        if(scroll_h <= 0) {
            area.y1 = obj->coords.y1 + end_space;
            area.y2 = obj->coords.y2 - end_space - hor_req_space - 1;
            area.x2 = obj->coords.x2 - side_space;
            area.x1 = area.x2 - tickness + 1;
        } else {
            lv_coord_t sb_y = (rem * sb) / scroll_h;
            sb_y = rem - sb_y;

            area.y1 = obj->coords.y1 + sb_y + end_space;
            area.y2 = area.y1 + sb_h - 1;
            area.x2 = obj->coords.x2 - side_space;
            area.x1 = area.x2 - tickness;
            if(area.y1 < obj->coords.y1 + end_space) {
                area.y1 = obj->coords.y1 + end_space;
                if(area.y1 + SCROLLBAR_MIN_SIZE > area.y2) area.y2 = area.y1 + SCROLLBAR_MIN_SIZE;
            }
            if(area.y2 > obj->coords.y2 - hor_req_space - end_space) {
                area.y2 = obj->coords.y2 - hor_req_space - end_space;
                if(area.y2 - SCROLLBAR_MIN_SIZE < area.y1) area.y1 = area.y2 - SCROLLBAR_MIN_SIZE;
            }
        }
        lv_draw_rect(&area, clip_area, &draw_dsc);
    }

    area.y2 = obj->coords.y2 - side_space;
    area.y1 =area.y2 - tickness;
    area.x1 = obj->coords.x1;
    area.x2 = obj->coords.x2;
    /*Draw horizontal scrollbar if the mode is ON or can be scrolled in this direction*/
    if(hor_draw && _lv_area_is_on(&area, clip_area)) {
        lv_coord_t content_w = obj_w + sl + sr;
        lv_coord_t sb_w = ((obj_w - end_space * 2 - ver_reg_space) * obj_w) / content_w;
        sb_w = LV_MATH_MAX(sb_w, SCROLLBAR_MIN_SIZE);
        rem = (obj_w - end_space * 2 - ver_reg_space) - sb_w;  /*Remaining size from the scrollbar track that is not the scrollbar itself*/
        lv_coord_t scroll_w = content_w - obj_w; /*The size of the content which can be really scrolled*/
        if(scroll_w <= 0) {
            area.y2 = obj->coords.y2 - side_space;
            area.y1 = area.y2 - tickness + 1;
            area.x1 = obj->coords.x1 + end_space;
            area.x2 = obj->coords.x2 - end_space - ver_reg_space - 1;
        } else {
            lv_coord_t sb_x = (rem * sr) / scroll_w;
            sb_x = rem - sb_x;

            area.x1 = obj->coords.x1 + sb_x + end_space;
            area.x2 = area.x1 + sb_w - 1;
            area.y2 = obj->coords.y2 - side_space;
            area.y1 = area.y2 - tickness;
            if(area.x1 < obj->coords.x1 + end_space) {
                area.x1 = obj->coords.x1 + end_space;
                if(area.x1 + SCROLLBAR_MIN_SIZE > area.x2) area.x2 = area.x1 + SCROLLBAR_MIN_SIZE;
            }
            if(area.x2 > obj->coords.x2 - ver_reg_space - end_space) {
                area.x2 = obj->coords.x2 - ver_reg_space - end_space;
                if(area.x2 - SCROLLBAR_MIN_SIZE < area.x1) area.x1 = area.x2 - SCROLLBAR_MIN_SIZE;
            }
        }
        lv_draw_rect(&area, clip_area, &draw_dsc);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the draw descriptor for the scrollbar
 * @param obj pointer to an object
 * @param dsc the draw descriptor to initialize
 * @return LV_RES_OK: the scrollbar is visible; LV_RES_INV: the scrollbar is not visible
 */
static lv_res_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc)
{
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_opa = lv_obj_get_style_bg_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->bg_opa > LV_OPA_MIN) {
        dsc->bg_color = lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR);
    }

    dsc->border_opa = lv_obj_get_style_border_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->border_opa > LV_OPA_MIN) {
        dsc->border_width = lv_obj_get_style_border_width(obj, LV_PART_SCROLLBAR);
        if(dsc->border_width > 0) {
            dsc->border_color = lv_obj_get_style_border_color(obj, LV_PART_SCROLLBAR);
        } else {
            dsc->border_opa = LV_OPA_TRANSP;
        }
    }

    lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_SCROLLBAR);
    if(opa < LV_OPA_MAX) {
        dsc->bg_opa = (dsc->bg_opa * opa) >> 8;
        dsc->border_opa = (dsc->bg_opa * opa) >> 8;
    }

    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP) {
        dsc->radius = lv_obj_get_style_radius(obj, LV_PART_SCROLLBAR);
        return LV_RES_OK;
    } else {
        return LV_RES_INV;
    }
}

