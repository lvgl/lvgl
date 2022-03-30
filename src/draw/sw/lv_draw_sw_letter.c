/**
 * @file lv_draw_sw_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../../font/lv_font.h"
#include "../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_letter_normal(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                                     const lv_point_t * pos, lv_font_glyph_dsc_t * g, const uint8_t * map_p);


#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
static void draw_letter_subpx(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc, const lv_point_t * pos,
                              lv_font_glyph_dsc_t * g, const uint8_t * map_p);
#endif /*LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX*/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

const uint8_t _lv_bpp1_opa_table[2]  = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
const uint8_t _lv_bpp2_opa_table[4]  = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

const uint8_t _lv_bpp3_opa_table[8]  = {0, 36,  73, 109,   /*Opacity mapping with bpp = 3*/
                                        146, 182,  219, 255
                                       };

const uint8_t _lv_bpp4_opa_table[16] = {0,  17, 34,  51,  /*Opacity mapping with bpp = 4*/
                                        68, 85, 102, 119,
                                        136, 153, 170, 187,
                                        204, 221, 238, 255
                                       };

const uint8_t _lv_bpp8_opa_table[256] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                         32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                         48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
                                         64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                                         80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
                                         96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                                         112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                         128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
                                         144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                                         160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
                                         176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
                                         192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
                                         208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
                                         224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
                                         240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
                                        };

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
void lv_draw_sw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
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

    const uint8_t * map_p = lv_font_get_glyph_bitmap(g.resolved_font, letter);
    if(map_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: character's bitmap not found");
        return;
    }

    if(g.resolved_font->subpx) {
#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
        draw_letter_subpx(draw_ctx, dsc, &gpos, &g, map_p);
#else
        LV_LOG_WARN("Can't draw sub-pixel rendered letter because LV_USE_FONT_SUBPX == 0 in lv_conf.h");
#endif
    }
    else {
        draw_letter_normal(draw_ctx, dsc, &gpos, &g, map_p);
    }
}

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
    blend_dsc.opa = dsc->opa;
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
            lv_draw_sw_blend(draw_ctx, &blend_dsc);

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
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
        mask_p = 0;
    }

    lv_mem_buf_release(mask_buf);
}

#if LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX
static void draw_letter_subpx(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc, const lv_point_t * pos,
                              lv_font_glyph_dsc_t * g, const uint8_t * map_p)
{
    const uint8_t * bpp_opa_table;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    lv_opa_t opa = dsc->opa;
    if(bpp == 3) bpp = 4;

    switch(bpp) {
        case 1:
            bpp_opa_table = _lv_bpp1_opa_table;
            bitmask_init  = 0x80;
            break;
        case 2:
            bpp_opa_table = _lv_bpp2_opa_table;
            bitmask_init  = 0xC0;
            break;
        case 4:
            bpp_opa_table = _lv_bpp4_opa_table;
            bitmask_init  = 0xF0;
            break;
        case 8:
            bpp_opa_table = _lv_bpp8_opa_table;
            bitmask_init  = 0xFF;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            LV_LOG_WARN("lv_draw_letter: invalid bpp not found");
            return; /*Invalid bpp. Can't render the letter*/
    }

    int32_t col, row;

    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = box_w * bpp; /*Letter width in bits*/

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = pos->x >= draw_ctx->clip_area->x1 ? 0 : (draw_ctx->clip_area->x1 - pos->x) * 3;
    int32_t col_end   = pos->x + box_w / 3 <= draw_ctx->clip_area->x2 ? box_w : (draw_ctx->clip_area->x2 - pos->x + 1) * 3;
    int32_t row_start = pos->y >= draw_ctx->clip_area->y1 ? 0 : draw_ctx->clip_area->y1 - pos->y;
    int32_t row_end   = pos->y + box_h <= draw_ctx->clip_area->y2 ? box_h : draw_ctx->clip_area->y2 - pos->y + 1;

    /*Move on the map too*/
    int32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    lv_opa_t px_opa;
    int32_t col_bit;
    col_bit = bit_ofs & 0x7; /*"& 0x7" equals to "% 8" just faster*/

    lv_area_t map_area;
    map_area.x1 = col_start / 3 + pos->x;
    map_area.x2 = col_end / 3  + pos->x - 1;
    map_area.y1 = row_start + pos->y;
    map_area.y2 = map_area.y1;

    if(map_area.x2 <= map_area.x1) return;

    lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    int32_t mask_buf_size = box_w * box_h > hor_res ? hor_res : g->box_w * g->box_h;
    lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
    int32_t mask_p = 0;

    lv_color_t * color_buf = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));

    int32_t dest_buf_stride = lv_area_get_width(draw_ctx->buf_area);
    lv_color_t * dest_buf_tmp = draw_ctx->buf;

    /*Set a pointer on draw_buf to the first pixel of the letter*/
    dest_buf_tmp += ((pos->y - draw_ctx->buf_area->y1) * dest_buf_stride) + pos->x - draw_ctx->buf_area->x1;

    /*If the letter is partially out of mask the move there on draw_buf*/
    dest_buf_tmp += (row_start * dest_buf_stride) + col_start / 3;

    lv_area_t mask_area;
    lv_area_copy(&mask_area, &map_area);
    mask_area.y2 = mask_area.y1 + row_end;
    bool mask_any = lv_draw_mask_is_any(&map_area);
    uint8_t font_rgb[3];

    lv_color_t color = dsc->color;
