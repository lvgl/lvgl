/**
 * @file lv_mem.c
 * General and portable implementation of malloc and free.
 * The dynamic memory monitoring is also supported.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_mem.h"
#include "lv_tlsf.h"
#include "lv_assert.h"
#include "lv_log.h"
#include LV_STDLIB_INCLUDE
#if LV_USE_BUILTIN_MALLOC
    #include "lv_malloc_builtin.h"
#endif

#if LV_USE_BUILTIN_MEMCPY
    #include "lv_memcpy_builtin.h"
#endif

/*********************
 *      DEFINES
 *********************/
/*memset the allocated memories to 0xaa and freed memories to 0xbb (just for testing purposes)*/
#ifndef LV_MEM_ADD_JUNK
    #define LV_MEM_ADD_JUNK  0
#endif

#define ZERO_MEM_SENTINEL  0xa1b2c3d4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t zero_mem = ZERO_MEM_SENTINEL; /*Give the address of this variable if 0 byte should be allocated*/

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_MEM
    #define MEM_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define MEM_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_malloc(size_t size)
{
    MEM_TRACE("allocating %lu bytes", (unsigned long)size);
    if(size == 0) {
        MEM_TRACE("using zero_mem");
        return &zero_mem;
    }

    void * alloc = LV_MALLOC(size);

    if(alloc == NULL) {
        LV_LOG_INFO("couldn't allocate memory (%lu bytes)", (unsigned long)size);
#if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        LV_LOG_INFO("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d",
                    (int)(mon.total_size - mon.free_size), mon.used_pct, mon.frag_pct,
                    (int)mon.free_biggest_size);
        return NULL;
#endif
    }

#if LV_MEM_ADD_JUNK
    lv_memset(alloc, 0xaa, size);
#endif

    MEM_TRACE("allocated at %p", alloc);

    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_free(void * data)
{
    MEM_TRACE("freeing %p", data);
    if(data == &zero_mem) return;
    if(data == NULL) return;

    LV_FREE(data);
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_realloc(void * data_p, size_t new_size)
{
    MEM_TRACE("reallocating %p with %lu size", data_p, (unsigned long)new_size);
    if(new_size == 0) {
        MEM_TRACE("using zero_mem");
        lv_free(data_p);
        return &zero_mem;
    }

    if(data_p == &zero_mem) return lv_malloc(new_size);

    void * new_p = LV_REALLOC(data_p, new_size);
    if(new_p == NULL) {
        LV_LOG_ERROR("couldn't reallocate memory");
        return NULL;
    }

    MEM_TRACE("reallocated at %p", new_p);
    return new_p;
}

void * lv_memcpy(void * dst, const void * src, size_t len)
{
    return LV_MEMCPY(dst, src, len);
}

void lv_memset(void * dst, uint8_t v, size_t len)
{
    LV_MEMSET(dst, v, len);
}

size_t lv_strlen(const char * str)
{
    return LV_STRLEN(str);
}

size_t lv_strncpy(char * dst, size_t dest_size, const char * src)
{
    return LV_STRNCPY(dst, dest_size, src);
}

lv_res_t lv_mem_test(void)
{
    if(zero_mem != ZERO_MEM_SENTINEL) {
        LV_LOG_WARN("zero_mem is written");
        return LV_RES_INV;
    }

#if LV_USE_BUILTIN_MALLOC
    return lv_mem_test_builtin();
#else
    return LV_RES_OK;
#endif
}

void lv_mem_monitor(lv_mem_monitor_t * mon_p)
{
    /*Init the data*/
    lv_memset(mon_p, 0, sizeof(lv_mem_monitor_t));
#if LV_USE_BUILTIN_MALLOC
    lv_mem_monitor_builtin(mon_p);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
