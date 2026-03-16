/**
 * @file lv_draw_eve5_sw_cache.c
 *
 * Software rendering texture cache for EVE5
 * 
 * Caches SW-rendered textures (box shadows, etc.) to avoid re-rendering
 * and re-uploading identical content. Uses coordinate normalization so
 * the same visual element at different positions shares a cached texture.
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

static int compare_entry(const lv_draw_eve5_sw_cache_entry_t *entry,
                         lv_draw_task_type_t type,
                         int32_t w, int32_t h,
                         const void *dsc_data, uint32_t dsc_size);
static lv_draw_eve5_sw_cache_entry_t *find_lru_entry(lv_draw_eve5_sw_cache_t *cache);
static void free_entry(lv_draw_eve5_unit_t *u, lv_draw_eve5_sw_cache_entry_t *entry);

/**********************
 * GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve5_sw_cache_init(lv_draw_eve5_unit_t *u)
{
    lv_draw_eve5_sw_cache_t *cache = &u->sw_cache;
    
    if(cache->initialized) return;
    
    lv_memzero(cache->entries, sizeof(cache->entries));
    cache->frame = 0;
    cache->initialized = true;
    
    LV_LOG_INFO("EVE5: SW cache initialized, capacity=%d", EVE5_SW_CACHE_CAPACITY);
}

void lv_draw_eve5_sw_cache_deinit(lv_draw_eve5_unit_t *u)
{
    lv_draw_eve5_sw_cache_t *cache = &u->sw_cache;
    
    for(uint32_t i = 0; i < EVE5_SW_CACHE_CAPACITY; i++) {
        if(cache->entries[i].valid) {
            free_entry(u, &cache->entries[i]);
        }
    }
    cache->initialized = false;
}

void lv_draw_eve5_sw_cache_new_frame(lv_draw_eve5_unit_t *u)
{
    u->sw_cache.frame++;
}

/**
 * Look up a cached SW-rendered texture.
 * 
 * @param u         Draw unit
 * @param type      Task type
 * @param w         Texture width (from normalized _real_area)
 * @param h         Texture height
 * @param dsc_data  Pointer to descriptor data after base (will be compared)
 * @param dsc_size  Size of descriptor data to compare
 * @param out_handle Output: GPU handle if found
 * @param out_stride Output: EVE stride if found
 * @return true if cache hit
 */
bool lv_draw_eve5_sw_cache_lookup(lv_draw_eve5_unit_t *u,
                                   lv_draw_task_type_t type,
                                   int32_t w, int32_t h,
                                   const void *dsc_data, uint32_t dsc_size,
                                   Esd_GpuHandle *out_handle,
                                   uint32_t *out_stride)
{
    lv_draw_eve5_sw_cache_t *cache = &u->sw_cache;
    
    for(uint32_t i = 0; i < EVE5_SW_CACHE_CAPACITY; i++) {
        lv_draw_eve5_sw_cache_entry_t *entry = &cache->entries[i];
        
        if(!entry->valid) continue;
        
        if(compare_entry(entry, type, w, h, dsc_data, dsc_size) == 0) {
            /* Cache hit */
            entry->last_used_frame = cache->frame;
            *out_handle = entry->handle;
            *out_stride = entry->eve_stride;
            
            LV_LOG_TRACE("EVE5: SW cache hit, type=%d %dx%d", type, w, h);
            return true;
        }
    }
    
    return false;
}

/**
 * Insert a SW-rendered texture into the cache.
 * 
 * @param u         Draw unit (for eviction/allocation)
 * @param type      Task type
 * @param w         Texture width
 * @param h         Texture height
 * @param dsc_data  Descriptor data to copy (after base)
 * @param dsc_size  Size of descriptor data
 * @param handle    GPU handle to store
 * @param eve_stride EVE stride of the texture
 */
