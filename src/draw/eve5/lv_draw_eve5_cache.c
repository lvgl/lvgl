/**
 * @file lv_draw_eve5_cache.c
 *
 * EVE5 (BT820) Asset Cache Implementation
 *
 * Simple lookup caches for images and glyphs. The GPU allocator owns
 * memory lifecycle - these caches are just indexes that validate
 * allocations are still present on each lookup.
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/**********************
 * STATIC PROTOTYPES
 **********************/
static uint32_t hash_image_sample(const lv_image_dsc_t *img);
static void image_cache_compact(lv_draw_eve5_unit_t *u, lv_draw_eve5_image_cache_t *cache);
static void glyph_cache_compact(lv_draw_eve5_unit_t *u, lv_draw_eve5_glyph_cache_t *cache);

/**********************
 * IMAGE CACHE
 **********************/

void lv_draw_eve5_image_cache_init(lv_draw_eve5_image_cache_t *cache, uint32_t capacity)
{
    cache->entries = lv_calloc(capacity, sizeof(lv_draw_eve5_image_cache_entry_t));
    LV_ASSERT_MALLOC(cache->entries);
    cache->capacity = capacity;
    cache->count = 0;
}

void lv_draw_eve5_image_cache_deinit(lv_draw_eve5_image_cache_t *cache)
{
    /* Note: We don't free GPU allocations here - allocator owns them */
    lv_free(cache->entries);
    cache->entries = NULL;
    cache->capacity = 0;
    cache->count = 0;
}

/**
 * Hash image data by sampling throughout the image.
 * Catches differences in any region while staying cheap.
 */
static uint32_t hash_image_sample(const lv_image_dsc_t *img)
{
    uint32_t h = 2166136261u;

    /* Mix in header */
    h = (h ^ img->header.w) * 16777619u;
    h = (h ^ img->header.h) * 16777619u;
    h = (h ^ img->header.cf) * 16777619u;
    h = (h ^ img->header.stride) * 16777619u;

    uint32_t total_size = img->header.stride * img->header.h;
    if(total_size == 0) return h;

    /* Sample 16 bytes from 8 positions spread across the image */
    const uint32_t num_samples = 8;
    const uint32_t bytes_per_sample = 16;

    for(uint32_t s = 0; s < num_samples; s++) {
        /* Position: 0, 1/8, 2/8, ... 7/8 through the data */
        uint32_t offset = (total_size * s) / num_samples;
        uint32_t end = LV_MIN(offset + bytes_per_sample, total_size);

        for(uint32_t i = offset; i < end; i++) {
            h = (h ^ img->data[i]) * 16777619u;
        }
    }

    return h;
}

/**
 * Remove entries whose GPU allocations have been reclaimed.
 */
static void image_cache_compact(lv_draw_eve5_unit_t *u, lv_draw_eve5_image_cache_t *cache)
{
    for(uint32_t i = 0; i < cache->count; ) {
        uint32_t addr = Esd_GpuAlloc_Get(u->allocator, cache->entries[i].gpu_handle);
        if(addr == GA_INVALID) {
            /* Swap with last and shrink */
            cache->entries[i] = cache->entries[--cache->count];
        }
        else {
            i++;
        }
    }
}

uint32_t lv_draw_eve5_image_cache_lookup(lv_draw_eve5_unit_t *u,
                                          const lv_image_dsc_t *img_dsc,
                                          uint8_t *out_eve_format,
                                          int32_t *out_eve_stride)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    uintptr_t key = (uintptr_t)img_dsc->data;
    uint32_t key_hash = hash_image_sample(img_dsc);

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        if(e->key == key && e->key_hash == key_hash) {
            /* Verify allocation is still valid */
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                *out_eve_format = e->eve_format;
                *out_eve_stride = e->eve_stride;
                LV_LOG_TRACE("EVE5: Image cache hit for %p at 0x%08X", 
                             (void *)key, addr);
                return addr;
            }
            
            /* Allocator evicted it - remove stale entry */
            LV_LOG_TRACE("EVE5: Image cache stale entry for %p", (void *)key);
            cache->entries[i] = cache->entries[--cache->count];
            break;
        }
    }

    return GA_INVALID;
}

