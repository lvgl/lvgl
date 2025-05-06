/**
 * @file eve_ram_g.h
 *
 */

/*  Created on: 19 nov 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

#ifndef LV_EVE_RAM_G_H
#define LV_EVE_RAM_G_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE
#include LV_STDINT_INCLUDE
#include LV_STDBOOL_INCLUDE

/*********************
 *      DEFINES
 *********************/

#define MAX_MEMORY 1024 * 1024  /* Defines the size of memory as 1MB */

#define FONT_BLOCK_START 0      /* Starting index for font blocks in the memory */
#define MAX_FONT 8              /* Maximum number of font blocks */

#define IMAGE_BLOCK_START MAX_FONT  /* Starting index for image blocks in the memory.*/
#define MAX_IMAGE 32                 /* Maximum number of image blocks.*/

#define MAX_BLOCKS 256               /* Total number of blocks in the memory.*/
#define NOT_FOUND_BLOCK 0xFFAAFFAA   /* A constant to represent 'block not found'.*/

/**********************
 *      TYPEDEFS
 **********************/

/* Structure representing a block in the memory.*/
typedef struct {
    uint32_t address; /* Address of the block in memory.*/
    uint32_t size;    /* Size of the block.*/
    uint8_t id;       /* Identifier for the block.*/
    uint8_t * source; /* Pointer to the source of the data stored in the block.*/
    bool loaded;      /* Flag to indicate if the block is loaded.*/
} ramg_mem_block;


/* Enumeration to define the types of data.*/
typedef enum {
    TYPE_IMAGE = 0,
    TYPE_FONT = 1,
    OTHER
} DataType;



/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Function to update the RAM_G pointer with the given size.*/
void update_ramg_ptr(uint32_t size);

/* Function to get the current RAM_G pointer value. */
uint32_t get_ramg_ptr(void);

/* Init Memory blocks */
void init_eve_ramg();

/* Searches for the next free memory block */
uint32_t next_free_ramg_block(DataType data);

/* This function checks if the image has already been loaded into ram_g. If it hasn't been loaded, it returns 'NOT FOUND',
 * otherwise it returns the ID of the block where it is located."
 */
uint32_t find_ramg_image(const uint8_t * imageSource);

/* "This function checks if the font has already been loaded into ram_g. If it hasn't been loaded, it returns 'NOT FOUND',
 * otherwise it returns the ID of the block where it is located."
 */
uint32_t find_ramg_font(const uint8_t * fontSource);

uint32_t get_bitmap_addr(uint8_t id);

void set_source_ramg_block(uint8_t Id, const  uint8_t * src);

void set_addr_ramg_block(uint8_t Id, uint32_t addr);

void set_state_ramg_block(uint8_t Id, bool state);

void set_size_ramg_block(uint8_t Id, uint32_t sz);

bool update_ramg_block(uint8_t id, uint8_t * src, uint32_t addr, uint32_t sz);


#endif/*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE_RAM_G_H */
