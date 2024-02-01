/**
* @file lv_tiny_ttf.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_tiny_ttf.h"

#if LV_USE_TINY_TTF

/*********************
 *      DEFINES
 *********************/
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_HEAP_FACTOR_SIZE_32 50
#define STBTT_HEAP_FACTOR_SIZE_128 20
#define STBTT_HEAP_FACTOR_SIZE_DEFAULT 10
#define STBTT_malloc(x, u) ((void)(u), lv_malloc(x))
#define STBTT_free(x, u) ((void)(u), lv_free(x))

#if LV_TINY_TTF_FILE_SUPPORT != 0
/* for stream support */
#define STBTT_STREAM_TYPE ttf_cb_stream_t *
#define STBTT_STREAM_SEEK(s, x) ttf_cb_stream_seek(s, x);
#define STBTT_STREAM_READ(s, x, y) ttf_cb_stream_read(s, x, y);

/* a hydra stream that can be in memory or from a file*/
typedef struct ttf_cb_stream {
    lv_fs_file_t * file;
    const void * data;
    size_t size;
    size_t position;
} ttf_cb_stream_t;

static void ttf_cb_stream_read(ttf_cb_stream_t * stream, void * data, size_t to_read);
static void ttf_cb_stream_seek(ttf_cb_stream_t * stream, size_t position);
#endif

#include "stb_rect_pack.h"
#include "stb_truetype_htcw.h"

#define tiny_ttf_cache LV_GLOBAL_DEFAULT()->tiny_ttf_cache
/**********************
 *      TYPEDEFS
 **********************/
typedef struct ttf_font_desc {
    lv_fs_file_t file;
#if LV_TINY_TTF_FILE_SUPPORT != 0
    ttf_cb_stream_t stream;
#else
    const uint8_t * stream;
#endif
    stbtt_fontinfo info;
    float scale;
    int ascent;
    int descent;
} ttf_font_desc_t;

typedef struct _tiny_ttf_cache_data_t {
    lv_font_t * font;
    uint32_t unicode;
    uint32_t size;
    lv_draw_buf_t * draw_buf;
} tiny_ttf_cache_data_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool ttf_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                 uint32_t unicode_letter_next);
static const void * ttf_get_glyph_bitmap_cb(lv_font_glyph_dsc_t * g_dsc,
                                            uint32_t unicode_letter, lv_draw_buf_t * draw_buf);
static void ttf_release_glyph_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc);
static lv_font_t * lv_tiny_ttf_create(const char * path, const void * data, size_t data_size,
                                      int32_t font_size,
                                      size_t cache_size);

static bool tiny_ttf_cache_create_cb(tiny_ttf_cache_data_t * node, void * user_data);
static void tiny_ttf_cache_free_cb(tiny_ttf_cache_data_t * node, void * user_data);
static lv_cache_compare_res_t tiny_ttf_cache_compare_cb(const tiny_ttf_cache_data_t * lhs,
                                                        const tiny_ttf_cache_data_t * rhs);
/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_tiny_ttf_set_size(lv_font_t * font, int32_t font_size)
{
    if(font_size <= 0) {
        LV_LOG_ERROR("invalid font size: %"PRIx32, font_size);
        return;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    dsc->scale = stbtt_ScaleForMappingEmToPixels(&dsc->info, font_size);
    int line_gap = 0;
    stbtt_GetFontVMetrics(&dsc->info, &dsc->ascent, &dsc->descent, &line_gap);
    font->line_height = (int32_t)(dsc->scale * (dsc->ascent - dsc->descent + line_gap));
    font->base_line = (int32_t)(dsc->scale * (line_gap - dsc->descent));
}

void lv_tiny_ttf_destroy(lv_font_t * font)
{
    LV_ASSERT_NULL(font);

    if(font->dsc != NULL) {
        ttf_font_desc_t * ttf = (ttf_font_desc_t *)font->dsc;
#if LV_TINY_TTF_FILE_SUPPORT != 0
        if(ttf->stream.file != NULL) {
            lv_fs_close(&ttf->file);
        }
#endif
        lv_cache_drop_all(tiny_ttf_cache, (void *)font->dsc);
        lv_free(ttf);
        font->dsc = NULL;
    }

    lv_free(font);
}

void lv_tiny_ttf_init(void)
{
    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)tiny_ttf_cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)tiny_ttf_cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)tiny_ttf_cache_free_cb,
    };

    tiny_ttf_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(tiny_ttf_cache_data_t), 128, ops);
}

