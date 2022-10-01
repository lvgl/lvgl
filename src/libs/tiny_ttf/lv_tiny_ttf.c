#include "../../../lvgl.h"
#include "../../misc/lv_gc.h"
#if LV_USE_TINY_TTF
#include <stdio.h>
#include "lv_tiny_ttf.h"
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_HEAP_FACTOR_SIZE_32 50
#define STBTT_HEAP_FACTOR_SIZE_128 20
#define STBTT_HEAP_FACTOR_SIZE_DEFAULT 10
#define STBTT_malloc(x,u)  ((void)(u),lv_malloc(x))
#define STBTT_free(x,u)    ((void)(u),lv_free(x))


#if LV_TINY_TTF_FILE_SUPPORT !=0
// a hydra stream that can be in memory or from a file
typedef struct ttf_cb_stream {
    lv_fs_file_t * file;
    const void * data;
    size_t size;
    size_t position;
} ttf_cb_stream_t;

static void ttf_cb_stream_read(ttf_cb_stream_t * stream, void * data, size_t to_read)
{
    if(stream->file != NULL) {
        uint32_t br;
        lv_fs_read(stream->file, data, to_read, &br);
    }
    else {
        if(to_read + stream->position >= stream->size) {
            to_read = stream->size - stream->position;
        }
        memcpy(data, ((const unsigned char *)stream->data + stream->position), to_read);
        stream->position += to_read;
    }
}
static void ttf_cb_stream_seek(ttf_cb_stream_t * stream, size_t position)
{
    if(stream->file != NULL) {
        lv_fs_seek(stream->file, position, LV_FS_SEEK_SET);
    }
    else {
        if(position > stream->size) {
            stream->position = stream->size;
        }
        else {
            stream->position = position;
        }
    }
}

#define STBTT_STREAM_TYPE ttf_cb_stream_t*
#define STBTT_STREAM_SEEK(s,x) ttf_cb_stream_seek(s,x);
#define STBTT_STREAM_READ(s,x,y) ttf_cb_stream_read(s,x,y);
#endif

#include "stb_rect_pack.h"
#include "stb_truetype_htcw.h"

typedef struct ttf_font_desc {
    lv_fs_file_t file;
#if LV_TINY_TTF_FILE_SUPPORT !=0
    ttf_cb_stream_t stream;
#else
    const unsigned char * stream;
#endif
    stbtt_fontinfo info;
    float scale;
    int ascent;
    int descent;
} ttf_font_desc_t;

