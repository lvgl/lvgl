/**
 * @file lv_mem_dcache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"
#include "lv_mem_dcache.h"
#include LV_DCACHE_INCLUDE

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

void lv_mem_clean_dcache_all(void)
{
    LV_MEM_CLEAN_DCACHE_ALL;
}

void lv_mem_clean_dcache(lv_uintptr_t start, lv_uintptr_t end)
{
    LV_UNUSED(start);
    LV_UNUSED(end);
    LV_MEM_CLEAN_DCACHE;
}

void lv_mem_flush_dcache_all(void)
{
    LV_MEM_FLUSH_DCACHE_ALL;
}

void lv_mem_flush_dcache(lv_uintptr_t start, lv_uintptr_t end)
{
    LV_UNUSED(start);
    LV_UNUSED(end);
    LV_MEM_FLUSH_DCACHE;
}

void lv_mem_invalidate_dcache_all(void)
{
    LV_MEM_INVALIDATE_DCACHE_ALL;
}

void lv_mem_invalidate_dcache(lv_uintptr_t start, lv_uintptr_t end)
{
    LV_UNUSED(start);
    LV_UNUSED(end);
    LV_MEM_INVALIDATE_DCACHE;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
