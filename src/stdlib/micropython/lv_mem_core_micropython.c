/**
 * @file lv_mem_core_micropython.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_MICROPYTHON

#include <include/lv_mp_mem_custom_include.h>
#include <stdlib.h>

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

void lv_mem_init(void)
{
    return; /*Nothing to init*/
}

void lv_mem_deinit(void)
{
    return; /*Nothing to deinit*/

}

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes)
{
    /*Not supported*/
    LV_UNUSED(mem);
    LV_UNUSED(bytes);
    return NULL;
}

void lv_mem_remove_pool(lv_mem_pool_t pool)
{
    /*Not supported*/
    LV_UNUSED(pool);
    return;
}

void * lv_malloc_core(size_t size)
{
#if MICROPY_ENABLE_GC
    void * p = gc_alloc(size, 0);
#else
    void * p = malloc(size);
#endif
    if(p == NULL) m_malloc_fail(size);
    return p;
}

void * lv_realloc_core(void * p, size_t new_size)
{
#if MICROPY_ENABLE_GC
    void * new_p = gc_realloc(p, new_size, true);
    /*gc_realloc() fails while the GC is locked (e.g. in a finaliser), even
     *to shrink. LVGL tolerates a failed shrink, so keep the block.*/
    if(new_p == NULL && p != NULL && new_size <= gc_nbytes(p)) return p;
#else
    void * new_p = realloc(p, new_size);
#endif
    if(new_p == NULL) m_malloc_fail(new_size);
    return new_p;
}

void lv_free_core(void * p)
{
#if MICROPY_ENABLE_GC
    gc_free(p);
#else
    free(p);
#endif
}

void lv_mem_monitor_core(lv_mem_monitor_t * mon_p)
{
    /*Not supported*/
    LV_UNUSED(mon_p);
    return;
}

lv_result_t lv_mem_test_core(void)
{
    /*Not supported*/
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_MICROPYTHON*/
