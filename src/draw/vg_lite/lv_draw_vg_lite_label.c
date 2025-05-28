/**
 * @file lv_draw_vg_lite_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite.h"
#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_pending.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../misc/lv_area_private.h"
#include "../../libs/freetype/lv_freetype_private.h"
#include "../lv_draw_label_private.h"


/*********************
 *      DEFINES
 *********************/

#define PATH_DATA_COORD_FORMAT VG_LITE_S16

#if LV_VG_LITE_FLUSH_MAX_COUNT > 0
    #define PATH_FLUSH_COUNT_MAX 0
#else
    /* When using IDLE Flush mode, reduce the number of flushes */
    #define PATH_FLUSH_COUNT_MAX 8
#endif

#define FT_F26DOT6_SHIFT 6

/** After converting the font reference size, it is also necessary to scale the 26dot6 data
 * in the path to the real physical size
 */
#define FT_F26DOT6_TO_PATH_SCALE(x) (LV_FREETYPE_F26DOT6_TO_FLOAT(x) / (1 << FT_F26DOT6_SHIFT))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc);

static void bitmap_cache_release_cb(void * entry, void * user_data);

#if LV_USE_FREETYPE
    static void freetype_outline_event_cb(lv_event_t * e);
    static void draw_letter_outline(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc);
#endif /* LV_USE_FREETYPE */

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vg_lite_label_init(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);

#if LV_USE_FREETYPE
    /*Set up the freetype outline event*/
    lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, u);
#endif /* LV_USE_FREETYPE */

    u->bitmap_font_pending = lv_vg_lite_pending_create(sizeof(lv_font_glyph_dsc_t), 8);
    lv_vg_lite_pending_set_free_cb(u->bitmap_font_pending, bitmap_cache_release_cb, NULL);
}

void lv_draw_vg_lite_label_deinit(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(u->bitmap_font_pending)
    lv_vg_lite_pending_destroy(u->bitmap_font_pending);
    u->bitmap_font_pending = NULL;
}

void lv_draw_vg_lite_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
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

void lv_draw_vg_lite_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                           const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_label_iterate_characters(t, dsc, coords, draw_letter_cb);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;
    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    if(!glyph_draw_dsc->glyph_data) {
                        return;
                    }

                    draw_letter_bitmap(t, glyph_draw_dsc);
                }
                break;

