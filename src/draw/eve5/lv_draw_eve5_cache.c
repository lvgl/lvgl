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
static uint32_t hash_image_header(const lv_image_dsc_t *img);
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
 * Hash image header fields only.
 * Uses header metadata (w, h, cf, stride) as a quick fingerprint.
 * Does NOT sample image data — that was causing performance issues
 * by reading 128 bytes from potentially uncached memory on every lookup.
 * For LVGL images, the data pointer + header is sufficient to identify
 * unique images; content rarely changes at the same address.
 */
static uint32_t hash_image_header(const lv_image_dsc_t *img)
{
    uint32_t h = 2166136261u;

    /* Mix in header fields only — no data sampling */
    h = (h ^ img->header.w) * 16777619u;
    h = (h ^ img->header.h) * 16777619u;
    h = (h ^ img->header.cf) * 16777619u;
    h = (h ^ img->header.stride) * 16777619u;

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
                                          uint16_t *out_eve_format,
                                          int32_t *out_eve_stride,
                                          uint32_t *out_palette_addr)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    uintptr_t key = (uintptr_t)img_dsc->data;
    uint32_t key_hash = hash_image_header(img_dsc);

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        if(e->key == key && e->key_hash == key_hash) {
            /* Verify allocation is still valid */
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                *out_eve_format = e->eve_format;
                *out_eve_stride = e->eve_stride;
                if(out_palette_addr) {
                    *out_palette_addr = (e->palette_offset != GA_INVALID)
                                        ? (addr + e->palette_offset) : GA_INVALID;
                }
                LV_LOG_TRACE("EVE5: Image cache hit for %p at 0x%08X",
                             (void *)key, addr + e->image_offset);
                return addr + e->image_offset;
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
    e->key_hash = hash_image_header(img_dsc);
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

uint32_t lv_draw_eve5_image_cache_lookup_raw(lv_draw_eve5_unit_t *u,
                                              uintptr_t key, uint32_t key_hash,
                                              uint16_t *out_eve_format,
                                              int32_t *out_eve_stride,
                                              int32_t *out_width, int32_t *out_height,
                                              uint32_t *out_palette_addr)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    LV_UNUSED(key);  /* FILE sources: LVGL copies path strings, so match by hash only */

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        /* Match by hash only for FILE sources. LVGL's lv_image_set_src() calls
         * lv_strdup() for FILE paths, so each widget has its own copy at a
         * different address. The hash (FNV-1a of path content) is the same. */
        if(e->key_hash == key_hash) {
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr != GA_INVALID) {
                *out_eve_format = e->eve_format;
                *out_eve_stride = e->eve_stride;
                *out_width = e->width;
                *out_height = e->height;
                if(out_palette_addr) {
                    *out_palette_addr = (e->palette_offset != GA_INVALID)
                                        ? (addr + e->palette_offset) : GA_INVALID;
                }
                LV_LOG_TRACE("EVE5: Image cache hit (raw) hash=0x%08X at 0x%08X",
                             key_hash, addr + e->image_offset);
                return addr + e->image_offset;
            }

            LV_LOG_TRACE("EVE5: Image cache stale entry (raw) hash=0x%08X", key_hash);
            cache->entries[i] = cache->entries[--cache->count];
            break;
        }
    }

    return GA_INVALID;
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
 * Find image by img_dsc pointer — returns handle-based result.
 */
bool lv_draw_eve5_image_cache_find(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc, eve5_gpu_image_t *out)
{
    lv_draw_eve5_image_cache_t *cache = &u->image_cache;
    uintptr_t key = (uintptr_t)img_dsc->data;
    uint32_t key_hash = hash_image_header(img_dsc);

    for(uint32_t i = 0; i < cache->count; i++) {
        lv_draw_eve5_image_cache_entry_t *e = &cache->entries[i];

        if(e->key == key && e->key_hash == key_hash) {
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

/**********************
 * CANVAS CACHE
 **********************/

void lv_draw_eve5_canvas_cache_init(lv_draw_eve5_unit_t *u)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;
    lv_memzero(cache->entries, sizeof(cache->entries));
    cache->frame = 0;
    cache->initialized = true;
}

void lv_draw_eve5_canvas_cache_new_frame(lv_draw_eve5_unit_t *u)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;
    cache->frame++;

    /* Check for entries whose GPU allocation was reclaimed by allocator.
     * Note: We don't do LRU eviction — canvas entries are treated as permanent.
     * Proper lifecycle management requires LVGL buffer management hooks. */
    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(!e->valid) continue;

        /* Check if GPU allocation was reclaimed by allocator */
        uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
        if(addr == GA_INVALID) {
            LV_LOG_TRACE("EVE5: Canvas cache entry %p invalidated (allocator reclaimed)",
                         e->data_ptr);
            e->valid = false;
        }
    }
}