void lv_draw_eve5_image_cache_insert(lv_draw_eve5_unit_t *u,
                                      const lv_image_dsc_t *img_dsc,
                                      Esd_GpuHandle handle,
                                      uint8_t eve_format,
                                      int32_t eve_stride)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;

    /* Try to make room if at capacity */
    if(cache->count >= cache->capacity) {
        image_cache_compact(u, cache);
    }

    if(cache->count >= cache->capacity) {
        LV_LOG_WARN("EVE5: Image cache full, not caching");
        return;
    }

    lv_draw_eve5_image_cache_entry_t *e = &cache->entries[cache->count++];
    e->key = (uintptr_t)img_dsc->data;
    e->key_hash = hash_image_sample(img_dsc);
    e->gpu_handle = handle;
    e->eve_format = eve_format;
    e->eve_stride = eve_stride;
    e->width = img_dsc->header.w;
    e->height = img_dsc->header.h;

    LV_LOG_TRACE("EVE5: Image cached %p (%dx%d)", 
                 (void *)e->key, e->width, e->height);
}

/**********************
 * GLYPH CACHE
 **********************/

void lv_draw_eve5_glyph_cache_init(lv_draw_eve5_glyph_cache_t *cache, uint32_t capacity)
{
    cache->entries = lv_calloc(capacity, sizeof(lv_draw_eve5_glyph_cache_entry_t));
    LV_ASSERT_MALLOC(cache->entries);
    cache->capacity = capacity;
    cache->count = 0;
}

void lv_draw_eve5_glyph_cache_deinit(lv_draw_eve5_glyph_cache_t *cache)
{
    lv_free(cache->entries);
    cache->entries = NULL;
    cache->capacity = 0;
    cache->count = 0;
}

/**
 * Remove entries whose GPU allocations have been reclaimed.
 */
static void glyph_cache_compact(lv_draw_eve5_unit_t *u, lv_draw_eve5_glyph_cache_t *cache)
{
    for(uint32_t i = 0; i < cache->count; ) {
        uint32_t addr = Esd_GpuAlloc_Get(u->allocator, cache->entries[i].gpu_handle);
        if(addr == GA_INVALID) {
            cache->entries[i] = cache->entries[--cache->count];
        }
        else {
            i++;
        }
    }
}

uint32_t lv_draw_eve5_glyph_cache_lookup(lv_draw_eve5_unit_t *u,
                                          const uint8_t *glyph_bitmap,
                                          uint16_t *out_stride)
{
    lv_draw_eve5_glyph_cache_t *cache = &u->glyph_cache;
    uintptr_t key = (uintptr_t)glyph_bitmap;

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_glyph_cache_entry_t *e = &cache->entries[i];

        if(e->key == key) {
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                *out_stride = e->stride;
                return addr;
            }

            /* Stale entry */
            cache->entries[i] = cache->entries[--cache->count];
            break;
        }
    }

    return GA_INVALID;
}

void lv_draw_eve5_glyph_cache_insert(lv_draw_eve5_unit_t *u,
                                      const uint8_t *glyph_bitmap,
                                      Esd_GpuHandle handle,
                                      uint16_t width,
                                      uint16_t height,
                                      uint16_t stride)
{
    lv_draw_eve5_glyph_cache_t *cache = &u->glyph_cache;

    if(cache->count >= cache->capacity) {
        glyph_cache_compact(u, cache);
    }

    if(cache->count >= cache->capacity) {
        LV_LOG_TRACE("EVE5: Glyph cache full, not caching");
        return;
    }

    lv_draw_eve5_glyph_cache_entry_t *e = &cache->entries[cache->count++];
    e->key = (uintptr_t)glyph_bitmap;
    e->gpu_handle = handle;
    e->width = width;
    e->height = height;
    e->stride = stride;
}

#endif /* LV_USE_DRAW_EVE5 */