void lv_draw_eve5_sw_cache_insert(lv_draw_eve5_unit_t *u,
                                   lv_draw_task_type_t type,
                                   int32_t w, int32_t h,
                                   const void *dsc_data, uint32_t dsc_size,
                                   Esd_GpuHandle handle,
                                   uint32_t eve_stride)
{
    lv_draw_eve5_sw_cache_t *cache = &u->sw_cache;
    
    /* Find free slot or evict LRU */
    lv_draw_eve5_sw_cache_entry_t *entry = NULL;
    
    for(uint32_t i = 0; i < EVE5_SW_CACHE_CAPACITY; i++) {
        if(!cache->entries[i].valid) {
            entry = &cache->entries[i];
            break;
        }
    }
    
    if(entry == NULL) {
        /* Evict LRU entry */
        entry = find_lru_entry(cache);
        if(entry) {
            LV_LOG_TRACE("EVE5: SW cache evicting entry type=%d", entry->type);
            free_entry(u, entry);
        }
        else {
            LV_LOG_WARN("EVE5: SW cache full, cannot insert");
            return;
        }
    }
    
    /* Allocate and copy descriptor data */
    entry->dsc_data = lv_malloc(dsc_size);
    if(!entry->dsc_data) {
        LV_LOG_ERROR("EVE5: Failed to allocate SW cache descriptor copy");
        return;
    }
    lv_memcpy(entry->dsc_data, dsc_data, dsc_size);
    
    entry->dsc_size = dsc_size;
    entry->type = type;
    entry->w = w;
    entry->h = h;
    entry->handle = handle;
    entry->eve_stride = eve_stride;
    entry->last_used_frame = cache->frame;
    entry->valid = true;
    
    LV_LOG_INFO("EVE5: SW cache insert type=%d %dx%d stride=%u",
                type, w, h, eve_stride);
}

/**
 * Drop a specific entry from the cache (e.g., for non-static content).
 */
void lv_draw_eve5_sw_cache_drop(lv_draw_eve5_unit_t *u,
                                 lv_draw_task_type_t type,
                                 int32_t w, int32_t h,
                                 const void *dsc_data, uint32_t dsc_size)
{
    lv_draw_eve5_sw_cache_t *cache = &u->sw_cache;
    
    for(uint32_t i = 0; i < EVE5_SW_CACHE_CAPACITY; i++) {
        lv_draw_eve5_sw_cache_entry_t *entry = &cache->entries[i];
        
        if(!entry->valid) continue;
        
        if(compare_entry(entry, type, w, h, dsc_data, dsc_size) == 0) {
            free_entry(u, entry);
            return;
        }
    }
}

/**********************
 * STATIC FUNCTIONS
 **********************/

static int compare_entry(const lv_draw_eve5_sw_cache_entry_t *entry,
                         lv_draw_task_type_t type,
                         int32_t w, int32_t h,
                         const void *dsc_data, uint32_t dsc_size)
{
    if(entry->type != type) return (entry->type > type) ? 1 : -1;
    if(entry->w != w) return (entry->w > w) ? 1 : -1;
    if(entry->h != h) return (entry->h > h) ? 1 : -1;
    if(entry->dsc_size != dsc_size) return (entry->dsc_size > dsc_size) ? 1 : -1;
    
    return lv_memcmp(entry->dsc_data, dsc_data, dsc_size);
}

static lv_draw_eve5_sw_cache_entry_t *find_lru_entry(lv_draw_eve5_sw_cache_t *cache)
{
    lv_draw_eve5_sw_cache_entry_t *lru = NULL;
    uint32_t oldest_frame = UINT32_MAX;
    
    for(uint32_t i = 0; i < EVE5_SW_CACHE_CAPACITY; i++) {
        if(cache->entries[i].valid && cache->entries[i].last_used_frame < oldest_frame) {
            oldest_frame = cache->entries[i].last_used_frame;
            lru = &cache->entries[i];
        }
    }
    
    return lru;
}

static void free_entry(lv_draw_eve5_unit_t *u, lv_draw_eve5_sw_cache_entry_t *entry)
{
    if(!entry->valid) return;
    
    if(entry->dsc_data) {
        lv_free(entry->dsc_data);
        entry->dsc_data = NULL;
    }
    
    if(entry->handle.Id != GA_HANDLE_INVALID.Id) {
        Esd_GpuAlloc_Free(u->allocator, entry->handle);
    }
    
    entry->valid = false;
}

#endif /* LV_USE_DRAW_EVE5 */
