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

#define LV_NANOVG_FBO_CACHE_CNT 4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* context */
    lv_draw_nanovg_unit_t * u;

    /* key */
    int width;
    int height;
    int flags;
    enum NVGtexture format;

    /* value */
    struct NVGLUframebuffer * fbo;
} fbo_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool fbo_create_cb(fbo_item_t * item, void * user_data);
static void fbo_free_cb(fbo_item_t * item, void * user_data);
static lv_cache_compare_res_t fbo_compare_cb(const fbo_item_t * lhs, const fbo_item_t * rhs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nanovg_fbo_cache_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->fbo_cache == NULL);

    const lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)fbo_compare_cb,
        .create_cb = (lv_cache_create_cb_t)fbo_create_cb,
        .free_cb = (lv_cache_free_cb_t)fbo_free_cb,
    };

    u->fbo_cache = lv_cache_create(&lv_cache_class_lru_ll_count, sizeof(fbo_item_t), LV_NANOVG_FBO_CACHE_CNT, ops);
    lv_cache_set_name(u->fbo_cache, "NVG_FBO");
}

void lv_nanovg_fbo_cache_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->fbo_cache);

    lv_cache_destroy(u->fbo_cache, NULL);
    u->fbo_cache = NULL;
}

struct _lv_cache_entry_t * lv_nanovg_fbo_cache_get(lv_draw_nanovg_unit_t * u, int width, int height, int flags,
                                                   int format)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(u);

    fbo_item_t search_key = { 0 };
    search_key.u = u;
    search_key.width = width;
    search_key.height = height;
    search_key.flags = flags;
    search_key.format = format;

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(u->fbo_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        cache_node_entry = lv_cache_acquire_or_create(u->fbo_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("FBO cache creating failed");
            LV_PROFILER_DRAW_END;
            return NULL;
        }
    }

    LV_PROFILER_DRAW_END;
    return cache_node_entry;
}

void lv_nanovg_fbo_cache_release(struct _lv_draw_nanovg_unit_t * u, struct _lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(entry);
    lv_cache_release(u->fbo_cache, entry, NULL);
}

struct NVGLUframebuffer * lv_nanovg_fbo_cache_entry_to_fb(struct _lv_cache_entry_t * entry)
{
    LV_ASSERT_NULL(entry);
    fbo_item_t * fbo_item = lv_cache_entry_get_data(entry);
    return fbo_item->fbo;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool fbo_create_cb(fbo_item_t * item, void * user_data)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_UNUSED(user_data);

    item->fbo = nvgluCreateFramebuffer(item->u->vg, item->width, item->height, item->flags, item->format);
    if(!item->fbo) {
        LV_LOG_ERROR("Failed to create FBO");
    }

    LV_PROFILER_DRAW_END;
    return item->fbo != NULL;
}

static void fbo_free_cb(fbo_item_t * item, void * user_data)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_UNUSED(user_data);

    nvgluDeleteFramebuffer(item->fbo);

    LV_PROFILER_DRAW_END;
}

static lv_cache_compare_res_t fbo_compare_cb(const fbo_item_t * lhs, const fbo_item_t * rhs)
{
    if(lhs->width != rhs->width) {
        return lhs->width > rhs->width ? 1 : -1;
    }

    if(lhs->height != rhs->height) {
        return lhs->height > rhs->height ? 1 : -1;
    }

    if(lhs->flags != rhs->flags) {
        return lhs->flags > rhs->flags ? 1 : -1;
    }

    if(lhs->format != rhs->format) {
        return lhs->format > rhs->format ? 1 : -1;
    }

    return 0;
}

#endif /* LV_USE_DRAW_NANOVG */
