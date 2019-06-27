/**
 * @file lv_mem.h
 *
 */

#ifndef LV_MEM_H
#define LV_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include "lv_log.h"

/*********************
 *      DEFINES
 *********************/
// Check windows
#ifdef __WIN64
#define LV_MEM_ENV64
#endif

// Check GCC
#ifdef __GNUC__
#if defined(__x86_64__) || defined(__ppc64__)
#define LV_MEM_ENV64
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    uint32_t total_size;
    uint32_t free_cnt;
    uint32_t free_size;
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint8_t used_pct;
    uint8_t frag_pct;
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init(void);

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_mem_alloc(uint32_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_mem_free(const void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_mem_realloc(void * data_p, uint32_t new_size);

/**
 * Join the adjacent free memory blocks
 */
void lv_mem_defrag(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p);

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes
 */
uint32_t lv_mem_get_size(const void * data);

/**********************
 *      MACROS
 **********************/

/**
 * Halt on NULL pointer
 * p pointer to a memory
 */
#if LV_USE_LOG == 0
#define lv_mem_assert(p)                                                                                               \
    {                                                                                                                  \
        if(p == NULL)                                                                                                  \
            while(1)                                                                                                   \
                ;                                                                                                      \
    }
#else
#define lv_mem_assert(p)                                                                                               \
    {                                                                                                                  \
        if(p == NULL) {                                                                                                \
            LV_LOG_ERROR("Out of memory!");                                                                            \
            while(1)                                                                                                   \
                ;                                                                                                      \
        }                                                                                                              \
    }
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MEM_H*/
