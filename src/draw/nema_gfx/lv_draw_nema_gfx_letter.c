/**
 * @file lv_draw_nema_gfx_letter.c
 *
 */

/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../../font/lv_font.h"
#include "../../core/lv_refr.h"
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  FORWARD DECLARATION
 **********************/

void nema_raster_rect(int x, int y, int w, int h);

void nema_set_matrix(nema_matrix3x3_t m);

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static lv_res_t draw_nema_gfx_letter_normal(lv_draw_ctx_t * draw_ctx,
                                                                  const lv_draw_label_dsc_t * dsc,
                                                                  const lv_point_t * pos, lv_font_glyph_dsc_t * g, const uint8_t * map_p);

LV_ATTRIBUTE_FAST_MEM static lv_res_t draw_nema_gfx_letter_blend(lv_draw_ctx_t * draw_ctx,
                                                                 const lv_draw_sw_blend_dsc_t * dsc, lv_font_glyph_dsc_t * g);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area  (truncated to draw_buf area)
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
void lv_draw_nema_gfx_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter)
{
    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(dsc->font, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%" PRIX32, letter);

#if LV_USE_FONT_PLACEHOLDER
            /* draw placeholder */
            lv_area_t glyph_coords;
            lv_draw_rect_dsc_t glyph_dsc;
            lv_coord_t begin_x = pos_p->x + g.ofs_x;
            lv_coord_t begin_y = pos_p->y + g.ofs_y;
            lv_area_set(&glyph_coords, begin_x, begin_y, begin_x + g.box_w, begin_y + g.box_h);
            lv_draw_rect_dsc_init(&glyph_dsc);
            glyph_dsc.bg_opa = LV_OPA_MIN;
            glyph_dsc.outline_opa = LV_OPA_MIN;
            glyph_dsc.shadow_opa = LV_OPA_MIN;
            glyph_dsc.bg_img_opa = LV_OPA_MIN;
            glyph_dsc.border_color = dsc->color;
            glyph_dsc.border_width = 1;
            draw_ctx->draw_rect(draw_ctx, &glyph_dsc, &glyph_coords);
#endif
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    lv_point_t gpos;
    gpos.x = pos_p->x + g.ofs_x;
    gpos.y = pos_p->y + (dsc->font->line_height - dsc->font->base_line) - g.box_h - g.ofs_y;

    /*If the letter is completely out of mask don't draw it*/
    if(gpos.x + g.box_w < draw_ctx->clip_area->x1 ||
       gpos.x > draw_ctx->clip_area->x2 ||
       gpos.y + g.box_h < draw_ctx->clip_area->y1 ||
       gpos.y > draw_ctx->clip_area->y2)  {
        return;
    }

    if(g.resolved_font->subpx) {
#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
        return lv_draw_sw_letter(draw_ctx, dsc, pos_p, letter);
#else
        LV_LOG_WARN("Can't draw sub-pixel rendered letter because LV_USE_FONT_SUBPX == 0 in lv_conf.h");
        return ;
#endif
    }

    const uint8_t * map_p = lv_font_get_glyph_bitmap(g.resolved_font, letter);
    if(map_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: character's bitmap not found");
        return;
    }

    if(draw_nema_gfx_letter_normal(draw_ctx, dsc, &gpos, &g, map_p) != LV_RES_OK)
        return lv_draw_sw_letter(draw_ctx, dsc, pos_p, letter);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static lv_res_t draw_nema_gfx_letter_normal(lv_draw_ctx_t * draw_ctx,
                                                                  const lv_draw_label_dsc_t * dsc,
                                                                  const lv_point_t * pos, lv_font_glyph_dsc_t * g, const uint8_t * map_p)
{

    uint32_t bpp = g->bpp;
    lv_opa_t opa = dsc->opa;
    if(bpp == 3) bpp = 4;

#if LV_USE_IMGFONT
    if(bpp == LV_IMGFONT_BPP) { //is imgfont
        lv_area_t fill_area;
        fill_area.x1 = pos->x;
        fill_area.y1 = pos->y;
        fill_area.x2 = pos->x + g->box_w - 1;
        fill_area.y2 = pos->y + g->box_h - 1;
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.angle = 0;
        img_dsc.zoom = LV_IMG_ZOOM_NONE;
        img_dsc.opa = dsc->opa;
        img_dsc.blend_mode = dsc->blend_mode;
        lv_draw_img(draw_ctx, &img_dsc, &fill_area, map_p);
        return;
    }
#endif

    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = pos->x >= draw_ctx->clip_area->x1 ? 0 : draw_ctx->clip_area->x1 - pos->x;
    int32_t col_end   = pos->x + box_w <= draw_ctx->clip_area->x2 ? box_w : draw_ctx->clip_area->x2 - pos->x + 1;
    int32_t row_start = pos->y >= draw_ctx->clip_area->y1 ? 0 : draw_ctx->clip_area->y1 - pos->y;
    int32_t row_end   = pos->y + box_h <= draw_ctx->clip_area->y2 ? box_h : draw_ctx->clip_area->y2 - pos->y + 1;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;
    blend_dsc.blend_mode = dsc->blend_mode;

    lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    uint32_t mask_buf_size = box_w * box_h > hor_res ? hor_res : box_w * box_h;
    blend_dsc.mask_buf = (uint8_t *)map_p;

    /*Initially leave fill area as is, to check for mask_area*/
    lv_area_t fill_area;
    fill_area.x1 = col_start + pos->x;
    fill_area.x2 = col_end  + pos->x - 1;
    fill_area.y1 = row_start + pos->y;
    fill_area.y2 = row_end + pos->y - 1;

#if LV_DRAW_COMPLEX
    lv_coord_t fill_w = lv_area_get_width(&fill_area);
    lv_area_t mask_area;
    lv_area_copy(&mask_area, &fill_area);
    mask_area.y2 = mask_area.y1 + row_end;
    bool mask_any = lv_draw_mask_is_any(&mask_area);
#endif
    /*After the mask checking change fill area to what NemaGFX wants*/
    fill_area.x1 = pos->x;
    fill_area.y1 = pos->y;

    blend_dsc.blend_area = &fill_area;
    blend_dsc.mask_area = &fill_area;

#if LV_DRAW_COMPLEX
    /*Apply masks if any*/
    if(mask_any) {
        return LV_RES_INV;
    }
#endif
    lv_res_t result = draw_nema_gfx_letter_blend(draw_ctx, &blend_dsc, g);
    return result;
}

static inline uint8_t _bpp_nema_gfx_format(lv_font_glyph_dsc_t * g)
{

    uint32_t bpp = g->bpp ;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            return NEMA_A1;
        case 2:
            return NEMA_A2;
        case 4:
            return NEMA_A4;
        default:
            return NEMA_A8;
    }
}

LV_ATTRIBUTE_FAST_MEM static lv_res_t draw_nema_gfx_letter_blend(lv_draw_ctx_t * draw_ctx,
                                                                 const lv_draw_sw_blend_dsc_t * dsc, lv_font_glyph_dsc_t * g)
{
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;

    lv_area_t blend_area;

    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return LV_RES_OK; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(draw_ctx->buf), lv_area_get_width(draw_ctx->buf_area),
                      lv_area_get_height(draw_ctx->buf_area), LV_NEMA_GFX_COLOR_FORMAT,
                      lv_area_get_width(draw_ctx->buf_area)*LV_NEMA_GFX_FORMAT_MULTIPLIER);

    //Set Clipping Area
    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_ctx->clip_area);
    lv_area_move(&clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    uint8_t opacity;
    lv_color32_t col32 = {.full = lv_color_to32(dsc->color)};
    if(dsc->opa < LV_OPA_MAX && dsc->opa > LV_OPA_MIN) {
        opacity = (uint8_t)(((uint16_t)col32.ch.alpha * dsc->opa) >> 8);
    }
    else if(dsc->opa >= LV_OPA_MAX) {
        opacity = col32.ch.alpha;
    }

    uint32_t color = nema_rgba(col32.ch.red, col32.ch.green, col32.ch.blue, opacity);

    nema_set_tex_color(color);

    lv_coord_t x = dsc->blend_area->x1;
    lv_coord_t y = dsc->blend_area->y1;
    lv_coord_t w = g->box_w;
    lv_coord_t h = g->box_h;

    nema_bind_src_tex((uintptr_t)(dsc->mask_buf), w * h, 1, _bpp_nema_gfx_format(g), -1, 1);

    if(opacity < 255U) {
        nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
        nema_set_const_color(color);
    }
    else {
        nema_set_blend_blit(NEMA_BL_SIMPLE);
    }

    nema_matrix3x3_t m = {
        1,    w,   -x - (y * w) - (0.5 * w),
        0,    1,                   0,
        0,    0,                   1
    };

    nema_set_matrix(m);
    nema_raster_rect(x, y, w, h);

    nema_cl_submit(&(nema_gfx_draw_ctx->cl));

    return LV_RES_OK;

}
#endif
