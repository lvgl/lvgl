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
static uint32_t draw_buf_size = 0;

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
void * lv_draw_get_buf(uint32_t size)
{
    if(size <= draw_buf_size) return LV_GC_ROOT(_lv_draw_buf);

    LV_LOG_TRACE("lv_draw_get_buf: allocate");

    draw_buf_size = size;

    if(LV_GC_ROOT(_lv_draw_buf) == NULL) {
        LV_GC_ROOT(_lv_draw_buf) = lv_mem_alloc(size);
        lv_mem_assert(LV_GC_ROOT(_lv_draw_buf));
        return LV_GC_ROOT(_lv_draw_buf);
    }

    LV_GC_ROOT(_lv_draw_buf) = lv_mem_realloc(LV_GC_ROOT(_lv_draw_buf), size);
    lv_mem_assert(LV_GC_ROOT(_lv_draw_buf));
    return LV_GC_ROOT(_lv_draw_buf);
}

/**
 * Free the draw buffer
 */
void lv_draw_free_buf(void)
{
    if(LV_GC_ROOT(_lv_draw_buf)) {
        lv_mem_free(LV_GC_ROOT(_lv_draw_buf));
        LV_GC_ROOT(_lv_draw_buf) = NULL;
        draw_buf_size = 0;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
