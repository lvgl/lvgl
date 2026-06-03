/**
 * @file lv_draw_eve5_ring.c
 *
 * Pre-BT820 transient-allocation scratch ring.
 *
 * The EVE HAL ships two GPU allocators behind the EVE_GpuAlloc API. The
 * pre-BT820 variant caps the total number of live allocation handles at 64;
 * a frame with a couple of dozen gradients can blow past that on its own,
 * and a sustained UI burns through the slot count faster than the deferred-
 * free pipeline reclaims them. The BT820 variant (EVE_GpuAlloc5) is purpose-
 * built for high-count allocations on the chip's larger memory and isn't
 * subject to the same ceiling.
 *
 * On pre-BT820 chips we reserve one allocation handle up front for a fixed-
 * size scratch ring and serve an unbounded number of frame-local gradient
 * uploads from it.
 *
 * Lifetime: ring memory must remain stable for as long as the GPU is replaying
 * the display list that references it. CMD_SWAP rotates the active DL bank;
 * after the second swap following an allocation, the GPU can no longer see it.
 * We track three cursors:
 *
 *   safe_until   curr_start    write_abs
 *       |            |             |
 *       v            v             v
 *   ...|--displayed--|--building--|--free---|...   (linear/abs space)
 *
 *   - write_abs: where the next allocation lands.
 *   - curr_start: write_abs snapshot at the most recent CMD_SWAP — start of
 *     the frame that's now being scanned out.
 *   - safe_until: write_abs snapshot at the swap before that — content older
 *     than this is no longer referenced by any DL the GPU is replaying.
 *
 * Cursors are absolute (unwrapped) 32-bit counters; the in-ring offset is
 * (cursor % size). The free byte count is ring_size - (write_abs - safe_until)
 * and allocations fail once that delta would exceed ring_size.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

void lv_draw_eve5_ring_init(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_ring_t * r = &u->scratch_ring;
    r->handle = GA_HANDLE_INVALID;
    r->base = GA_INVALID;
    r->size = 0;
    r->write_abs = 0;
    r->curr_start = 0;
    r->safe_until = 0;

    /* BT820+ uses EVE_GpuAlloc5, which doesn't have the 64-handle cap, so
     * gradients can keep using the allocator directly. Skip the ring. */
    if(EVE_Hal_supportRenderTarget(u->hal)) return;

    r->handle = EVE_GpuAlloc_Alloc(u->allocator, LV_DRAW_EVE5_RING_SIZE, GA_ALIGN_128);
    r->base = EVE_GpuAlloc_Get(u->allocator, r->handle);
    if(r->base == GA_INVALID) {
        LV_LOG_WARN("EVE5: scratch ring alloc failed (%u bytes); gradients will fall back to EVE_GpuAlloc",
                    (unsigned)LV_DRAW_EVE5_RING_SIZE);
        r->handle = GA_HANDLE_INVALID;
        return;
    }
    r->size = LV_DRAW_EVE5_RING_SIZE;
    LV_LOG_INFO("EVE5: scratch ring at 0x%08X (%u bytes)", r->base, (unsigned)r->size);
}

void lv_draw_eve5_ring_deinit(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_ring_t * r = &u->scratch_ring;
    if(r->base != GA_INVALID && r->handle.Id != GA_HANDLE_INVALID.Id) {
        EVE_GpuAlloc_Free(u->allocator, r->handle);
    }
    r->handle = GA_HANDLE_INVALID;
    r->base = GA_INVALID;
    r->size = 0;
}

uint32_t lv_draw_eve5_ring_alloc(lv_draw_eve5_unit_t * u, uint32_t size, uint32_t align)
{
    lv_draw_eve5_ring_t * r = &u->scratch_ring;
    if(r->base == GA_INVALID || size == 0) return GA_INVALID;
    if(align == 0) align = 1;

    /* Round size up to alignment so successive allocations stay aligned. */
    size = (size + (align - 1)) & ~(align - 1);
    if(size > r->size) return GA_INVALID;

    /* Align the next slot. */
    uint32_t pos_abs = (r->write_abs + (align - 1)) & ~(align - 1);
    uint32_t pos_in_ring = pos_abs % r->size;

    /* If the alloc would straddle the end of the ring, advance to the next
     * ring boundary (the wasted bytes count toward the in-use accounting). */
    if(pos_in_ring + size > r->size) {
        pos_abs += (r->size - pos_in_ring);
        pos_in_ring = 0;
    }

    /* The total content tracked by the ring (still being scanned out plus
     * what we're about to add) must fit. */
    if((pos_abs + size) - r->safe_until > r->size) {
        LV_LOG_WARN("EVE5: scratch ring full (size=%u, requested=%u, in-flight=%u)",
                    (unsigned)r->size, (unsigned)size,
                    (unsigned)(r->write_abs - r->safe_until));
        return GA_INVALID;
    }

    r->write_abs = pos_abs + size;
    return r->base + pos_in_ring;
}

void lv_draw_eve5_ring_swap(lv_draw_eve5_unit_t * u)
{
    lv_draw_eve5_ring_t * r = &u->scratch_ring;
    /* Two-frame lag: the swap that just happened released the frame before
     * curr_start. The frame between curr_start and write_abs is now what
     * the GPU is scanning out. */
    r->safe_until = r->curr_start;
    r->curr_start = r->write_abs;
}

#endif /* LV_USE_DRAW_EVE5 */
