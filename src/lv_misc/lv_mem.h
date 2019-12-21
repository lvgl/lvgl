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
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Heap information structure.
 */
typedef struct
{
    uint32_t total_size; /**< Total heap size */
    uint32_t free_cnt;
    uint32_t free_size; /**< Size of available memory */
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint8_t used_pct; /**< Percentage used */
    uint8_t frag_pct; /**< Amount of fragmentation */
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init(void);

/**
 * Clean up the memory buffer which frees all the allocated memories.
 * @note It work only if `LV_MEM_CUSTOM == 0`
 */
void lv_mem_deinit(void);

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_mem_alloc(size_t size);

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
void * lv_mem_realloc(void * data_p, size_t new_size);

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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MEM_H*/
