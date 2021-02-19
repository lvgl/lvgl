/**
 * @file lv_mem.c
 * General and portable implementation of malloc and free.
 * The dynamic memory monitoring is also supported.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_mem.h"
#include "lv_math.h"
#include "lv_gc.h"
#include "lv_debug.h"
#include <string.h>

#if LV_MEM_CUSTOM != 0
    #include LV_MEM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
/*Add memory junk on alloc (0xaa) and free(0xbb) (just for testing purposes)*/
#ifndef LV_MEM_ADD_JUNK
    #define LV_MEM_ADD_JUNK 0
#endif

#ifndef LV_MEM_FULL_DEFRAG_CNT
    #define LV_MEM_FULL_DEFRAG_CNT 16
#endif

#ifdef LV_ARCH_64
    #define MEM_UNIT uint64_t
#else
    #define MEM_UNIT uint32_t
#endif

/**********************
 *      TYPEDEFS
 **********************/

#if LV_ENABLE_GC == 0 /*gc custom allocations must not include header*/

/*The size of this union must be 4/8 bytes (uint32_t/uint64_t)*/
typedef union {
    struct {
        MEM_UNIT used : 1;    /* 1: if the entry is used*/
        MEM_UNIT d_size : 31; /* Size of the data*/
    } s;
    MEM_UNIT header; /* The header (used + d_size)*/
} lv_mem_header_t;

typedef struct {
    lv_mem_header_t header;
    uint8_t first_data; /*First data byte in the allocated data (Just for easily create a pointer)*/
} lv_mem_ent_t;

#endif /* LV_ENABLE_GC */

#ifdef LV_ARCH_64
    #define ALIGN_MASK 0x7
#else
    #define ALIGN_MASK 0x3
#endif

#define MEM_BUF_SMALL_SIZE 16

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_MEM_CUSTOM == 0
    static lv_mem_ent_t * ent_get_next(lv_mem_ent_t * act_e);
    static void * ent_alloc(lv_mem_ent_t * e, size_t size);
    static void ent_trunc(lv_mem_ent_t * e, size_t size);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_MEM_CUSTOM == 0
    static uint8_t * work_mem;
#endif

static uint32_t zero_mem; /*Give the address of this variable if 0 byte should be allocated*/

#if LV_MEM_CUSTOM == 0
    static uint32_t mem_max_size; /*Tracks the maximum total size of memory ever used from the internal heap*/
#endif

static uint8_t mem_buf1_32[MEM_BUF_SMALL_SIZE];
static uint8_t mem_buf2_32[MEM_BUF_SMALL_SIZE];

static lv_mem_buf_t mem_buf_small[] = {{.p = mem_buf1_32, .size = MEM_BUF_SMALL_SIZE, .used = 0},
    {.p = mem_buf2_32, .size = MEM_BUF_SMALL_SIZE, .used = 0}
};

/**********************
 *      MACROS
 **********************/

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
void _lv_mem_init(void)
{
#if LV_MEM_CUSTOM == 0

#if LV_MEM_ADR == 0
    /*Allocate a large array to store the dynamically allocated data*/
    static LV_MEM_ATTR MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    work_mem = (uint8_t *)work_mem_int;
#else
    work_mem = (uint8_t *)LV_MEM_ADR;
#endif

    lv_mem_ent_t * full = (lv_mem_ent_t *)work_mem;
    full->header.s.used = 0;
    /*The total mem size reduced by the first header and the close patterns */
    full->header.s.d_size = LV_MEM_SIZE - sizeof(lv_mem_header_t);
#endif
}

/**
 * Clean up the memory buffer which frees all the allocated memories.
 * @note It work only if `LV_MEM_CUSTOM == 0`
 */
