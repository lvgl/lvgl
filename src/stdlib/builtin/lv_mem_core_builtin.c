/**
 * @file lv_mem_core_builtin.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_mem.h"
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

#include "lv_tlsf.h"
#include "../lv_string.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_ll.h"
#include "../../misc/lv_math.h"
#include "../../osal/lv_os.h"
#include "../../core/lv_global.h"

#ifdef LV_MEM_POOL_INCLUDE
    #include LV_MEM_POOL_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
/*memset the allocated memories to 0xaa and freed memories to 0xbb (just for testing purposes)*/
#ifndef LV_MEM_ADD_JUNK
    #define LV_MEM_ADD_JUNK  0
#endif

#ifdef LV_ARCH_64
    #define MEM_UNIT         uint64_t
    #define ALIGN_MASK       0x7
#else
    #define MEM_UNIT         uint32_t
    #define ALIGN_MASK       0x3
#endif
#define state LV_GLOBAL_DEFAULT()->tlsf_state

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_mem_walker(void * ptr, size_t size, int used, void * user);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_MEM
    #define LV_TRACE_MEM(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_MEM(...)
#endif

#define _COPY(d, s) *d = *s; d++; s++;
#define _SET(d, v) *d = v; d++;
#define _REPEAT8(expr) expr expr expr expr expr expr expr expr

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_mem_init(void)
{
#if LV_USE_OS
    lv_mutex_init(&state.mutex);
#endif

#if LV_MEM_ADR == 0
#ifdef LV_MEM_POOL_ALLOC
    state.tlsf = lv_tlsf_create_with_pool((void *)LV_MEM_POOL_ALLOC(LV_MEM_SIZE), LV_MEM_SIZE);
#else
    /*Allocate a large array to store the dynamically allocated data*/
    static LV_ATTRIBUTE_LARGE_RAM_ARRAY MEM_UNIT work_mem_int[LV_MEM_SIZE / sizeof(MEM_UNIT)];
    state.tlsf = lv_tlsf_create_with_pool((void *)work_mem_int, LV_MEM_SIZE);
#endif
#else
    state.tlsf = lv_tlsf_create_with_pool((void *)LV_MEM_ADR, LV_MEM_SIZE);
#endif

    lv_ll_init(&state.pool_ll, sizeof(lv_pool_t));

    /*Record the first pool*/
    lv_pool_t * pool_p = lv_ll_ins_tail(&state.pool_ll);
    LV_ASSERT_MALLOC(pool_p);
    *pool_p = lv_tlsf_get_pool(state.tlsf);

#if LV_MEM_ADD_JUNK
    LV_LOG_WARN("LV_MEM_ADD_JUNK is enabled which makes LVGL much slower");
#endif
}

void lv_mem_deinit(void)
{
    lv_ll_clear(&state.pool_ll);
    lv_tlsf_destroy(state.tlsf);
#if LV_USE_OS
    lv_mutex_delete(&state.mutex);
#endif
}

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes)
{
    lv_mem_pool_t new_pool = lv_tlsf_add_pool(state.tlsf, mem, bytes);
    if(!new_pool) {
        LV_LOG_WARN("failed to add memory pool, address: %p, size: %zu", mem, bytes);
        return NULL;
    }

    lv_pool_t * pool_p = lv_ll_ins_tail(&state.pool_ll);
    LV_ASSERT_MALLOC(pool_p);
    *pool_p = new_pool;

    return new_pool;
}

void lv_mem_remove_pool(lv_mem_pool_t pool)
{
    lv_pool_t * pool_p;
    LV_LL_READ(&state.pool_ll, pool_p) {
        if(*pool_p == pool) {
            lv_ll_remove(&state.pool_ll, pool_p);
            lv_free(pool_p);
            lv_tlsf_remove_pool(state.tlsf, pool);
            return;
        }
    }
    LV_LOG_WARN("invalid pool: %p", pool);
}

void * lv_malloc_core(size_t size)
{
#if LV_USE_OS
    lv_mutex_lock(&state.mutex);
#endif
    void * p = lv_tlsf_malloc(state.tlsf, size);

    if(p) {
        state.cur_used += lv_tlsf_block_size(p);
        state.max_used = LV_MAX(state.cur_used, state.max_used);
    }

#if LV_USE_OS
    lv_mutex_unlock(&state.mutex);
#endif
    return p;
}

void * lv_realloc_core(void * p, size_t new_size)
{
#if LV_USE_OS
    lv_mutex_lock(&state.mutex);
#endif

    size_t old_size = lv_tlsf_block_size(p);
    void * p_new = lv_tlsf_realloc(state.tlsf, p, new_size);

    if(p_new) {
        state.cur_used -= old_size;
        state.cur_used += lv_tlsf_block_size(p_new);
        state.max_used = LV_MAX(state.cur_used, state.max_used);
    }
#if LV_USE_OS
    lv_mutex_unlock(&state.mutex);
#endif

    return p_new;
}

void lv_free_core(void * p)
{
#if LV_USE_OS
    lv_mutex_lock(&state.mutex);
#endif

#if LV_MEM_ADD_JUNK
    lv_memset(p, 0xbb, lv_tlsf_block_size(data));
#endif
    size_t size = lv_tlsf_block_size(p);
    lv_tlsf_free(state.tlsf, p);
    if(state.cur_used > size) state.cur_used -= size;
    else state.cur_used = 0;

#if LV_USE_OS
    lv_mutex_unlock(&state.mutex);
#endif
}

void lv_mem_monitor_core(lv_mem_monitor_t * mon_p)
{
    /*Init the data*/
    lv_memzero(mon_p, sizeof(lv_mem_monitor_t));
    LV_TRACE_MEM("begin");

    lv_pool_t * pool_p;
    LV_LL_READ(&state.pool_ll, pool_p) {
        lv_tlsf_walk_pool(*pool_p, lv_mem_walker, mon_p);
    }

    mon_p->used_pct = 100 - (uint64_t)100U * mon_p->free_size / mon_p->total_size;
    if(mon_p->free_size > 0) {
        mon_p->frag_pct = (uint64_t)mon_p->free_biggest_size * 100U / mon_p->free_size;
        mon_p->frag_pct = 100 - mon_p->frag_pct;
    }
    else {
        mon_p->frag_pct = 0; /*no fragmentation if all the RAM is used*/
    }

    mon_p->max_used = state.max_used;

    LV_TRACE_MEM("finished");
}

lv_result_t lv_mem_test_core(void)
{
#if LV_USE_OS
    lv_mutex_lock(&state.mutex);
#endif
    if(lv_tlsf_check(state.tlsf)) {
        LV_LOG_WARN("failed");
#if LV_USE_OS
        lv_mutex_unlock(&state.mutex);
#endif
        return LV_RESULT_INVALID;
    }

    lv_pool_t * pool_p;
    LV_LL_READ(&state.pool_ll, pool_p) {
        if(lv_tlsf_check_pool(*pool_p)) {
            LV_LOG_WARN("pool failed");
#if LV_USE_OS
            lv_mutex_unlock(&state.mutex);
#endif
            return LV_RESULT_INVALID;
        }
    }

    LV_TRACE_MEM("passed");
#if LV_USE_OS
    lv_mutex_unlock(&state.mutex);
#endif
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_mem_walker(void * ptr, size_t size, int used, void * user)
{
    LV_UNUSED(ptr);

    lv_mem_monitor_t * mon_p = user;
    mon_p->total_size += size;
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
#endif /*LV_STDLIB_BUILTIN*/
