/**
 * @file lv_draw_eve5_residency.c
 *
 * EVE5 (BT820) Residency: the draw unit's record of its EVE copies
 *
 * With LV_USE_DRAW_VRAM, LVGL attaches the EVE copy of a buffer to the buffer
 * (vram_res), tells the draw unit when it destroys the buffer, and moves the
 * pixels between CPU memory and EVE memory when a draw unit or the CPU needs
 * them. The draw unit keeps a list of the copies it attached, as LVGL never
 * destroys some buffers (static ones, image descriptors in the program): it
 * releases those when it's deleted (lv_deinit).
 *
 * Without it, the buffers are LVGL's own, in CPU memory, and the draw unit
 * records the EVE copies itself, by the address of what they copy:
 *
 * - A child layer has no buffer: its render target is recorded by the layer,
 *   and freed when LVGL deletes the layer (LV_EVENT_CHILD_DELETED).
 * - A layer with a buffer, a canvas's or a snapshot's, renders on an EVE copy
 *   of the buffer's pixels, uploaded unless the copy is known to be current.
 *   LVGL asks for the result when the layer is done (LV_EVENT_SCREEN_LOAD_START),
 *   which reads it back into the buffer. The EVE copy is kept, as the image
 *   of the buffer.
 * - An image in CPU memory is uploaded when drawn, and kept while it's
 *   unchanged. LVGL's image cache drops an image it's told changed
 *   (lv_image_cache_drop, LV_EVENT_INVALIDATE_AREA). Nothing tells about the
 *   CPU writing into a buffer, such as a canvas's pixels: those are compared
 *   with a hash of the pixels the copy was made from, once per refresh. An
 *   image in the program (no handlers, neither allocated nor modifiable) is
 *   taken as constant.
 * - An image the decoders read, a file or encoded data, is decoded into EVE
 *   memory, by the hardware or from the pixels a software decoder gives,
 *   which aren't kept. It's recorded by the file's path or by the data.
 * - The display's buffers, which the CPU doesn't render the frame into, are
 *   recorded by the display driver (the swapchain) or at dispatch (the tile
 *   of the frame, which the display takes when it's flushed).
 *
 * Images are released, least recently used first, when more than
 * LV_DRAW_EVE5_RES_MAX are recorded, and their EVE memory is evictable, since
 * they can be uploaded or decoded again.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5 && !LV_USE_DRAW_VRAM

/**********************
 * TYPEDEFS
 **********************/

struct lv_draw_eve5_res_entry_t {
    lv_draw_eve5_res_entry_t * hash_next;
    lv_draw_eve5_res_entry_t * lru_prev;
    lv_draw_eve5_res_entry_t * lru_next;
    const void * key;           /**< The layer, buffer, image, or encoded image data */
    char * path;                /**< The key of a file, its own copy of the path */
    lv_eve5_vram_res_t * vr;
    lv_draw_eve5_res_kind_t kind;
    uint32_t used;              /**< res_render it was last used in */

    /* A BUFFER as it was when its EVE copy was last known to be current */
    lv_image_header_t header;
    const void * data;
    uint32_t data_size;
    uint64_t hash;              /**< Of the pixels, when check_pixels */
    uint32_t checked;           /**< res_refresh the hash was last compared in */
    bool check_pixels;          /**< The CPU can change the pixels without notice */
};

/**********************
 * STATIC PROTOTYPES
 **********************/

static void entry_free(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e, bool free_vr);

/**********************
 * HASHING
 **********************/

static inline uint32_t key_bucket(const void * key)
{
    uintptr_t k = (uintptr_t)key;
    return (uint32_t)((k >> 4) ^ (k >> 12)) & (LV_DRAW_EVE5_RES_BUCKETS - 1);
}

static uint32_t path_bucket(const char * path)
{
    uint32_t h = 2166136261u;
    while(*path) {
        h = (h ^ (uint8_t)*path++) * 16777619u;
    }
    return h & (LV_DRAW_EVE5_RES_BUCKETS - 1);
}

/* FNV-1a over the pixels, a word at a time where aligned. Only tells whether
 * they changed: it's checked with the header, data pointer and size. */
