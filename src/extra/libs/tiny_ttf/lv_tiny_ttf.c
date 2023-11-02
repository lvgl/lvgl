#include "lv_tiny_ttf.h"

#if LV_USE_TINY_TTF
#include <stdio.h>
#include "../../../misc/lv_lru.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_HEAP_FACTOR_SIZE_32 50
#define STBTT_HEAP_FACTOR_SIZE_128 20
#define STBTT_HEAP_FACTOR_SIZE_DEFAULT 10
#define STBTT_malloc(x, u) ((void)(u), lv_mem_alloc(x))
#define STBTT_free(x, u) ((void)(u), lv_mem_free(x))
#define TTF_MALLOC(x) (lv_mem_alloc(x))
#define TTF_FREE(x) (lv_mem_free(x))

#if LV_TINY_TTF_FILE_SUPPORT
/* a hydra stream that can be in memory or from a file*/
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
        lv_memcpy(data, ((const unsigned char *)stream->data + stream->position), to_read);
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

/* for stream support */
#define STBTT_STREAM_TYPE ttf_cb_stream_t *
#define STBTT_STREAM_SEEK(s, x) ttf_cb_stream_seek(s, x);
#define STBTT_STREAM_READ(s, x, y) ttf_cb_stream_read(s, x, y);
#endif /*LV_TINY_TTF_FILE_SUPPORT*/

#include "stb_rect_pack.h"
#include "stb_truetype_htcw.h"

typedef struct ttf_font_desc {
    lv_fs_file_t file;
#if LV_TINY_TTF_FILE_SUPPORT
    ttf_cb_stream_t stream;
#else
    const uint8_t * stream;
#endif
    stbtt_fontinfo info;
    float scale;
    int ascent;
    int descent;
    lv_lru_t * bitmap_cache;
} ttf_font_desc_t;

typedef struct ttf_bitmap_cache_key {
    uint32_t unicode_letter;
    lv_coord_t line_height;
} ttf_bitmap_cache_key_t;

static bool ttf_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                 uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20 ||
       unicode_letter == 0xf8ff || /*LV_SYMBOL_DUMMY*/
       unicode_letter == 0x200c) { /*ZERO WIDTH NON-JOINER*/
        dsc_out->box_w = 0;
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0; /*height of the bitmap in [px]*/
        dsc_out->ofs_x = 0; /*X offset of the bitmap in [pf]*/
        dsc_out->ofs_y = 0; /*Y offset of the bitmap in [pf]*/
        dsc_out->bpp = 0;
        dsc_out->is_placeholder = false;
        return true;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    int g1 = stbtt_FindGlyphIndex(&dsc->info, (int)unicode_letter);
    if(g1 == 0) {
        /* Glyph not found */
        return false;
    }
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
    dsc_out->box_w = (x2 - x1 + 1);         /*width of the bitmap in [px]*/
    dsc_out->box_h = (y2 - y1 + 1);         /*height of the bitmap in [px]*/
    dsc_out->ofs_x = x1;                    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = -y2;                   /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;                       /*Bits per pixel: 1/2/4/8*/
    dsc_out->is_placeholder = false;
    return true; /*true: glyph found; false: glyph was not found*/
}

static const uint8_t * ttf_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    const stbtt_fontinfo * info = (const stbtt_fontinfo *)&dsc->info;
    int g1 = stbtt_FindGlyphIndex(info, (int)unicode_letter);
    if(g1 == 0) {
        /* Glyph not found */
        return NULL;
    }
    int x1, y1, x2, y2;
    stbtt_GetGlyphBitmapBox(info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
    int w, h;
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;
    uint32_t stride = w;
    /*Try to load from cache*/
    ttf_bitmap_cache_key_t cache_key;
    lv_memset(&cache_key, 0, sizeof(cache_key)); /*Zero padding*/
    cache_key.unicode_letter = unicode_letter;
    cache_key.line_height = font->line_height;
    uint8_t * buffer = NULL;
    lv_lru_get(dsc->bitmap_cache, &cache_key, sizeof(cache_key), (void **)&buffer);
    if(buffer) {
        return buffer;
    }
    LV_LOG_TRACE("cache miss for letter: %u", unicode_letter);
    /*Prepare space in cache*/
    size_t szb = h * stride;
    buffer = lv_mem_alloc(szb);
    if(!buffer) {
        LV_LOG_ERROR("failed to allocate cache value");
        return NULL;
    }
    lv_memset(buffer, 0, szb);
    if(LV_LRU_OK != lv_lru_set(dsc->bitmap_cache, &cache_key, sizeof(cache_key), buffer, szb)) {
        LV_LOG_ERROR("failed to add cache value");
        lv_mem_free(buffer);
        return NULL;
    }
    /*Render into cache*/
    stbtt_MakeGlyphBitmap(info, buffer, w, h, stride, dsc->scale, dsc->scale, g1);
    return buffer;
}

