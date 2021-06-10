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
#include "lv_gc.h"
#include "lv_assert.h"
#include <string.h>

#if LV_MEM_CUSTOM != 0
    #include LV_MEM_CUSTOM_INCLUDE
#endif


/*********************
 *      DEFINES
 *********************/
/*memset the allocated memories to 0xaa and freed memories to 0xbb (just for testing purposes)*/
#ifndef LV_MEM_ADD_JUNK
#  define LV_MEM_ADD_JUNK  0
#endif

#ifdef LV_ARCH_64
#  define MEM_UNIT         uint64_t
#  define ALIGN_MASK       0x7
#else
#  define MEM_UNIT         uint32_t
#  define ALIGN_MASK       0x7
#endif

#define ZERO_MEM_SENTINEL  0xa1b2c3d4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_MEM_CUSTOM == 0
    static void lv_mem_walker(void * ptr, size_t size, int used, void * user);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_MEM_CUSTOM == 0
    static lv_tlsf_t tlsf;
#endif

static uint32_t zero_mem = ZERO_MEM_SENTINEL; /*Give the address of this variable if 0 byte should be allocated*/

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_MEM
#  define MEM_TRACE(...) LV_LOG_TRACE( __VA_ARGS__)
#else
#  define MEM_TRACE(...)
#endif

#define COPY32 *d32 = *s32; d32++; s32++;
#define COPY8 *d8 = *s8; d8++; s8++;
#define SET32(x) *d32 = x; d32++;
#define SET8(x) *d8 = x; d8++;
#define REPEAT8(expr) expr expr expr expr expr expr expr expr

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init(void)
{
#if LV_MEM_CUSTOM == 0

#if LV_MEM_ADR == 0
    /*Allocate a large array to store the dynamically allocated data*/
    static LV_ATTRIBUTE_LARGE_RAM_ARRAY MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    tlsf = lv_tlsf_create_with_pool((void *)work_mem_int, LV_MEM_SIZE);
#else
    tlsf = lv_tlsf_create_with_pool((void *)LV_MEM_ADR, LV_MEM_SIZE);
#endif
#endif

#if LV_MEM_ADD_JUNK
    LV_LOG_WARN("LV_MEM_ADD_JUNK is enabled which makes LVGL much slower")
#endif
}

/**
 * Clean up the memory buffer which frees all the allocated memories.
 * @note It work only if `LV_MEM_CUSTOM == 0`
 */