void _lv_mem_deinit(void)
{
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * full = (lv_mem_ent_t *)work_mem;
    full->header.s.used = 0;
    /*The total mem size reduced by the first header and the close patterns */
    full->header.s.d_size = LV_MEM_SIZE - sizeof(lv_mem_header_t);
#endif
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_mem_alloc(size_t size)
{
    if(size == 0) {
        return &zero_mem;
    }

    /*Round the size up to ALIGN_MASK*/
    size = (size + ALIGN_MASK) & (~ALIGN_MASK);
    void * alloc = NULL;

#if LV_MEM_CUSTOM == 0
    /*Use the built-in allocators*/
    lv_mem_ent_t * e = NULL;

    /* Search for a appropriate entry*/
    do {
        /* Get the next entry*/
        e = ent_get_next(e);

        /*If there is next entry then try to allocate there*/
        if(e != NULL) {
            alloc = ent_alloc(e, size);
        }
        /* End if there is not next entry OR the alloc. is successful*/
    } while(e != NULL && alloc == NULL);

#else
    /*Use custom, user defined malloc function*/
#if LV_ENABLE_GC == 1 /*gc must not include header*/
    alloc = LV_MEM_CUSTOM_ALLOC(size);
#else                 /* LV_ENABLE_GC */
    /*Allocate a header too to store the size*/
    alloc = LV_MEM_CUSTOM_ALLOC(size + sizeof(lv_mem_header_t));
    if(alloc != NULL) {
        ((lv_mem_ent_t *)alloc)->header.s.d_size = size;
        ((lv_mem_ent_t *)alloc)->header.s.used   = 1;

        alloc = &((lv_mem_ent_t *)alloc)->first_data;
    }
#endif                /* LV_ENABLE_GC */
#endif                /* LV_MEM_CUSTOM */

#if LV_MEM_ADD_JUNK
    if(alloc != NULL) _lv_memset(alloc, 0xaa, size);
#endif

    if(alloc == NULL) {
        LV_LOG_WARN("Couldn't allocate memory");
    }
    else {
#if LV_MEM_CUSTOM == 0
        /* just a safety check, should always be true */
        if((uintptr_t) alloc > (uintptr_t) work_mem) {
            if((((uintptr_t) alloc - (uintptr_t) work_mem) + size) > mem_max_size) {
                mem_max_size = ((uintptr_t) alloc - (uintptr_t) work_mem) + size;
            }
        }
#endif
    }

    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_mem_free(const void * data)
{
    if(data == &zero_mem) return;
    if(data == NULL) return;

#if LV_MEM_ADD_JUNK
    _lv_memset((void *)data, 0xbb, _lv_mem_get_size(data));
#endif

#if LV_ENABLE_GC == 0
    /*e points to the header*/
    lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *)data - sizeof(lv_mem_header_t));
    e->header.s.used = 0;
#endif

#if LV_MEM_CUSTOM == 0
#if LV_MEM_AUTO_DEFRAG
    static uint16_t full_defrag_cnt = 0;
    full_defrag_cnt++;
    if(full_defrag_cnt < LV_MEM_FULL_DEFRAG_CNT) {
        /* Make a simple defrag.
         * Join the following free entries after this*/
        lv_mem_ent_t * e_next;
        e_next = ent_get_next(e);
        while(e_next != NULL) {
            if(e_next->header.s.used == 0) {
                e->header.s.d_size += e_next->header.s.d_size + sizeof(e->header);
            }
            else {
                break;
            }
            e_next = ent_get_next(e_next);
        }
    }
    else {
        full_defrag_cnt = 0;
        lv_mem_defrag();

    }
#endif /*LV_MEM_AUTO_DEFRAG*/
#else /*Use custom, user defined free function*/
#if LV_ENABLE_GC == 0
    LV_MEM_CUSTOM_FREE(e);
#else
    LV_MEM_CUSTOM_FREE((void *)data);
#endif /*LV_ENABLE_GC*/
#endif
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */

#if LV_ENABLE_GC == 0

void * lv_mem_realloc(void * data_p, size_t new_size)
{
    /*Round the size up to ALIGN_MASK*/
    new_size = (new_size + ALIGN_MASK) & (~ALIGN_MASK);

    /*data_p could be previously freed pointer (in this case it is invalid)*/
    if(data_p != NULL) {
        lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *)data_p - sizeof(lv_mem_header_t));
        if(e->header.s.used == 0) {
            data_p = NULL;
        }
    }

    uint32_t old_size = _lv_mem_get_size(data_p);
    if(old_size == new_size) return data_p; /*Also avoid reallocating the same memory*/

