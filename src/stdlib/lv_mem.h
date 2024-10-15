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

#include "lv_string.h"

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void * lv_mem_pool_t;

/**
 * Heap information structure.
 */
typedef struct {
    size_t total_size;  /**< Total heap size */
    size_t free_cnt;
    size_t free_size;   /**< Size of available memory */
    size_t free_biggest_size;
    size_t used_cnt;
    size_t max_used;    /**< Max size of Heap memory used */
    uint8_t used_pct;   /**< Percentage used */
    uint8_t frag_pct;   /**< Amount of fragmentation */
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize to use malloc/free/realloc etc
 */
void lv_mem_init(void);

/**
 * Drop all dynamically allocated memory and reset the memory pools' state
 */
void lv_mem_deinit(void);

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes);

void lv_mem_remove_pool(lv_mem_pool_t pool);

/**
 * Allocate memory dynamically
 * @param size requested size in bytes
 * @return pointer to allocated uninitialized memory, or NULL on failure
 */
void * lv_malloc(size_t size);

/**
 * Allocate a block of zeroed memory dynamically
 * @param num requested number of element to be allocated.
 * @param size requested size of each element in bytes.
 * @return pointer to allocated zeroed memory, or NULL on failure
 */
void * lv_calloc(size_t num, size_t size);

/**
 * Allocate zeroed memory dynamically
 * @param size requested size in bytes
 * @return pointer to allocated zeroed memory, or NULL on failure
 */
void * lv_zalloc(size_t size);

/**
 * Allocate zeroed memory dynamically
 * @param size requested size in bytes
 * @return pointer to allocated zeroed memory, or NULL on failure
 */
void * lv_malloc_zeroed(size_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_free(void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data_p pointer to an allocated memory.
 *               Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory, NULL on failure
 */
void * lv_realloc(void * data_p, size_t new_size);

/**
 * Used internally to execute a plain `malloc` operation
 * @param size      size in bytes to `malloc`
 */
void * lv_malloc_core(size_t size);

/**
 * Used internally to execute a plain `free` operation
 * @param p      memory address to free
 */
void lv_free_core(void * p);

/**
 * Used internally to execute a plain realloc operation
 * @param p         memory address to realloc
 * @param new_size  size in bytes to realloc
 */
void * lv_realloc_core(void * p, size_t new_size);

/**
 * Used internally by lv_mem_monitor() to gather LVGL heap state information.
 * @param mon_p      pointer to lv_mem_monitor_t object to be populated.
 */
void lv_mem_monitor_core(lv_mem_monitor_t * mon_p);

lv_result_t lv_mem_test_core(void);

/**
 * @brief Tests the memory allocation system by allocating and freeing a block of memory.
 * @return LV_RESULT_OK if the memory allocation system is working properly, or LV_RESULT_INVALID if there is an error.
 */
lv_result_t lv_mem_test(void);

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
