/**
 * @file lv_draw_nanovg_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_image_cache.h"
#include "../lv_draw_label_private.h"
#include "../lv_draw_image_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../misc/lv_pending.h"
#include "../../libs/freetype/lv_freetype.h"

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
    lv_font_glyph_dsc_t g_dsc;

    /* value */
    int image_handle;
} letter_item_t;

/**********************
*  STATIC PROTOTYPES
**********************/

static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void letter_cache_release_cb(void * entry, void * user_data);
static bool letter_create_cb(letter_item_t * item, void * user_data);
static void letter_free_cb(letter_item_t * item, void * user_data);
static lv_cache_compare_res_t letter_compare_cb(const letter_item_t * lhs, const letter_item_t * rhs);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_nanovg_label_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->letter_cache == NULL);
    LV_ASSERT(u->letter_pending == NULL);

    const lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)letter_compare_cb,
        .create_cb = (lv_cache_create_cb_t)letter_create_cb,
        .free_cb = (lv_cache_free_cb_t)letter_free_cb,
    };

    u->letter_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(letter_item_t), LV_NANOVG_LETTER_CACHE_CNT, ops);
    lv_cache_set_name(u->letter_cache, "NVG_LETTER");
    u->letter_pending = lv_pending_create(sizeof(lv_cache_entry_t *), 4);
    lv_pending_set_free_cb(u->letter_pending, letter_cache_release_cb, u->letter_cache);
}

void lv_draw_nanovg_label_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT(u->letter_cache);
    LV_ASSERT(u->letter_pending);

    lv_pending_destroy(u->letter_pending);
    u->letter_pending = NULL;

    lv_cache_destroy(u->letter_cache, NULL);
    u->letter_cache = NULL;
}

void lv_draw_nanovg_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
    LV_ASSERT_NULL(t);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(coords);

    if(dsc->opa <= LV_OPA_MIN)
        return;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = coords->x1, .y = coords->y1
    },
    dsc->font, dsc->unicode, draw_letter_cb);

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }

    LV_PROFILER_DRAW_END;
}

void lv_draw_nanovg_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_label_iterate_characters(t, dsc, coords, draw_letter_cb);
    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

static bool draw_letter_clip_areas(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, lv_area_t * letter_area,
                                   lv_area_t * cliped_area)
{
    *letter_area = *dsc->letter_coords;

    if(dsc->rotation) {
        const lv_point_t pivot = {
            .x = dsc->pivot.x,
            .y = dsc->g->box_h + dsc->g->ofs_y
        };

        lv_image_buf_get_transformed_area(
            letter_area,
            lv_area_get_width(dsc->letter_coords),
            lv_area_get_height(dsc->letter_coords),
            dsc->rotation,
            LV_SCALE_NONE,
            LV_SCALE_NONE,
            &pivot);
        lv_area_move(letter_area, dsc->letter_coords->x1, dsc->letter_coords->y1);
    }

    if(!lv_area_intersect(cliped_area, &t->clip_area, letter_area)) {
        return false;
    }

    return true;
}

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, int image_handle)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t image_area;
    lv_area_t clip_area;
    if(!draw_letter_clip_areas(t, dsc, &image_area, &clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    if(!dsc->rotation) {
        float x = dsc->letter_coords->x1;
        float y = dsc->letter_coords->y1;
        float w = lv_area_get_width(dsc->letter_coords);
        float h = lv_area_get_height(dsc->letter_coords);

        NVGpaint paint = nvgImagePattern(u->vg, x, y, w, h, 0, image_handle, 1.0f);
        paint.innerColor = paint.outerColor = nvgRGBA(dsc->color.red, dsc->color.green, dsc->color.blue, dsc->opa);

        nvgBeginPath(u->vg);
        nvgRect(u->vg, x, y, w, h);
        nvgFillPaint(u->vg, paint);
        nvgFill(u->vg);
    }
    else {
        /* TODO: draw rotated bitmap */
    }

    LV_PROFILER_DRAW_END;
}

static inline int letter_get_image_handle(lv_draw_nanovg_unit_t * u, lv_font_glyph_dsc_t * g_dsc)
{
    LV_PROFILER_DRAW_BEGIN;

    letter_item_t search_key = { 0 };
    search_key.u = u;
    search_key.g_dsc = *g_dsc;
    search_key.g_dsc.entry = NULL; /* Exclude the cache entry from the key */

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(u->letter_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        /* check if the cache is full */
        size_t free_size = lv_cache_get_free_size(u->letter_cache, NULL);
        if(free_size == 0) {
            LV_LOG_INFO("letter cache is full, release all pending cache entries");
            lv_nanovg_end_frame(u);
        }

        cache_node_entry = lv_cache_acquire_or_create(u->letter_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("letter cache creating failed");
            LV_PROFILER_DRAW_END;
            return -1;
        }
    }

    /* Add the new entry to the pending list */
    lv_pending_add(u->letter_pending, &cache_node_entry);

    letter_item_t * letter_item = lv_cache_entry_get_data(cache_node_entry);

    LV_PROFILER_DRAW_END;
    return letter_item->image_handle;
}

static void letter_cache_release_cb(void * entry, void * user_data)
{
    lv_cache_entry_t ** entry_p = entry;
    lv_cache_t * cache = user_data;
    lv_cache_release(cache, * entry_p, NULL);
}

static bool letter_create_cb(letter_item_t * item, void * user_data)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_UNUSED(user_data);
    lv_font_glyph_dsc_t * g_dsc = &item->g_dsc;

    const uint32_t w = g_dsc->box_w;
    const uint32_t h = g_dsc->box_h;

    lv_draw_buf_t * image_buf = lv_nanovg_reshape_global_image(item->u, LV_COLOR_FORMAT_A8, w, h);
    if(!image_buf) {
        LV_PROFILER_DRAW_END;
        return false;
    }

    if(!lv_font_get_glyph_bitmap(g_dsc, image_buf)) {
        LV_PROFILER_DRAW_END;
        return false;
    }

    LV_PROFILER_DRAW_BEGIN_TAG("nvgCreateImage");
    item->image_handle = nvgCreateImage(item->u->vg, w, h, 0, NVG_TEXTURE_ALPHA, lv_draw_buf_goto_xy(image_buf, 0, 0));
    LV_PROFILER_DRAW_END_TAG("nvgCreateImage");

    LV_LOG_TRACE("image_handle: %d", item->image_handle);
    LV_PROFILER_DRAW_END;
    return true;
}

