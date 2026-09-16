/**
 * @file lv_nanovg_fbo_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nanovg_fbo_cache.h"

#if LV_USE_DRAW_NANOVG

#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_utils.h"
#include "../../libs/nanovg/nanovg_gl_utils.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_NANOVG_FBO_POOL_MAX_UNUSED_TEXTURE_MEMORY
    #define LV_NANOVG_FBO_POOL_MAX_UNUSED_TEXTURE_MEMORY (6 * 1024 * 1024)
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_nanovg_fbo_t {
    struct NVGLUframebuffer * fbo;
    int width;
    int height;
    int flags;
    int format;
    bool in_use;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t fbo_bytes(const lv_nanovg_fbo_t * item);
static void pool_trim(lv_draw_nanovg_unit_t * u);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nanovg_fbo_cache_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    lv_ll_init(&u->fbo_pool, sizeof(lv_nanovg_fbo_t));
}

void lv_nanovg_fbo_cache_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);

    lv_nanovg_fbo_t * item;
    LV_LL_READ(&u->fbo_pool, item) {
        if(item->in_use) {
            LV_LOG_WARN("Framebuffer %p is still in use", (void *)item);
        }
        nvgluDeleteFramebuffer(item->fbo);
    }

    lv_ll_clear(&u->fbo_pool);
}

lv_nanovg_fbo_t * lv_nanovg_fbo_cache_get(lv_draw_nanovg_unit_t * u, int width, int height, int flags,
                                          int format)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(u);

    /* Try to find an already allocated framebuffer in the pool
     * by matching size, flags and format */
    lv_nanovg_fbo_t * item;
    LV_LL_READ(&u->fbo_pool, item) {
        if(item->in_use) {
            continue;
        }
        if(item->width != width || item->height != height) {
            continue;
        }
        if(item->flags != flags || item->format != format) {
            continue;
        }

        item->in_use = true;
        LV_PROFILER_DRAW_END;
        return item;
    }

    /* no free buffer matches the requirements, allocate a new entry in the pool */
    item = lv_ll_ins_head(&u->fbo_pool);
    if(item == NULL) {
        LV_LOG_ERROR("Failed to allocate the framebuffer pool entry");
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    lv_memzero(item, sizeof(*item));
    item->width = width;
    item->height = height;
    item->flags = flags;
    item->format = format;
    item->fbo = nvgluCreateFramebuffer(u->vg, width, height, flags, format);

    if(item->fbo == NULL) {
        LV_LOG_ERROR("Failed to create the framebuffer");
        lv_ll_remove(&u->fbo_pool, item);
        lv_free(item);
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    item->in_use = true;

    LV_PROFILER_DRAW_END;
    return item;
}

void lv_nanovg_fbo_cache_release(lv_draw_nanovg_unit_t * u, lv_nanovg_fbo_t * item)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(item);

    item->in_use = false;

    /* Move it to the head to keep the most recently used framebuffers at the start of the list */
    const bool head = true;
    lv_ll_chg_list(&u->fbo_pool, &u->fbo_pool, item, head);

    pool_trim(u);
}

struct NVGLUframebuffer * lv_nanovg_fbo_cache_entry_to_fb(lv_nanovg_fbo_t * item)
{
    LV_ASSERT_NULL(item);
    return item->fbo;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t fbo_bytes(const lv_nanovg_fbo_t * item)
{
    /* Every format the unit asks for is 4 bytes per pixel */
    return (uint32_t)item->width * (uint32_t)item->height * 4;
}

static void pool_trim(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT(u != NULL);
    uint32_t free_bytes = 0;
    lv_nanovg_fbo_t * item;
    LV_LL_READ(&u->fbo_pool, item) {
        if(item->in_use) {
            continue;
        }
        free_bytes += fbo_bytes(item);
    }

    /* Drop the least recently released ones until the free set fits the budget */
    while(free_bytes > LV_NANOVG_FBO_POOL_MAX_UNUSED_TEXTURE_MEMORY) {
        lv_nanovg_fbo_t * victim = NULL;
        LV_LL_READ_BACK(&u->fbo_pool, item) {
            if(!item->in_use) {
                victim = item;
                break;
            }
        }
        if(victim == NULL) {
            break;
        }

        free_bytes -= fbo_bytes(victim);
        nvgluDeleteFramebuffer(victim->fbo);
        lv_ll_remove(&u->fbo_pool, victim);
        lv_free(victim);
    }
}

#endif /* LV_USE_DRAW_NANOVG */
