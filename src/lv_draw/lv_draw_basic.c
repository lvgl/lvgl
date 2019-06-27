/**
 * @file lv_draw_basic.c
 *
 */

#include "lv_draw_basic.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../lv_core/lv_refr.h"
#include "../lv_hal/lv_hal.h"
#include "../lv_font/lv_font.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_log.h"

#include <stddef.h>
#include "lv_draw.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/*Always fill < 50 px with 'sw_color_fill' because of the hw. init overhead*/
#define VFILL_HW_ACC_SIZE_LIMIT 50

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sw_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void sw_color_fill(lv_color_t * mem, lv_coord_t mem_width, const lv_area_t * fill_area, lv_color_t color,
                          lv_opa_t opa);

static inline lv_color_t color_mix_2_alpha(lv_color_t bg_color, lv_opa_t bg_opa, lv_color_t fg_color, lv_opa_t fg_opa);

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
 * Put a pixel in the Virtual Display Buffer
 * @param x pixel x coordinate
 * @param y pixel y coordinate
 * @param mask_p fill only on this mask (truncated to VDB area)
 * @param color pixel color
 * @param opa opacity of the area (0..255)
 */
void lv_draw_px(lv_coord_t x, lv_coord_t y, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa)
{

    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Pixel out of the mask*/
    if(x < mask_p->x1 || x > mask_p->x2 || y < mask_p->y1 || y > mask_p->y2) {
        return;
    }

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    uint32_t vdb_width  = lv_area_get_width(&vdb->area);

    /*Make the coordinates relative to VDB*/
    x -= vdb->area.x1;
    y -= vdb->area.y1;

    if(disp->driver.set_px_cb) {
        disp->driver.set_px_cb(&disp->driver, (uint8_t *)vdb->buf_act, vdb_width, x, y, color, opa);
    } else {
        bool scr_transp = false;
#if LV_COLOR_SCREEN_TRANSP
        scr_transp = disp->driver.screen_transp;
#endif

        lv_color_t * vdb_px_p = vdb->buf_act;
        vdb_px_p += y * vdb_width + x;

        if(scr_transp == false) {
            if(opa == LV_OPA_COVER) {
                *vdb_px_p = color;
            } else {
                *vdb_px_p = lv_color_mix(color, *vdb_px_p, opa);
            }
        } else {
#if LV_COLOR_DEPTH == 32
            *vdb_px_p = color_mix_2_alpha(*vdb_px_p, (*vdb_px_p).ch.alpha, color, opa);
#endif
        }
    }
}

/**
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask  (truncated to VDB area)
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void lv_draw_fill(const lv_area_t * cords_p, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa)
{
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_area_t res_a;
    bool union_ok;

    /*Get the union of cord and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = lv_area_intersect(&res_a, cords_p, mask_p);

    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false) {
        return;
    }

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    lv_area_t vdb_rel_a; /*Stores relative coordinates on vdb*/
    vdb_rel_a.x1 = res_a.x1 - vdb->area.x1;
    vdb_rel_a.y1 = res_a.y1 - vdb->area.y1;
    vdb_rel_a.x2 = res_a.x2 - vdb->area.x1;
    vdb_rel_a.y2 = res_a.y2 - vdb->area.y1;

    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    /*Move the vdb_tmp to the first row*/
    vdb_buf_tmp += vdb_width * vdb_rel_a.y1;

