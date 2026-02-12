/**
 * @file lv_nanovg_image_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nanovg_image_cache.h"

#if LV_USE_DRAW_NANOVG

#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_utils.h"
#include "../lv_image_decoder_private.h"
#include "../../misc/lv_pending.h"
#include "../../misc/lv_iter.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

typedef struct {
    /* context */
    lv_draw_nanovg_unit_t * u;

    /* key */
    lv_draw_buf_t src_buf;
    lv_color32_t color;
    int image_flags;

    /* for drop search */
    const void * src;
    lv_image_src_t src_type;

    /* value */
    int image_handle;
} image_item_t;

/**********************
*  STATIC PROTOTYPES
**********************/

static void image_cache_release_cb(void * entry, void * user_data);
static bool image_create_cb(image_item_t * item, void * user_data);
static void image_free_cb(image_item_t * item, void * user_data);
static lv_cache_compare_res_t image_compare_cb(const image_item_t * lhs, const image_item_t * rhs);
static void image_cache_drop_collect_cb(void * elem);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_nanovg_image_cache_init(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->image_cache == NULL);
    LV_ASSERT(u->image_pending == NULL);

    const lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)image_compare_cb,
        .create_cb = (lv_cache_create_cb_t)image_create_cb,
        .free_cb = (lv_cache_free_cb_t)image_free_cb,
    };

    u->image_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(image_item_t), LV_NANOVG_IMAGE_CACHE_CNT, ops);
    lv_cache_set_name(u->image_cache, "NVG_IMAGE");
    u->image_pending = lv_pending_create(sizeof(lv_cache_entry_t *), 4);
    lv_pending_set_free_cb(u->image_pending, image_cache_release_cb, u->image_cache);

    lv_ll_init(&u->image_drop_ll, sizeof(image_item_t));
}

void lv_nanovg_image_cache_deinit(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->image_cache);
    LV_ASSERT(u->image_pending);

    lv_pending_destroy(u->image_pending);
    u->image_pending = NULL;

    lv_cache_destroy(u->image_cache, NULL);
    u->image_cache = NULL;
}

int lv_nanovg_image_cache_get_handle(struct _lv_draw_nanovg_unit_t * u,
                                     const void * src,
                                     lv_color32_t color,
                                     int image_flags,
                                     lv_image_header_t * header)
{
    LV_PROFILER_DRAW_BEGIN;

    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(src);

    lv_image_decoder_args_t args;
    lv_memzero(&args, sizeof(lv_image_decoder_args_t));

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, src, &args);
    if(res != LV_RESULT_OK) {
        lv_image_src_t type = lv_image_src_get_type(src);
        LV_UNUSED(type);
        LV_LOG_WARN("Failed to open image: type: %d, src: %p (%s)", type, src,
                    type == LV_IMAGE_SRC_FILE ? (const char *)src : "var");
        LV_PROFILER_DRAW_END;
        return -1;
    }

    const lv_draw_buf_t * decoded = decoder_dsc.decoded;
    if(decoded == NULL || decoded->data == NULL) {
        lv_image_decoder_close(&decoder_dsc);
        LV_LOG_ERROR("image data is NULL");
        LV_PROFILER_DRAW_END;
        return -1;
    }

    if(header) {
        *header = decoder_dsc.header;
    }

    image_item_t search_key = { 0 };
    search_key.u = u;
    search_key.src_buf = *decoded;
    search_key.color = color;
    search_key.image_flags = image_flags;
    search_key.src = src;
    search_key.src_type = lv_image_src_get_type(src);

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(u->image_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        /* check if the cache is full */
        size_t free_size = lv_cache_get_free_size(u->image_cache, NULL);
        if(free_size == 0) {
            LV_LOG_INFO("image cache is full, release all pending cache entries");
            lv_nanovg_end_frame(u);
        }

        cache_node_entry = lv_cache_acquire_or_create(u->image_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("image cache creating failed");
            lv_image_decoder_close(&decoder_dsc);
            LV_PROFILER_DRAW_END;
            return -1;
        }
    }

    lv_image_decoder_close(&decoder_dsc);

    /* Add the new entry to the pending list */
    lv_pending_add(u->image_pending, &cache_node_entry);

    image_item_t * image_item = lv_cache_entry_get_data(cache_node_entry);

    LV_PROFILER_DRAW_END;
    return image_item->image_handle;
}

void lv_nanovg_image_cache_drop(struct _lv_draw_nanovg_unit_t * u, const void * src)
{
    LV_ASSERT_NULL(u);
    LV_UNUSED(src);
    if(src == NULL) {
        lv_cache_drop_all(u->image_cache, NULL);
        return;
    }

    u->image_drop_src = src;

    lv_iter_t * iter = lv_cache_iter_create(u->image_cache);
    LV_ASSERT_NULL(iter);

    /* Collect all cache entries that match the drop source */
    lv_iter_inspect(iter, image_cache_drop_collect_cb);

    image_item_t * drop_item;
    LV_LL_READ(&u->image_drop_ll, drop_item) {
        lv_cache_drop(u->image_cache, drop_item, NULL);
    }

    lv_ll_clear(&u->image_drop_ll);
    lv_iter_destroy(iter);
    u->image_drop_src = NULL;
}

/**********************
*   STATIC FUNCTIONS
**********************/