#if LV_MEM_CUSTOM == 0
    /* Truncate the memory if the new size is smaller. */
    if(new_size < old_size) {
        lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *)data_p - sizeof(lv_mem_header_t));
        ent_trunc(e, new_size);
        return &e->first_data;
    }
#endif

    void * new_p;
    new_p = lv_mem_alloc(new_size);
    if(new_p == NULL) {
        LV_LOG_WARN("Couldn't allocate memory");
        return NULL;
    }

    if(data_p != NULL) {
        /*Copy the old data to the new. Use the smaller size*/
        if(old_size != 0 && new_size != 0) {
            _lv_memcpy(new_p, data_p, LV_MATH_MIN(new_size, old_size));
        }
        lv_mem_free(data_p);
    }

    return new_p;
}

#else /* LV_ENABLE_GC */

void * lv_mem_realloc(void * data_p, size_t new_size)
{
    void * new_p = LV_MEM_CUSTOM_REALLOC(data_p, new_size);
    if(new_p == NULL) LV_LOG_WARN("Couldn't allocate memory");
    return new_p;
}

#endif /* lv_enable_gc */

/**
 * Join the adjacent free memory blocks
 */
void lv_mem_defrag(void)
{
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * e_free;
    lv_mem_ent_t * e_next;
    e_free = ent_get_next(NULL);

    while(1) {
        /*Search the next free entry*/
        while(e_free != NULL) {
            if(e_free->header.s.used != 0) {
                e_free = ent_get_next(e_free);
            }
            else {
                break;
            }
        }

        if(e_free == NULL) return;

        /*Joint the following free entries to the free*/
        e_next = ent_get_next(e_free);
        while(e_next != NULL) {
            if(e_next->header.s.used == 0) {
                e_free->header.s.d_size += e_next->header.s.d_size + sizeof(e_next->header);
            }
            else {
                break;
            }

            e_next = ent_get_next(e_next);
        }

        if(e_next == NULL) return;

        /*Continue from the lastly checked entry*/
        e_free = e_next;
    }
#endif
}

lv_res_t lv_mem_test(void)
{
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * e;
    e = ent_get_next(NULL);
    while(e) {
        if(e->header.s.d_size > LV_MEM_SIZE) {
            return LV_RES_INV;
        }
        uint8_t * e8 = (uint8_t *) e;
        if(e8 + e->header.s.d_size > work_mem + LV_MEM_SIZE) {
            return LV_RES_INV;
        }
        e = ent_get_next(e);
    }
#endif
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
    _lv_memset(mon_p, 0, sizeof(lv_mem_monitor_t));
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * e;

    e = ent_get_next(NULL);

    while(e != NULL) {
        if(e->header.s.used == 0) {
            mon_p->free_cnt++;
            mon_p->free_size += e->header.s.d_size;
            if(e->header.s.d_size > mon_p->free_biggest_size) {
                mon_p->free_biggest_size = e->header.s.d_size;
            }
        }
        else {
            mon_p->used_cnt++;
        }

        e = ent_get_next(e);
    }
    mon_p->total_size = LV_MEM_SIZE;
    mon_p->max_used = mem_max_size;
    mon_p->used_pct = 100 - (100U * mon_p->free_size) / mon_p->total_size;
    if(mon_p->free_size > 0) {
        mon_p->frag_pct = mon_p->free_biggest_size * 100U / mon_p->free_size;
        mon_p->frag_pct = 100 - mon_p->frag_pct;
    }
    else {
        mon_p->frag_pct = 0; /*no fragmentation if all the RAM is used*/
    }
#endif
}

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes
 */

