/**
 * @file lv_draw_vg_lite_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#include "../../libs/freetype/lv_freetype.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_path.h"
#include "lv_draw_vg_lite_type.h"

/*********************
 *      DEFINES
 *********************/

#define PATH_QUALITY VG_LITE_MEDIUM
#define PATH_DATA_COORD_FORMAT VG_LITE_S16
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

static void draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void draw_letter_bitmap(lv_draw_vg_lite_unit_t * u, const lv_draw_glyph_dsc_t * dsc);

#if LV_USE_FREETYPE
    static void freetype_outline_event_cb(lv_event_t * e);
    static void draw_letter_outline(lv_draw_vg_lite_unit_t * u, const lv_draw_glyph_dsc_t * dsc);
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

void lv_draw_vg_lite_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                           const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_BEGIN;

#if LV_USE_FREETYPE
    static bool is_init = false;
    if(!is_init) {
        lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, draw_unit);
        is_init = true;
    }
#endif /* LV_USE_FREETYPE */

    lv_draw_label_iterate_characters(draw_unit, dsc, coords, draw_letter_cb);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;
    if(glyph_draw_dsc) {

        switch(glyph_draw_dsc->format) {
            case LV_DRAW_LETTER_BITMAP_FORMAT_A8: {
                    draw_letter_bitmap(u, glyph_draw_dsc);
                }
                break;

#if LV_USE_FREETYPE
            case LV_DRAW_LETTER_VECTOR_FORMAT: {
                    if(lv_freetype_is_outline_font(glyph_draw_dsc->g->resolved_font)) {
                        draw_letter_outline(u, glyph_draw_dsc);
                    }
                }
                break;
#endif /* LV_USE_FREETYPE */

            case LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE: {
                    lv_draw_image_dsc_t img_dsc;
                    lv_draw_image_dsc_init(&img_dsc);
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;
                    lv_draw_vg_lite_img(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords, false);
                }
                break;

#if LV_USE_FONT_PLACEHOLDER
            case LV_DRAW_LETTER_BITMAP_FORMAT_INVALID: {
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    lv_draw_vg_lite_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                }
                break;
#endif /* LV_USE_FONT_PLACEHOLDER */

            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_vg_lite_fill(draw_unit, fill_draw_dsc, fill_area);
    }
}

static void draw_letter_bitmap(lv_draw_vg_lite_unit_t * u, const lv_draw_glyph_dsc_t * dsc)
{
    lv_area_t clip_area;
    if(!_lv_area_intersect(&clip_area, u->base_unit.clip_area, dsc->letter_coords)) {
        return;
    }

    LV_PROFILER_BEGIN;

    lv_area_t image_area = *dsc->letter_coords;

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);
    vg_lite_translate(image_area.x1, image_area.y1, &matrix);

    vg_lite_buffer_t src_buf;
    lv_draw_buf_t * draw_buf = dsc->glyph_data;
    lv_vg_lite_buffer_from_draw_buf(&src_buf, draw_buf);

    vg_lite_color_t color;
    color = lv_vg_lite_color(dsc->color, dsc->opa, true);

    LV_VG_LITE_ASSERT_SRC_BUFFER(&src_buf);
    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);

    /* If clipping is not required, blit directly */
    if(_lv_area_is_in(&image_area, u->base_unit.clip_area, false)) {
        /* The image area is the coordinates relative to the image itself */
        lv_area_t src_area = image_area;
        lv_area_move(&src_area, -image_area.x1, -image_area.y1);

        /* rect is used to crop the pixel-aligned padding area */
        vg_lite_rectangle_t rect;
        lv_vg_lite_rect(&rect, &src_area);
        LV_PROFILER_BEGIN_TAG("vg_lite_blit_rect");
        LV_VG_LITE_CHECK_ERROR(vg_lite_blit_rect(
                                   &u->target_buffer,
                                   &src_buf,
                                   &rect,
                                   &matrix,
                                   VG_LITE_BLEND_SRC_OVER,
                                   color,
                                   VG_LITE_FILTER_LINEAR));
        LV_PROFILER_END_TAG("vg_lite_blit_rect");
    }
    else {
        lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_S16);
        lv_vg_lite_path_append_rect(
            path,
            clip_area.x1, clip_area.y1,
            lv_area_get_width(&clip_area), lv_area_get_height(&clip_area),
            0, 0);
        lv_vg_lite_path_set_bonding_box_area(path, &clip_area);
        lv_vg_lite_path_end(path);

        vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);
        LV_VG_LITE_ASSERT_PATH(vg_lite_path);

        vg_lite_matrix_t path_matrix;
        vg_lite_identity(&path_matrix);
        lv_vg_lite_matrix_multiply(&path_matrix, &u->global_matrix);

        LV_PROFILER_BEGIN_TAG("vg_lite_draw_pattern");
        LV_VG_LITE_CHECK_ERROR(vg_lite_draw_pattern(
                                   &u->target_buffer,
                                   vg_lite_path,
                                   VG_LITE_FILL_EVEN_ODD,
                                   &path_matrix,
                                   &src_buf,
                                   &matrix,
                                   VG_LITE_BLEND_SRC_OVER,
                                   VG_LITE_PATTERN_COLOR,
                                   color,
                                   color,
                                   VG_LITE_FILTER_LINEAR));
        LV_PROFILER_END_TAG("vg_lite_draw_pattern");

        lv_vg_lite_path_drop(u, path);
    }

    /* TODO: The temporary buffer of the built-in font is reused.
     * You need to wait for the GPU to finish using the buffer before releasing it.
     * Later, use the font cache for management to improve efficiency.
     */
    lv_vg_lite_finish(&u->base_unit);
    LV_PROFILER_END;
}