uint32_t lv_draw_eve5_canvas_cache_lookup(lv_draw_eve5_unit_t *u,
                                           const void *data_ptr,
                                           uint32_t *out_width,
                                           uint32_t *out_height,
                                           uint32_t *out_aligned_width,
                                           uint16_t *out_eve_format,
                                           uint32_t *out_stride,
                                           uint32_t *out_palette_addr)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(!e->valid) continue;

        if(e->data_ptr == data_ptr) {
            /* Verify allocation still valid */
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr == GA_INVALID) {
                e->valid = false;
                return GA_INVALID;
            }

            /* Update LRU */
            e->last_used_frame = cache->frame;

            if(out_width) *out_width = e->width;
            if(out_height) *out_height = e->height;
            if(out_aligned_width) *out_aligned_width = e->aligned_width;
            if(out_eve_format) *out_eve_format = e->eve_format;
            if(out_stride) *out_stride = e->stride;
            if(out_palette_addr) *out_palette_addr = e->palette_addr;

            LV_LOG_TRACE("EVE5: Canvas cache hit for %p at 0x%08X (fmt=%d)",
                         data_ptr, addr + e->source_offset, e->eve_format);
            return addr + e->source_offset;
        }
    }

    return GA_INVALID;
}

void lv_draw_eve5_canvas_cache_insert(lv_draw_eve5_unit_t *u,
                                       const void *data_ptr,
                                       Esd_GpuHandle handle,
                                       uint32_t width, uint32_t height,
                                       uint32_t aligned_width,
                                       uint16_t eve_format,
                                       uint32_t stride,
                                       uint32_t palette_addr,
                                       uint32_t source_offset)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    /* Look for existing entry to update */
    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->valid && e->data_ptr == data_ptr) {
            /* Update existing entry */
            e->gpu_handle = handle;
            e->width = width;
            e->height = height;
            e->aligned_width = aligned_width;
            e->eve_format = eve_format;
            e->stride = stride;
            e->palette_addr = palette_addr;
            e->source_offset = source_offset;
            e->last_used_frame = cache->frame;
            LV_LOG_TRACE("EVE5: Canvas cache updated %p (%ux%u fmt=%d)",
                         data_ptr, width, height, eve_format);
            return;
        }
    }

    /* Find empty slot */
    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(!e->valid) {
            e->data_ptr = (void *)data_ptr;
            e->gpu_handle = handle;
            e->width = width;
            e->height = height;
            e->aligned_width = aligned_width;
            e->eve_format = eve_format;
            e->stride = stride;
            e->palette_addr = palette_addr;
            e->source_offset = source_offset;
            e->last_used_frame = cache->frame;
            e->valid = true;
            e->is_premultiplied = false;  /* New allocations start non-premultiplied */
            LV_LOG_TRACE("EVE5: Canvas cached %p (%ux%u fmt=%d)",
                         data_ptr, width, height, eve_format);
            return;
        }
    }

    /* Cache full - find oldest entry to evict */
    uint32_t oldest_idx = 0;
    uint32_t oldest_frame = UINT32_MAX;
    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->last_used_frame < oldest_frame) {
            oldest_frame = e->last_used_frame;
            oldest_idx = i;
        }
    }

    lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[oldest_idx];
    LV_LOG_TRACE("EVE5: Canvas cache evicting %p for %p",
                 e->data_ptr, data_ptr);
    Esd_GpuAlloc_Free(u->allocator, e->gpu_handle);

    e->data_ptr = (void *)data_ptr;
    e->gpu_handle = handle;
    e->width = width;
    e->height = height;
    e->aligned_width = aligned_width;
    e->eve_format = eve_format;
    e->stride = stride;
    e->palette_addr = palette_addr;
    e->source_offset = source_offset;
    e->last_used_frame = cache->frame;
    e->valid = true;
    e->is_premultiplied = false;  /* New allocations start non-premultiplied */
}

