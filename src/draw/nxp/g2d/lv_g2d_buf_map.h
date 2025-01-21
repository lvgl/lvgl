
/**
 * @file lv_g2d_buf_map.h
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_G2D_BUF_MAP_H
#define LV_G2D_BUF_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_G2D

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Map item definition. */
typedef struct lv_map_item {
    /* Virtual address buffer. */
    void * key;
    struct g2d_buf * value;
} lv_map_item_t;

/* Linked list for collision handling. */
typedef struct lv_list {
    lv_map_item_t * item;
    struct lv_list * next;
} lv_list_t;

/*Buf map definition. */
typedef struct lv_buf_map {
    lv_map_item_t ** items;
    lv_list_t ** overflow_list;
    int size;
    int count;
} lv_buf_map_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_create_buf_map();

void lv_free_buf_map();

void lv_insert_buf_map(void * key, struct g2d_buf * value);

struct g2d_buf * lv_search_buf_map(void * key);

void lv_free_item(void * key);

void print_table();

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_G2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_G2D_BUF_MAP_H */