#if LV_USE_GPU
    static LV_ATTRIBUTE_MEM_ALIGN lv_color_t color_array_tmp[LV_HOR_RES_MAX]; /*Used by 'lv_disp_mem_blend'*/
    static lv_coord_t last_width = -1;

    lv_coord_t w = lv_area_get_width(&vdb_rel_a);
    /*Don't use hw. acc. for every small fill (because of the init overhead)*/
    if(w < VFILL_HW_ACC_SIZE_LIMIT) {
        sw_color_fill(vdb->buf_act, vdb_width, &vdb_rel_a, color, opa);
    }
    /*Not opaque fill*/
    else if(opa == LV_OPA_COVER) {
        /*Use hw fill if present*/
        if(disp->driver.gpu_fill_cb) {
            disp->driver.gpu_fill_cb(&disp->driver, vdb->buf_act, vdb_width, &vdb_rel_a, color);
        }
        /*Use hw blend if present and the area is not too small*/
        else if(lv_area_get_height(&vdb_rel_a) > VFILL_HW_ACC_SIZE_LIMIT && disp->driver.gpu_blend_cb) {
            /*Fill a  one line sized buffer with a color and blend this later*/
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }
                last_width = w;
            }

            /*Blend the filled line to every line VDB line-by-line*/
            lv_coord_t row;
            for(row = vdb_rel_a.y1; row <= vdb_rel_a.y2; row++) {
                disp->driver.gpu_blend_cb(&disp->driver, &vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Else use sw fill if no better option*/
        else {
            sw_color_fill(vdb->buf_act, vdb_width, &vdb_rel_a, color, opa);
        }

    }
    /*Fill with opacity*/
    else {
        /*Use hw blend if present*/
        if(disp->driver.gpu_blend_cb) {
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }

                last_width = w;
            }
            lv_coord_t row;
            for(row = vdb_rel_a.y1; row <= vdb_rel_a.y2; row++) {
                disp->driver.gpu_blend_cb(&disp->driver, &vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Use sw fill with opa if no better option*/
        else {
            sw_color_fill(vdb->buf_act, vdb_width, &vdb_rel_a, color, opa);
        }
    }
#else
    sw_color_fill(vdb->buf_act, vdb_width, &vdb_rel_a, color, opa);
#endif
}

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area  (truncated to VDB area)
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
void lv_draw_letter(const lv_point_t * pos_p, const lv_area_t * mask_p, const lv_font_t * font_p, uint32_t letter,
                    lv_color_t color, lv_opa_t opa)
{
    /*clang-format off*/
    const uint8_t bpp1_opa_table[2]  = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const uint8_t bpp2_opa_table[4]  = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/
    const uint8_t bpp4_opa_table[16] = {0,  17, 34,  51,  /*Opacity mapping with bpp = 4*/
                                        68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    /*clang-format on*/

    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("Font: character's bitmap not found");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false) return;

    lv_coord_t pos_x = pos_p->x + g.ofs_x;
    lv_coord_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    const uint8_t * bpp_opa_table;
    uint8_t bitmask_init;
    uint8_t bitmask;

    switch(g.bpp) {
        case 1:
            bpp_opa_table = bpp1_opa_table;
            bitmask_init  = 0x80;
            break;
        case 2:
            bpp_opa_table = bpp2_opa_table;
            bitmask_init  = 0xC0;
            break;
        case 4:
            bpp_opa_table = bpp4_opa_table;
            bitmask_init  = 0xF0;
            break;
        case 8:
            bpp_opa_table = NULL;
            bitmask_init  = 0xFF;
            break;       /*No opa table, pixel value will be used directly*/
        default: return; /*Invalid bpp. Can't render the letter*/
    }

    const uint8_t * map_p = lv_font_get_glyph_bitmap(font_p, letter);

    if(map_p == NULL) return;

    /*If the letter is completely out of mask don't draw it */
    if(pos_x + g.box_w < mask_p->x1 || pos_x > mask_p->x2 || pos_y + g.box_h < mask_p->y1 || pos_y > mask_p->y2) return;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    lv_coord_t vdb_width     = lv_area_get_width(&vdb->area);
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    lv_coord_t col, row;

    uint8_t width_byte_scr = g.box_w >> 3; /*Width in bytes (on the screen finally) (e.g. w = 11 -> 2 bytes wide)*/
    if(g.box_w & 0x7) width_byte_scr++;
    uint16_t width_bit = g.box_w * g.bpp; /*Letter width in bits*/

    /* Calculate the col/row start/end on the map*/
    lv_coord_t col_start = pos_x >= mask_p->x1 ? 0 : mask_p->x1 - pos_x;
    lv_coord_t col_end   = pos_x + g.box_w <= mask_p->x2 ? g.box_w : mask_p->x2 - pos_x + 1;
    lv_coord_t row_start = pos_y >= mask_p->y1 ? 0 : mask_p->y1 - pos_y;
    lv_coord_t row_end   = pos_y + g.box_h <= mask_p->y2 ? g.box_h : mask_p->y2 - pos_y + 1;

    /*Set a pointer on VDB to the first pixel of the letter*/
    vdb_buf_tmp += ((pos_y - vdb->area.y1) * vdb_width) + pos_x - vdb->area.x1;

    /*If the letter is partially out of mask the move there on VDB*/
    vdb_buf_tmp += (row_start * vdb_width) + col_start;

    /*Move on the map too*/
    uint32_t bit_ofs = (row_start * width_bit) + (col_start * g.bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    lv_opa_t px_opa;
    uint16_t col_bit;
    col_bit = bit_ofs & 0x7; /* "& 0x7" equals to "% 8" just faster */

    bool scr_transp = false;
#if LV_COLOR_SCREEN_TRANSP
    scr_transp = disp->driver.screen_transp;
#endif

    for(row = row_start; row < row_end; row++) {
        bitmask = bitmask_init >> col_bit;
        for(col = col_start; col < col_end; col++) {
            letter_px = (*map_p & bitmask) >> (8 - col_bit - g.bpp);
            if(letter_px != 0) {
                if(opa == LV_OPA_COVER) {
                    px_opa = g.bpp == 8 ? letter_px : bpp_opa_table[letter_px];
                } else {
                    px_opa = g.bpp == 8 ? (uint16_t)((uint16_t)letter_px * opa) >> 8
                                        : (uint16_t)((uint16_t)bpp_opa_table[letter_px] * opa) >> 8;
                }

                if(disp->driver.set_px_cb) {
                    disp->driver.set_px_cb(&disp->driver, (uint8_t *)vdb->buf_act, vdb_width,
                                           (col + pos_x) - vdb->area.x1, (row + pos_y) - vdb->area.y1, color, px_opa);
                } else if(vdb_buf_tmp->full != color.full) {
                    if(px_opa > LV_OPA_MAX)
                        *vdb_buf_tmp = color;
                    else if(px_opa > LV_OPA_MIN) {
                        if(scr_transp == false) {
                            *vdb_buf_tmp = lv_color_mix(color, *vdb_buf_tmp, px_opa);
                        } else {
#if LV_COLOR_DEPTH == 32
                            *vdb_buf_tmp = color_mix_2_alpha(*vdb_buf_tmp, (*vdb_buf_tmp).ch.alpha, color, px_opa);
#endif
                        }
                    }
                }
            }

            vdb_buf_tmp++;

            if(col_bit < 8 - g.bpp) {
                col_bit += g.bpp;
                bitmask = bitmask >> g.bpp;
            } else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }
        }
        col_bit += ((g.box_w - col_end) + col_start) * g.bpp;

        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;
        vdb_buf_tmp += vdb_width - (col_end - col_start); /*Next row in VDB*/
    }
}

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to VDB area)
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map
 * @param chroma_keyed true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel
 * @param recolor mix the pixels with this color
 * @param recolor_opa the intense of recoloring
 */
