/**
 * @file lv_draw_sw_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../core/lv_disp.h"
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

LV_ATTRIBUTE_FAST_MEM static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_letter_dsc_t * draw_dsc);


#if LV_DRAW_SW_FONT_SUBPX
static void draw_letter_subpx(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc, const lv_point_t * pos,
                              lv_font_glyph_dsc_t * g, const uint8_t * map_p);
#endif /*LV_DRAW_SW_FONT_SUBPX*/

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

void lv_draw_sw_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_label_interate_letters(draw_unit, dsc, coords, draw_letter);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_letter(lv_draw_unit_t * draw_unit, lv_draw_letter_dsc_t * draw_dsc)
{
    if(draw_dsc->bitmap == NULL) {
#if LV_USE_FONT_PLACEHOLDER
        /* Draw a placeholder rectangle*/
        lv_draw_rect_dsc_t rect_draw_dsc;
        lv_draw_rect_dsc_init(&rect_draw_dsc);
        rect_draw_dsc.bg_opa = LV_OPA_MIN;
        rect_draw_dsc.outline_opa = 0;
        rect_draw_dsc.shadow_opa = 0;
        rect_draw_dsc.bg_img_opa = 0;
        rect_draw_dsc.border_color = draw_dsc->letter_color;
        rect_draw_dsc.border_width = 1;
        lv_draw_sw_rect(draw_unit, &rect_draw_dsc, draw_dsc->bg_coords);
#endif
        return;
    }

    if(draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_A8) {
        lv_draw_sw_blend_dsc_t blend_dsc;
        lv_memzero(&blend_dsc, sizeof(blend_dsc));
        blend_dsc.color = draw_dsc->letter_color;
        blend_dsc.opa = LV_OPA_COVER;
        blend_dsc.blend_mode = draw_dsc->blend_mode;
        blend_dsc.mask_buf = draw_dsc->bitmap;
        blend_dsc.mask_area = draw_dsc->letter_coords;
        blend_dsc.blend_area = draw_dsc->letter_coords;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
#if LV_USE_FONT_SUBPX && 0
    if(draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_SUBPX) {

    }
#endif

#if LV_USE_IMGFONT
    if(draw_dsc->format == LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE) {
        lv_area_t fill_area;
        fill_area.x1 = pos->x;
        fill_area.y1 = pos->y;
        fill_area.x2 = pos->x + g->box_w - 1;
        fill_area.y2 = pos->y + g->box_h - 1;
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.angle = 0;
        img_dsc.zoom = LV_ZOOM_NONE;
        img_dsc.opa = dsc->opa;
        img_dsc.blend_mode = dsc->blend_mode;
        lv_draw_img(draw_ctx, &img_dsc, &fill_area, map_p);
        return;
    }
#endif
}

#if LV_USE_FONT_SUBPX && 0
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
    lv_opa_t * mask_buf = lv_malloc(mask_buf_size);
    int32_t mask_p = 0;

    lv_color_t * color_buf = lv_malloc(mask_buf_size * sizeof(lv_color_t));

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
    uint8_t txt_rgb[3] = {color.ch.red, color.ch.green, color.ch.blue};

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
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
                uint8_t bg_rgb[3] = {dest_buf_tmp->ch.red, dest_buf_tmp->ch.green, dest_buf_tmp->ch.blue};

#if LV_DRAW_SW_FONT_SUBPX_BGR
                res_color.ch.blue = (uint32_t)((uint32_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.red = (uint32_t)((uint32_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#else
                res_color.ch.red = (uint32_t)((uint16_t)txt_rgb[0] * font_rgb[0] + (bg_rgb[0] * (255 - font_rgb[0]))) >> 8;
                res_color.ch.blue = (uint32_t)((uint16_t)txt_rgb[2] * font_rgb[2] + (bg_rgb[2] * (255 - font_rgb[2]))) >> 8;
#endif

                res_color.ch.green = (uint32_t)((uint32_t)txt_rgb[1] * font_rgb[1] + (bg_rgb[1] * (255 - font_rgb[1]))) >> 8;

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

#if LV_USE_DRAW_MASKS
        /*Apply masks if any*/
        if(mask_any) {
            blend_dsc.mask_res = lv_draw_mask_apply(mask_buf + mask_p_start, map_area.x1, map_area.y2,
                                                    lv_area_get_width(&map_area));
            if(blend_dsc.mask_res == LV_DRAW_MASK_RES_TRANSP) {
                lv_memzero(mask_buf + mask_p_start, lv_area_get_width(&map_area));
            }
        }
#endif
        if((int32_t) mask_p + (col_end - col_start) < mask_buf_size) {
            map_area.y2 ++;
        }
        else {
            blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
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
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }

    lv_free(mask_buf);
    lv_free(color_buf);
}
#endif /*LV_DRAW_SW_FONT_SUBPX*/

#endif /*LV_USE_DRAW_SW*/
