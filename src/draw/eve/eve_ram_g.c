/**
 * @file eve_ram_g.c
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
#include "eve_ram_g.h"
#include "lv_eve.h"


/**********************
 * STATIC PROTOTYPES
 **********************/


/***********************
 * GLOBAL VARIABLES
 ***********************/

/*  Memory blocks are organized by type, with 8 (MAX_FONT 8) spaces for Fonts and 32 (MAX_IMAGE 32 )spaces for images (these values can be modified as needed).
 *  This structure is designed to speed up the search process, eliminating the need to traverse the entire array of blocks.
 */
static ramg_mem_block blocks[MAX_BLOCKS];
static uint32_t ramGptr = EVE_RAM_G;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t get_bitmap_addr(uint8_t id)
{
    return blocks[id].address;
}

void update_ramg_ptr(uint32_t size)
{
    ramGptr += size;
    uint32_t alignment = 4;
    ramGptr = (ramGptr + alignment - 1) & ~(alignment - 1); /*RamG Aligned*/
}

uint32_t get_ramg_ptr(void)
{
    return ramGptr;

}

void init_eve_ramg()
{
    for(int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].address = 0;
        blocks[i].loaded = false;
        blocks[i].id = 0;
        blocks[i].size = 0;
        blocks[i].source = NULL;
    }
}

uint32_t next_free_ramg_block(DataType data)
{
    uint32_t start = 0;
    uint32_t end = 0;


    if(data == TYPE_IMAGE) {
        start = IMAGE_BLOCK_START;
        end =  MAX_IMAGE;
    }
    else if(data == TYPE_FONT) {
        start = FONT_BLOCK_START;
        end =  MAX_FONT;
    }

    for(uint32_t i = start; i < end; i++) {
        if(blocks[i].loaded == false) {
            blocks[i].loaded = true;
            return i;
        }
    }

    return NOT_FOUND_BLOCK;
}


void set_size_ramg_block(uint8_t Id, uint32_t sz)
{

    blocks[Id].size = sz;

}

void set_source_ramg_block(uint8_t Id, const uint8_t * src)
{

    if(Id < MAX_BLOCKS) {
        blocks[Id].source = (uint8_t *)src;
    }

}


void set_addr_ramg_block(uint8_t Id, uint32_t addr)
{

    blocks[Id].address = addr;

}

void set_state_ramg_block(uint8_t Id, bool state)
{

    blocks[Id].loaded = state;

}

bool update_ramg_block(uint8_t id, uint8_t * src, uint32_t addr, uint32_t sz)
{
    blocks[id].source = src;
    blocks[id].address = addr;
    blocks[id].size = sz;
    blocks[id].loaded = true;
    blocks[id].id = id;
    update_ramg_ptr(sz);

    if(ramGptr > EVE_RAM_G_SIZE) {
        return true;
    }
    else {
        return false;
    }
}


uint32_t find_ramg_image(const uint8_t * imageSource)
{

    for(uint32_t i = IMAGE_BLOCK_START; i < MAX_IMAGE; i++) {
        if(blocks[i].source == imageSource) {
            return i;
        }
    }

    return NOT_FOUND_BLOCK;
}


uint32_t find_ramg_font(const uint8_t * fontSource)
{

    for(uint32_t i = FONT_BLOCK_START; i < MAX_FONT; i++) {
        if(blocks[i].source == fontSource) {
            return i;
        }
    }

    return NOT_FOUND_BLOCK;
}

#endif/*LV_USE_EVE_DRAW*/