void lv_draw_map(const lv_area_t * cords_p, const lv_area_t * mask_p, const uint8_t * map_p, lv_opa_t opa,
                 bool chroma_key, bool alpha_byte, lv_color_t recolor, lv_opa_t recolor_opa)
{

    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_area_t masked_a;
    bool union_ok;

    /*Get the union of map size and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = lv_area_intersect(&masked_a, cords_p, mask_p);

    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false) return;

    /*The pixel size in byte is different if an alpha byte is added too*/
    uint8_t px_size_byte = alpha_byte ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);

    /*If the map starts OUT of the masked area then calc. the first pixel*/
    lv_coord_t map_width = lv_area_get_width(cords_p);
    if(cords_p->y1 < masked_a.y1) {
        map_p += (uint32_t)map_width * ((masked_a.y1 - cords_p->y1)) * px_size_byte;
    }
    if(cords_p->x1 < masked_a.x1) {
        map_p += (masked_a.x1 - cords_p->x1) * px_size_byte;
    }

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Stores coordinates relative to the current VDB*/
    masked_a.x1 = masked_a.x1 - vdb->area.x1;
    masked_a.y1 = masked_a.y1 - vdb->area.y1;
    masked_a.x2 = masked_a.x2 - vdb->area.x1;
    masked_a.y2 = masked_a.y2 - vdb->area.y1;

    lv_coord_t vdb_width     = lv_area_get_width(&vdb->area);
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += (uint32_t)vdb_width * masked_a.y1; /*Move to the first row*/
    vdb_buf_tmp += (uint32_t)masked_a.x1;             /*Move to the first col*/

    lv_coord_t row;
    lv_coord_t map_useful_w = lv_area_get_width(&masked_a);

    bool scr_transp = false;
