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
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

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
typedef struct {
    uint32_t total_size; /**< Total heap size*/
    uint32_t free_cnt;
    uint32_t free_size; /**< Size of available memory*/
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint32_t max_used; /**< Max size of Heap memory used*/
    uint8_t used_pct; /**< Percentage used*/
    uint8_t frag_pct; /**< Amount of fragmentation*/
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_malloc(size_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_free(void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory, NULL on failure
 */
void * lv_realloc(void * data_p, size_t new_size);


void * lv_memcpy(void * dst, const void * src, size_t len);

void lv_memset(void * dst, uint8_t v, size_t len);

/**
 * Same as `memset(dst, 0x00, len)`.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
static inline void lv_memzero(void * dst, size_t len)
{
    lv_memset(dst, 0x00, len);
}

size_t lv_strlen(const char * str);

size_t lv_strncpy(char * dst, size_t dest_size, const char * src);



/**
 *
 * @return
 */
lv_res_t lv_mem_test(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a lv_mem_monitor_t variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_H*/