static lv_font_t * lv_tiny_ttf_create(const char * path, const void * data, size_t data_size, lv_coord_t font_size,
                                      size_t cache_size)
{
    if((path == NULL && data == NULL) || 0 >= font_size) {
        LV_LOG_ERROR("tiny_ttf: invalid argument\n");
        return NULL;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)TTF_MALLOC(sizeof(ttf_font_desc_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
#if LV_TINY_TTF_FILE_SUPPORT
    if(path != NULL) {
        if(LV_FS_RES_OK != lv_fs_open(&dsc->file, path, LV_FS_MODE_RD)) {
            LV_LOG_ERROR("tiny_ttf: unable to open %s\n", path);
            goto err_after_dsc;
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
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        goto err_after_dsc;
    }

#else
    dsc->stream = (const uint8_t *)data;
    LV_UNUSED(data_size);
    if(0 == stbtt_InitFont(&dsc->info, dsc->stream, stbtt_GetFontOffsetForIndex(dsc->stream, 0))) {
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        goto err_after_dsc;
    }
#endif

    dsc->bitmap_cache = lv_lru_create(cache_size, font_size * font_size, lv_mem_free, lv_mem_free);
    if(dsc->bitmap_cache == NULL) {
        LV_LOG_ERROR("failed to create lru cache");
        goto err_after_dsc;
    }

    lv_font_t * out_font = (lv_font_t *)TTF_MALLOC(sizeof(lv_font_t));
    if(out_font == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        goto err_after_bitmap_cache;
    }
    lv_memset(out_font, 0, sizeof(lv_font_t));
    out_font->get_glyph_dsc = ttf_get_glyph_dsc_cb;
    out_font->get_glyph_bitmap = ttf_get_glyph_bitmap_cb;
    out_font->dsc = dsc;
    lv_tiny_ttf_set_size(out_font, font_size);
    return out_font;
err_after_bitmap_cache:
    lv_lru_del(dsc->bitmap_cache);
err_after_dsc:
    TTF_FREE(dsc);
    return NULL;
}
#if LV_TINY_TTF_FILE_SUPPORT
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, lv_coord_t font_size, size_t cache_size)
{
    return lv_tiny_ttf_create(path, NULL, 0, font_size, cache_size);
}
lv_font_t * lv_tiny_ttf_create_file(const char * path, lv_coord_t font_size)
{
    return lv_tiny_ttf_create_file_ex(path, font_size, 4096);
}
#endif /*LV_TINY_TTF_FILE_SUPPORT*/
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, lv_coord_t font_size, size_t cache_size)
{
    return lv_tiny_ttf_create(NULL, data, data_size, font_size, cache_size);
}
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, lv_coord_t font_size)
{
    return lv_tiny_ttf_create_data_ex(data, data_size, font_size, 4096);
}
void lv_tiny_ttf_set_size(lv_font_t * font, lv_coord_t font_size)
{
    if(font_size <= 0) {
        LV_LOG_ERROR("invalid font size: %"PRIx32, font_size);
        return;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    dsc->scale = stbtt_ScaleForMappingEmToPixels(&dsc->info, font_size);
    int line_gap = 0;
    stbtt_GetFontVMetrics(&dsc->info, &dsc->ascent, &dsc->descent, &line_gap);
    font->line_height = (lv_coord_t)(dsc->scale * (dsc->ascent - dsc->descent + line_gap));
    font->base_line = (lv_coord_t)(dsc->scale * (line_gap - dsc->descent));
}
void lv_tiny_ttf_destroy(lv_font_t * font)
{
    if(font != NULL) {
        if(font->dsc != NULL) {
            ttf_font_desc_t * ttf = (ttf_font_desc_t *)font->dsc;
#if LV_TINY_TTF_FILE_SUPPORT
            if(ttf->stream.file != NULL) {
                lv_fs_close(&ttf->file);
            }
#endif
            lv_lru_del(ttf->bitmap_cache);
            TTF_FREE(ttf);
        }
        TTF_FREE(font);
    }
}
#endif /*LV_USE_TINY_TTF*/
