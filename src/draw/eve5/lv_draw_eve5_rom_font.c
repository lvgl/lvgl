/**
 * @file lv_draw_eve5_rom_font.c
 *
 * Bitmap handle LRU pool + EVE ROM font handle cache.
 *
 * The handle pool tracks ownership of every bitmap handle. Each slot stores
 * an owner pointer plus prev/next links into a doubly-linked LRU list.
 * Reserved slots (CO scratch, slots above the runtime BITMAP_HANDLE_MASK on
 * smaller chips) are excluded from the list and never allocated. Allocation
 * is either preferred (force-take a specific handle, evicting current owner)
 * or LRU (pop the tail). Both move the slot to the MRU head and overwrite
 * owner. Eviction detection is pointer-equality on the owner field — the
 * previous holder's handle_check returns false the moment a new owner is
 * recorded.
 *
 * Each rom_idx maps to a bitmap handle through this pool. Owner identity
 * is the address of the matching rom_font_slots entry — stable per rom_idx,
 * distinct from any caller a bitmap font might use as owner. The slot
 * itself caches the most recently allocated handle so repeated resolve()
 * calls fast-path through handle_check + handle_touch.
 *
 * Range macros LV_DRAW_EVE5_ROM_FONT_{MIN,CAP,NBCAP} mirror ESD's pattern.
 * MIN inclusive, CAP/MAX exclusive. CAP is compile-time (sizes the slot
 * array); MAX is runtime (== CAP on single-target builds, runtime via
 * EVE_Hal_supportLargeFont in MULTI builds).
 *
 * Native vs non-native rom_idx:
 *   native: rom_idx <= BITMAP_HANDLE_MASK — force-bind handle == rom_idx
 *           (BT820: all of 16..34; FT81X-class: 16..31).
 *   non-native: rom_idx > BITMAP_HANDLE_MASK — LRU-allocate any free handle
 *               (FT81X-class only, rom indices 32..34). Each switch among
 *               32..34 issues a CMD_ROMFONT re-bind.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"
#include "../../drivers/display/eve5/lv_eve5_asset_font.h"

#if LV_USE_DRAW_EVE5

/* Asset-font draw-time state hooks. Declared locally to keep the dsc-
 * shape out of the public header — these are coupled to the draw unit's
 * resolve, not part of the application-facing asset font API. */
void * lv_eve5_asset_font_get_owner_internal(const lv_font_t * font);
uint8_t lv_eve5_asset_font_get_cached_handle_internal(const lv_font_t * font);
void lv_eve5_asset_font_set_cached_handle_internal(const lv_font_t * font, uint8_t handle);
uint32_t lv_eve5_asset_font_get_last_bound_addr_internal(const lv_font_t * font);
void lv_eve5_asset_font_set_last_bound_addr_internal(const lv_font_t * font, uint32_t addr);

/*********************
 * HANDLE POOL
 *********************/

static inline void lru_remove(lv_draw_eve5_handle_pool_t * pool, uint8_t h)
{
    lv_draw_eve5_handle_slot_t * s = &pool->slots[h];
    uint8_t p = s->lru_prev;
    uint8_t n = s->lru_next;
    if(p != 0xFFu) pool->slots[p].lru_next = n;
    else pool->lru_head = n;
    if(n != 0xFFu) pool->slots[n].lru_prev = p;
    else pool->lru_tail = p;
    s->lru_prev = 0xFFu;
    s->lru_next = 0xFFu;
}

static inline void lru_push_head(lv_draw_eve5_handle_pool_t * pool, uint8_t h)
{
    lv_draw_eve5_handle_slot_t * s = &pool->slots[h];
    s->lru_prev = 0xFFu;
    s->lru_next = pool->lru_head;
    if(pool->lru_head != 0xFFu) pool->slots[pool->lru_head].lru_prev = h;
    else pool->lru_tail = h;
    pool->lru_head = h;
}

