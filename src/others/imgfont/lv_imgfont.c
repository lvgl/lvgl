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

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_font_t font;
    lv_imgfont_get_path_cb_t path_cb;
    void * user_data;
} imgfont_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const void * imgfont_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, uint32_t unicode,
                                             lv_draw_buf_t * draw_buf);
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
lv_font_t * lv_imgfont_create(uint16_t height, lv_imgfont_get_path_cb_t path_cb, void * user_data)
{
    imgfont_dsc_t * dsc = lv_malloc_zeroed(sizeof(imgfont_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    dsc->path_cb = path_cb;
    dsc->user_data = user_data;

    lv_font_t * font = &dsc->font;
    font->dsc = dsc;
    font->get_glyph_dsc = imgfont_get_glyph_dsc;
    font->get_glyph_bitmap = imgfont_get_glyph_bitmap;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = height;
    font->base_line = 0;
    font->underline_position = 0;
    font->underline_thickness = 0;

    return font;
}

void lv_imgfont_destroy(lv_font_t * font)
{
    LV_ASSERT_NULL(font);

    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    lv_free(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const void * imgfont_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, uint32_t unicode,
                                             lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(draw_buf);
    const lv_font_t * font = g_dsc->resolved_font;

    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    int32_t offset_y = 0;
    const void * img_src = dsc->path_cb(font, unicode, 0, &offset_y, dsc->user_data);
    return img_src;
}

static bool imgfont_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                  uint32_t unicode, uint32_t unicode_next)
{
    LV_ASSERT_NULL(font);

    imgfont_dsc_t * dsc = (imgfont_dsc_t *)font->dsc;
    LV_ASSERT_NULL(dsc);
    if(dsc->path_cb == NULL) return false;

    int32_t offset_y = 0;

    const void * img_src = dsc->path_cb(font, unicode, unicode_next, &offset_y, dsc->user_data);
    if(img_src == NULL) return false;

    lv_image_header_t header;
    if(lv_image_decoder_get_info(img_src, &header) != LV_RESULT_OK) {
        return false;
    }

    dsc_out->is_placeholder = 0;
    dsc_out->adv_w  = header.w;
    dsc_out->box_w  = header.w;
    dsc_out->box_h  = header.h;
    dsc_out->ofs_x  = 0;
    dsc_out->ofs_y  = offset_y;
    dsc_out->format = LV_FONT_GLYPH_FORMAT_IMAGE;   /* is image identifier */

    return true;
}

#endif /*LV_USE_IMGFONT*/