void lv_tiny_ttf_deinit(void)
{
    lv_cache_destroy(tiny_ttf_cache, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_TINY_TTF_FILE_SUPPORT != 0
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
#endif

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
    dsc_out->box_w = (x2 - x1 + 1);         /*width of the bitmap in [px]*/
    dsc_out->box_h = (y2 - y1 + 1);         /*height of the bitmap in [px]*/
    dsc_out->ofs_x = x1;                    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = -y2;                   /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;                       /*Bits per pixel: 1/2/4/8*/
    dsc_out->is_placeholder = false;
    return true; /*true: glyph found; false: glyph was not found*/
}

static const void * ttf_get_glyph_bitmap_cb(lv_font_glyph_dsc_t * g_dsc,
                                            uint32_t unicode_letter, lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(draw_buf);
    const lv_font_t * font = g_dsc->resolved_font;
    tiny_ttf_cache_data_t search_key = {
        .font = (lv_font_t *)font,
        .unicode = unicode_letter,
        .size = font->line_height,
    };

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(tiny_ttf_cache, &search_key, (void *)font->dsc);

    if(entry == NULL) {
        LV_LOG_ERROR("cache not allocated\n");
        return NULL;
    }

    g_dsc->entry = entry;
    tiny_ttf_cache_data_t * cached_data = lv_cache_entry_get_data(entry);
    return cached_data->draw_buf;
}

static void ttf_release_glyph_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    if(g_dsc->entry == NULL) {
        return;
    }
    lv_cache_release(tiny_ttf_cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

static lv_font_t * lv_tiny_ttf_create(const char * path, const void * data, size_t data_size, int32_t font_size,
                                      size_t cache_size)
{
    LV_UNUSED(data_size);
    LV_UNUSED(cache_size);
    if((path == NULL && data == NULL) || 0 >= font_size) {
        LV_LOG_ERROR("tiny_ttf: invalid argument\n");
        return NULL;
    }
    ttf_font_desc_t * dsc = lv_malloc_zeroed(sizeof(ttf_font_desc_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
#if LV_TINY_TTF_FILE_SUPPORT != 0
    if(path != NULL) {
        if(LV_FS_RES_OK != lv_fs_open(&dsc->file, path, LV_FS_MODE_RD)) {
            lv_free(dsc);
            LV_LOG_ERROR("tiny_ttf: unable to open %s\n", path);
            return NULL;
        }
        dsc->stream.file = &dsc->file;
    }
    else {
        dsc->stream.data = (const uint8_t *)data;
        dsc->stream.size = data_size;
    }
    if(0 == stbtt_InitFont(&dsc->info, &dsc->stream, stbtt_GetFontOffsetForIndex(&dsc->stream, 0))) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }

#else
    dsc->stream = (const uint8_t *)data;
    if(0 == stbtt_InitFont(&dsc->info, dsc->stream, stbtt_GetFontOffsetForIndex(dsc->stream, 0))) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }
#endif

    lv_font_t * out_font = lv_malloc_zeroed(sizeof(lv_font_t));
    if(out_font == NULL) {
        lv_free(dsc);
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
    out_font->get_glyph_dsc = ttf_get_glyph_dsc_cb;
    out_font->get_glyph_bitmap = ttf_get_glyph_bitmap_cb;
    out_font->release_glyph = ttf_release_glyph_cb;
    out_font->dsc = dsc;
    lv_tiny_ttf_set_size(out_font, font_size);
    return out_font;
}
#if LV_TINY_TTF_FILE_SUPPORT != 0
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, int32_t font_size, size_t cache_size)
{
    return lv_tiny_ttf_create(path, NULL, 0, font_size, cache_size);
}
lv_font_t * lv_tiny_ttf_create_file(const char * path, int32_t font_size)
{
    return lv_tiny_ttf_create(path, NULL, 0, font_size, 0);
}
#endif
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, int32_t font_size, size_t cache_size)
{
    return lv_tiny_ttf_create(NULL, data, data_size, font_size, cache_size);
}
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, int32_t font_size)
{
    return lv_tiny_ttf_create(NULL, data, data_size, font_size, 0);
}

/*-----------------
 * Cache Callbacks
 *----------------*/

static bool tiny_ttf_cache_create_cb(tiny_ttf_cache_data_t * node, void * user_data)
{

    ttf_font_desc_t * dsc = (ttf_font_desc_t *)user_data;
    uint32_t unicode_letter = node->unicode;

    const stbtt_fontinfo * info = (const stbtt_fontinfo *)&dsc->info;
    int g1 = stbtt_FindGlyphIndex(info, (int)unicode_letter);
    if(g1 == 0) {
        /* Glyph not found */
        return false;
    }
    int x1, y1, x2, y2;
    stbtt_GetGlyphBitmapBox(info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
    int w, h;
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
    if(NULL == draw_buf) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return false;
    }

    lv_draw_buf_clear(draw_buf, NULL);

    uint32_t stride = draw_buf->header.stride;
    stbtt_MakeGlyphBitmap(info, draw_buf->data, w, h, stride, dsc->scale, dsc->scale, g1);

    node->draw_buf = draw_buf;
    return true;
}

static void tiny_ttf_cache_free_cb(tiny_ttf_cache_data_t * node, void * user_data)
{
    LV_UNUSED(user_data);

    lv_draw_buf_destroy(node->draw_buf);
}

static lv_cache_compare_res_t tiny_ttf_cache_compare_cb(const tiny_ttf_cache_data_t * lhs,
                                                        const tiny_ttf_cache_data_t * rhs)
{
    if(lhs->font != rhs->font) {
        return lhs->font > rhs->font ? 1 : -1;
    }

    if(lhs->unicode != rhs->unicode) {
        return lhs->unicode > rhs->unicode ? 1 : -1;
    }

    if(lhs->size != rhs->size) {
        return lhs->size > rhs->size ? 1 : -1;
    }

    return 0;
}

#endif
