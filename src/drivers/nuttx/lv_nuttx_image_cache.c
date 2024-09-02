/**
 * @file lv_nuttx_image_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_image_cache.h"
#include "../../core/lv_global.h"
#include "../../../lvgl.h"

#if LV_USE_NUTTX

#include "../../draw/lv_draw_buf_private.h"
#include <nuttx/mm/mm.h>

/*********************
 *      DEFINES
 *********************/

#define HEAP_NAME "GImageCache"

#define img_cache_p         (LV_GLOBAL_DEFAULT()->img_cache)
#define img_header_cache_p  (LV_GLOBAL_DEFAULT()->img_header_cache)
#define ctx                 (*(lv_nuttx_ctx_image_cache_t **)&LV_GLOBAL_DEFAULT()->nuttx_ctx->image_cache)
#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t * mem;
    uint32_t mem_size;

    char name[sizeof(HEAP_NAME) + 10]; /**< +10 characters to store task pid. */

    struct mm_heap_s * heap;
    uint32_t heap_size;

    bool initialized;
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
    lv_draw_buf_handlers_t * handlers = image_cache_draw_buf_handlers;
    handlers->buf_malloc_cb = malloc_cb;
    handlers->buf_free_cb = free_cb;

    ctx = lv_malloc_zeroed(sizeof(lv_nuttx_ctx_image_cache_t));
    LV_ASSERT_MALLOC(ctx);

    ctx->initialized = false;
}

void lv_nuttx_image_cache_deinit(void)
{
    if(ctx->initialized == false) goto FREE_CONTEXT;

    mm_uninitialize(ctx->heap);
    free(ctx->mem);

FREE_CONTEXT:
    lv_free(ctx);

    ctx = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool defer_init(void)
{
    if(ctx->mem != NULL && ctx->heap != NULL) {
        return true;
    }

    if(lv_image_cache_is_enabled() == false) {
        LV_LOG_INFO("Image cache is not initialized yet. Skipping deferred initialization. Because max_size is 0.");
        return false;
    }

    ctx->mem_size = img_cache_p->max_size;
    ctx->mem = malloc(ctx->mem_size);
    LV_ASSERT_MALLOC(ctx->mem);

    if(ctx->mem == NULL) {
        LV_LOG_ERROR("Failed to allocate memory for image cache");
        ctx->initialized = false;
        return false;
    }

    lv_snprintf(ctx->name, sizeof(ctx->name), HEAP_NAME "[%-4" LV_PRIu32 "]", (uint32_t)gettid());

    ctx->heap = mm_initialize(
                    ctx->name,
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

    ctx->initialized = true;
    return true;
}

static void * malloc_cb(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    if(ctx->initialized == false) {
        if(defer_init() == false) return NULL;
    }

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
