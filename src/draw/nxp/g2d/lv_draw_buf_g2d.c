/**
 * @file lv_draw_buf_g2d.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D
#include "../../lv_draw_buf_private.h"
#include "g2d.h"
#include "lv_g2d_buf_map.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t color_format);

static void _buf_free(void * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_g2d_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->buf_malloc_cb = _buf_malloc;
    handlers->buf_free_cb = _buf_free;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    struct g2d_buf* buf = g2d_alloc(size_bytes, 1);
    g2d_insert_buf_map(buf->buf_vaddr, buf);
    return buf->buf_vaddr;
}

static void _buf_free(void * buf)
{
    struct g2d_buf * buf_g2d = lv_search_buf_map(buf);
    g2d_free(buf_g2d);
}


#endif /*LV_USE_DRAW_G2D*/