static void image_cache_release_cb(void * entry, void * user_data)
{
    lv_cache_entry_t ** entry_p = entry;
    lv_cache_t * cache = user_data;
    lv_cache_release(cache, * entry_p, NULL);
}

static bool image_create_cb(image_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    const uint32_t w = item->src_buf.header.w;
    const uint32_t h = item->src_buf.header.h;
    const lv_color_format_t cf = item->src_buf.header.cf;
    const uint32_t stride = item->src_buf.header.stride;
    enum NVGtexture nvg_tex_type = NVG_TEXTURE_BGRA;

    /* Determine texture type and pixel size based on color format */
    switch(cf) {
        case LV_COLOR_FORMAT_A8:
            nvg_tex_type = NVG_TEXTURE_ALPHA;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            nvg_tex_type = NVG_TEXTURE_BGRA;
            break;

        case LV_COLOR_FORMAT_XRGB8888:
            nvg_tex_type = NVG_TEXTURE_BGRX;
            break;

        case LV_COLOR_FORMAT_RGB888:
            nvg_tex_type = NVG_TEXTURE_BGR;
            break;

        case LV_COLOR_FORMAT_RGB565:
            nvg_tex_type = NVG_TEXTURE_RGB565;
            break;

        default:
            LV_LOG_ERROR("Unsupported image format: %d", cf);
            return false;
    }

    void * data = NULL;

    /* Check if stride is tightly packed */
    uint32_t tight_stride = (w * lv_color_format_get_bpp(cf) + 7) >> 3;
    if(stride == tight_stride) {
        /* Stride matches, use source buffer directly (zero-copy) */
        data = lv_draw_buf_goto_xy(&item->src_buf, 0, 0);
        LV_LOG_TRACE("Image stride matches: %" LV_PRIu32, stride);
    }
    else {
        /* Stride doesn't match, need to copy with tight alignment */
        lv_draw_buf_t * tmp_buf = lv_nanovg_reshape_global_image(item->u, cf, w, h);
        if(!tmp_buf) {
            LV_LOG_ERROR("Failed to allocate temp buffer for stride conversion");
            return false;
        }

        lv_draw_buf_copy(tmp_buf, NULL, &item->src_buf, NULL);
        data = lv_draw_buf_goto_xy(tmp_buf, 0, 0);
        LV_LOG_TRACE("Image stride converted: %" LV_PRIu32 " -> %" LV_PRIu32, stride, tight_stride);
    }

    int flags = item->image_flags;
    if(cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED
       || lv_draw_buf_has_flag(&item->src_buf, LV_IMAGE_FLAGS_PREMULTIPLIED)) {
        flags |= NVG_IMAGE_PREMULTIPLIED;
    }

    LV_PROFILER_DRAW_BEGIN_TAG("nvgCreateImage");
    int image_handle = nvgCreateImage(item->u->vg, w, h, flags, nvg_tex_type, data);
    LV_PROFILER_DRAW_END_TAG("nvgCreateImage");

    if(image_handle < 0) {
        return false;
    }

    if(item->src_type == LV_IMAGE_SRC_FILE) {
        item->src = lv_strdup(item->src);
        LV_ASSERT_MALLOC(item->src);
    }

    item->image_handle = image_handle;
    return true;
}

static void image_free_cb(image_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_DRAW_BEGIN;
    LV_LOG_TRACE("image_handle: %d", item->image_handle);
    nvgDeleteImage(item->u->vg, item->image_handle);
    item->image_handle = -1;

    if(item->src_type == LV_IMAGE_SRC_FILE) {
        lv_free((void *)item->src);
        item->src = NULL;
    }

    item->src_type = LV_IMAGE_SRC_UNKNOWN;

    LV_PROFILER_DRAW_END;
}

static lv_cache_compare_res_t image_compare_cb(const image_item_t * lhs, const image_item_t * rhs)
{
    if(lhs->image_flags != rhs->image_flags) {
        return lhs->image_flags > rhs->image_flags ? 1 : -1;
    }

    uint32_t lhs_color = *(uint32_t *)&lhs->color;
    uint32_t rhs_color = *(uint32_t *)&rhs->color;

    if(lhs_color != rhs_color) {
        return lhs_color > rhs_color ? 1 : -1;
    }

    int cmp_res = lv_memcmp(&lhs->src_buf, &rhs->src_buf, sizeof(lv_draw_buf_t));
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

static void image_cache_drop_collect_cb(void * elem)
{
    /**
     * If the cache is deleted during the traversal process,
     * it will cause iter to become invalid.
     * Therefore, we will first add it to the drop collection list and postpone the deletion.
     */
    LV_ASSERT_NULL(elem);
    image_item_t * item = elem;
    const void * src = item->u->image_drop_src;
    LV_ASSERT_NULL(src);
    lv_image_src_t src_type = lv_image_src_get_type(src);

    if((src_type == LV_IMAGE_SRC_FILE && lv_strcmp(item->src, src) == 0)
       || (src_type == LV_IMAGE_SRC_VARIABLE && item->src == src)) {
        image_item_t * drop_item = lv_ll_ins_tail(&item->u->image_drop_ll);
        LV_ASSERT_MALLOC(drop_item);
        *drop_item = *item;
    }
}

#endif /*LV_USE_DRAW_NANOVG*/
