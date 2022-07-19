/**
 * @file lv_mem_builtin.h
 *
 */

#ifndef LV_MEM_BUILTIN_H
#define LV_MEM_BUILTIN_H

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
 * Same as `memcpy` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
LV_ATTRIBUTE_FAST_MEM void * lv_memcpy_builtin(void * dst, const void * src, size_t len);

/**
 * Same as `memset` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param v value to set [0..255]
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset_builtin(void * dst, uint8_t v, size_t len);

size_t lv_strlen_builtin(const char * str);

size_t lv_strncpy_builtin(char * dst, size_t dest_size, const char * src);;

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