uint32_t lv_draw_eve5_canvas_cache_get_or_create(lv_draw_eve5_unit_t *u,
                                                   lv_layer_t *layer,
                                                   uint16_t target_eve_format,
                                                   uint8_t target_bpp,
                                                   uint32_t *out_aligned_width,
                                                   uint16_t *out_eve_format,
                                                   uint32_t *out_stride,
                                                   bool *out_is_new,
                                                   bool *out_is_premultiplied)
{
    if(!layer->draw_buf || !layer->draw_buf->data) {
        if(out_is_new) *out_is_new = false;
        if(out_is_premultiplied) *out_is_premultiplied = false;
        return GA_INVALID;
    }

    const void *data_ptr = layer->draw_buf->data;
    uint32_t width = lv_area_get_width(&layer->buf_area);
    uint32_t height = lv_area_get_height(&layer->buf_area);
    uint32_t aligned_w = ALIGN_UP(width, 16);
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    /* Try cache lookup first */
    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(!e->valid || e->data_ptr != data_ptr) continue;

        /* Verify allocation still valid */
        uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
        if(addr == GA_INVALID) {
            e->valid = false;
            break;  /* Need to allocate new */
        }

        /* Validate cached size matches */
        if(e->width == width && e->height == height) {
            e->last_used_frame = cache->frame;
            if(out_aligned_width) *out_aligned_width = e->aligned_width;
            if(out_eve_format) *out_eve_format = e->eve_format;
            if(out_stride) *out_stride = e->stride;
            if(out_is_new) *out_is_new = false;  /* Reusing existing allocation */
            if(out_is_premultiplied) *out_is_premultiplied = e->is_premultiplied;
            return addr + e->source_offset;
        }

        /* Size mismatch - need to reallocate */
        LV_LOG_TRACE("EVE5: Canvas size changed %ux%u -> %ux%u, reallocating",
                     e->width, e->height, width, height);
        break;
    }

    /* Allocate new GPU texture in the target format.
     * CMD_RENDERTARGET requires 128-byte alignment. */
    uint32_t stride = aligned_w * target_bpp;
    uint32_t size = stride * height;
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, size, GA_ALIGN_128);

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5: Failed to allocate canvas GPU memory (%u bytes, fmt=%d)", size, target_eve_format);
        if(out_is_new) *out_is_new = false;
        if(out_is_premultiplied) *out_is_premultiplied = false;
        return GA_INVALID;
    }

    /* Cache the allocation with target format (no palette) */
    lv_draw_eve5_canvas_cache_insert(u, data_ptr, handle, width, height, aligned_w,
                                      target_eve_format, stride, GA_INVALID, 0);

    if(out_aligned_width) *out_aligned_width = aligned_w;
    if(out_eve_format) *out_eve_format = target_eve_format;
    if(out_stride) *out_stride = stride;
    if(out_is_new) *out_is_new = true;  /* Fresh allocation */
    if(out_is_premultiplied) *out_is_premultiplied = false;  /* New = not premultiplied */
    return addr;
}

/**
 * Set the premultiplied state of a canvas cache entry.
 */
void lv_draw_eve5_canvas_cache_set_premultiplied(lv_draw_eve5_unit_t *u,
                                                   const void *data_ptr,
                                                   bool is_premultiplied)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->valid && e->data_ptr == data_ptr) {
            e->is_premultiplied = is_premultiplied;
            LV_LOG_TRACE("EVE5: Canvas %p set premultiplied=%d", data_ptr, is_premultiplied);
            return;
        }
    }
}

/**
 * Check if a canvas cache entry is premultiplied.
 */
bool lv_draw_eve5_canvas_cache_is_premultiplied(lv_draw_eve5_unit_t *u,
                                                  const void *data_ptr)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->valid && e->data_ptr == data_ptr) {
            return e->is_premultiplied;
        }
    }
    return false;  /* Not found = assume not premultiplied */
}

/**
 * Get the EVE format of a canvas cache entry.
 */
uint16_t lv_draw_eve5_canvas_cache_get_format(lv_draw_eve5_unit_t *u,
                                               const void *data_ptr)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->valid && e->data_ptr == data_ptr) {
            return e->eve_format;
        }
    }
    return ARGB8;  /* Not found = assume ARGB8 (default render target format) */
}

/**
 * Update the format, handle, and stride of a canvas cache entry.
 * Used when converting a canvas from native format to ARGB8.
 *
 * IMPORTANT: This does NOT free the old allocation. The caller must handle
 * lifecycle explicitly (e.g., via track_frame_alloc for deferred free).
 */
void lv_draw_eve5_canvas_cache_set_format(lv_draw_eve5_unit_t *u,
                                            const void *data_ptr,
                                            Esd_GpuHandle new_handle,
                                            uint16_t eve_format,
                                            uint32_t stride)
{
    lv_draw_eve5_canvas_cache_t *cache = &u->canvas_cache;

    for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
        lv_draw_eve5_canvas_cache_entry_t *e = &cache->entries[i];
        if(e->valid && e->data_ptr == data_ptr) {
            /* Caller is responsible for freeing old allocation if needed */
            e->gpu_handle = new_handle;
            e->eve_format = eve_format;
            e->stride = stride;
            e->palette_addr = GA_INVALID;  /* No palette for direct format */
            e->source_offset = 0;
            LV_LOG_TRACE("EVE5: Canvas %p format updated to %d (stride=%u)",
                         data_ptr, eve_format, stride);
            return;
        }
    }
}

#endif /* LV_USE_DRAW_EVE5 */