#if LV_USE_FREETYPE

static void draw_letter_outline(lv_draw_vg_lite_unit_t * u, const lv_draw_glyph_dsc_t * dsc)
{
    /* get clip area */
    lv_area_t path_clip_area;
    if(!_lv_area_intersect(&path_clip_area, u->base_unit.clip_area, dsc->letter_coords)) {
        return;
    }

    LV_PROFILER_BEGIN;

    /* vg-lite bounding_box will crop the pixels on the edge, so +1px is needed here */
    path_clip_area.x2++;
    path_clip_area.y2++;

    lv_vg_lite_path_t * outline = (lv_vg_lite_path_t *)dsc->glyph_data;
    lv_point_t pos = {dsc->letter_coords->x1, dsc->letter_coords->y1};

    /* calc convert matrix */
    float scale = FT_F26DOT6_TO_PATH_SCALE(lv_freetype_outline_get_scale(dsc->g->resolved_font));
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    /* convert to vg-lite coordinate */
    vg_lite_translate(pos.x - dsc->g->ofs_x, pos.y + dsc->g->box_h + dsc->g->ofs_y, &matrix);

    /* scale size */
    vg_lite_scale(scale, scale, &matrix);

    /* Cartesian coordinates to LCD coordinates */
    lv_vg_lite_matrix_flip_y(&matrix);

    /* calc inverse matrix */
    vg_lite_matrix_t result;
    if(!lv_vg_lite_matrix_inverse(&result, &matrix)) {
        LV_LOG_ERROR("no inverse matrix");
        LV_PROFILER_END;
        return;
    }

    lv_point_precise_t p1 = { path_clip_area.x1, path_clip_area.y1 };
    lv_point_precise_t p1_res = lv_vg_lite_matrix_transform_point(&result, &p1);

    lv_point_precise_t p2 = { path_clip_area.x2, path_clip_area.y2 };
    lv_point_precise_t p2_res = lv_vg_lite_matrix_transform_point(&result, &p2);

    /* Since the font uses Cartesian coordinates, the y coordinates need to be reversed */
    lv_vg_lite_path_set_bonding_box(outline, p1_res.x, p2_res.y, p2_res.x, p1_res.y);

    /* Move to the position relative to the first address of the buffer */
    lv_layer_t * layer = u->base_unit.target_layer;
    vg_lite_translate(-layer->buf_area.x1 / scale, layer->buf_area.y1 / scale, &matrix);

    vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(outline);

    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);
    LV_VG_LITE_ASSERT_PATH(vg_lite_path);

    LV_PROFILER_BEGIN_TAG("vg_lite_draw");
    LV_VG_LITE_CHECK_ERROR(vg_lite_draw(
                               &u->target_buffer, vg_lite_path, VG_LITE_FILL_NON_ZERO,
                               &matrix, VG_LITE_BLEND_SRC_OVER, lv_vg_lite_color(dsc->color, dsc->opa, true)));
    LV_PROFILER_END_TAG("vg_lite_draw");

    LV_PROFILER_END;
}

static void vg_lite_outline_push(const lv_freetype_outline_event_param_t * param)
{
    LV_PROFILER_BEGIN;
    lv_vg_lite_path_t * outline = param->outline;
    LV_ASSERT_NULL(outline);

    lv_freetype_outline_type_t type = param->type;
    switch(type) {
        case LV_FREETYPE_OUTLINE_END:
            lv_vg_lite_path_end(outline);
            break;
        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_vg_lite_path_move_to(outline, param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_vg_lite_path_line_to(outline, param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            lv_vg_lite_path_cubic_to(outline, param->control1.x, param->control1.y,
                                     param->control2.x, param->control2.y,
                                     param->to.x, param->to.y);
            break;
        case LV_FREETYPE_OUTLINE_CONIC_TO:
            lv_vg_lite_path_quad_to(outline, param->control1.x, param->control1.y,
                                    param->to.x, param->to.y);
            break;
        default:
            LV_LOG_ERROR("unknown point type: %d", type);
            LV_ASSERT(false);
            break;
    }
    LV_PROFILER_END;
}

static void freetype_outline_event_cb(lv_event_t * e)
{
    LV_PROFILER_BEGIN;
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
    LV_PROFILER_END;
}

#endif /* LV_USE_FREETYPE */

#endif /*LV_USE_DRAW_VG_LITE*/
