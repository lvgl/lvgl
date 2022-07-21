/**
 * @file lv_malloc_builtin.h
 *
 */

#ifndef LV_MALLOC_BUILTIN_H
#define LV_MALLOC_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init_builtin(void);

/**
 * Clean up the memory buffer which frees all the allocated memories.
 */
void lv_mem_deinit_builtin(void);

void * lv_malloc_builtin(size_t size);
void * lv_realloc_builtin(void * p, size_t new_size);
void lv_free_builtin(void * p);

/**
 *
 * @return
 */
lv_res_t lv_mem_test_builtin(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a lv_mem_monitor_t variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor_builtin(lv_mem_monitor_t * mon_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_BUILTIN_H*/
