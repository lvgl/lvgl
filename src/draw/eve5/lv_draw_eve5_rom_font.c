/**
 * @file lv_draw_eve5_rom_font.c
 *
 * EVE ROM font handle cache.
 *
 * The firmware exposes built-in fonts at indices 16..(MAX-1):
 *   - 16..31 are universal (FT800+)
 *   - 32..34 are LARGEFONT (FT810+ excluding BT88X) — added unicode rom fonts
 *
 * CMD_TEXT accepts a bitmap handle argument; the firmware looks up the rom
 * font binding for that handle. Binding is established with
 * CMD_ROMFONT(handle, font_idx). To stay efficient we bind once at unit init
 * (handle == idx) and keep the binding across frames. Coprocessor resets
 * invalidate the firmware-side binding; consumers must call
 * lv_draw_eve5_rom_font_invalidate() so the next resolve re-emits
 * CMD_ROMFONT lazily.
 *
 * Range macros LV_DRAW_EVE5_ROM_FONT_{MIN,CAP,NBCAP} mirror ESD's pattern.
 * MIN inclusive, CAP/MAX exclusive. CAP is compile-time (sizes the slot
 * array), MAX is runtime (== CAP on single-target builds, runtime via
 * EVE_Hal_supportLargeFont in MULTI builds).
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

void lv_draw_eve5_rom_font_init(lv_draw_eve5_unit_t * u)
{
    lv_memzero(u->rom_font_slots, sizeof(u->rom_font_slots));
    u->rom_font_handle_mask = 0;
    u->rom_font_generation = 1;

    /* Pre-bind handles up to the runtime MAX so the first frame doesn't pay
     * setup cost. CMD_ROMFONT is FT810+ (EVE2+); on FT800 (EVE1) handles
     * 16..31 are pre-configured at boot and the command is unsupported, so
     * we just stamp the slot generation without emitting. */
    EVE_HalContext * phost = u->hal;
    uint8_t max_excl = lv_draw_eve5_rom_font_max(phost);

    for(uint8_t f = LV_DRAW_EVE5_ROM_FONT_MIN; f < max_excl; f++) {
        if(EVE_GEN >= EVE2) {
            EVE_CoCmd_romFont(phost, f, f);
        }
        lv_draw_eve5_rom_font_slot_t * slot = &u->rom_font_slots[f - LV_DRAW_EVE5_ROM_FONT_MIN];
        slot->handle = f;
        slot->generation = u->rom_font_generation;
        u->rom_font_handle_mask |= ((uint64_t)1 << f);
    }
}

uint8_t lv_draw_eve5_rom_font_resolve(lv_draw_eve5_unit_t * u, uint8_t rom_idx)
{
	EVE_HalContext * phost = u->hal;

    /* Compile-time bound first so the slot array access can't overrun on
     * builds where CAP is 32 (no LARGEFONT support). */
    if(rom_idx < LV_DRAW_EVE5_ROM_FONT_MIN || rom_idx >= LV_DRAW_EVE5_ROM_FONT_CAP) {
        return 0xFF;
    }
    /* Runtime bound covers MULTI builds where CAP=35 but the actual chip
     * may not ship large fonts. */
    if(rom_idx >= lv_draw_eve5_rom_font_max(u->hal)) {
        return 0xFF;
    }

    lv_draw_eve5_rom_font_slot_t * slot = &u->rom_font_slots[rom_idx - LV_DRAW_EVE5_ROM_FONT_MIN];
    if(slot->generation == u->rom_font_generation) {
        return slot->handle;
    }

    if(EVE_GEN >= EVE2) {
        EVE_CoCmd_romFont(u->hal, rom_idx, rom_idx);
    }
    slot->handle = rom_idx;
    slot->generation = u->rom_font_generation;
    u->rom_font_handle_mask |= ((uint64_t)1 << rom_idx);
    return rom_idx;
}

void lv_draw_eve5_rom_font_invalidate(lv_draw_eve5_unit_t * u)
{
    /* Bump generation, skipping 0 since 0 means "unbound" in slot.generation */
    u->rom_font_generation++;
    if(u->rom_font_generation == 0) u->rom_font_generation = 1;
    u->rom_font_handle_mask = 0;
}

#endif /* LV_USE_DRAW_EVE5 */
