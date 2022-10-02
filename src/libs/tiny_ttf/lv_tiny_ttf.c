#include "lv_tiny_ttf.h"
#if LV_USE_TINY_TTF
#include <stdio.h>

#ifndef LV_TINY_TTF_DEFAULT_CACHE_SIZE
    #define LV_TINY_TTF_DEFAULT_CACHE_SIZE 4096
#endif
#ifndef LV_TINY_TTF_CACHE_BUCKETS
    #define LV_TINY_TTF_CACHE_BUCKETS 16
#endif

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_HEAP_FACTOR_SIZE_32 50
#define STBTT_HEAP_FACTOR_SIZE_128 20
#define STBTT_HEAP_FACTOR_SIZE_DEFAULT 10
#define STBTT_malloc(x,u)  ((void)(u),lv_malloc(x))
#define STBTT_free(x,u)    ((void)(u),lv_free(x))
#define TTF_CACHE_MALLOC(x)  (lv_malloc(x))
#define TTF_CACHE_REALLOC(x,y)  (lv_realloc(x,y))
#define TTF_CACHE_FREE(x)    (lv_free(x))
#define TTF_MALLOC(x)  (lv_malloc(x))
#define TTF_FREE(x)    (lv_free(x))
typedef void * ttf_cache_handle_t;
typedef struct ttf_cache_entry {
    int key;
    unsigned long long age;
    int size;
    void * data;
} ttf_cache_entry_t;
typedef struct ttf_cache_bucket {
    int capacity;
    ttf_cache_entry_t * entries;
} ttf_cache_bucket_t;

typedef struct ttf_cache {
    int max_size;
    int bucket_size;
    int total_size;
    unsigned long long age;
    ttf_cache_bucket_t * buckets;
} ttf_cache_t;