static uint64_t pixels_hash(const uint8_t * p, uint32_t n, bool * zero)
{
    lv_draw_eve5_stats.hashes++;
    lv_draw_eve5_stats.hash_bytes += n;
    uint64_t h = 0xcbf29ce484222325ULL;
    uint32_t any = 0;
    uint32_t i = 0;
    if(((uintptr_t)p & 3) == 0) {
        const uint32_t * w = (const uint32_t *)p;
        uint32_t words = n / 4;
        for(; i < words; i++) {
            uint32_t v = w[i];
            any |= v;
            h = (h ^ v) * 0x100000001b3ULL;
        }
        i *= 4;
    }
    for(; i < n; i++) {
        any |= p[i];
        h = (h ^ p[i]) * 0x100000001b3ULL;
    }
    if(zero) *zero = any == 0;
    return h;
}

/* The bytes of an image's pixels, and palette or alpha plane: data_size can
 * be more than there is, when LVGL aligned the data of a buffer it was given */
static uint32_t pixels_size(const lv_image_dsc_t * img)
{
    const lv_image_header_t * header = &img->header;
    if(header->stride == 0) return img->data_size;
    uint32_t size = (uint32_t)header->stride * header->h;
    if(header->cf == LV_COLOR_FORMAT_RGB565A8) size += (uint32_t)(header->stride / 2) * header->h;
    else if(LV_COLOR_FORMAT_IS_INDEXED(header->cf)) size += LV_COLOR_INDEXED_PALETTE_SIZE(header->cf) * 4;
    return LV_MIN(size, img->data_size);
}

/* A buffer the program allocated or can write, a canvas's, a draw buffer, can
 * change in CPU memory without LVGL telling. An image descriptor in the
 * program is constant. */
static inline bool buffer_is_mutable(const lv_image_dsc_t * img)
{
    return (img->header.flags & (LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED)) != 0
           || ((const lv_draw_buf_t *)img)->handlers != NULL;
}

/**********************
 * TABLE
 **********************/

static lv_draw_eve5_res_entry_t * find(lv_draw_eve5_unit_t * u, const void * key)
{
    for(lv_draw_eve5_res_entry_t * e = u->res_buckets[key_bucket(key)]; e != NULL; e = e->hash_next) {
        if(e->key == key && e->path == NULL) return e;
    }
    return NULL;
}

static lv_draw_eve5_res_entry_t * find_path(lv_draw_eve5_unit_t * u, const char * path)
{
    for(lv_draw_eve5_res_entry_t * e = u->res_buckets[path_bucket(path)]; e != NULL; e = e->hash_next) {
        if(e->path != NULL && lv_strcmp(e->path, path) == 0) return e;
    }
    return NULL;
}

static void lru_unlink(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e)
{
    if(e->lru_prev) e->lru_prev->lru_next = e->lru_next;
    else u->res_lru_head = e->lru_next;
    if(e->lru_next) e->lru_next->lru_prev = e->lru_prev;
    else u->res_lru_tail = e->lru_prev;
    e->lru_prev = e->lru_next = NULL;
}

static void touch(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e)
{
    e->used = u->res_render;
    if(u->res_lru_head == e) return;
    lru_unlink(u, e);
    e->lru_next = u->res_lru_head;
    if(u->res_lru_head) u->res_lru_head->lru_prev = e;
    u->res_lru_head = e;
    if(u->res_lru_tail == NULL) u->res_lru_tail = e;
}

/* Release the least recently used images beyond LV_DRAW_EVE5_RES_MAX, but none
 * the render in progress uses */
static void enforce_limit(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_res_entry_t * e = u->res_lru_tail;
    while(u->res_count > LV_DRAW_EVE5_RES_MAX && e != NULL) {
        lv_draw_eve5_res_entry_t * prev = e->lru_prev;
        if((e->kind == LV_DRAW_EVE5_RES_BUFFER || e->kind == LV_DRAW_EVE5_RES_SOURCE) && e->used != u->res_render) {
            entry_free(u, e, true);
        }
        e = prev;
    }
}

static lv_draw_eve5_res_entry_t * entry_add(lv_draw_eve5_unit_t * u, const void * key, const char * path)
{
    lv_draw_eve5_res_entry_t * e = lv_malloc_zeroed(sizeof(lv_draw_eve5_res_entry_t));
    if(e == NULL) return NULL;
    uint32_t bucket;
    if(path != NULL) {
        size_t len = lv_strlen(path);
        e->path = lv_malloc(len + 1);
        if(e->path == NULL) {
            lv_free(e);
            return NULL;
        }
        lv_memcpy(e->path, path, len + 1);
        e->key = e->path;
        bucket = path_bucket(path);
    }
    else {
        e->key = key;
        bucket = key_bucket(key);
    }
    e->hash_next = u->res_buckets[bucket];
    u->res_buckets[bucket] = e;
    u->res_count++;
    touch(u, e);
    return e;
}

