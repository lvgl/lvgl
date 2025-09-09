/**
 * @file lv_draw_eve_letter.c
 *
 */

/*  Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE

#include "../lv_draw_private.h"
#include "../lv_draw_label_private.h"
#include "../lv_draw_rect.h"
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_eve_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                  lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);
static void font_bitmap_to_ramg(uint32_t addr, const uint8_t * src, uint32_t width,
                                uint32_t height);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define GET_NIBBLE_1(w) ((uint8_t) ((w) >> 4))
#define GET_NIBBLE_2(w) ((uint8_t) ((w) & 0xf))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);
    lv_eve_save_context();
    lv_eve_primitive(LV_EVE_PRIMITIVE_BITMAPS);
    lv_draw_label_iterate_characters(t, dsc, coords, lv_draw_eve_letter_cb);
    lv_eve_restore_context();
}

bool lv_draw_eve_label_font_check(const lv_font_t * font)
{
    if(font->get_glyph_bitmap != lv_font_get_bitmap_fmt_txt) {
        LV_LOG_WARN("lv_draw_eve can only render static fonts for now.");
        return false;
    }

    const lv_font_fmt_txt_dsc_t * font_dsc = font->dsc;

    /* Only 4 bpp is supported for now. Support for 1 and 8 bpp can be added. (EVE_L1, EVE_L8) */
    if(font_dsc->bpp != 4) {
        LV_LOG_WARN("lv_draw_eve can only render static fonts for now.");
        return false;
    }

    return true;
}

uint32_t lv_draw_eve_label_upload_glyph(bool burst_is_active, const lv_font_fmt_txt_dsc_t * font_dsc,
                                        uint32_t gid_index)
{
    const lv_font_fmt_txt_glyph_dsc_t * glyph_dsc = &font_dsc->glyph_dsc[gid_index];
    const uint8_t * glyph_bitmap = &font_dsc->glyph_bitmap[glyph_dsc->bitmap_index];

    uint16_t g_box_w = glyph_dsc->box_w;
    uint16_t g_box_h = glyph_dsc->box_h;

    uint16_t g_aligned_stride = (g_box_w + 1) / 2;

    uint32_t glyph_ramg_size = g_aligned_stride * g_box_h;

    uint32_t ramg_addr;
    uintptr_t glyph_ramg_key = (uintptr_t) glyph_bitmap;
    bool font_is_loaded = lv_draw_eve_ramg_get_addr(&ramg_addr, glyph_ramg_key, glyph_ramg_size, 1);

    /* If the font is not yet loaded in ramG, load it */
    if(!font_is_loaded && ramg_addr != LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
        if(burst_is_active) {
            EVE_end_cmd_burst();
        }

        font_bitmap_to_ramg(ramg_addr, glyph_bitmap, g_box_w, g_box_h);

        if(burst_is_active) {
            EVE_start_cmd_burst();
        }
    }

    return ramg_addr;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_eve_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                  lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{

    if(fill_draw_dsc && fill_area) {
        /* draw UNDERLINE and STRIKETHROUGH */
        lv_eve_draw_rect_simple(fill_area->x1, fill_area->y1, fill_area->x2, fill_area->y2, 0);
    }

    if(glyph_draw_dsc == NULL)
        return;  /* Important */

    const lv_font_t * font = glyph_draw_dsc->g->resolved_font;

    if(!lv_draw_eve_label_font_check(font)) {
        return;
    }

    const lv_font_fmt_txt_dsc_t * font_dsc = font->dsc;
    uint32_t gid_index = glyph_draw_dsc->g->gid.index;
    const lv_font_fmt_txt_glyph_dsc_t * glyph_dsc = &font_dsc->glyph_dsc[gid_index];

    uint16_t g_box_w = glyph_dsc->box_w;
    uint16_t g_box_h = glyph_dsc->box_h;

    uint16_t g_aligned_stride = (g_box_w + 1) / 2;

    uint8_t bpp_eve = EVE_L4;

    uint32_t ramg_addr = lv_draw_eve_label_upload_glyph(true, font_dsc, gid_index);
    if(ramg_addr == LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
        LV_LOG_WARN("Could not load glyph because space could not be allocated in RAM_G.");
        return;
    }

    lv_eve_color_opa(glyph_draw_dsc->opa);
    lv_eve_color(glyph_draw_dsc->color);

    EVE_cmd_dl_burst(BITMAP_SOURCE(ramg_addr));

    EVE_cmd_dl_burst(BITMAP_SIZE(EVE_NEAREST, EVE_BORDER, EVE_BORDER, g_box_w, g_box_h));
    EVE_cmd_dl_burst(BITMAP_LAYOUT(bpp_eve, g_aligned_stride, g_box_h));

    lv_eve_vertex_2f(glyph_draw_dsc->letter_coords->x1, glyph_draw_dsc->letter_coords->y1);
}

static void font_bitmap_to_ramg(uint32_t addr, const uint8_t * src, uint32_t width,
                                uint32_t height)
{
    uint32_t stride = (width + 1) / 2;

    if(width % 2 == 0) {
        uint32_t size = stride * height;
        EVE_memWrite_flash_buffer(addr, src, size);
        return;
    }

    uint8_t * row_buf = lv_malloc(stride);
    LV_ASSERT_MALLOC(row_buf);

    uint32_t src_i = 0;
    uint8_t nibble_1;
    uint8_t nibble_2;
    uint8_t key = 0;

    /* Iterate through each row of the bitmap*/
    for(uint32_t y = 0; y < height; y++) {
        /* Iterate through each byte of the row*/
        uint32_t row_i;
        for(row_i = 0; row_i < (width / 2); ++row_i) {
            /*Get the two nibbles from the current byte*/
            if(key == 0) {
                nibble_1 = GET_NIBBLE_1(src[src_i]);
                nibble_2 = GET_NIBBLE_2(src[src_i]);
            }
            else {
                nibble_1 = GET_NIBBLE_2(src[src_i - 1]);
                nibble_2 = GET_NIBBLE_1(src[src_i]);
            }

            /*Combine the nibbles and assign the result to the output byte*/
            row_buf[row_i] = (nibble_1 << 4) | nibble_2;

            src_i++;
        }

        /*process the last remaining nibble*/
        row_buf[row_i] =
            (key == 0) ?
            (GET_NIBBLE_1(src[src_i])) << 4 | 0x0 : (GET_NIBBLE_2(src[src_i - 1])) << 4 | 0x0;
        key = (key == 0) ? 1 : 0;
        src_i += (key == 1) ? 1 : 0;

        EVE_memWrite_sram_buffer(addr, row_buf, stride);
        addr += stride;
    }

    lv_free(row_buf);
}


#endif /*LV_USE_DRAW_EVE*/