#if LV_COLOR_SCREEN_TRANSP
    scr_transp = disp->driver.screen_transp;
#endif

    /*The simplest case just copy the pixels into the VDB*/
    if(chroma_key == false && alpha_byte == false && opa == LV_OPA_COVER && recolor_opa == LV_OPA_TRANSP) {

        /*Use the custom VDB write function is exists*/
        if(disp->driver.set_px_cb) {
            lv_coord_t col;
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
                for(col = 0; col < map_useful_w; col++) {
                    lv_color_t px_color = *((lv_color_t *)&map_p[(uint32_t)col * px_size_byte]);
                    disp->driver.set_px_cb(&disp->driver, (uint8_t *)vdb->buf_act, vdb_width, col + masked_a.x1, row,
                                           px_color, opa);
                }
                map_p += map_width * px_size_byte; /*Next row on the map*/
            }
        }
        /*Normal native VDB*/
        else {
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
#if LV_USE_GPU
                if(disp->driver.gpu_blend_cb == false) {
                    sw_mem_blend(vdb_buf_tmp, (lv_color_t *)map_p, map_useful_w, opa);
                } else {
                    disp->driver.gpu_blend_cb(&disp->driver, vdb_buf_tmp, (lv_color_t *)map_p, map_useful_w, opa);
                }
#else
                sw_mem_blend(vdb_buf_tmp, (lv_color_t *)map_p, map_useful_w, opa);
#endif
                map_p += map_width * px_size_byte; /*Next row on the map*/
                vdb_buf_tmp += vdb_width;          /*Next row on the VDB*/
            }
        }
    }

    /*In the other cases every pixel need to be checked one-by-one*/
    else {

        lv_coord_t col;
        lv_color_t last_img_px  = LV_COLOR_BLACK;
        lv_color_t recolored_px = lv_color_mix(recolor, last_img_px, recolor_opa);
        for(row = masked_a.y1; row <= masked_a.y2; row++) {
            for(col = 0; col < map_useful_w; col++) {
                lv_opa_t opa_result  = opa;
                uint8_t * px_color_p = (uint8_t *)&map_p[(uint32_t)col * px_size_byte];
                lv_color_t px_color;

                /*Calculate with the pixel level alpha*/
                if(alpha_byte) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
                    px_color.full = px_color_p[0];
#elif LV_COLOR_DEPTH == 16
                    /*Because of Alpha byte 16 bit color can start on odd address which can cause
                     * crash*/
                    px_color.full = px_color_p[0] + (px_color_p[1] << 8);
#elif LV_COLOR_DEPTH == 32
                    px_color = *((lv_color_t *)px_color_p);
#endif
                    lv_opa_t px_opa = *(px_color_p + LV_IMG_PX_SIZE_ALPHA_BYTE - 1);
                    if(px_opa == LV_OPA_TRANSP)
                        continue;
                    else if(px_opa != LV_OPA_COVER)
                        opa_result = (uint32_t)((uint32_t)px_opa * opa_result) >> 8;
                } else {
                    px_color = *((lv_color_t *)px_color_p);
                }

                /*Handle chroma key*/
                if(chroma_key && px_color.full == disp->driver.color_chroma_key.full) continue;

                /*Re-color the pixel if required*/
                if(recolor_opa != LV_OPA_TRANSP) {
                    if(last_img_px.full != px_color.full) { /*Minor acceleration: calculate only for
                                                               new colors (save the last)*/
                        last_img_px  = px_color;
                        recolored_px = lv_color_mix(recolor, last_img_px, recolor_opa);
                    }
                    /*Handle custom VDB write is present*/
                    if(disp->driver.set_px_cb) {
                        disp->driver.set_px_cb(&disp->driver, (uint8_t *)vdb->buf_act, vdb_width, col + masked_a.x1,
                                               row, recolored_px, opa_result);
                    }
                    /*Normal native VDB write*/
                    else {
                        if(opa_result == LV_OPA_COVER)
                            vdb_buf_tmp[col].full = recolored_px.full;
                        else
                            vdb_buf_tmp[col] = lv_color_mix(recolored_px, vdb_buf_tmp[col], opa_result);
                    }
                } else {
                    /*Handle custom VDB write is present*/
                    if(disp->driver.set_px_cb) {
                        disp->driver.set_px_cb(&disp->driver, (uint8_t *)vdb->buf_act, vdb_width, col + masked_a.x1,
                                               row, px_color, opa_result);
                    }
                    /*Normal native VDB write*/
                    else {

                        if(opa_result == LV_OPA_COVER)
                            vdb_buf_tmp[col] = px_color;
                        else {
                            if(scr_transp == false) {
                                vdb_buf_tmp[col] = lv_color_mix(px_color, vdb_buf_tmp[col], opa_result);
                            } else {
#if LV_COLOR_DEPTH == 32
                                vdb_buf_tmp[col] = color_mix_2_alpha(vdb_buf_tmp[col], vdb_buf_tmp[col].ch.alpha,
                                                                     px_color, opa_result);
#endif
                            }
                        }
                    }
                }
            }

            map_p += map_width * px_size_byte; /*Next row on the map*/
            vdb_buf_tmp += vdb_width;          /*Next row on the VDB*/
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Blend pixels to destination memory using opacity
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
static void sw_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    if(opa == LV_OPA_COVER) {
        memcpy(dest, src, length * sizeof(lv_color_t));
    } else {
        uint32_t col;
        for(col = 0; col < length; col++) {
            dest[col] = lv_color_mix(src[col], dest[col], opa);
        }
    }
}

