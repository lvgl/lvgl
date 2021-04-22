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
#include "lv_log.h"
#include "lv_types.h"

#if LV_MEMCPY_MEMSET_STD
#include <string.h>
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef LV_MEM_BUF_MAX_NUM
#define LV_MEM_BUF_MAX_NUM    16
#endif

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

typedef struct {
    void * p;
    uint16_t size;
    uint8_t used : 1;
} lv_mem_buf_t;

typedef lv_mem_buf_t lv_mem_buf_arr_t[LV_MEM_BUF_MAX_NUM];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the dyn_mem module (work memory and other variables)
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
void lv_mem_free(void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_mem_realloc(void * data_p, size_t new_size);

/**
 *
 * @return
 */
lv_res_t lv_mem_test(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p);


/**
 * Get a temporal buffer with the given size.
 * @param size the required size
 */
void * lv_mem_buf_get(uint32_t size);

/**
 * Release a memory buffer
 * @param p buffer to release
 */
void lv_mem_buf_release(void * p);

/**
 * Free all memory buffers
 */
void lv_mem_buf_free_all(void);

//! @cond Doxygen_Suppress

#if LV_MEMCPY_MEMSET_STD

/**
 * Wrapper for the standard memcpy
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
static inline void * lv_memcpy(void * dst, const void * src, size_t len)
{
    return memcpy(dst, src, len);
}

/**
 * Wrapper for the standard memcpy
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
static inline void * lv_memcpy_small(void * dst, const void * src, size_t len)
{
    return memcpy(dst, src, len);
}

/**
 * Wrapper for the standard memset
 * @param dst pointer to the destination buffer
 * @param v value to set [0..255]
 * @param len number of byte to set
 */
static inline void lv_memset(void * dst, uint8_t v, size_t len)
{
    memset(dst, v, len);
}

/**
 * Wrapper for the standard memset with fixed 0x00 value
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
static inline void lv_memset_00(void * dst, size_t len)
{
    memset(dst, 0x00, len);
}

/**
 * Wrapper for the standard memset with fixed 0xFF value
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
static inline void lv_memset_ff(void * dst, size_t len)
{
    memset(dst, 0xFF, len);
}

#else
/**
 * Same as `memcpy` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
LV_ATTRIBUTE_FAST_MEM void * lv_memcpy(void * dst, const void * src, size_t len);

/**
 * Same as `memcpy` but optimized to copy only a few bytes.
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
LV_ATTRIBUTE_FAST_MEM static inline void * lv_memcpy_small(void * dst, const void * src, size_t len)
{
    uint8_t * d8 = (uint8_t *)dst;
    const uint8_t * s8 = (const uint8_t *)src;

    while(len) {
        *d8 = *s8;
        d8++;
        s8++;
        len--;
    }

    return dst;
}

/**
 * Same as `memset` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param v value to set [0..255]
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset(void * dst, uint8_t v, size_t len);

/**
 * Same as `memset(dst, 0x00, len)` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset_00(void * dst, size_t len);

/**
 * Same as `memset(dst, 0xFF, len)` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset_ff(void * dst, size_t len);

//! @endcond

#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_H*/