static void entry_free(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e, bool free_vr)
{
    lv_draw_eve5_res_entry_t ** link = &u->res_buckets[e->path ? path_bucket(e->path) : key_bucket(e->key)];
    while(*link != e) link = &(*link)->hash_next;
    *link = e->hash_next;
    lru_unlink(u, e);
    u->res_count--;

    /* The display's buffers are the display's */
    if(free_vr && e->vr != NULL && e->kind != LV_DRAW_EVE5_RES_TARGET) {
        /* ScopedFree: the texture may still be referenced by an in-flight
         * display list, and is reclaimed once it completes */
        EVE_GpuAlloc_ScopedFree(u->allocator, e->vr->gpu_handle);
        lv_free(e->vr);
    }
    lv_free(e->path);
    lv_free(e);
}

/* Note the buffer as it is now, its EVE copy being current */
static void commit(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e, const lv_image_dsc_t * img)
{
    e->header = img->header;
    e->data = img->data;
    e->data_size = img->data_size;
    e->check_pixels = buffer_is_mutable(img) && img->data != NULL;
    if(e->check_pixels) e->hash = pixels_hash(img->data, pixels_size(img), NULL);
    e->checked = u->res_refresh;
}

/* Whether the EVE copy of a buffer is still a copy of it */
static bool is_current(lv_draw_eve5_unit_t * u, lv_draw_eve5_res_entry_t * e, const lv_image_dsc_t * img)
{
    if(e->data != img->data || e->data_size != img->data_size) return false;
    if(lv_memcmp(&e->header, &img->header, sizeof(lv_image_header_t)) != 0) return false;
    if(!e->check_pixels) return true;
    /* Once per refresh: nothing runs between the tiles of a refresh to change
     * the pixels. Outside a refresh, such as while a canvas is drawn on, always. */
    if((u->res_refresh & 1) && e->checked == u->res_refresh) return true;
    if(pixels_hash(img->data, pixels_size(img), NULL) != e->hash) return false;
    e->checked = u->res_refresh;
    return true;
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

lv_eve5_vram_res_t * lv_draw_eve5_res_get(lv_draw_eve5_unit_t * u, const void * key)
{
    lv_draw_eve5_res_entry_t * e = find(u, key);
    if(e == NULL) return NULL;
    touch(u, e);
    return e->vr;
}

void lv_draw_eve5_res_set_kind(lv_draw_eve5_unit_t * u, const void * key, lv_eve5_vram_res_t * vr,
                               lv_draw_eve5_res_kind_t kind)
{
    lv_draw_eve5_res_entry_t * e = find(u, key);
    if(vr == NULL) {
        if(e != NULL) entry_free(u, e, false);
        return;
    }
    if(e == NULL) {
        e = entry_add(u, key, NULL);
        if(e == NULL) {
            LV_LOG_WARN("EVE5: Out of memory for a residency, freeing it");
            EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
            lv_free(vr);
            return;
        }
    }
    else {
        touch(u, e);
    }
    e->vr = vr;
    e->kind = kind;
    if(kind == LV_DRAW_EVE5_RES_BUFFER) commit(u, e, key);
    enforce_limit(u);
}

lv_eve5_vram_res_t * lv_draw_eve5_res_detach(lv_draw_eve5_unit_t * u, const void * key)
{
    lv_draw_eve5_res_entry_t * e = find(u, key);
    if(e == NULL) return NULL;
    lv_eve5_vram_res_t * vr = e->vr;
    entry_free(u, e, false);
    return vr;
}

void lv_draw_eve5_res_free(lv_draw_eve5_unit_t * u, const void * key)
{
    lv_draw_eve5_res_entry_t * e = find(u, key);
    if(e != NULL) entry_free(u, e, true);
}

lv_eve5_vram_res_t * lv_draw_eve5_res_image(lv_draw_eve5_unit_t * u, const lv_image_dsc_t * img)
{
    lv_draw_eve5_res_entry_t * e = find(u, img);
    if(e == NULL) return NULL;
    if(e->kind == LV_DRAW_EVE5_RES_BUFFER && !is_current(u, e, img)) {
        entry_free(u, e, true);
        return NULL;
    }
    touch(u, e);
    return e->vr;
}

void lv_draw_eve5_res_commit(lv_draw_eve5_unit_t * u, const lv_draw_buf_t * buf)
{
    lv_draw_eve5_res_entry_t * e = find(u, buf);
    if(e != NULL && e->kind == LV_DRAW_EVE5_RES_BUFFER) commit(u, e, (const lv_image_dsc_t *)buf);
}

lv_eve5_vram_res_t * lv_draw_eve5_res_source(lv_draw_eve5_unit_t * u, const void * src)
{
    lv_image_src_t type = lv_image_src_get_type(src);
    lv_draw_eve5_res_entry_t * e;
    if(type == LV_IMAGE_SRC_FILE) {
        e = find_path(u, src);
    }
    else if(type == LV_IMAGE_SRC_VARIABLE) {
        e = find(u, src);
        /* Encoded data is constant; another image at its address differs */
        const lv_image_dsc_t * img = src;
        if(e != NULL && e->kind == LV_DRAW_EVE5_RES_SOURCE
           && (e->data != img->data || e->data_size != img->data_size
               || lv_memcmp(&e->header, &img->header, sizeof(lv_image_header_t)) != 0)) {
            entry_free(u, e, true);
            e = NULL;
        }
    }
    else {
        return NULL;
    }
    if(e == NULL) return NULL;
    touch(u, e);
    return e->vr;
}

bool lv_draw_eve5_res_set_source(lv_draw_eve5_unit_t * u, const void * src, lv_eve5_vram_res_t * vr)
{
    lv_image_src_t type = lv_image_src_get_type(src);
    lv_draw_eve5_res_entry_t * e;
    if(type == LV_IMAGE_SRC_FILE) {
        e = find_path(u, src);
        if(e == NULL) e = entry_add(u, NULL, src);
    }
    else if(type == LV_IMAGE_SRC_VARIABLE) {
        e = find(u, src);
        if(e == NULL) e = entry_add(u, src, NULL);
        if(e != NULL) {
            const lv_image_dsc_t * img = src;
            e->header = img->header;
            e->data = img->data;
            e->data_size = img->data_size;
        }
    }
    else {
        e = NULL;
    }
    if(e == NULL) {
        LV_LOG_WARN("EVE5: No residency for a decoded image, freeing it");
        EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
        lv_free(vr);
        return false;
    }
    if(e->vr != NULL && e->vr != vr) {
        EVE_GpuAlloc_ScopedFree(u->allocator, e->vr->gpu_handle);
        lv_free(e->vr);
    }
    e->vr = vr;
    e->kind = LV_DRAW_EVE5_RES_SOURCE;
    touch(u, e);
    enforce_limit(u);
    return true;
}

bool lv_draw_eve5_res_prepare_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen)
{
    const void * key = eve5_layer_key(layer);
    lv_draw_buf_t * buf = layer->draw_buf;

    /* A new render: what it uses is kept until the next */
    u->res_render++;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
#endif

    lv_draw_eve5_res_entry_t * e = find(u, key);
    bool ok = true;
    if(is_screen || buf == NULL) {
        /* The swapchain, a tile of the frame, or a child layer: EVE memory
         * the layer renders to, which only the draw unit writes */
        if(e != NULL && e->vr != NULL) {
            touch(u, e);
        }
        else {
            uint32_t w = buf ? buf->header.w : (uint32_t)lv_area_get_width(&layer->buf_area);
            uint32_t h = buf ? buf->header.h : (uint32_t)lv_area_get_height(&layer->buf_area);
            lv_color_format_t cf = buf ? (lv_color_format_t)buf->header.cf : layer->color_format;
            lv_eve5_vram_res_t * vr = lv_draw_eve5_vram_create(u, w, h, cf, GA_ALIGN_128);
            if(vr != NULL) {
                lv_draw_eve5_res_set_kind(u, key, vr, buf ? LV_DRAW_EVE5_RES_TARGET : LV_DRAW_EVE5_RES_LAYER);
            }
            ok = vr != NULL;
        }
    }
    else if(e == NULL || e->vr == NULL || EVE_GpuAlloc_Get(u->allocator, e->vr->gpu_handle) == GA_INVALID
            || !is_current(u, e, (const lv_image_dsc_t *)buf)) {
        /* A canvas or a snapshot: the layer draws on the buffer's pixels,
         * unless they're all zero, as a buffer that was cleared is */
        if(e != NULL) entry_free(u, e, true);
        bool zero = true;
        if(buf->data != NULL) pixels_hash(buf->data, pixels_size((const lv_image_dsc_t *)buf), &zero);
        lv_eve5_vram_res_t * vr;
        if(zero) {
            vr = lv_draw_eve5_vram_create(u, buf->header.w, buf->header.h, (lv_color_format_t)buf->header.cf,
                                          GA_ALIGN_128);
            if(vr != NULL) lv_draw_eve5_res_set_kind(u, buf, vr, LV_DRAW_EVE5_RES_BUFFER);
        }
        else {
            vr = lv_draw_eve5_upload_image_to_gpu_ex(u, (lv_image_dsc_t *)buf, true, false);
        }
        ok = vr != NULL;
    }
    else {
        touch(u, e);
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif
    return ok;
}

/* The layer is done: read its result back into its buffer, which the
 * program owns. Its EVE copy stays, the buffer's image. */
static void read_back(lv_draw_eve5_unit_t * u, lv_layer_t * layer)
{
    lv_draw_buf_t * buf = layer->draw_buf;
    if(layer->parent != NULL || buf == NULL || buf->data == NULL) return;
    lv_draw_eve5_res_entry_t * e = find(u, buf);
    if(e == NULL || e->kind != LV_DRAW_EVE5_RES_BUFFER || e->vr == NULL || !e->vr->has_content) return;

    /* Render target writes may still be in flight */
    EVE_Cmd_waitFlush(u->hal);
    if(!lv_draw_eve5_download_image(u, buf, e->vr)) {
        LV_LOG_WARN("EVE5: Couldn't read a layer back into its buffer");
        return;
    }
    commit(u, e, (const lv_image_dsc_t *)buf);

    /* The CPU has the pixels too: the EVE copy can be uploaded again */
    EVE_GpuAlloc_UpdateFlags(u->allocator, e->vr->gpu_handle, GA_GC_FLAG, GA_GC_FLAG);
}

/* LVGL's image cache was told an image changed, or all of them with NULL */
static void drop(lv_draw_eve5_unit_t * u, const void * src)
{
    if(src == NULL) {
        for(uint32_t i = 0; i < LV_DRAW_EVE5_RES_BUCKETS; i++) {
            lv_draw_eve5_res_entry_t * e = u->res_buckets[i];
            while(e != NULL) {
                lv_draw_eve5_res_entry_t * next = e->hash_next;
                if(e->kind == LV_DRAW_EVE5_RES_BUFFER || e->kind == LV_DRAW_EVE5_RES_SOURCE) entry_free(u, e, true);
                e = next;
            }
        }
        return;
    }

    lv_image_src_t type = lv_image_src_get_type(src);
    lv_draw_eve5_res_entry_t * e = NULL;
    if(type == LV_IMAGE_SRC_FILE) e = find_path(u, src);
    else if(type == LV_IMAGE_SRC_VARIABLE) e = find(u, src);
    if(e != NULL && (e->kind == LV_DRAW_EVE5_RES_BUFFER || e->kind == LV_DRAW_EVE5_RES_SOURCE)) entry_free(u, e, true);
}

void lv_draw_eve5_res_event(lv_event_t * event)
{
    lv_draw_eve5_unit_t * u = lv_event_get_current_target(event);
    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
    LV_UNUSED(disp);

    switch(lv_event_get_code(event)) {
        case LV_EVENT_CHILD_DELETED: {
                /* A layer's render target, or the upload of a child layer
                 * the software renderer drew (non render-target chips). A
                 * canvas's or a snapshot's stays the image of its buffer. */
                lv_layer_t * layer = lv_event_get_param(event);
                if(layer == NULL || layer->parent == NULL) break;
#if LV_USE_OS
                lv_eve5_hal_lock(disp);
#endif
                lv_draw_eve5_res_free(u, layer);
                if(layer->draw_buf != NULL) lv_draw_eve5_res_free(u, layer->draw_buf);
#if LV_USE_OS
                lv_eve5_hal_unlock(disp);
#endif
                break;
            }
        case LV_EVENT_SCREEN_LOAD_START: {
                lv_layer_t * layer = lv_event_get_param(event);
                if(layer == NULL) break;
#if LV_USE_OS
                lv_eve5_hal_lock(disp);
#endif
                read_back(u, layer);
#if LV_USE_OS
                lv_eve5_hal_unlock(disp);
#endif
                break;
            }
        case LV_EVENT_INVALIDATE_AREA:
#if LV_USE_OS
            lv_eve5_hal_lock(disp);
#endif
            drop(u, lv_event_get_param(event));
#if LV_USE_OS
            lv_eve5_hal_unlock(disp);
#endif
            break;
        default:
            break;
    }
}

void lv_draw_eve5_res_refresh_event(lv_event_t * e)
{
    lv_draw_eve5_unit_t * u = lv_event_get_user_data(e);
    /* Odd from LV_EVENT_REFR_START to LV_EVENT_REFR_READY */
    u->res_refresh++;
}

void lv_draw_eve5_res_attach_cb(lv_draw_unit_t * draw_unit, const void * key, lv_eve5_vram_res_t * vr)
{
    lv_draw_eve5_res_set_kind((lv_draw_eve5_unit_t *)draw_unit, key, vr, LV_DRAW_EVE5_RES_TARGET);
}

lv_eve5_vram_res_t * lv_draw_eve5_res_detach_cb(lv_draw_unit_t * draw_unit, const void * key)
{
    return lv_draw_eve5_res_detach((lv_draw_eve5_unit_t *)draw_unit, key);
}

void lv_draw_eve5_res_deinit(lv_draw_eve5_unit_t * u)
{
    for(uint32_t i = 0; i < LV_DRAW_EVE5_RES_BUCKETS; i++) {
        while(u->res_buckets[i] != NULL) entry_free(u, u->res_buckets[i], true);
    }
    lv_draw_eve5_font_free_all(u);
}

#endif /* LV_USE_DRAW_EVE5 && !LV_USE_DRAW_VRAM */

#if LV_USE_DRAW_EVE5 && LV_USE_DRAW_VRAM

/* Take vr out of the list of residencies attached to buffers */
static void owned_remove(lv_eve5_vram_res_t * vr)
{
    if(vr->owner == NULL) return;
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)vr->base.unit;
    if(vr->owner_prev != NULL) vr->owner_prev->owner_next = vr->owner_next;
    else u->owned_list = vr->owner_next;
    if(vr->owner_next != NULL) vr->owner_next->owner_prev = vr->owner_prev;
    vr->owner = NULL;
    vr->owner_prev = NULL;
    vr->owner_next = NULL;
}