/**
 * Fill an area with a color
 * @param mem a memory address. Considered to a rectangular window according to 'mem_area'
 * @param mem_width width of the 'mem' buffer
 * @param fill_area coordinates of an area to fill. Relative to 'mem_area'.
 * @param color fill color
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
static void sw_color_fill(lv_color_t * mem, lv_coord_t mem_width, const lv_area_t * fill_area, lv_color_t color,
                          lv_opa_t opa)
{
    /*Set all row in vdb to the given color*/
    lv_coord_t row;
    lv_coord_t col;

    lv_disp_t * disp = lv_refr_get_disp_refreshing();
    if(disp->driver.set_px_cb) {
        for(col = fill_area->x1; col <= fill_area->x2; col++) {
            for(row = fill_area->y1; row <= fill_area->y2; row++) {
                disp->driver.set_px_cb(&disp->driver, (uint8_t *)mem, mem_width, col, row, color, opa);
            }
        }
    } else {
        mem += fill_area->y1 * mem_width; /*Go to the first row*/

        /*Run simpler function without opacity*/
        if(opa == LV_OPA_COVER) {

            /*Fill the first row with 'color'*/
            for(col = fill_area->x1; col <= fill_area->x2; col++) {
                mem[col] = color;
            }

            /*Copy the first row to all other rows*/
            lv_color_t * mem_first = &mem[fill_area->x1];
            lv_coord_t copy_size   = (fill_area->x2 - fill_area->x1 + 1) * sizeof(lv_color_t);
            mem += mem_width;

            for(row = fill_area->y1 + 1; row <= fill_area->y2; row++) {
                memcpy(&mem[fill_area->x1], mem_first, copy_size);
                mem += mem_width;
            }
        }
        /*Calculate with alpha too*/
        else {
            bool scr_transp = false;
#if LV_COLOR_SCREEN_TRANSP
            scr_transp = disp->driver.screen_transp;
#endif

            lv_color_t bg_tmp  = LV_COLOR_BLACK;
            lv_color_t opa_tmp = lv_color_mix(color, bg_tmp, opa);
            for(row = fill_area->y1; row <= fill_area->y2; row++) {
                for(col = fill_area->x1; col <= fill_area->x2; col++) {
                    if(scr_transp == false) {
                        /*If the bg color changed recalculate the result color*/
                        if(mem[col].full != bg_tmp.full) {
                            bg_tmp  = mem[col];
                            opa_tmp = lv_color_mix(color, bg_tmp, opa);
                        }

                        mem[col] = opa_tmp;

                    } else {
#if LV_COLOR_DEPTH == 32
                        mem[col] = color_mix_2_alpha(mem[col], mem[col].ch.alpha, color, opa);
#endif
                    }
                }
                mem += mem_width;
            }
        }
    }
}

