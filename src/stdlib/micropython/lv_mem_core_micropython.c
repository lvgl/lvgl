/**
 * @file lv_mem_core_micropython.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_MICROPYTHON

#include <include/lv_mp_mem_custom_include.h>

#if !MICROPY_TRACKED_ALLOC
    #error "LV_STDLIB_MICROPYTHON requires MICROPY_TRACKED_ALLOC=1 in mpconfigport.h"
#endif

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
    return m_tracked_calloc(1, size);
}

void * lv_realloc_core(void * p, size_t new_size)
{
    return m_tracked_realloc(p, new_size);
}

void lv_free_core(void * p)
{
    m_tracked_free(p);
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
