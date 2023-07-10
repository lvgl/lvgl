/**
 * @file lv_imgfont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_imgfont.h"

#if LV_USE_IMGFONT

/*********************
 *      DEFINES
 *********************/
#define LV_IMGFONT_PATH_MAX_LEN 64

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_font_t * font;
    lv_get_imgfont_path_cb_t path_cb;
    char path[LV_IMGFONT_PATH_MAX_LEN];
} imgfont_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const uint8_t * imgfont_get_glyph_bitmap(const lv_font_t * font, uint32_t unicode);
static bool imgfont_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                  uint32_t unicode, uint32_t unicode_next);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_font_t * lv_imgfont_create(uint16_t height, lv_get_imgfont_path_cb_t path_cb)
{
    LV_ASSERT_MSG(LV_IMGFONT_PATH_MAX_LEN > sizeof(lv_img_dsc_t),
                  "LV_IMGFONT_PATH_MAX_LEN must be greater than sizeof(lv_img_dsc_t)");

    size_t size = sizeof(imgfont_dsc_t) + sizeof(lv_font_t);
    imgfont_dsc_t * dsc = (imgfont_dsc_t *)lv_mem_alloc(size);
    if(dsc == NULL) return NULL;
    lv_memset_00(dsc, size);

    dsc->font = (lv_font_t *)(((char *)dsc) + sizeof(imgfont_dsc_t));
    dsc->path_cb = path_cb;

    lv_font_t * font = dsc->font;
    font->dsc = dsc;
    font->get_glyph_dsc = imgfont_get_glyph_dsc;
    font->get_glyph_bitmap = imgfont_get_glyph_bitmap;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = height;
    font->base_line = 0;
    font->underline_position = 0;
    font->underline_thickness = 0;

    return dsc->font;
}

void lv_imgfont_destroy(lv_font_t * font)
{
    if(font == NULL) {
        return;
    }

    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    lv_mem_free(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const uint8_t * imgfont_get_glyph_bitmap(const lv_font_t * font, uint32_t unicode)
{
    LV_UNUSED(unicode);
    LV_ASSERT_NULL(font);
    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    return (uint8_t *)dsc->path;
}

static bool imgfont_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                  uint32_t unicode, uint32_t unicode_next)
{
    LV_ASSERT_NULL(font);

    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    LV_ASSERT_NULL(dsc);
    if(dsc->path_cb == NULL) return false;

    if(!dsc->path_cb(dsc->font, dsc->path, LV_IMGFONT_PATH_MAX_LEN, unicode, unicode_next)) {
        return false;
    }

    lv_img_header_t header;
    if(lv_img_decoder_get_info(dsc->path, &header) != LV_RES_OK) {
        return false;
    }

    dsc_out->is_placeholder = 0;
    dsc_out->adv_w = header.w;
    dsc_out->box_w = header.w;
    dsc_out->box_h = header.h;
    dsc_out->bpp = LV_IMGFONT_BPP;   /* is image identifier */
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;

    return true;
}

#endif /*LV_USE_IMGFONT*/
