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
#include "../lv_draw_image_private.h"


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

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, vg_lite_buffer_t * src_buf);

static void bitmap_cache_release_cb(void * entry, void * user_data);

#if LV_USE_FREETYPE
    static void freetype_outline_event_cb(lv_event_t * e);
    static void draw_letter_outline(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc);
    static void outline_iter_cb(void * user_data, uint8_t op_code, const float * data, uint32_t len);
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

static inline bool init_buffer_from_glyph_dsc(vg_lite_buffer_t * buffer, lv_font_glyph_dsc_t * g_dsc)
{
    const void * glyph_bitmap = lv_font_get_glyph_static_bitmap(g_dsc);
    if(!glyph_bitmap) {
        return false;
    }

    if(!LV_VG_LITE_IS_ALIGNED(glyph_bitmap, 16)) {
        LV_LOG_WARN("Glyph data %p is not aligned to 16 bytes", glyph_bitmap);
        return false;
    }

    if(!LV_VG_LITE_IS_ALIGNED(g_dsc->stride, 16)) {
        LV_LOG_WARN("Glyph stride %" LV_PRIu32 " is not aligned to 16 bytes", g_dsc->stride);
        return false;
    }

    lv_vg_lite_buffer_init(buffer, glyph_bitmap, g_dsc->box_w, g_dsc->box_h, g_dsc->stride, VG_LITE_A8, false);
    return true;
}

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
                    vg_lite_buffer_t src_buf;
                    if(lv_font_has_static_bitmap(glyph_draw_dsc->g->resolved_font)) {
                        if(!init_buffer_from_glyph_dsc(&src_buf, glyph_draw_dsc->g)) {
                            return;
                        }
                    }
                    else {
                        glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                        if(!glyph_draw_dsc->glyph_data) {
                            return;
                        }

                        lv_vg_lite_buffer_from_draw_buf(&src_buf, glyph_draw_dsc->glyph_data);
                    }

                    draw_letter_bitmap(t, glyph_draw_dsc, &src_buf);
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

static inline void convert_letter_matrix(vg_lite_matrix_t * matrix, const lv_draw_glyph_dsc_t * dsc)
{
    vg_lite_translate(dsc->letter_coords->x1, dsc->letter_coords->y1, matrix);

    if(!dsc->rotation) {
        return;
    }

    const lv_point_t pivot = {
        .x = dsc->pivot.x,
        .y = dsc->g->box_h + dsc->g->ofs_y
    };
    vg_lite_translate(pivot.x, pivot.y, matrix);
    vg_lite_rotate(dsc->rotation / 10.0f, matrix);
    vg_lite_translate(-pivot.x, -pivot.y, matrix);
}

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