static bool ttf_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                 uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20 ||
       unicode_letter == 0xf8ff || /*LV_SYMBOL_DUMMY*/
       unicode_letter == 0x200c) { /*ZERO WIDTH NON-JOINER*/
        dsc_out->box_w = 0;
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;                                /*height of the bitmap in [px]*/
        dsc_out->ofs_x = 0;                                           /*X offset of the bitmap in [pf]*/
        dsc_out->ofs_y = 0;                                           /*Y offset of the bitmap in [pf]*/
        dsc_out->bpp = 0;
        return true;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    int g1 = stbtt_FindGlyphIndex(&dsc->info, (int)unicode_letter);
    int x1, y1, x2, y2;

    stbtt_GetGlyphBitmapBox(&dsc->info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
    int g2 = 0;
    if(unicode_letter_next != 0) {
        g2 = stbtt_FindGlyphIndex(&dsc->info, (int)unicode_letter_next);
    }
    int advw, lsb;
    stbtt_GetGlyphHMetrics(&dsc->info, g1, &advw, &lsb);
    int k = stbtt_GetGlyphKernAdvance(&dsc->info, g1, g2);
    dsc_out->adv_w = (uint16_t)floor((((float)advw + (float)k) * dsc->scale) +
                                     0.5f); /*Horizontal space required by the glyph in [px]*/

    dsc_out->adv_w = (uint16_t)floor((((float)advw + (float)k) * dsc->scale) +
                                     0.5f); /*Horizontal space required by the glyph in [px]*/
    dsc_out->box_w = (x2 - x1 + 1);                                /*width of the bitmap in [px]*/
    dsc_out->box_h = (y2 - y1 + 1);                                /*height of the bitmap in [px]*/
    dsc_out->ofs_x = x1;                                           /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = -y2; /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;     /*Bits per pixel: 1/2/4/8*/
    dsc_out->is_placeholder = false;
    return true;          /*true: glyph found; false: glyph was not found*/
}

static const uint8_t * ttf_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    const stbtt_fontinfo * info = (const stbtt_fontinfo *)&dsc->info;
    static size_t buffer_size = 0;
    int g1 = stbtt_FindGlyphIndex(info, (int)unicode_letter);
    int x1, y1, x2, y2;
    stbtt_GetGlyphBitmapBox(info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
    int w, h;
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;
    if(LV_GC_ROOT(_ttf_glyph_bitmap_buffer) == NULL) {
        buffer_size = (size_t)(w * h);
        LV_GC_ROOT(_ttf_glyph_bitmap_buffer) = (uint8_t *)lv_malloc(buffer_size);
        if(LV_GC_ROOT(_ttf_glyph_bitmap_buffer) == NULL) {
            buffer_size = 0;
            return NULL;
        }
        memset(LV_GC_ROOT(_ttf_glyph_bitmap_buffer), 0, buffer_size);
    }
    else {
        size_t s = w * h;
        if(s > buffer_size) {
            buffer_size = s;
            LV_GC_ROOT(_ttf_glyph_bitmap_buffer) = (uint8_t *)lv_realloc(LV_GC_ROOT(_ttf_glyph_bitmap_buffer), buffer_size);
            if(LV_GC_ROOT(_ttf_glyph_bitmap_buffer) == NULL) {
                buffer_size = 0;
                return NULL;
            }
            memset(LV_GC_ROOT(_ttf_glyph_bitmap_buffer), 0, buffer_size);
        }
    }
    stbtt_MakeGlyphBitmap(info, LV_GC_ROOT(_ttf_glyph_bitmap_buffer), w, h, w, dsc->scale, dsc->scale, g1);
    return LV_GC_ROOT(_ttf_glyph_bitmap_buffer); /*Or NULL if not found*/
}

static lv_font_t * lv_tiny_ttf_create(const char * path, const void * data, size_t data_size,  lv_coord_t line_height)
{

    if((path == NULL && data == NULL) || 0 >= line_height) {
        LV_LOG_ERROR("tiny_ttf: invalid argument\n");
        return NULL;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)lv_malloc(sizeof(ttf_font_desc_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
#if LV_TINY_TTF_FILE_SUPPORT !=0
    if(path != NULL) {
        if(LV_FS_RES_OK != lv_fs_open(&dsc->file, path, LV_FS_MODE_RD)) {
            LV_LOG_ERROR("tiny_ttf: unable to open %s\n", path);
            return NULL;
        }
        dsc->stream.file = &dsc->file;
    }
    else {
        dsc->stream.file = NULL;
        dsc->stream.data = (const uint8_t *)data;
        dsc->stream.size = data_size;
        dsc->stream.position = 0;
    }
    if(0 == stbtt_InitFont(&dsc->info, &dsc->stream, stbtt_GetFontOffsetForIndex(&dsc->stream, 0))) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }

#else
    LV_UNUSED(data_size);
    dsc->stream = (const uint8_t *)data;
    if(0 == stbtt_InitFont(&dsc->info, dsc->stream, stbtt_GetFontOffsetForIndex(dsc->stream, 0))) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }
#endif

    float scale = stbtt_ScaleForPixelHeight(&dsc->info, line_height);
    lv_font_t * out_font = (lv_font_t *)lv_malloc(sizeof(lv_font_t));
    if(out_font == NULL) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
    out_font->line_height = line_height;
    out_font->fallback = NULL;
    out_font->dsc = dsc;
    int line_gap;
    stbtt_GetFontVMetrics(&dsc->info, &dsc->ascent, &dsc->descent, &line_gap);
    dsc->scale = scale;
    out_font->base_line = line_height - (lv_coord_t)(dsc->ascent * scale);
    out_font->underline_position = (uint8_t)line_height - dsc->descent;
    out_font->underline_thickness = 0;
    out_font->subpx = 0;
    out_font->get_glyph_dsc = ttf_get_glyph_dsc_cb;
    out_font->get_glyph_bitmap = ttf_get_glyph_bitmap_cb;
    return out_font;
}
#if LV_TINY_TTF_FILE_SUPPORT !=0
lv_font_t * lv_tiny_ttf_create_file(const char * path, lv_coord_t line_height)
{
    return lv_tiny_ttf_create(path, NULL, 0, line_height);
}
#endif
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, lv_coord_t line_height)
{
    return lv_tiny_ttf_create(NULL, data, data_size, line_height);
}
void lv_tiny_ttf_set_size(lv_font_t * font, lv_coord_t line_height)
{
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    if(line_height > 0) {
        font->line_height = line_height;
        dsc->scale = stbtt_ScaleForPixelHeight(&dsc->info, line_height);
        font->base_line = line_height - (lv_coord_t)(dsc->ascent * dsc->scale);
        font->underline_position = (uint8_t)line_height - dsc->descent;
    }
}
void lv_tiny_ttf_destroy(lv_font_t * font)
{
    if(font != NULL) {
        if(font->dsc != NULL) {
            ttf_font_desc_t * ttf = (ttf_font_desc_t *)font->dsc;
#if LV_TINY_TTF_FILE_SUPPORT !=0
            if(ttf->stream.file != NULL) {
                lv_fs_close(&ttf->file);
            }
#endif
            lv_free(ttf);
        }
        lv_free(font);
    }
}
#endif
