#include "../../../lvgl.h"
#ifndef LV_USE_TINY_TTF
#define LV_USE_TINY_TTF 1
#endif
#if LV_USE_TINY_TTF
#include <stdio.h>
#include "lv_tiny_ttf.h"
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STREAM_TYPE lv_fs_file_t*
#define STBTT_STREAM_READ(s,x,y) uint32_t stream_br;\
                                lv_fs_read((STBTT_STREAM_TYPE)(s),(void*)(x),(uint32_t)(y),&stream_br);\
                                STBTT_assert((y)==stream_br);

#define STBTT_STREAM_SEEK(s,x)  lv_fs_seek((STBTT_STREAM_TYPE)(s),(uint32_t)(x),LV_FS_SEEK_SET);

#include "stb_rect_pack.h"
#include "stb_truetype_htcw.h"

typedef struct ttf_font_desc {
    lv_fs_file_t file;
    stbtt_fontinfo info;
    float scale;
    int ascent;
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
    dsc_out->adv_w = (((float)advw + (float)k) * dsc->scale) + 0.5f; /*Horizontal space required by the glyph in [px]*/
    dsc_out->box_w = (x2 - x1 + 1);                                /*width of the bitmap in [px]*/
    dsc_out->box_h = (y2 - y1 + 1);                                /*height of the bitmap in [px]*/
    dsc_out->ofs_x = x1;                                           /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = -y2; /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;     /*Bits per pixel: 1/2/4/8*/
    return true;          /*true: glyph found; false: glyph was not found*/
}

static const uint8_t * ttf_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    const stbtt_fontinfo* info = (const stbtt_fontinfo*)&dsc->info;
    static char * buffer = NULL;
    static size_t buffer_size = 0;
    int g1 = stbtt_FindGlyphIndex(info, (int)unicode_letter);
    int x1, y1, x2, y2;
    stbtt_GetGlyphBitmapBox(info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
    int w,h,xoff,yoff;
    w=x2-x1+1;
    h=y2-y1+1;
    if(buffer == NULL) {
        buffer_size = (size_t)(w * h);
        buffer = (uint8_t *)lv_mem_alloc(buffer_size);
        if(buffer == NULL) {
            buffer_size = 0;
            return NULL;
        }
        memset(buffer, 0, buffer_size);
    }
    else {
        size_t s = w * h;
        if(s > buffer_size) {
            buffer_size = s;
            buffer = (uint8_t *)lv_mem_realloc(buffer, buffer_size);
            if(buffer == NULL) {
                buffer_size = 0;
                return NULL;
            }
            memset(buffer, 0, buffer_size);
        }
    }
    stbtt_MakeGlyphBitmap(info,buffer,w,h,w,dsc->scale,dsc->scale,g1);
    return buffer; /*Or NULL if not found*/
}
lv_font_t * lv_tiny_ttf_create(const char * path, lv_coord_t line_height, lv_font_t * fallback)
{
    if(path == NULL || 0 >= line_height) {
        LV_LOG_ERROR("tiny_ttf: invalid argument\n");
        return NULL;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)lv_mem_alloc(sizeof(ttf_font_desc_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
    if(LV_FS_RES_OK!=lv_fs_open(&dsc->file,path,LV_FS_MODE_RD)) {
        LV_LOG_ERROR("tiny_ttf: unable to open %s\n",path);
        return NULL;
    }
    if(0 == stbtt_InitFont(&dsc->info, &dsc->file, 0)) {
        lv_mem_free(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }
    float scale = stbtt_ScaleForPixelHeight(&dsc->info, line_height);
    lv_font_t * out_font = (lv_font_t *)lv_mem_alloc(sizeof(lv_font_t));
    if(out_font == NULL) {
        lv_mem_free(dsc);
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
    out_font->line_height = line_height;
    out_font->fallback = fallback;
    out_font->dsc = dsc;
    int line_gap;
    int descent;
    stbtt_GetFontVMetrics(&dsc->info, &dsc->ascent, &descent, &line_gap);
    dsc->scale = scale;
    out_font->base_line = line_height - (lv_coord_t)(dsc->ascent * scale);
    out_font->underline_position = (uint8_t)line_height - descent;
    out_font->underline_thickness = 0;
    out_font->subpx = 0;
    out_font->get_glyph_dsc = ttf_get_glyph_dsc_cb;
    out_font->get_glyph_bitmap = ttf_get_glyph_bitmap_cb;
    return out_font;
}
void lv_tiny_ttf_destroy(lv_font_t * font)
{
    if(font != NULL) {
        if(font->dsc != NULL) {
            ttf_font_desc_t * ttf = (ttf_font_desc_t *)font->dsc;
            lv_fs_close(&ttf->file);
            lv_mem_free(ttf);
        }
        lv_mem_free(font);
    }
}
#endif