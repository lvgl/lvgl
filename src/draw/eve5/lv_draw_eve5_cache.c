/**
 * @file lv_draw_eve5_cache.c
 *
 * EVE5 (BT820) Asset Cache Implementation
 *
 * Simple lookup caches for images and glyphs. The GPU allocator owns
 * memory lifecycle - these caches are just indexes that validate
 * allocations are still present on each lookup.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/**********************
 * STATIC PROTOTYPES
 **********************/
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

void lv_draw_eve5_image_cache_insert(lv_draw_eve5_unit_t *u,
                                      const lv_image_dsc_t *img_dsc,
                                      Esd_GpuHandle handle,
                                      uint16_t eve_format,
                                      int32_t eve_stride,
                                      uint32_t image_offset, uint32_t palette_offset)
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
    e->key_hash = 0;  /* Not used for pointer-based const image lookup */
    e->gpu_handle = handle;
    e->eve_format = eve_format;
    e->eve_stride = eve_stride;
    e->width = img_dsc->header.w;
    e->height = img_dsc->header.h;
    e->image_offset = image_offset;
    e->palette_offset = palette_offset;

    LV_LOG_TRACE("EVE5: Image cached %p (%dx%d)",
                 (void *)e->key, e->width, e->height);
}

void lv_draw_eve5_image_cache_insert_raw(lv_draw_eve5_unit_t *u,
                                          uintptr_t key, uint32_t key_hash,
                                          Esd_GpuHandle handle,
                                          uint16_t eve_format,
                                          int32_t eve_stride,
                                          int16_t width, int16_t height,
                                          uint32_t image_offset, uint32_t palette_offset)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    LV_UNUSED(key);

    /* Check for existing entry with same hash (avoid duplicates from multiple widgets) */
    for(uint32_t i = 0; i < cache->count; i++) {
        if(cache->entries[i].key_hash == key_hash) {
            /* Already cached — update with new handle info */
            lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];
            e->gpu_handle = handle;
            e->eve_format = eve_format;
            e->eve_stride = eve_stride;
            e->width = width;
            e->height = height;
            e->image_offset = image_offset;
            e->palette_offset = palette_offset;
            LV_LOG_TRACE("EVE5: Image cache updated hash=0x%08X (%dx%d)",
                         key_hash, width, height);
            return;
        }
    }

    if(cache->count >= cache->capacity) {
        image_cache_compact(u, cache);
    }

    if(cache->count >= cache->capacity) {
        LV_LOG_WARN("EVE5: Image cache full, not caching (raw)");
        return;
    }

    lv_draw_eve5_image_cache_entry_t *e = &cache->entries[cache->count++];
    e->key = key_hash;  /* Store hash as key for consistency */
    e->key_hash = key_hash;
    e->gpu_handle = handle;
    e->eve_format = eve_format;
    e->eve_stride = eve_stride;
    e->width = width;
    e->height = height;
    e->image_offset = image_offset;
    e->palette_offset = palette_offset;

    LV_LOG_TRACE("EVE5: Image cached (raw) hash=0x%08X (%dx%d)",
                 key_hash, width, height);
}

/**********************
 * HANDLE-BASED CACHE LOOKUPS
 **********************/

/**
 * Find const ROM image by data pointer — returns handle-based result.
 * For const lv_image_dsc_t, the data pointer uniquely identifies the image
 * (immutable content in ROM/flash, never changes at the same address).
 */
bool lv_draw_eve5_image_cache_find(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc, eve5_gpu_image_t *out)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    uintptr_t key = (uintptr_t)img_dsc->data;

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        if(e->key == key) {
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                out->gpu_handle = e->gpu_handle;
                out->eve_format = e->eve_format;
                out->eve_stride = e->eve_stride;
                out->width = e->width;
                out->height = e->height;
                out->image_offset = e->image_offset;
                out->palette_offset = e->palette_offset;
                return true;
            }

            /* Stale entry */
            cache->entries[i] = cache->entries[--cache->count];
            break;
        }
    }

    return false;
}

/**
 * Find image by path hash — returns handle-based result.
 */
bool lv_draw_eve5_image_cache_find_by_hash(lv_draw_eve5_unit_t *u,
    uint32_t key_hash, eve5_gpu_image_t *out)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        if(e->key_hash == key_hash) {
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                out->gpu_handle = e->gpu_handle;
                out->eve_format = e->eve_format;
                out->eve_stride = e->eve_stride;
                out->width = e->width;
                out->height = e->height;
                out->image_offset = e->image_offset;
                out->palette_offset = e->palette_offset;
                return true;
            }

            /* Stale entry */
            cache->entries[i] = cache->entries[--cache->count];
            break;
        }
    }

    return false;
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

/* Canvas cache removed — canvas GPU allocations now use the VRAM residency
 * system (lv_draw_eve5_vram_res_t on draw_buf->vram_res). */

#endif /* LV_USE_DRAW_EVE5 */