static void letter_free_cb(letter_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_DRAW_BEGIN;
    LV_LOG_TRACE("image_handle: %d", item->image_handle);
    nvgDeleteImage(item->u->vg, item->image_handle);
    item->image_handle = -1;
    LV_PROFILER_DRAW_END;
}

static lv_cache_compare_res_t letter_compare_cb(const letter_item_t * lhs, const letter_item_t * rhs)
{
    int cmp_res = lv_memcmp(&lhs->g_dsc, &rhs->g_dsc, sizeof(lv_font_glyph_dsc_t));
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}


static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8: {
                    int image_handle = letter_get_image_handle(u, glyph_draw_dsc->g);
                    if(image_handle < 0) {
                        return;
                    }

                    draw_letter_bitmap(t, glyph_draw_dsc, image_handle);
                }
                break;

#if LV_USE_FREETYPE
            case LV_FONT_GLYPH_FORMAT_VECTOR: {
                    if(lv_freetype_is_outline_font(glyph_draw_dsc->g->resolved_font)) {
                        if(!glyph_draw_dsc->glyph_data) {
                            return;
                        }

                        /* TODO: draw_letter_outline(t, glyph_draw_dsc); */
                    }
                }
                break;
#endif /* LV_USE_FREETYPE */

            case LV_FONT_GLYPH_FORMAT_IMAGE: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    if(!glyph_draw_dsc->glyph_data) {
                        return;
                    }

                    lv_draw_image_dsc_t image_dsc;
                    lv_draw_image_dsc_init(&image_dsc);
                    image_dsc.opa = glyph_draw_dsc->opa;
                    image_dsc.src = glyph_draw_dsc->glyph_data;
                    image_dsc.rotation = glyph_draw_dsc->rotation;
                    lv_draw_nanovg_image(t, &image_dsc, glyph_draw_dsc->letter_coords, -1);
                }
                break;

#if LV_USE_FONT_PLACEHOLDER
            case LV_FONT_GLYPH_FORMAT_NONE: {
                    if(glyph_draw_dsc->bg_coords == NULL) break;
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    lv_draw_nanovg_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                }
                break;
#endif /* LV_USE_FONT_PLACEHOLDER */

            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_nanovg_fill(t, fill_draw_dsc, fill_area);
    }
}

#endif /* LV_USE_DRAW_NANOVG */