void lv_mem_deinit(void)
{
#if LV_MEM_CUSTOM == 0
    lv_tlsf_destroy(tlsf);
    lv_mem_init();
#endif
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_mem_alloc(size_t size)
{
    MEM_TRACE("allocating %d bytes", size);
    if(size == 0) {
        MEM_TRACE("using zero_mem");
        return &zero_mem;
    }

#if LV_MEM_CUSTOM == 0
    void * alloc = lv_tlsf_malloc(tlsf, size);
#else
    void * alloc = LV_MEM_CUSTOM_ALLOC(size);
#endif

#if LV_MEM_ADD_JUNK
    if(alloc != NULL) lv_memset(alloc, 0xaa, size);
#endif

    if(alloc == NULL) {
        LV_LOG_ERROR("couldn't allocate memory (%d bytes)", size);
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        LV_LOG_ERROR("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d",
               (int)mon.total_size - mon.free_size, mon.used_pct, mon.frag_pct,
               (int)mon.free_biggest_size);
    }

    MEM_TRACE("allocated at %p", alloc);
    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_mem_free(void * data)
{
    MEM_TRACE("freeing %p", data);
    if(data == &zero_mem) return;
    if(data == NULL) return;

#if LV_MEM_CUSTOM == 0
#  if LV_MEM_ADD_JUNK
    lv_memset(data, 0xbb, lv_tlsf_block_size(data));
#  endif
    lv_tlsf_free(tlsf, data);
#else
    LV_MEM_CUSTOM_FREE(data);
#endif
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_mem_realloc(void * data_p, size_t new_size)
{
    MEM_TRACE("reallocating %p with %d size", data_p, new_size);
    if(new_size == 0) {
        MEM_TRACE("using zero_mem");
        lv_mem_free(data_p);
        return &zero_mem;
    }

    if(data_p == &zero_mem) return lv_mem_alloc(new_size);

#if LV_MEM_CUSTOM == 0
    void * new_p = lv_tlsf_realloc(tlsf, data_p, new_size);
#else
    void * new_p = LV_MEM_CUSTOM_REALLOC(data_p, new_size);
#endif
    if(new_p == NULL) {
        LV_LOG_ERROR("couldn't allocate memory");
        return NULL;
    }

    MEM_TRACE("allocated at %p", new_p);
    return new_p;
}

lv_res_t lv_mem_test(void)
{
    if(zero_mem != ZERO_MEM_SENTINEL) {
        LV_LOG_WARN("zero_mem is written");
        return LV_RES_INV;
    }

#if LV_MEM_CUSTOM == 0
    if(lv_tlsf_check(tlsf)) {
        LV_LOG_WARN("failed");
        return LV_RES_INV;
    }

    if (lv_tlsf_check_pool(lv_tlsf_get_pool(tlsf))) {
        LV_LOG_WARN("pool failed");
        return LV_RES_INV;
    }
#endif
    MEM_TRACE("passed");
    return LV_RES_OK;
}

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p)
{
    /*Init the data*/
    lv_memset(mon_p, 0, sizeof(lv_mem_monitor_t));
#if LV_MEM_CUSTOM == 0
    MEM_TRACE("begin");

    lv_tlsf_walk_pool(lv_tlsf_get_pool(tlsf), lv_mem_walker, mon_p);

    mon_p->total_size = LV_MEM_SIZE;
    mon_p->used_pct = 100 - (100U * mon_p->free_size) / mon_p->total_size;
    if(mon_p->free_size > 0) {
        mon_p->frag_pct = mon_p->free_biggest_size * 100U / mon_p->free_size;
        mon_p->frag_pct = 100 - mon_p->frag_pct;
    }
    else {
        mon_p->frag_pct = 0; /*no fragmentation if all the RAM is used*/
    }

    MEM_TRACE("finished");
#endif
}


/**
 * Get a temporal buffer with the given size.
 * @param size the required size
 */
void * lv_mem_buf_get(uint32_t size)
{
    if(size == 0) return NULL;

    MEM_TRACE("begin, getting %d bytes", size);

    /*Try to find a free buffer with suitable size*/
    int8_t i_guess = -1;
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).used == 0 && LV_GC_ROOT(lv_mem_buf[i]).size >= size) {
            if(LV_GC_ROOT(lv_mem_buf[i]).size == size) {
                LV_GC_ROOT(lv_mem_buf[i]).used = 1;
                return LV_GC_ROOT(lv_mem_buf[i]).p;
            }
            else if(i_guess < 0) {
                i_guess = i;
            }
            /*If size of `i` is closer to `size` prefer it*/
            else if(LV_GC_ROOT(lv_mem_buf[i]).size < LV_GC_ROOT(lv_mem_buf[i_guess]).size) {
                i_guess = i;
            }
        }
    }

    if(i_guess >= 0) {
        LV_GC_ROOT(lv_mem_buf[i_guess]).used = 1;
        MEM_TRACE("returning already allocated buffer (buffer id: %d, address: %p)", i_guess, LV_GC_ROOT(lv_mem_buf[i_guess]).p);
        return LV_GC_ROOT(lv_mem_buf[i_guess]).p;
    }

    /*Reallocate a free buffer*/
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).used == 0) {
            /*if this fails you probably need to increase your LV_MEM_SIZE/heap size*/
            void * buf = lv_mem_realloc(LV_GC_ROOT(lv_mem_buf[i]).p, size);
            LV_ASSERT_MSG(buf != NULL, "Out of memory, can't allocate a new buffer (increase your LV_MEM_SIZE/heap size)");
            if(buf == NULL) return NULL;

            LV_GC_ROOT(lv_mem_buf[i]).used = 1;
            LV_GC_ROOT(lv_mem_buf[i]).size = size;
            LV_GC_ROOT(lv_mem_buf[i]).p    = buf;
            MEM_TRACE("allocated (buffer id: %d, address: %p)", i, LV_GC_ROOT(lv_mem_buf[i]).p);
            return LV_GC_ROOT(lv_mem_buf[i]).p;
        }
    }

    LV_LOG_ERROR("no more buffers. (increase LV_MEM_BUF_MAX_NUM)");
    LV_ASSERT_MSG(false, "No more buffers. Increase LV_MEM_BUF_MAX_NUM.");
    return NULL;
}

/**
 * Release a memory buffer
 * @param p buffer to release
 */
void lv_mem_buf_release(void * p)
{
    MEM_TRACE("begin (address: %p)", p);

    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).p == p) {
            LV_GC_ROOT(lv_mem_buf[i]).used = 0;
            return;
        }
    }

    LV_LOG_ERROR("p is not a known buffer")
}

/**
 * Free all memory buffers
 */