void lv_draw_eve5_handle_pool_init(lv_draw_eve5_unit_t * u)
{
    EVE_HalContext * phost = u->hal;
    lv_draw_eve5_handle_pool_t * pool = &u->handle_pool;

    /* Runtime handle count. On MULTI builds BITMAP_HANDLE_MASK is read from
     * phost; on single-target it's a compile-time constant. Slots above
     * `cap` are permanently reserved. */
    uint8_t cap = (uint8_t)(BITMAP_HANDLE_MASK + 1u);
    if(cap > LV_DRAW_EVE5_HANDLE_CAP) cap = LV_DRAW_EVE5_HANDLE_CAP;

    for(uint8_t i = 0; i < LV_DRAW_EVE5_HANDLE_CAP; i++) {
        pool->slots[i].owner = NULL;
        pool->slots[i].lru_prev = 0xFFu;
        pool->slots[i].lru_next = 0xFFu;
        pool->slots[i].reserved = (i >= cap);
    }

    /* CO scratch: coprocessor commands (CMD_TEXT, CMD_BUTTON, ...) trash
     * this handle, so no caller may claim ownership of it. */
    uint8_t scratch = (uint8_t)EVE_CO_SCRATCH_HANDLE;
    if(scratch < cap) pool->slots[scratch].reserved = true;

    /* Build the LRU list across non-reserved slots. Initial order is
     * ascending by index; all entries are unowned so any ordering is fine. */
    pool->lru_head = 0xFFu;
    pool->lru_tail = 0xFFu;
    for(uint8_t i = 0; i < cap; i++) {
        if(pool->slots[i].reserved) continue;
        if(pool->lru_head == 0xFFu) {
            pool->lru_head = i;
        }
        else {
            pool->slots[pool->lru_tail].lru_next = i;
            pool->slots[i].lru_prev = pool->lru_tail;
        }
        pool->lru_tail = i;
    }
}

uint8_t lv_draw_eve5_handle_alloc(lv_draw_eve5_unit_t * u, void * owner, uint8_t preferred)
{
    lv_draw_eve5_handle_pool_t * pool = &u->handle_pool;

    uint8_t h;
    if(preferred < LV_DRAW_EVE5_HANDLE_CAP && !pool->slots[preferred].reserved) {
        h = preferred;
    }
    else {
        h = pool->lru_tail;
        if(h == 0xFFu) return 0xFFu; /* All handles reserved — should not happen. */
    }

    pool->slots[h].owner = owner;
    lru_remove(pool, h);
    lru_push_head(pool, h);
    return h;
}

void lv_draw_eve5_handle_touch(lv_draw_eve5_unit_t * u, uint8_t handle)
{
    lv_draw_eve5_handle_pool_t * pool = &u->handle_pool;
    if(handle >= LV_DRAW_EVE5_HANDLE_CAP) return;
    if(pool->slots[handle].reserved) return;
    lru_remove(pool, handle);
    lru_push_head(pool, handle);
}

bool lv_draw_eve5_handle_check(const lv_draw_eve5_unit_t * u, uint8_t handle, const void * owner)
{
    if(handle >= LV_DRAW_EVE5_HANDLE_CAP) return false;
    if(u->handle_pool.slots[handle].reserved) return false;
    return u->handle_pool.slots[handle].owner == owner;
}

void lv_draw_eve5_handle_invalidate_all(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_handle_pool_t * pool = &u->handle_pool;
    for(uint8_t i = 0; i < LV_DRAW_EVE5_HANDLE_CAP; i++) {
        if(!pool->slots[i].reserved) {
            pool->slots[i].owner = NULL;
        }
    }
}

/*********************
 * ROM FONT CACHE
 *********************/

static inline bool rom_idx_is_native_handle(EVE_HalContext * phost, uint8_t rom_idx)
{
    LV_UNUSED(phost);
    return rom_idx <= BITMAP_HANDLE_MASK;
}

static inline void * rom_font_owner(lv_draw_eve5_unit_t * u, uint8_t rom_idx)
{
    return &u->rom_font_slots[rom_idx - LV_DRAW_EVE5_ROM_FONT_MIN];
}

void lv_draw_eve5_rom_font_init(lv_draw_eve5_unit_t * u)
{
    EVE_HalContext * phost = u->hal;

    for(uint8_t i = 0; i < LV_DRAW_EVE5_ROM_FONT_NBCAP; i++) {
        u->rom_font_slots[i].handle = 0xFFu;
    }

    /* Pre-bind native rom fonts to their identity handles. CMD_ROMFONT is
     * FT810+ (EVE2+); on FT800 the 16..31 mapping is hardwired. Non-native
     * indices bind lazily in resolve(). */
    uint8_t max_excl = lv_draw_eve5_rom_font_max(u->hal);
    for(uint8_t f = LV_DRAW_EVE5_ROM_FONT_MIN; f < max_excl; f++) {
        if(!rom_idx_is_native_handle(u->hal, f)) continue;
        uint8_t h = lv_draw_eve5_handle_alloc(u, rom_font_owner(u, f), f);
        if(EVE_GEN >= EVE2) {
            EVE_CoCmd_romFont(u->hal, h, f);
        }
        u->rom_font_slots[f - LV_DRAW_EVE5_ROM_FONT_MIN].handle = h;
    }
}

