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
    bool independent_image_heap;

    lv_draw_buf_malloc_cb malloc_cb;
    lv_draw_buf_free_cb free_cb;

#if LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    lv_draw_buf_malloc_cb malloc_cb_default;
    lv_draw_buf_free_cb free_cb_default;
#endif
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

void lv_nuttx_image_cache_init(bool use_independent_image_heap)
{
    lv_draw_buf_handlers_t * handlers = image_cache_draw_buf_handlers;

    ctx = lv_malloc_zeroed(sizeof(lv_nuttx_ctx_image_cache_t));
    LV_ASSERT_MALLOC(ctx);

    ctx->malloc_cb = handlers->buf_malloc_cb;
    ctx->free_cb = handlers->buf_free_cb;

    handlers->buf_malloc_cb = malloc_cb;
    handlers->buf_free_cb = free_cb;

#if LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    handlers = lv_draw_buf_get_handlers();
    ctx->malloc_cb_default = handlers->buf_malloc_cb;
    ctx->free_cb_default = handlers->buf_free_cb;

    handlers->buf_malloc_cb = malloc_cb;
    handlers->buf_free_cb = free_cb;
#endif

    ctx->initialized = false;
    ctx->independent_image_heap = use_independent_image_heap;
}

void lv_nuttx_image_cache_deinit(void)
{
    if(ctx->independent_image_heap == false) goto FREE_CONTEXT;
    if(ctx->initialized == false) goto FREE_CONTEXT;

    mm_uninitialize(ctx->heap);
    free(ctx->mem);

FREE_CONTEXT:
    lv_draw_buf_handlers_t * handlers = image_cache_draw_buf_handlers;
    handlers->buf_malloc_cb = ctx->malloc_cb;
    handlers->buf_free_cb = ctx->free_cb;

#if LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    handlers = lv_draw_buf_get_handlers();
    handlers->buf_malloc_cb = ctx->malloc_cb_default;
    handlers->buf_free_cb = ctx->free_cb_default;
#endif

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

static void heap_memdump(struct mm_heap_s * heap)
{
    struct mm_memdump_s dump = {
        PID_MM_ALLOC,
#if CONFIG_MM_BACKTRACE >= 0
        0,
        ULONG_MAX
#endif
    };

    mm_memdump(heap, &dump);
}

static void * malloc_cb(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);
    if(ctx->independent_image_heap == true && ctx->initialized == false) {
        if(defer_init() == false) return NULL;
    }

    /*Allocate larger memory to be sure it can be aligned as needed*/
    size_bytes += LV_DRAW_BUF_ALIGN - 1;
    uint32_t cache_max_size = lv_cache_get_max_size(img_cache_p, NULL);

    if(lv_cache_is_enabled(img_cache_p) && size_bytes > cache_max_size) {
        LV_LOG_ERROR("data size (%" LV_PRIu32 ") is larger than max size (%" LV_PRIu32 ")",
                     (uint32_t)size_bytes,
                     cache_max_size);
        return NULL;
    }

    while(1) {
        void * mem = NULL;
        if(ctx->independent_image_heap) {
            mem = mm_malloc(ctx->heap, size_bytes);
        }
        else if((!lv_cache_is_enabled(img_cache_p)) || (lv_cache_get_size(img_cache_p, NULL) + size_bytes < cache_max_size)) {
            mem = ctx->malloc_cb(size_bytes, color_format);
        }
        if(mem) return mem;
        LV_LOG_INFO("appears to be out of memory. attempting to evict one cache entry. with allocated size %" LV_PRIu32,
                    (uint32_t)size_bytes);
        bool evict_res = lv_cache_evict_one(img_cache_p, NULL);
        if(evict_res == false) {
            LV_LOG_ERROR("failed to evict one cache entry");
            heap_memdump(ctx->heap);
            return NULL;
        }
    }
}

static void free_cb(void * draw_buf)
{
    if(ctx->independent_image_heap == true) {
        mm_free(ctx->heap, draw_buf);
    }
    else {
        ctx->free_cb(draw_buf);
    }
}

#endif /* LV_USE_NUTTX */
