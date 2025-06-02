/**
 * @file lv_g2d_buf_map.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#include "lv_g2d_buf_map.h"

#if LV_USE_DRAW_G2D
#include <stdio.h>
#include "lv_g2d_utils.h"
#include "g2d.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static unsigned long _map_hash_function(void * ptr);

static lv_map_item_t * _map_create_item(void * key, struct g2d_buf * value);

static void _map_free_item(lv_map_item_t * item);

static void _handle_collision(unsigned long index, lv_map_item_t * item);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_buf_map_t * table;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void g2d_create_buf_map(void)
{
    table = (lv_buf_map_t *) lv_malloc(sizeof(lv_buf_map_t));
    table->size = LV_G2D_HASH_TABLE_SIZE;
    table->count = 0;
    table->items = (lv_map_item_t **) lv_malloc_zeroed(table->size * sizeof(lv_map_item_t *));
    table->overflow_list = (lv_array_t **) lv_malloc_zeroed(table->size * sizeof(lv_array_t *));
}

void g2d_free_buf_map(void)
{
    for(int i = 0; i < table->size; i++) {
        if(table->overflow_list[i]) {
            lv_array_deinit(table->overflow_list[i]);
            lv_free(table->overflow_list[i]);
        }

        lv_map_item_t * item = table->items[i];
        if(item != NULL)
            _map_free_item(item);
    }

    lv_free(table->items);
    lv_free(table->overflow_list);
    lv_free(table);
}

void g2d_insert_buf_map(void * key, struct g2d_buf * value)
{
    lv_map_item_t * item = _map_create_item(key, value);
    int index = _map_hash_function(key);

    if(table->items[index] == NULL) {
        /* Key not found. */
        if(table->count == table->size) {
            /* Table is full. */
            _map_free_item(item);
            G2D_ASSERT_MSG(false, "Hash table is full.");
            return;
        }
    }
    else {
        if(table->items[index]->key == key) {
            /* Key already exists, update value. */
            table->items[index]->value = value;
            return;
        }
        else {
            /* Handle the collision */
            _handle_collision(index, item);
            return;
        }
    }

    /* Insert item. */
    table->items[index] = item;
    table->count++;
}

struct g2d_buf * g2d_search_buf_map(void * key)
{
    int index = _map_hash_function(key);
    lv_map_item_t * item = table->items[index];
    lv_array_t * list = (lv_array_t *)table->overflow_list[index];

    if(item == NULL)
        return NULL;

    if(item->key == key)
        return item->value;

    if(list == NULL)
        return NULL;

    for(uint32_t i = 0; i < lv_array_size(list); i++) {
        item = (lv_map_item_t *)lv_array_at(list, i);
        if(item->key == key)
            return item->value;
    }

    return NULL;
}

void g2d_free_item(void * key)
{
    /* Delete an item from the table. */
    int index = _map_hash_function(key);
    lv_map_item_t * item = table->items[index];
    lv_array_t * list = (lv_array_t *)table->overflow_list[index];

    if(item == NULL) {
        return;
    }
    else if(list == NULL && item->key == key) {
        /* No collision chain, just remove item. */
        table->items[index] = NULL;
        _map_free_item(item);
        table->count--;
        return;
    }
    else if(list != NULL) {
        /* Collision chain exists. */
        for(uint32_t i = 0; i < lv_array_size(list); i++) {
            item = (lv_map_item_t *)lv_array_at(list, i);
            if(item->key == key) {
                lv_array_remove(list, i);
                return;
            }
        }
    }
}

void g2d_print_table(void)
{
    LV_LOG("\nHash Table\n-------------------\n");

    for(int i = 0; i < table->size; i++) {
        if(table->items[i]) {
            LV_LOG("Index:%d, Key:%p, Value:%p\n", i, table->items[i] -> key, (void *)table->items[i]->value);
            if(table->overflow_list[i]) {
                for(uint32_t j = 0 ; j < lv_array_size(table->overflow_list[i]); j++) {
                    lv_map_item_t * item = (lv_map_item_t *)lv_array_at(table->overflow_list[i], j);
                    LV_LOG("Index:%d, Key:%p, Value:%p\n", i, item -> key, (void *)item->value);
                }

            }
        }
    }

    LV_LOG("-------------------\n\n");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static unsigned long _map_hash_function(void * ptr)
{
    unsigned long i = 0;
    char str[64];
    G2D_ASSERT_MSG(ptr, "Key is null.");
    sprintf(str, "%p", ptr);

    for(int j = 0; str[j]; j++)
        i += str[j];

    return i % LV_G2D_HASH_TABLE_SIZE;
}

static void _handle_collision(unsigned long index, lv_map_item_t * item)
{
    if(table->overflow_list[index] == NULL) {
        /* Create the list. */
        lv_array_t * list = (lv_array_t *) lv_malloc(sizeof(lv_array_t));;
        lv_array_init(list, LV_ARRAY_DEFAULT_CAPACITY, sizeof(lv_map_item_t));
        lv_array_push_back(list, item);
        table->overflow_list[index] = list;
        return;
    }
    else {
        lv_array_t * list = (lv_array_t *)table->overflow_list[index];
        for(uint32_t i = 0; i < lv_array_size(list); i++) {
            lv_map_item_t * it = (lv_map_item_t *)lv_array_at(list, i);
            if(it->key == item->key) {
                /* Key exists, update value. */
                it->value = item->value;
                return;
            }
        }
        /* Insert to the list. */
        lv_array_push_back(table->overflow_list[index], item);
        return;
    }
}

static lv_map_item_t * _map_create_item(void * key, struct g2d_buf * value)
{
    lv_map_item_t * item = (lv_map_item_t *) lv_malloc(sizeof(lv_map_item_t));
    G2D_ASSERT_MSG(item, "Failed to alloc item.");
    item->key = key;
    item->value = value;
    return item;
}

static void _map_free_item(lv_map_item_t * item)
{
    /* Also free the g2d_buf. */
    g2d_free(item->value);
    item->key = NULL;
    item->value = NULL;
    lv_free(item);
    item = NULL;
}

#endif /*LV_USE_DRAW_G2D*/