uint8_t lv_draw_eve5_rom_font_resolve(lv_draw_eve5_unit_t * u, uint8_t rom_idx)
{
    EVE_HalContext * phost = u->hal;

    /* Compile-time bound first so the slot array access can't overrun on
     * builds where CAP is 32 (no LARGEFONT support). */
    if(rom_idx < LV_DRAW_EVE5_ROM_FONT_MIN || rom_idx >= LV_DRAW_EVE5_ROM_FONT_CAP) {
        return 0xFFu;
    }
    /* Runtime bound covers MULTI builds where CAP=35 but the actual chip
     * may not ship large fonts. */
    if(rom_idx >= lv_draw_eve5_rom_font_max(u->hal)) {
        return 0xFFu;
    }

    lv_draw_eve5_rom_font_slot_t * slot = &u->rom_font_slots[rom_idx - LV_DRAW_EVE5_ROM_FONT_MIN];
    void * owner = rom_font_owner(u, rom_idx);

    /* Fast path: still own our previously-allocated handle. */
    if(slot->handle != 0xFFu && lv_draw_eve5_handle_check(u, slot->handle, owner)) {
        lv_draw_eve5_handle_touch(u, slot->handle);
        return slot->handle;
    }

    /* (Re-)bind. Native rom_idx forces identity; non-native LRU-allocates. */
    uint8_t preferred = rom_idx_is_native_handle(u->hal, rom_idx) ? rom_idx : 0xFFu;
    uint8_t h = lv_draw_eve5_handle_alloc(u, owner, preferred);
    if(EVE_GEN >= EVE2) {
        EVE_CoCmd_romFont(u->hal, h, rom_idx);
    }
    slot->handle = h;
    return h;
}

void lv_draw_eve5_rom_font_invalidate(lv_draw_eve5_unit_t * u)
{
    /* Coprocessor reset: every binding is gone. The handle pool's owner
     * slots are cleared too so previous holders' handle_check returns false. */
    lv_draw_eve5_handle_invalidate_all(u);
    for(uint8_t i = 0; i < LV_DRAW_EVE5_ROM_FONT_NBCAP; i++) {
        u->rom_font_slots[i].handle = 0xFFu;
    }
}

/*********************
 * ASSET FONT BINDING
 *
 * Asset fonts share the bitmap handle pool with ROM fonts. The asset
 * font's dsc pointer is the stable owner identity; the per-dsc cached
 * handle field is the fast-path slot (queried via
 * lv_eve5_asset_font_get_cached_handle_internal). Allocation is always
 * non-preferred — asset fonts have no identity-handle constraint.
 *********************/

#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
uint8_t lv_draw_eve5_asset_font_resolve(lv_draw_eve5_unit_t * u, const lv_font_t * font)
{
    void * owner = lv_eve5_asset_font_get_owner_internal(font);
    if(owner == NULL) return 0xFFu;

    /* Always Esd_GpuAlloc_Get the current address — never cache the
     * address itself, only the value we last bound (for change detection).
     * Get is the canonical way to ask the allocator "where does this
     * handle live right now" and the address must come fresh every time. */
    uint32_t addr = lv_eve5_asset_font_get_address(font);
    if(addr == GA_INVALID) return 0xFFu;

    uint8_t cached = lv_eve5_asset_font_get_cached_handle_internal(font);
    uint32_t last_addr = lv_eve5_asset_font_get_last_bound_addr_internal(font);

    bool handle_held = (cached != 0xFFu && lv_draw_eve5_handle_check(u, cached, owner));

    /* Fast path: still own our handle AND its bind still points at the
     * current address. The CMD_SETFONT2 binding persists in the
     * coprocessor's font table as long as the handle isn't reassigned to
     * something else, so we skip the re-emit. Same persistence model
     * the rom-font path uses (rom font handles 16-34 have a hardware
     * default; asset fonts get it from the prior CMD_SETFONT2). */
    if(handle_held && addr == last_addr) {
        lv_draw_eve5_handle_touch(u, cached);
        return cached;
    }

    uint8_t h = handle_held
                ? cached
                : lv_draw_eve5_handle_alloc(u, owner, 0xFFu);
    if(h == 0xFFu) return 0xFFu;
    if(handle_held) lv_draw_eve5_handle_touch(u, h);

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    /* CMD_SETFONT2 binds (handle, font_block_addr, first_char). For
     * extended format 2 (.reloc default) first_char is 0. */
    EVE_CoCmd_setFont2(u->hal, h, addr, lv_eve5_asset_font_get_first_char(font));
#endif

    lv_eve5_asset_font_set_cached_handle_internal(font, h);
    lv_eve5_asset_font_set_last_bound_addr_internal(font, addr);
    return h;
}
#else
uint8_t lv_draw_eve5_asset_font_resolve(lv_draw_eve5_unit_t * u, const lv_font_t * font)
{
    LV_UNUSED(u);
    LV_UNUSED(font);
    return 0xFFu;
}
#endif

#endif /* LV_USE_DRAW_EVE5 */