static unsigned long long ttf_cache_get_oldest_age(ttf_cache_handle_t * handle)
{
    ttf_cache_t * cache = (ttf_cache_t *)handle;
    unsigned long long result = (unsigned long long) - 1;
    for(int i = 0; i < cache->bucket_size; ++i) {
        ttf_cache_bucket_t * bucket = &cache->buckets[i];
        if(bucket->entries != NULL) {
            for(int j = 0; j < bucket->capacity; ++j) {
                ttf_cache_entry_t * entry = &bucket->entries[j];
                if(entry->age != 0 && entry->age < result) {
                    result = entry->age;
                }
            }
        }
    }
    if(result == (unsigned long long) - 1) {
        return 0;
    }
    return result;
}
static ttf_cache_handle_t ttf_cache_create(int max_size, int buckets)
{
    ttf_cache_t * result = (ttf_cache_t *)TTF_CACHE_MALLOC(sizeof(ttf_cache_t));
    if(result == NULL) {
        return NULL;
    }
    result->age = 1;
    result->max_size = max_size;
    result->total_size = 0;
    result->bucket_size = buckets;
    result->buckets = (ttf_cache_bucket_t *)TTF_CACHE_MALLOC(buckets * sizeof(ttf_cache_bucket_t));
    if(result->buckets == NULL) {
        TTF_CACHE_FREE(result);
        return NULL;
    }
    for(int i = 0; i < buckets; ++i) {
        result->buckets[i].capacity = 0;
        result->buckets[i].entries = NULL;
    }
    return result;
}
static void * ttf_cache_get(ttf_cache_handle_t handle, int key)
{
    if(handle == NULL) {
        return NULL;
    }
    ttf_cache_t * cache = (ttf_cache_t *)handle;
    int ci = key % cache->bucket_size;
    ttf_cache_bucket_t * bucket = &cache->buckets[ci];
    for(int i = 0; i < bucket->capacity; ++i) {
        ttf_cache_entry_t * entry = &bucket->entries[i];
        if(entry->age != 0 && entry->key == key) {
            entry->age = ++cache->age;
            return entry->data;
        }
    }
    return NULL;
}
static void * ttf_cache_add(ttf_cache_handle_t handle, int key, int size)
{
    if(handle == NULL) {
        return NULL;
    }
    ttf_cache_t * cache = (ttf_cache_t *)handle;
    int ci = key % cache->bucket_size;
    ttf_cache_bucket_t * bucket = &cache->buckets[ci];
    for(int i = 0; i < bucket->capacity; ++i) {
        ttf_cache_entry_t * entry = &bucket->entries[i];
        if(entry->age != 0 && entry->key == key) {
            TTF_CACHE_FREE(entry->data);
            cache->total_size -= entry->size;
            entry->age = 0;
            break;
        }
    }
    while(cache->total_size > 0 && (cache->max_size < cache->total_size + size)) {
        // expire entries
        unsigned long long oldest = ttf_cache_get_oldest_age(handle);
        if(oldest == 0) {
            break;
        }
        for(int i = 0; i < cache->bucket_size; ++i) {
            ttf_cache_bucket_t * bucket2 = &cache->buckets[i];
            for(int j = 0; j < bucket2->capacity; ++j) {
                ttf_cache_entry_t * entry = &bucket2->entries[j];
                if(entry->age == oldest) {
                    if(entry->data != NULL) {
                        TTF_CACHE_FREE(entry->data);
                        entry->data = NULL;
                        entry->age = 0;
                        cache->total_size -= entry->size;
                        entry->size = 0;
                        i = cache->bucket_size;
                        break;
                    }
                }
            }
        }
    }
    if(bucket->entries == NULL) {
        bucket->capacity = 4;
        bucket->entries = (ttf_cache_entry_t *)TTF_CACHE_MALLOC(sizeof(ttf_cache_entry_t) * bucket->capacity);
        if(bucket->entries == NULL) {
            return NULL;
        }
        for(int i = 0; i < bucket->capacity; ++i) {
            bucket->entries[i].age = 0;
            bucket->entries[i].data = NULL;
            bucket->entries[i].size = 0;
        }
    }
    for(int i = 0; i < bucket->capacity; ++i) {
        ttf_cache_entry_t * entry = &bucket->entries[i];
        if(entry->age == 0) {
            entry->data = TTF_CACHE_MALLOC(size);
            if(entry->data == NULL) {
                return NULL;
            }
            entry->size = size;
            entry->age = cache->age;
            entry->key = key;
            cache->total_size += size;
            return entry->data;
        }
    }
    int newcap = bucket->capacity * 2;
    ttf_cache_entry_t * te = (ttf_cache_entry_t *)TTF_CACHE_REALLOC(bucket->entries, sizeof(ttf_cache_entry_t) * newcap);
    if(te == NULL) {
        return NULL;
    }
    bucket->entries = te;
    for(int i = bucket->capacity; i < newcap; ++i) {
        bucket->entries[i].age = 0;
        bucket->entries[i].data = NULL;
        bucket->entries[i].size = 0;
    }
    void * result = TTF_CACHE_MALLOC(size);
    bucket->entries[bucket->capacity].data = result;
    if(result == NULL) {
        return NULL;
    }
    bucket->entries[bucket->capacity].size = size;
    bucket->entries[bucket->capacity].age = cache->age;
    bucket->entries[bucket->capacity].key = key;
    bucket->capacity = newcap;
    return result;
}

static void ttf_cache_clear(ttf_cache_handle_t handle)
{
    if(handle == NULL) {
        return;
    }
    ttf_cache_t * cache = (ttf_cache_t *)handle;
    for(int i = 0; i < cache->bucket_size; ++i) {
        ttf_cache_bucket_t * bucket = &cache->buckets[i];
        if(bucket->entries != NULL) {
            for(int j = 0; j < bucket->capacity; ++j) {
                ttf_cache_entry_t * entry = &bucket->entries[j];
                if(entry->age != 0 && entry->data != NULL) {
                    TTF_CACHE_FREE(entry->data);
                }
            }
            TTF_CACHE_FREE(cache->buckets[i].entries);
        }
    }
}
static void ttf_cache_destroy(ttf_cache_handle_t handle)
{
    ttf_cache_clear(handle);
    TTF_CACHE_FREE((ttf_cache_t *)handle);
}
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
    const uint8_t * stream;
#endif
    stbtt_fontinfo info;
    ttf_cache_handle_t cache;
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
    uint8_t * buffer = (uint8_t *)ttf_cache_get(dsc->cache, unicode_letter);
    if(buffer == NULL) {
        int g1 = stbtt_FindGlyphIndex(info, (int)unicode_letter);
        int x1, y1, x2, y2;
        stbtt_GetGlyphBitmapBox(info, g1, dsc->scale, dsc->scale, &x1, &y1, &x2, &y2);
        int w, h;
        w = x2 - x1 + 1;
        h = y2 - y1 + 1;
        int buffer_size = w * h;
        buffer = ttf_cache_add(dsc->cache, unicode_letter, buffer_size);
        if(buffer == NULL) {
            return NULL;
        }
        memset(buffer, 0, buffer_size);
        stbtt_MakeGlyphBitmap(info, buffer, w, h, w, dsc->scale, dsc->scale, g1);
    }
    return buffer; /*Or NULL if not found*/
}

