/**
 * @file lv_nuttx_image_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_cache.h"
#include "../../../lvgl.h"

#if LV_CACHE_DEF_SIZE > 0

#if LV_USE_NUTTX

#include <nuttx/mm/mm.h>

/*********************
 *      DEFINES
 *********************/

#define HEAP_NAME "GImageCache"

#define img_cache_p         (LV_GLOBAL_DEFAULT()->img_cache)
#define img_header_cache_p  (LV_GLOBAL_DEFAULT()->img_header_cache)
#define ctx                 (*(lv_nuttx_ctx_image_cache_t **)&LV_GLOBAL_DEFAULT()->nuttx_ctx->image_cache)
/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t * mem;
    uint32_t mem_size;

    struct mm_heap_s * heap;
    uint32_t heap_size;
} lv_nuttx_ctx_image_cache_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * malloc_cb(size_t size_bytes, lv_color_format_t color_format);
static void free_cb(void * draw_buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nuttx_image_cache_init(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->buf_malloc_cb = malloc_cb;
    handlers->buf_free_cb = free_cb;

    ctx = lv_malloc_zeroed(sizeof(lv_nuttx_ctx_image_cache_t));
    LV_ASSERT_MALLOC(ctx);

    ctx->mem_size = LV_CACHE_DEF_SIZE;
    ctx->mem = malloc(ctx->mem_size);
    LV_ASSERT_MALLOC(ctx->mem);

    ctx->heap = mm_initialize(
                    HEAP_NAME,
                    ctx->mem,
                    ctx->mem_size
                );

    struct mallinfo info  = mm_mallinfo(ctx->heap);
    ctx->heap_size = info.arena;

    LV_LOG_USER("heap info:");
    LV_LOG_USER("  heap: %p", ctx->heap);
    LV_LOG_USER("  mem: %p", ctx->mem);
    LV_LOG_USER("  mem_size: %" LV_PRIu32, ctx->mem_size);
    LV_LOG_USER("  arena: %d", info.arena);
    LV_LOG_USER("  ordblks: %d", info.ordblks);
    LV_LOG_USER("  aordblks: %d", info.aordblks);
    LV_LOG_USER("  mxordblk: %d", info.mxordblk);
    LV_LOG_USER("  uordblks: %d", info.uordblks);
    LV_LOG_USER("  fordblks: %d", info.fordblks);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * malloc_cb(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    /*Allocate larger memory to be sure it can be aligned as needed*/
    size_bytes += LV_DRAW_BUF_ALIGN - 1;
    uint32_t cache_max_size = lv_cache_get_max_size(img_cache_p, NULL);

    if(size_bytes > cache_max_size) {
        LV_LOG_ERROR("data size (%" LV_PRIu32 ") is larger than max size (%" LV_PRIu32 ")",
                     (uint32_t)size_bytes,
                     cache_max_size);
        return NULL;
    }

    while(1) {
        void * mem = mm_malloc(ctx->heap, size_bytes);
        if(mem) return mem;
        LV_LOG_INFO("appears to be out of memory. attempting to evict one cache entry. with allocated size %" LV_PRIu32,
                    (uint32_t)size_bytes);
        bool evict_res = lv_cache_evict_one(img_cache_p, NULL);
        if(evict_res == false) {
            LV_LOG_ERROR("failed to evict one cache entry");
            return NULL;
        }
    }
}

static void free_cb(void * draw_buf)
{
    mm_free(ctx->heap, draw_buf);
}

#endif /* LV_USE_NUTTX */
#endif /* LV_CACHE_DEF_SIZE > 0 */