#if LV_USE_FREETYPE
            case LV_FONT_GLYPH_FORMAT_VECTOR: {
                    if(lv_freetype_is_outline_font(glyph_draw_dsc->g->resolved_font)) {
                        if(!glyph_draw_dsc->glyph_data) {
                            return;
                        }

                        draw_letter_outline(t, glyph_draw_dsc);
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
                    lv_draw_vg_lite_img(t, &image_dsc, glyph_draw_dsc->letter_coords, false);
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
                    lv_draw_vg_lite_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                }
                break;
#endif /* LV_USE_FONT_PLACEHOLDER */

            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_vg_lite_fill(t, fill_draw_dsc, fill_area);
    }

    /* Flush in time to avoid accumulation of drawing commands */
    u->letter_count++;
    if(u->letter_count > PATH_FLUSH_COUNT_MAX) {
        lv_vg_lite_flush(u);
    }
}

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc)
{
    lv_area_t clip_area;
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;
    if(!lv_area_intersect(&clip_area, &t->clip_area, dsc->letter_coords)) {
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    const lv_area_t image_area = *dsc->letter_coords;

    vg_lite_matrix_t matrix = u->global_matrix;

    const bool is_rotated = dsc->rotation % 3600 != 0;

    if(!is_rotated) {
        vg_lite_translate(image_area.x1, image_area.y1, &matrix);
    }
    else {
        vg_lite_translate(image_area.x1 + dsc->pivot.x, image_area.y1 + (dsc->g->box_h + dsc->g->ofs_y), &matrix);
        vg_lite_rotate(dsc->rotation / 10.0f, &matrix);
        vg_lite_translate(-dsc->pivot.x, -dsc->g->box_h - dsc->g->ofs_y, &matrix);
    }

    vg_lite_buffer_t src_buf;
    const lv_draw_buf_t * draw_buf = dsc->glyph_data;
    lv_vg_lite_buffer_from_draw_buf(&src_buf, draw_buf);

    const vg_lite_color_t color = lv_vg_lite_color(dsc->color, dsc->opa, true);

    /* If clipping is not required, blit directly */
    if(lv_area_is_in(&image_area, &t->clip_area, false)) {
        /* rect is used to crop the pixel-aligned padding area */
        vg_lite_rectangle_t rect = {
            .x = 0,
            .y = 0,
            .width = lv_area_get_width(&image_area),
            .height = lv_area_get_height(&image_area)
        };

        lv_vg_lite_blit_rect(
            &u->target_buffer,
            &src_buf,
            &rect,
            &matrix,
            VG_LITE_BLEND_SRC_OVER,
            color,
            VG_LITE_FILTER_LINEAR);
    }
    else {
        lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_S16);
        lv_vg_lite_path_append_rect(
            path,
            clip_area.x1, clip_area.y1,
            lv_area_get_width(&clip_area), lv_area_get_height(&clip_area),
            0);
        lv_vg_lite_path_set_bounding_box_area(path, &clip_area);
        lv_vg_lite_path_end(path);

        vg_lite_matrix_t path_matrix = u->global_matrix;
        if(is_rotated) vg_lite_rotate(dsc->rotation / 10.0f, &path_matrix);

        lv_vg_lite_draw_pattern(
            &u->target_buffer,
            lv_vg_lite_path_get_path(path),
            VG_LITE_FILL_EVEN_ODD,
            &path_matrix,
            &src_buf,
            &matrix,
            VG_LITE_BLEND_SRC_OVER,
            VG_LITE_PATTERN_COLOR,
            0,
            color,
            VG_LITE_FILTER_LINEAR);

        lv_vg_lite_path_drop(u, path);
    }

    /* Check if the data has cache and add it to the pending list */
    if(dsc->g->entry) {
        /* Increment the cache reference count */
        lv_cache_entry_acquire_data(dsc->g->entry);
        lv_vg_lite_pending_add(u->bitmap_font_pending, dsc->g);
    }
    else {
        /* No caching, wait for GPU finish before releasing the data */
        lv_vg_lite_finish(u);
    }

    LV_PROFILER_DRAW_END;
}

static void bitmap_cache_release_cb(void * entry, void * user_data)
{
    LV_UNUSED(user_data);
    lv_font_glyph_dsc_t * g_dsc = entry;
    lv_font_glyph_release_draw_data(g_dsc);
}

#if LV_USE_FREETYPE