#if LV_COLOR_16_SWAP == 0
    uint8_t txt_rgb[3] = {color.ch.red, color.ch.green, color.ch.blue};
#else
    uint8_t txt_rgb[3] = {color.ch.red, (color.ch.green_h << 3) + color.ch.green_l, color.ch.blue};
#endif

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &map_area;
    blend_dsc.mask_area = &map_area;
    blend_dsc.src_buf = color_buf;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.opa = opa;
    blend_dsc.blend_mode = dsc->blend_mode;

    for(row = row_start ; row < row_end; row++) {
        uint32_t subpx_cnt = 0;
        bitmask = bitmask_init >> col_bit;
        int32_t mask_p_start = mask_p;

        for(col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (8 - col_bit - bpp);
            if(letter_px != 0) {
                if(opa >= LV_OPA_MAX) {
                    px_opa = bpp == 8 ? letter_px : bpp_opa_table[letter_px];
                }
                else {
                    px_opa = bpp == 8 ? (uint32_t)((uint32_t)letter_px * opa) >> 8
                             : (uint32_t)((uint32_t)bpp_opa_table[letter_px] * opa) >> 8;
                }
            }
            else {
                px_opa = 0;
            }

            font_rgb[subpx_cnt] = px_opa;

            subpx_cnt ++;
            if(subpx_cnt == 3) {
                subpx_cnt = 0;

                lv_color_t res_color;
#if LV_COLOR_16_SWAP == 0
                uint8_t bg_rgb[3] = {dest_buf_tmp->ch.red, dest_buf_tmp->ch.green, dest_buf_tmp->ch.blue};
#else
                uint8_t bg_rgb[3] = {dest_buf_tmp->ch.red,
                                     (dest_buf_tmp->ch.green_h << 3) + dest_buf_tmp->ch.green_l,
                                     dest_buf_tmp->ch.blue
                                    };
#endif

#if LV_FONT_SUBPX_BGR
                res_color.ch.blue = (uint32_t)((uint32_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.red = (uint32_t)((uint32_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#else
                res_color.ch.red = (uint32_t)((uint16_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.blue = (uint32_t)((uint16_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#endif

#if LV_COLOR_16_SWAP == 0
                res_color.ch.green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
#else
                uint8_t green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;
                res_color.ch.green_h = green >> 3;
                res_color.ch.green_l = green & 0x7;
#endif

#if LV_COLOR_DEPTH == 32
                res_color.ch.alpha = 0xff;
#endif

                if(font_rgb[0] == 0 && font_rgb[1] == 0 && font_rgb[2] == 0) mask_buf[mask_p] = LV_OPA_TRANSP;
                else mask_buf[mask_p] = LV_OPA_COVER;
                color_buf[mask_p] = res_color;

                /*Next mask byte*/
                mask_p++;
                dest_buf_tmp++;
            }

            /*Go to the next column*/
            if(col_bit < (int32_t)(8 - bpp)) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            }
            else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }
        }

        /*Apply masks if any*/
        if(mask_any) {
            blend_dsc.mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, map_area.x1, map_area.y2,
                                                    lv_area_get_width(&map_area));
            if(blend_dsc.mask_res == LV_DRAW_MASK_RES_TRANSP) {
                lv_memset_00(mask_buf + mask_p_start, lv_area_get_width(&map_area));
            }
        }

        if((int32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            map_area.y2 ++;
        }
        else {
            lv_draw_sw_blend(draw_ctx, &blend_dsc);

            map_area.y1 = map_area.y2 + 1;
            map_area.y2 = map_area.y1;
            mask_p = 0;
        }

        col_bit += ((box_w - col_end) + col_start) * bpp;

        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;

        /*Next row in draw_buf*/
        dest_buf_tmp += dest_buf_stride - (col_end - col_start) / 3;
    }

    /*Flush the last part*/
    if(map_area.y1 != map_area.y2) {
        map_area.y2--;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }

    lv_mem_buf_release(mask_buf);
    lv_mem_buf_release(color_buf);
}
#endif /*LV_DRAW_COMPLEX && LV_USE_FONT_SUBPX*/