void lv_mem_buf_free_all(void)
{
    for(uint8_t i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(lv_mem_buf[i]).p) {
            lv_mem_free(LV_GC_ROOT(lv_mem_buf[i]).p);
            LV_GC_ROOT(lv_mem_buf[i]).p = NULL;
            LV_GC_ROOT(lv_mem_buf[i]).used = 0;
            LV_GC_ROOT(lv_mem_buf[i]).size = 0;
        }
    }
}

#if LV_MEMCPY_MEMSET_STD == 0
/**
 * Same as `memcpy` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param src pointer to the source buffer
 * @param len number of byte to copy
 */
LV_ATTRIBUTE_FAST_MEM void * lv_memcpy(void * dst, const void * src, size_t len)
{
    uint8_t * d8 = dst;
    const uint8_t * s8 = src;

    lv_uintptr_t d_align = (lv_uintptr_t)d8 & ALIGN_MASK;
    lv_uintptr_t s_align = (lv_uintptr_t)s8 & ALIGN_MASK;

    /*Byte copy for unaligned memories*/
    if(s_align != d_align) {
        while(len > 32) {
            REPEAT8(COPY8);
            REPEAT8(COPY8);
            REPEAT8(COPY8);
            REPEAT8(COPY8);
            len -= 32;
        }
        while(len) {
            COPY8
            len--;
        }
        return dst;
    }

    /*Make the memories aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            COPY8;
            d_align--;
            len--;
        }
    }

    uint32_t * d32 = (uint32_t *)d8;
    const uint32_t * s32 = (uint32_t *)s8;
    while(len > 32) {
        REPEAT8(COPY32)
        len -= 32;
    }

    while(len > 4) {
        COPY32;
        len -= 4;
    }

    d8 = (uint8_t *)d32;
    s8 = (const uint8_t *)s32;
    while(len) {
        COPY8
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
LV_ATTRIBUTE_FAST_MEM void lv_memset(void * dst, uint8_t v, size_t len)
{

    uint8_t * d8 = (uint8_t *)dst;

    uintptr_t d_align = (lv_uintptr_t) d8 & ALIGN_MASK;

    /*Make the address aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            SET8(v);
            len--;
            d_align--;
        }
    }

    uint32_t v32 = (uint32_t)v + ((uint32_t)v << 8) + ((uint32_t)v << 16) + ((uint32_t)v << 24);

    uint32_t * d32 = (uint32_t *)d8;

    while(len > 32) {
        REPEAT8(SET32(v32));
        len -= 32;
    }

    while(len > 4) {
        SET32(v32);
        len -= 4;
    }

    d8 = (uint8_t *)d32;
    while(len) {
        SET8(v);
        len--;
    }
}

/**
 * Same as `memset(dst, 0x00, len)` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset_00(void * dst, size_t len)
{
    uint8_t * d8 = (uint8_t *)dst;
    uintptr_t d_align = (lv_uintptr_t) d8 & ALIGN_MASK;

    /*Make the address aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            SET8(0);
            len--;
            d_align--;
        }
    }

    uint32_t * d32 = (uint32_t *)d8;
    while(len > 32) {
        REPEAT8(SET32(0));
        len -= 32;
    }

    while(len > 4) {
        SET32(0);
        len -= 4;
    }

    d8 = (uint8_t *)d32;
    while(len) {
        SET8(0);
        len--;
    }
}

/**
 * Same as `memset(dst, 0xFF, len)` but optimized for 4 byte operation.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
LV_ATTRIBUTE_FAST_MEM void lv_memset_ff(void * dst, size_t len)
{
    uint8_t * d8 = (uint8_t *)dst;
    uintptr_t d_align = (lv_uintptr_t) d8 & ALIGN_MASK;

    /*Make the address aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            SET8(0xFF);
            len--;
            d_align--;
        }
    }

    uint32_t * d32 = (uint32_t *)d8;
    while(len > 32) {
        REPEAT8(SET32(0xFFFFFFFF));
        len -= 32;
    }

    while(len > 4) {
        SET32(0xFFFFFFFF);
        len -= 4;
    }

    d8 = (uint8_t *)d32;
    while(len) {
        SET8(0xFF);
        len--;
    }
}

#endif /*LV_MEMCPY_MEMSET_STD*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_MEM_CUSTOM == 0
static void lv_mem_walker(void * ptr, size_t size, int used, void * user)
{
    LV_UNUSED(ptr);

    lv_mem_monitor_t * mon_p = user;
    if(used) {
        mon_p->used_cnt++;
    }
    else {
        mon_p->free_cnt++;
        mon_p->free_size += size;
        if(size > mon_p->free_biggest_size)
            mon_p->free_biggest_size = size;
    }
}
#endif