static void draw_letter_outline(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;
    /* get clip area */
    lv_area_t path_clip_area;
    if(!lv_area_intersect(&path_clip_area, &t->clip_area, dsc->letter_coords)) {
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    /* vg-lite bounding_box will crop the pixels on the edge, so +1px is needed here */
    path_clip_area.x2++;
    path_clip_area.y2++;

    lv_vg_lite_path_t * outline = (lv_vg_lite_path_t *)dsc->glyph_data;
    const lv_point_t pos = {dsc->letter_coords->x1, dsc->letter_coords->y1};
    /* scale size */
    const float scale = FT_F26DOT6_TO_PATH_SCALE(lv_freetype_outline_get_scale(dsc->g->resolved_font));
    const bool is_rotated = dsc->rotation % 3600 != 0;

    /* calc convert matrix */
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    if(!is_rotated) {
        vg_lite_translate(pos.x - dsc->g->ofs_x, pos.y + dsc->g->box_h + dsc->g->ofs_y, &matrix);
        vg_lite_scale(scale, scale, &matrix);
    }
    else {
        vg_lite_translate(pos.x - dsc->g->ofs_x + dsc->pivot.x, pos.y + dsc->g->box_h + dsc->g->ofs_y, &matrix);
        vg_lite_rotate(dsc->rotation / 10.0f, &matrix);
        vg_lite_translate(-dsc->pivot.x, 0, &matrix);
        vg_lite_scale(scale, scale, &matrix);
    }

    if(vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR)) {
        /* set scissor area */
        lv_vg_lite_set_scissor_area(&t->clip_area);

        /* no bounding box */
        lv_vg_lite_path_set_bounding_box(outline,
                                         (float)PATH_COORD_MIN, (float)PATH_COORD_MIN,
                                         (float)PATH_COORD_MAX, (float)PATH_COORD_MAX);
    }
    else {
        if(is_rotated) {
            LV_LOG_WARN("clip may be incorrect when vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR) is false");
        }

        /* calc inverse matrix */
        vg_lite_matrix_t result;
        if(!lv_vg_lite_matrix_inverse(&result, &matrix)) {
            LV_LOG_ERROR("no inverse matrix");
            LV_PROFILER_DRAW_END;
            return;
        }

        const lv_point_precise_t p1 = { path_clip_area.x1, path_clip_area.y1 };
        const lv_point_precise_t p1_res = lv_vg_lite_matrix_transform_point(&result, &p1);

        const lv_point_precise_t p2 = { path_clip_area.x2, path_clip_area.y2 };
        const lv_point_precise_t p2_res = lv_vg_lite_matrix_transform_point(&result, &p2);

        /* Since the font uses Cartesian coordinates, the y coordinates need to be reversed */
        if(is_rotated)
            lv_vg_lite_path_set_bounding_box(outline,
                                             (float)PATH_COORD_MIN, (float)PATH_COORD_MIN,
                                             (float)PATH_COORD_MAX, (float)PATH_COORD_MAX);
        else lv_vg_lite_path_set_bounding_box(outline, p1_res.x, p2_res.y, p2_res.x, p1_res.y);
    }

    /* matrix for drawing, different from matrix for calculating the bounding box */
    vg_lite_matrix_t draw_matrix = u->global_matrix;
    lv_vg_lite_matrix_multiply(&draw_matrix, &matrix);

    lv_vg_lite_draw(
        &u->target_buffer,
        lv_vg_lite_path_get_path(outline),
        VG_LITE_FILL_NON_ZERO,
        &draw_matrix,
        VG_LITE_BLEND_SRC_OVER,
        lv_vg_lite_color(dsc->color, dsc->opa, true));

    LV_PROFILER_DRAW_END;
}

static void vg_lite_outline_push(const lv_freetype_outline_event_param_t * param)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_vg_lite_path_t * outline = param->outline;
    LV_ASSERT_NULL(outline);

    lv_freetype_outline_type_t type = param->type;
    switch(type) {

        /**
         * Reverse the Y-axis coordinate direction to achieve
         * the conversion from Cartesian coordinate system to LCD coordinate system
         */
        case LV_FREETYPE_OUTLINE_END:
            lv_vg_lite_path_end(outline);
            break;
        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_vg_lite_path_move_to(outline, param->to.x, -param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_vg_lite_path_line_to(outline, param->to.x, -param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            lv_vg_lite_path_cubic_to(outline, param->control1.x, -param->control1.y,
                                     param->control2.x, -param->control2.y,
                                     param->to.x, -param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CONIC_TO:
            lv_vg_lite_path_quad_to(outline, param->control1.x, -param->control1.y,
                                    param->to.x, -param->to.y);
            break;
        default:
            LV_LOG_ERROR("unknown point type: %d", type);
            LV_ASSERT(false);
            break;
    }
    LV_PROFILER_DRAW_END;
}

static void freetype_outline_event_cb(lv_event_t * e)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_event_code_t code = lv_event_get_code(e);
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);
    switch(code) {
        case LV_EVENT_CREATE:
            param->outline = lv_vg_lite_path_create(PATH_DATA_COORD_FORMAT);
            break;
        case LV_EVENT_DELETE:
            lv_vg_lite_path_destroy(param->outline);
            break;
        case LV_EVENT_INSERT:
            vg_lite_outline_push(param);
            break;
        default:
            LV_LOG_WARN("unknown event code: %d", code);
            break;
    }
    LV_PROFILER_DRAW_END;
}

#endif /* LV_USE_FREETYPE */

#endif /*LV_USE_DRAW_VG_LITE*/