void lv_draw_eve5_res_set(lv_draw_eve5_unit_t * u, const void * buf, lv_eve5_vram_res_t * vr)
{
    lv_draw_buf_t * b = (lv_draw_buf_t *)buf;
    lv_eve5_vram_res_t * old = (lv_eve5_vram_res_t *)b->vram_res;
    /* A copy of another buffer's descriptor can hold the other's residency */
    if(old != NULL && old != vr && old->owner == b) owned_remove(old);
    b->vram_res = (lv_draw_buf_vram_res_t *)vr;
    if(vr == NULL) return;
    if(vr->owner == NULL) {
        vr->owner_prev = NULL;
        vr->owner_next = u->owned_list;
        if(u->owned_list != NULL) u->owned_list->owner_prev = vr;
        u->owned_list = vr;
    }
    vr->owner = b;
}

void lv_draw_eve5_res_destroy(lv_eve5_vram_res_t * vr)
{
    if(vr == NULL) return;
    owned_remove(vr);
    lv_free(vr);
}

void lv_draw_eve5_res_keep_for_layer(lv_draw_eve5_unit_t * u, const void * buf, lv_eve5_vram_res_t * vr)
{
    lv_draw_eve5_res_set(u, buf, NULL);
    owned_remove(vr);
    vr->owner_next = u->layer_list;
    u->layer_list = vr;
}