/**
 * Mix two colors. Both color can have alpha value. It requires ARGB888 colors.
 * @param bg_color background color
 * @param bg_opa alpha of the background color
 * @param fg_color foreground color
 * @param fg_opa alpha of the foreground color
 * @return the mixed color. the alpha channel (color.alpha) contains the result alpha
 */
static inline lv_color_t color_mix_2_alpha(lv_color_t bg_color, lv_opa_t bg_opa, lv_color_t fg_color, lv_opa_t fg_opa)
{

#if LV_COLOR_SCREEN_TRANSP
    /* Pick the foreground if it's fully opaque or the Background is fully transparent*/
    if(fg_opa > LV_OPA_MAX || bg_opa <= LV_OPA_MIN) {
        fg_color.ch.alpha = fg_opa;
        return fg_color;
    }
    /*Transparent foreground: use the Background*/
    else if(fg_opa <= LV_OPA_MIN) {
        return bg_color;
    }
    /*Opaque background: use simple mix*/
    else if(bg_opa >= LV_OPA_MAX) {
        return lv_color_mix(fg_color, bg_color, fg_opa);
    }
    /*Both colors have alpha. Expensive calculation need to be applied*/
    else {
        /*Save the parameters and the result. If they will be asked again don't compute again*/
        static lv_opa_t fg_opa_save     = 0;
        static lv_opa_t bg_opa_save     = 0;
        static lv_color_t fg_color_save = {{0}};
        static lv_color_t bg_color_save = {{0}};
        static lv_color_t c             = {{0}};

        if(fg_opa != fg_opa_save || bg_opa != bg_opa_save || fg_color.full != fg_color_save.full ||
           bg_color.full != bg_color_save.full) {
            fg_opa_save        = fg_opa;
            bg_opa_save        = bg_opa;
            fg_color_save.full = fg_color.full;
            bg_color_save.full = bg_color.full;
            /*Info:
             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
            lv_opa_t alpha_res = 255 - ((uint16_t)((uint16_t)(255 - fg_opa) * (255 - bg_opa)) >> 8);
            if(alpha_res == 0) {
                while(1)
                    ;
            }
            lv_opa_t ratio = (uint16_t)((uint16_t)fg_opa * 255) / alpha_res;
            c              = lv_color_mix(fg_color, bg_color, ratio);
            c.ch.alpha     = alpha_res;
        }
        return c;
    }
#else
    (void)bg_color; /*Unused*/
    (void)fg_color; /*Unused*/
    (void)bg_opa;   /*Unused*/
    (void)fg_opa;   /*Unused*/

    return LV_COLOR_BLACK;

#endif /*LV_COLOR_SCREEN_TRANSP*/
}