#if LV_ENABLE_GC == 0

uint32_t _lv_mem_get_size(const void * data)
{
    if(data == NULL) return 0;
    if(data == &zero_mem) return 0;

    lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *)data - sizeof(lv_mem_header_t));

    return e->header.s.d_size;
}

#else /* LV_ENABLE_GC */

uint32_t _lv_mem_get_size(const void * data)
{
    return LV_MEM_CUSTOM_GET_SIZE(data);
}

#endif /*LV_ENABLE_GC*/

/**
 * Get a temporal buffer with the given size.
 * @param size the required size
 */
void * _lv_mem_buf_get(uint32_t size)
{
    if(size == 0) return NULL;

    /*Try small static buffers first*/
    uint8_t i;
    if(size <= MEM_BUF_SMALL_SIZE) {
        for(i = 0; i < sizeof(mem_buf_small) / sizeof(mem_buf_small[0]); i++) {
            if(mem_buf_small[i].used == 0) {
                mem_buf_small[i].used = 1;
                return mem_buf_small[i].p;
            }
        }
    }

    /*Try to find a free buffer with suitable size */
    int8_t i_guess = -1;
    for(i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(_lv_mem_buf[i]).used == 0 && LV_GC_ROOT(_lv_mem_buf[i]).size >= size) {
            if(LV_GC_ROOT(_lv_mem_buf[i]).size == size) {
                LV_GC_ROOT(_lv_mem_buf[i]).used = 1;
                return LV_GC_ROOT(_lv_mem_buf[i]).p;
            }
            else if(i_guess < 0) {
                i_guess = i;
            }
            /*If size of `i` is closer to `size` prefer it*/
            else if(LV_GC_ROOT(_lv_mem_buf[i]).size < LV_GC_ROOT(_lv_mem_buf[i_guess]).size) {
                i_guess = i;
            }
        }
    }

    if(i_guess >= 0) {
        LV_GC_ROOT(_lv_mem_buf[i_guess]).used = 1;
        return LV_GC_ROOT(_lv_mem_buf[i_guess]).p;
    }

    /*Reallocate a free buffer*/
    for(i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(_lv_mem_buf[i]).used == 0) {
            /*if this fails you probably need to increase your LV_MEM_SIZE/heap size*/
            void * buf = lv_mem_realloc(LV_GC_ROOT(_lv_mem_buf[i]).p, size);
            if(buf == NULL) {
                LV_DEBUG_ASSERT(false, "Out of memory, can't allocate a new buffer (increase your LV_MEM_SIZE/heap size)", 0x00);
                return NULL;
            }
            LV_GC_ROOT(_lv_mem_buf[i]).used = 1;
            LV_GC_ROOT(_lv_mem_buf[i]).size = size;
            LV_GC_ROOT(_lv_mem_buf[i]).p    = buf;
            return LV_GC_ROOT(_lv_mem_buf[i]).p;
        }
    }

    LV_DEBUG_ASSERT(false, "No free buffer. Increase LV_MEM_BUF_MAX_NUM.", 0x00);
    return NULL;
}

/**
 * Release a memory buffer
 * @param p buffer to release
 */
void _lv_mem_buf_release(void * p)
{
    uint8_t i;

    /*Try small static buffers first*/
    for(i = 0; i < sizeof(mem_buf_small) / sizeof(mem_buf_small[0]); i++) {
        if(mem_buf_small[i].p == p) {
            mem_buf_small[i].used = 0;
            return;
        }
    }

    for(i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(_lv_mem_buf[i]).p == p) {
            LV_GC_ROOT(_lv_mem_buf[i]).used = 0;
            return;
        }
    }

    LV_LOG_ERROR("lv_mem_buf_release: p is not a known buffer")
}