void lv_draw_eve5_res_release_layer(lv_draw_eve5_unit_t * u)
{
    while(u->layer_list != NULL) {
        lv_eve5_vram_res_t * vr = u->layer_list;
        u->layer_list = vr->owner_next;
        /* ScopedFree: the layer's display lists may still be running */
        EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
        lv_free(vr);
    }
}

void lv_draw_eve5_res_deinit(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_res_release_layer(u);
    lv_eve5_vram_res_t * vr = u->owned_list;
    while(vr != NULL) {
        lv_eve5_vram_res_t * next = vr->owner_next;
        if(vr->owner->vram_res == (lv_draw_buf_vram_res_t *)vr) {
            vr->owner->vram_res = NULL;
            EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
            lv_free(vr);
        }
        else {
            /* Left as it is: the buffer was dropped or written anew without
             * releasing it (lv_draw_buf_release_vram), and a copy of its
             * descriptor may still hold it */
            LV_LOG_WARN("EVE5: buffer %p lost its residency %p without releasing it",
                        (void *)vr->owner, (void *)vr);
        }
        vr = next;
    }
    u->owned_list = NULL;
    lv_draw_eve5_font_free_all(u);
}

#endif /* LV_USE_DRAW_EVE5 && LV_USE_DRAW_VRAM */
