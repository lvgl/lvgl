/**
 * @file lv_draw_eve_ram_g.c
 *
 */

/*  Created on: 19 nov 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE
#include "lv_draw_eve_ram_g.h"
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/

#define RAMG_DEBUG 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * STATIC PROTOTYPES
 **********************/

static void ramg_init(lv_draw_eve_ramg_t * ramg);
static uint32_t hash_key(uintptr_t key);
static uint32_t fnv_1a_hash(const void * src, size_t len);
static void grow_hash_table(lv_draw_eve_ramg_t * ramg);

#if RAMG_DEBUG
    static void ramg_debug(lv_draw_eve_ramg_t * ramg, uint32_t key_hash, uint32_t table_index);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_eve_ramg_get_addr(uint32_t * addr_dst, uintptr_t key,
                               uint32_t addr_size, uint32_t addr_align)
{
    LV_ASSERT(key != 0);

    lv_draw_eve_ramg_t * ramg = &lv_draw_eve_unit_g->ramg;

    if(ramg->hash_table_cell_count == 0) {
        ramg_init(ramg);
    }

    uint32_t key_hash = hash_key(key);
    uint32_t table_index = key_hash % ramg->hash_table_cell_count;
    lv_draw_eve_ramg_hash_table_cell_t * cell;

    while(1) {
        cell = &ramg->hash_table[table_index];

        if(cell->key == key) {
#if RAMG_DEBUG
            ramg_debug(ramg, key_hash, table_index);
#endif

            *addr_dst = cell->addr;
            return true;
        }

        if(cell->key == 0) {
            break;
        }

        table_index++;
        if(table_index >= ramg->hash_table_cell_count) table_index = 0;
    }

    uint32_t addr_ret = LV_ALIGN_UP(ramg->ramg_addr_end, addr_align);
    uint32_t addr_new_end = addr_ret + addr_size;

    if(addr_new_end > 1024 * 1024) {
        LV_LOG_WARN("EVE on-chip 1 MB RAM_G for images and fonts has run out.");
        *addr_dst = LV_DRAW_EVE_RAMG_OUT_OF_RAMG;
        return false;
    }

    ramg->ramg_addr_end = addr_new_end;
    ramg->hash_table_cells_occupied++;

    cell->key = key;
    cell->addr = addr_ret;
    *addr_dst = addr_ret;

#if RAMG_DEBUG
    ramg_debug(ramg, key_hash, table_index);
#endif

    if(ramg->hash_table_cells_occupied > ramg->hash_table_cell_count / 4 * 3) {
        grow_hash_table(ramg);
    }

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ramg_init(lv_draw_eve_ramg_t * ramg)
{
    ramg->hash_table_cell_count = 32;
    ramg->hash_table = lv_calloc(32, sizeof(lv_draw_eve_ramg_hash_table_cell_t));
    LV_ASSERT_MALLOC(ramg->hash_table);
}

static uint32_t hash_key(uintptr_t key)
{
    return fnv_1a_hash(&key, sizeof(key));
}

static uint32_t fnv_1a_hash(const void * src, size_t len)
{
    const uint8_t * src_u8 = src;
    uint32_t hash = 2166136261u;
    for(size_t i = 0; i < len; i++) {
        hash ^= src_u8[i];
        hash *= 16777619u;
    }
    return hash;
}

static void grow_hash_table(lv_draw_eve_ramg_t * ramg)
{
    uint32_t old_cell_count = ramg->hash_table_cell_count;
    lv_draw_eve_ramg_hash_table_cell_t * old_hash_table = ramg->hash_table;

    ramg->hash_table_cell_count += ramg->hash_table_cell_count / 2;
    ramg->hash_table = lv_calloc(ramg->hash_table_cell_count,
                                 sizeof(lv_draw_eve_ramg_hash_table_cell_t));
    LV_ASSERT_MALLOC(ramg->hash_table);

    for(uint32_t i = 0; i < old_cell_count; i++) {
        lv_draw_eve_ramg_hash_table_cell_t * old_cell = &old_hash_table[i];

        if(old_cell->key == 0) continue;

        uint32_t key_hash = hash_key(old_cell->key);
        uint32_t new_table_index = key_hash % ramg->hash_table_cell_count;
        lv_draw_eve_ramg_hash_table_cell_t * new_cell_dst = &ramg->hash_table[new_table_index];

        while(new_cell_dst->key != 0) {
            new_table_index++;
            if(new_table_index >= ramg->hash_table_cell_count) new_table_index = 0;
            new_cell_dst = &ramg->hash_table[new_table_index];
        }
        *new_cell_dst = *old_cell;
    }

    lv_free(old_hash_table);
}

#if RAMG_DEBUG
/*
Print tables like this:
    113 kB of RAM_G used
    ================================-==-=====---=---===---=====-=-=-=-==--=======----=-==-==---=--===--=-=-=========================
    ========^$==========--=-==-=-=--=--=========---=----========-------===--=====----=======--=====--====--=====-=--=-=

'-' unoccupied cells
'=' occupied cells
'^' where the hash pointed to in the table initially and linear probing started
'$' where linear probing ended because a matching or unoccupied cell
    was found. This symbol is not shown if the initial guess was correct.

This example has 244 cells. Each cell uses 8 bytes of local RAM, so just under 2 kB.
Each cell represents an allocation in EVE RAM_G. The RAM_G allocation sizes are not
represented in this table, except for the overall "113 kB of RAM_G used" message.
*/
static void ramg_debug(lv_draw_eve_ramg_t * ramg, uint32_t key_hash, uint32_t table_index)
{
    uint32_t table_index_initial_guess = key_hash % ramg->hash_table_cell_count;

    lv_log("%u kB of RAM_G used\n", (unsigned) ramg->ramg_addr_end / 1024);

    for(uint32_t i = 0; i < ramg->hash_table_cell_count; i++) {
        if(i != 0 && i % 128 == 0) {
            lv_log("\n");
        }

        if(i == table_index_initial_guess) {
            lv_log("^");
        }
        else if(i == table_index) {
            lv_log("$");
        }
        else if(ramg->hash_table[i].key) {
            lv_log("=");
        }
        else {
            lv_log("-");
        }
    }

    lv_log("\n\n");
}
#endif

#endif/*LV_USE_EVE_DRAW*/

