/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_log.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Give a buffer with the given to use during drawing.
 * Be careful to not use the buffer while other processes are using it.
 * @param size the required size
 */
void * lv_draw_buf_get(uint32_t size)
{
    /*Try to find a free buffer with suitable size */
    uint8_t i;
    for(i = 0; i < LV_DRAW_BUF_MAX_NUM; i++) {
        if(_lv_draw_buf[i].used == 0 && _lv_draw_buf[i].size >= size) {
            _lv_draw_buf[i].used = 1;
            return  _lv_draw_buf[i].p;
        }
    }

    /*Reallocate a free buffer*/
    for(i = 0; i < LV_DRAW_BUF_MAX_NUM; i++) {
        if(_lv_draw_buf[i].used == 0) {
            _lv_draw_buf[i].used = 1;
            _lv_draw_buf[i].size = size;
            _lv_draw_buf[i].p = lv_mem_realloc(_lv_draw_buf[i].p, size);
            lv_mem_assert(_lv_draw_buf[i].p);
            return  _lv_draw_buf[i].p;
        }
    }

    LV_LOG_ERROR("lv_draw_buf_get: no free buffer. Increase LV_DRAW_BUF_MAX_NUM.");

    return NULL;
}

/**
 * Release the draw buffer
 * @param p buffer to release
 */
void lv_draw_buf_release(void * p)
{
    uint8_t i;
    for(i = 0; i < LV_DRAW_BUF_MAX_NUM; i++) {
        if(_lv_draw_buf[i].p == p) {
            _lv_draw_buf[i].used = 0;
            return;
        }
    }

    LV_LOG_ERROR("lv_draw_buf_release: p is not a known buffer")
}

/**
 * Free all draw buffers
 */
void lv_draw_buf_free_all(void)
{
    uint8_t i;
    uint32_t s = 0;
    for(i = 0; i < LV_DRAW_BUF_MAX_NUM; i++) {
        if(_lv_draw_buf[i].p) {
            s+= _lv_draw_buf[i].size;
            lv_mem_free(_lv_draw_buf[i].p);
            _lv_draw_buf[i].p = NULL;
            _lv_draw_buf[i].used = 0;
            _lv_draw_buf[i].size = 0;
        }
    }
    if(s) printf("draf_buf free %d bytes\n", s);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
