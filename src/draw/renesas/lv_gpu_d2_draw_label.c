/**
 * @file lv_gpu_d2_draw_label.c
 *
 * @description HAL layer for display driver
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_draw_label.h"
#include "../../misc/lv_assert.h"
#include "../../core/lv_refr.h"
#include "lv_gpu_d2_ra6m3.h"

#if LV_USE_GPU_RA6M3_G2D
#include LV_GPU_RA6M3_G2D_INCLUDE

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern const uint8_t _lv_bpp1_opa_table[2];
extern const uint8_t _lv_bpp2_opa_table[4];
extern const uint8_t _lv_bpp4_opa_table[16];
extern const uint8_t _lv_bpp8_opa_table[256];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                                     const lv_point_t * pos, lv_font_glyph_dsc_t * g, const uint8_t * map_p)
{

    const uint8_t * bpp_opa_table_p;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    lv_opa_t opa = dsc->opa;
    uint32_t shades;
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

    switch(bpp) {
        case 1:
            bpp_opa_table_p = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            shades = 2;
            break;
        case 2:
            bpp_opa_table_p = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            shades = 4;
            break;
        case 4:
            bpp_opa_table_p = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            shades = 16;
            break;
        case 8:
            bpp_opa_table_p = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            shades = 256;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp");
            return; /*Invalid bpp. Can't render the letter*/
    }

    static lv_opa_t opa_table[256];
    static lv_opa_t prev_opa = LV_OPA_TRANSP;
    static uint32_t prev_bpp = 0;
    if(opa < LV_OPA_MAX) {
        if(prev_opa != opa || prev_bpp != bpp) {
            uint32_t i;
            for(i = 0; i < shades; i++) {
                opa_table[i] = bpp_opa_table_p[i] == LV_OPA_COVER ? opa : ((bpp_opa_table_p[i] * opa) >> 8);
            }
        }
        bpp_opa_table_p = opa_table;
        prev_opa = opa;
        prev_bpp = bpp;
    }

    int32_t col, row;
    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = pos->x >= draw_ctx->clip_area->x1 ? 0 : draw_ctx->clip_area->x1 - pos->x;
    int32_t col_end   = pos->x + box_w <= draw_ctx->clip_area->x2 ? box_w : draw_ctx->clip_area->x2 - pos->x + 1;
    int32_t row_start = pos->y >= draw_ctx->clip_area->y1 ? 0 : draw_ctx->clip_area->y1 - pos->y;
    int32_t row_end   = pos->y + box_h <= draw_ctx->clip_area->y2 ? box_h : draw_ctx->clip_area->y2 - pos->y + 1;

    /*Move on the map too*/
    uint32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    uint32_t col_bit;
    col_bit = bit_ofs & 0x7; /*"& 0x7" equals to "% 8" just faster*/

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.color = dsc->color;
    blend_dsc.opa = LV_OPA_COVER;
    blend_dsc.blend_mode = dsc->blend_mode;

    lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    uint32_t mask_buf_size = box_w * box_h > hor_res ? hor_res : box_w * box_h;
    lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
    blend_dsc.mask_buf = mask_buf;
    int32_t mask_p = 0;

    lv_area_t fill_area;
    fill_area.x1 = col_start + pos->x;
    fill_area.x2 = col_end  + pos->x - 1;
    fill_area.y1 = row_start + pos->y;
    fill_area.y2 = fill_area.y1;
#if LV_DRAW_COMPLEX
    lv_coord_t fill_w = lv_area_get_width(&fill_area);
    lv_area_t mask_area;
    lv_area_copy(&mask_area, &fill_area);
    mask_area.y2 = mask_area.y1 + row_end;
    bool mask_any = lv_draw_mask_is_any(&mask_area);
#endif
    blend_dsc.blend_area = &fill_area;
    blend_dsc.mask_area = &fill_area;

    uint32_t col_bit_max = 8 - bpp;
    uint32_t col_bit_row_ofs = (box_w + col_start - col_end) * bpp;

    for(row = row_start ; row < row_end; row++) {
#if LV_DRAW_COMPLEX
        int32_t mask_p_start = mask_p;
#endif
        bitmask = bitmask_init >> col_bit;
        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (col_bit_max - col_bit);
            if(letter_px) {
                mask_buf[mask_p] = bpp_opa_table_p[letter_px];
            }
            else {
                mask_buf[mask_p] = 0;
            }

            /*Go to the next column*/
            if(col_bit < col_bit_max) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }

            /*Next mask byte*/
            mask_p++;
        }

#if LV_DRAW_COMPLEX
        /*Apply masks if any*/
        if(mask_any) {
            blend_dsc.mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, fill_area.x1, fill_area.y2,
                                                    fill_w);
            if(blend_dsc.mask_res == LV_DRAW_MASK_RES_TRANSP) {
                lv_memset_00(mask_buf + mask_p_start, fill_w);
            }
        }
#endif

        if((uint32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            fill_area.y2 ++;
        }
        else {
            blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
            lv_draw_ra6m3_2d_blend(draw_ctx, &blend_dsc);

            fill_area.y1 = fill_area.y2 + 1;
            fill_area.y2 = fill_area.y1;
            mask_p = 0;
        }

        col_bit += col_bit_row_ofs;
        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;
    }

    /*Flush the last part*/
    if(fill_area.y1 != fill_area.y2) {
        fill_area.y2--;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        lv_draw_ra6m3_2d_blend(draw_ctx, &blend_dsc);
        mask_p = 0;
    }

    lv_mem_buf_release(mask_buf);
}

void lv_draw_gpu_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                        uint32_t letter)
{
    const lv_font_t * font_p = dsc->font;

    lv_opa_t opa = dsc->opa;
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%X", letter);
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
       gpos.y > draw_ctx->clip_area->y2) {
        return;
    }

    const uint8_t * map_p = lv_font_get_glyph_bitmap(font_p, letter);
    if(map_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: character's bitmap not found");
        return;
    }

    if(font_p->subpx) {
#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
        draw_letter_subpx(pos_x, pos_y, &g, clip_area, map_p, color, opa, blend_mode);
#else
        LV_LOG_WARN("Can't draw sub-pixel rendered letter because LV_USE_FONT_SUBPX == 0 in lv_conf.h");
#endif
    }
    else {
        draw_letter_normal(draw_ctx, dsc, &gpos, &g, map_p);
    }
}

#endif