static void draw_letter_bitmap(lv_draw_task_t * t, const lv_draw_glyph_dsc_t * dsc, vg_lite_buffer_t * src_buf)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t image_area;
    lv_area_t clip_area;
    if(!draw_letter_clip_areas(t, dsc, &image_area, &clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    vg_lite_matrix_t matrix = u->global_matrix;
    convert_letter_matrix(&matrix, dsc);

    const vg_lite_color_t color = lv_vg_lite_color(dsc->color, dsc->opa, true);

    /* If rotation is not required, blit directly */
    if(!dsc->rotation) {
        vg_lite_rectangle_t rect = {
            .x = clip_area.x1 - image_area.x1,
            .y = clip_area.y1 - image_area.y1,
            .width = lv_area_get_width(&clip_area),
            .height = lv_area_get_height(&clip_area)
        };

        /* add offset for clipped area */
        if(rect.x || rect.y) {
            vg_lite_translate(rect.x, rect.y, &matrix);
        }

        lv_vg_lite_blit_rect(
            &u->target_buffer,
            src_buf,
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
            image_area.x1, image_area.y1,
            lv_area_get_width(&image_area), lv_area_get_height(&image_area),
            0);
        lv_vg_lite_path_end(path);
        lv_vg_lite_path_set_bounding_box_area(path, &clip_area);

        vg_lite_matrix_t path_matrix = u->global_matrix;

        lv_vg_lite_draw_pattern(
            &u->target_buffer,
            lv_vg_lite_path_get_path(path),
            VG_LITE_FILL_EVEN_ODD,
            &path_matrix,
            src_buf,
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
    else if(!lv_font_has_static_bitmap(dsc->g->resolved_font)) {
        /* If there is no caching or no static bitmap is used, wait for the GPU to finish before releasing the data. */
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
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t letter_area;
    lv_area_t path_clip_area;
    if(!draw_letter_clip_areas(t, dsc, &letter_area, &path_clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    /* vg-lite bounding_box will crop the pixels on the edge, so +1px is needed here */
    path_clip_area.x2++;
    path_clip_area.y2++;

    lv_vg_lite_path_t * outline = (lv_vg_lite_path_t *)dsc->glyph_data;
    const lv_point_t glyph_pos = {
        dsc->letter_coords->x1 - dsc->g->ofs_x,
        dsc->letter_coords->y1 + dsc->g->box_h + dsc->g->ofs_y
    };
    /* scale size */
    const float scale = FT_F26DOT6_TO_PATH_SCALE(lv_freetype_outline_get_scale(dsc->g->resolved_font));

    const bool has_rotation_with_cliped = dsc->rotation && !lv_area_is_in(&letter_area, &t->clip_area, false);

    /* calc convert matrix */
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    if(!has_rotation_with_cliped && dsc->rotation) {
        vg_lite_translate(glyph_pos.x + dsc->pivot.x, glyph_pos.y, &matrix);
        vg_lite_rotate(dsc->rotation / 10.0f, &matrix);
        vg_lite_translate(-dsc->pivot.x, 0, &matrix);
    }
    else {
        vg_lite_translate(glyph_pos.x, glyph_pos.y, &matrix);
    }

    vg_lite_scale(scale, scale, &matrix);

    /* matrix for drawing, different from matrix for calculating the bounding box */
    vg_lite_matrix_t draw_matrix = u->global_matrix;
    lv_vg_lite_matrix_multiply(&draw_matrix, &matrix);

    /* calc inverse matrix */
    vg_lite_matrix_t result;
    if(!lv_vg_lite_matrix_inverse(&result, &matrix)) {
        LV_LOG_ERROR("no inverse matrix");
        lv_vg_lite_matrix_dump_info(&matrix);
        LV_PROFILER_DRAW_END;
        return;
    }

    const lv_point_precise_t p1 = { path_clip_area.x1, path_clip_area.y1 };
    const lv_point_precise_t p1_res = lv_vg_lite_matrix_transform_point(&result, &p1);
    const lv_point_precise_t p2 = { path_clip_area.x2, path_clip_area.y2 };
    const lv_point_precise_t p2_res = lv_vg_lite_matrix_transform_point(&result, &p2);

    if(has_rotation_with_cliped) {
        /**
         * When intersecting the clipping region,
         * rotate the path contents without rotating the bounding box for cropping
         */
        vg_lite_matrix_t internal_matrix;
        vg_lite_identity(&internal_matrix);
        const float pivot_x = dsc->pivot.x / scale;
        const float pivot_y = dsc->g->box_h + dsc->g->ofs_y;
        vg_lite_translate(pivot_x, pivot_y, &internal_matrix);
        vg_lite_rotate(dsc->rotation / 10.0f, &internal_matrix);
        vg_lite_translate(-pivot_x, -pivot_y, &internal_matrix);

        lv_vg_lite_path_t * outline_transformed = lv_vg_lite_path_get(u, VG_LITE_FP32);
        lv_vg_lite_path_set_transform(outline_transformed, &internal_matrix);
        lv_vg_lite_path_for_each_data(lv_vg_lite_path_get_path(outline), outline_iter_cb, outline_transformed);
        lv_vg_lite_path_set_bounding_box(outline_transformed, p1_res.x, p2_res.y, p2_res.x, p1_res.y);

        lv_vg_lite_draw(
            &u->target_buffer,
            lv_vg_lite_path_get_path(outline_transformed),
            VG_LITE_FILL_NON_ZERO,
            &draw_matrix,
            VG_LITE_BLEND_SRC_OVER,
            lv_vg_lite_color(dsc->color, dsc->opa, true));

        lv_vg_lite_path_drop(u, outline_transformed);

        LV_PROFILER_DRAW_END;
        return;
    }

    if(dsc->rotation) {
        /* The bounding rectangle before scaling relative to the original coordinates of the path */
        lv_area_t box_area;
        box_area.x1 = dsc->g->ofs_x;
        box_area.y1 = -dsc->g->box_h - dsc->g->ofs_y;
        lv_area_set_width(&box_area, dsc->g->box_w);
        lv_area_set_height(&box_area, dsc->g->box_h);

        /* Workaround for loss of rotation precision */
        lv_area_increase(&box_area, 5, 5);

        /* Scale the path area to fit the original path data */
        lv_vg_lite_path_set_bounding_box(outline,
                                         box_area.x1 / scale,
                                         box_area.y1 / scale,
                                         box_area.x2 / scale,
                                         box_area.y2 / scale);
    }
    else {
        lv_vg_lite_path_set_bounding_box(outline, p1_res.x, p2_res.y, p2_res.x, p1_res.y);
    }

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

static void outline_iter_cb(void * user_data, uint8_t op_code, const float * data, uint32_t len)
{
    LV_UNUSED(len);
    typedef struct {
        float x;
        float y;
    } point_t;

    lv_vg_lite_path_t * path = user_data;
    const point_t * pt = (point_t *)data;

    switch(op_code) {
        case VLC_OP_MOVE:
            lv_vg_lite_path_move_to(path, pt->x, pt->y);
            break;
        case VLC_OP_LINE:
            lv_vg_lite_path_line_to(path, pt->x, pt->y);
            break;
        case VLC_OP_QUAD:
            lv_vg_lite_path_quad_to(path, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
            break;
        case VLC_OP_CUBIC:
            lv_vg_lite_path_cubic_to(path, pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
            break;
        case VLC_OP_CLOSE:
            lv_vg_lite_path_close(path);
            break;
        case VLC_OP_END:
            lv_vg_lite_path_end(path);
            break;
        default:
            LV_ASSERT_FORMAT_MSG(false, "unknown op_code: %d", op_code);
            break;
    }
}

#endif /* LV_USE_FREETYPE */

#endif /*LV_USE_DRAW_VG_LITE*/