static lv_font_t * lv_tiny_ttf_create(const char * path, const void * data, size_t data_size,  lv_coord_t line_height,
                                      size_t cache_size)
{
    LV_UNUSED(data_size);
    if((path == NULL && data == NULL) || 0 >= line_height) {
        LV_LOG_ERROR("tiny_ttf: invalid argument\n");
        return NULL;
    }
    if(cache_size < 1) {
        cache_size = LV_TINY_TTF_DEFAULT_CACHE_SIZE;
    }
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)TTF_MALLOC(sizeof(ttf_font_desc_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        return NULL;
    }
    dsc->cache = ttf_cache_create(cache_size, LV_TINY_TTF_CACHE_BUCKETS);
    if(dsc->cache == NULL) {
        LV_LOG_ERROR("tiny_ttf: out of memory\n");
        TTF_FREE(dsc);
        return NULL;
    }
#if LV_TINY_TTF_FILE_SUPPORT !=0
    if(path != NULL) {
        if(LV_FS_RES_OK != lv_fs_open(&dsc->file, path, LV_FS_MODE_RD)) {
            ttf_cache_destroy(dsc->cache);
            TTF_FREE(dsc);
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
        ttf_cache_destroy(dsc->cache);
        TTF_FREE(dsc);

        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }

#else
    dsc->stream = (const uint8_t *)data;
    if(0 == stbtt_InitFont(&dsc->info, dsc->stream, stbtt_GetFontOffsetForIndex(dsc->stream, 0))) {
        ttf_cache_destroy(dsc->cache);
        TTF_FREE(dsc);
        LV_LOG_ERROR("tiny_ttf: init failed\n");
        return NULL;
    }

#endif

    float scale = stbtt_ScaleForPixelHeight(&dsc->info, line_height);
    lv_font_t * out_font = (lv_font_t *)TTF_MALLOC(sizeof(lv_font_t));
    if(out_font == NULL) {
        ttf_cache_destroy(dsc->cache);
        TTF_FREE(dsc);
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
lv_font_t * lv_tiny_ttf_create_file_ex(const char * path, lv_coord_t line_height, size_t cache_size)
{
    return lv_tiny_ttf_create(path, NULL, 0, line_height, cache_size);
}
lv_font_t * lv_tiny_ttf_create_file(const char * path, lv_coord_t line_height)
{
    return lv_tiny_ttf_create(path, NULL, 0, line_height, 0);
}
#endif
lv_font_t * lv_tiny_ttf_create_data_ex(const void * data, size_t data_size, lv_coord_t line_height, size_t cache_size)
{
    return lv_tiny_ttf_create(NULL, data, data_size, line_height, cache_size);
}
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, lv_coord_t line_height)
{
    return lv_tiny_ttf_create(NULL, data, data_size, line_height, 0);
}
void lv_tiny_ttf_set_size(lv_font_t * font, lv_coord_t line_height)
{
    ttf_font_desc_t * dsc = (ttf_font_desc_t *)font->dsc;
    if(line_height > 0) {
        font->line_height = line_height;
        dsc->scale = stbtt_ScaleForPixelHeight(&dsc->info, line_height);
        font->base_line = line_height - (lv_coord_t)(dsc->ascent * dsc->scale);
        font->underline_position = (uint8_t)line_height - dsc->descent;
        ttf_cache_clear(dsc->cache);
    }
}
void lv_tiny_ttf_destroy(lv_font_t * font)
{
    if(font != NULL) {
        if(font->dsc != NULL) {
            ttf_font_desc_t * ttf = (ttf_font_desc_t *)font->dsc;
            ttf_cache_destroy(ttf->cache);
#if LV_TINY_TTF_FILE_SUPPORT !=0
            if(ttf->stream.file != NULL) {
                lv_fs_close(&ttf->file);
            }
#endif
            TTF_FREE(ttf);
        }
        TTF_FREE(font);
    }
}
#endif