/**
 * Free all memory buffers
 */
void _lv_mem_buf_free_all(void)
{
    uint8_t i;
    for(i = 0; i < sizeof(mem_buf_small) / sizeof(mem_buf_small[0]); i++) {
        mem_buf_small[i].used = 0;
    }

    for(i = 0; i < LV_MEM_BUF_MAX_NUM; i++) {
        if(LV_GC_ROOT(_lv_mem_buf[i]).p) {
            lv_mem_free(LV_GC_ROOT(_lv_mem_buf[i]).p);
            LV_GC_ROOT(_lv_mem_buf[i]).p = NULL;
            LV_GC_ROOT(_lv_mem_buf[i]).used = 0;
            LV_GC_ROOT(_lv_mem_buf[i]).size = 0;
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
LV_ATTRIBUTE_FAST_MEM void * _lv_memcpy(void * dst, const void * src, size_t len)
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
LV_ATTRIBUTE_FAST_MEM void _lv_memset(void * dst, uint8_t v, size_t len)
{

    uint8_t * d8 = (uint8_t *) dst;

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

    uint32_t v32 = v + (v << 8) + (v << 16) + (v << 24);

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
LV_ATTRIBUTE_FAST_MEM void _lv_memset_00(void * dst, size_t len)
{
    uint8_t * d8 = (uint8_t *) dst;
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
LV_ATTRIBUTE_FAST_MEM void _lv_memset_ff(void * dst, size_t len)
{
    uint8_t * d8 = (uint8_t *) dst;
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
/**
 * Give the next entry after 'act_e'
 * @param act_e pointer to an entry
 * @return pointer to an entry after 'act_e'
 */
static lv_mem_ent_t * ent_get_next(lv_mem_ent_t * act_e)
{
    lv_mem_ent_t * next_e = NULL;

    if(act_e == NULL) { /*NULL means: get the first entry*/
        next_e = (lv_mem_ent_t *)work_mem;
    }
    else {   /*Get the next entry */
        uint8_t * data = &act_e->first_data;
        next_e         = (lv_mem_ent_t *)&data[act_e->header.s.d_size];

        if(&next_e->first_data >= &work_mem[LV_MEM_SIZE]) next_e = NULL;
    }

    return next_e;
}

/**
 * Try to do the real allocation with a given size
 * @param e try to allocate to this entry
 * @param size size of the new memory in bytes
 * @return pointer to the allocated memory or NULL if not enough memory in the entry
 */
static void * ent_alloc(lv_mem_ent_t * e, size_t size)
{
    void * alloc = NULL;
    /*If the memory is free and big enough then use it */
    if(e->header.s.used == 0 && e->header.s.d_size >= size) {
        /*Truncate the entry to the desired size */
        ent_trunc(e, size);
        e->header.s.used = 1;

        /*Save the allocated data*/
        alloc = &e->first_data;
    }

    return alloc;
}

/**
 * Truncate the data of entry to the given size
 * @param e Pointer to an entry
 * @param size new size in bytes
 */
static void ent_trunc(lv_mem_ent_t * e, size_t size)
{
    /*Round the size up to ALIGN_MASK*/
    size = (size + ALIGN_MASK) & (~ALIGN_MASK);

    /*Don't let empty space only for a header without data*/
    if(e->header.s.d_size == size + sizeof(lv_mem_header_t)) {
        size = e->header.s.d_size;
    }

    /* Create the new entry after the current if there is space for it */
    if(e->header.s.d_size != size) {
        uint8_t * e_data             = &e->first_data;
        lv_mem_ent_t * after_new_e   = (lv_mem_ent_t *)&e_data[size];
        after_new_e->header.s.used   = 0;
        after_new_e->header.s.d_size = (uint32_t)e->header.s.d_size - size - sizeof(lv_mem_header_t);

        /* Set the new size for the original entry */
        e->header.s.d_size = (uint32_t)size;
    }
}

#endif
