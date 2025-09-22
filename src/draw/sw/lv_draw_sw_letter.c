/**
 * @file lv_draw_sw_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_draw_label_private.h"
#include "../../draw/lv_draw_private.h"
#include "lv_draw_sw.h"

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG

    #include "../../libs/freetype/lv_freetype_private.h"
    #include "../lv_draw_vector_private.h"

#endif

#if LV_USE_DRAW_SW

#include "../../display/lv_display.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../../font/lv_font.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG

typedef struct {
    lv_vector_path_t * inside_path;     /*The regular glyph*/
    lv_vector_path_t * outside_path;    /*A bigger glyph that goes in the background for the letter outline*/
    lv_vector_path_t * cur_path;
} lv_draw_sw_letter_outlines_t;

#endif /* LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG */

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                                       lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG

    static void freetype_outline_event_cb(lv_event_t * e);
    static void draw_letter_outline(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc);

#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
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

void lv_draw_sw_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_DRAW_BEGIN;

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    static bool is_init = false;
    if(!is_init) {
        lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, t);
        is_init = true;
    }
#endif

    lv_draw_label_iterate_characters(t, dsc, coords, draw_letter_cb);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void LV_ATTRIBUTE_FAST_MEM draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                                 lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    if(glyph_draw_dsc->bg_coords == NULL) break;
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    lv_draw_sw_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
                }
                break;
            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8:
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
                    if(glyph_draw_dsc->rotation % 3600 == 0 && glyph_draw_dsc->format != LV_FONT_GLYPH_FORMAT_IMAGE) {
                        lv_area_t mask_area = *glyph_draw_dsc->letter_coords;

                        if(lv_font_has_static_bitmap(glyph_draw_dsc->g->resolved_font) &&
                           glyph_draw_dsc->g->format == LV_FONT_GLYPH_FORMAT_A8) {
                            glyph_draw_dsc->g->req_raw_bitmap = 1;
                            const void * bitmap = lv_font_get_glyph_static_bitmap(glyph_draw_dsc->g);
                            lv_draw_sw_blend_dsc_t blend_dsc;
                            lv_memzero(&blend_dsc, sizeof(blend_dsc));
                            blend_dsc.color = glyph_draw_dsc->color;
                            blend_dsc.opa = glyph_draw_dsc->opa;
                            blend_dsc.mask_buf = bitmap;
                            blend_dsc.mask_area = &mask_area;
                            blend_dsc.mask_stride = glyph_draw_dsc->g->stride;
                            blend_dsc.blend_area = glyph_draw_dsc->letter_coords;
                            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                            lv_draw_sw_blend(t, &blend_dsc);
                        }
                        else {
                            glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                            if(glyph_draw_dsc->glyph_data == NULL) {
                                LV_LOG_WARN("Couldn't get the bitmap of a glyph");
                                break;
                            }

                            mask_area.x2 = mask_area.x1 + lv_draw_buf_width_to_stride(lv_area_get_width(&mask_area), LV_COLOR_FORMAT_A8) - 1;
                            lv_draw_sw_blend_dsc_t blend_dsc;
                            lv_memzero(&blend_dsc, sizeof(blend_dsc));
                            blend_dsc.color = glyph_draw_dsc->color;
                            blend_dsc.opa = glyph_draw_dsc->opa;
                            const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;
                            blend_dsc.mask_buf = draw_buf->data;
                            blend_dsc.mask_area = &mask_area;
                            blend_dsc.mask_stride = draw_buf->header.stride;
                            blend_dsc.blend_area = glyph_draw_dsc->letter_coords;
                            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                            lv_draw_sw_blend(t, &blend_dsc);
                        }
                    }
                    else {
                        glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                        lv_draw_image_dsc_t img_dsc;
                        lv_draw_image_dsc_init(&img_dsc);
                        img_dsc.rotation = glyph_draw_dsc->rotation;
                        img_dsc.scale_x = LV_SCALE_NONE;
                        img_dsc.scale_y = LV_SCALE_NONE;
                        img_dsc.opa = glyph_draw_dsc->opa;
                        img_dsc.src = glyph_draw_dsc->glyph_data;
                        img_dsc.recolor = glyph_draw_dsc->color;
                        img_dsc.pivot = (lv_point_t) {
                            .x = glyph_draw_dsc->pivot.x,
                            .y = glyph_draw_dsc->g->box_h + glyph_draw_dsc->g->ofs_y
                        };
                        lv_draw_sw_image(t, &img_dsc, glyph_draw_dsc->letter_coords);
                    }
                    break;
                }
                break;
#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
            case LV_FONT_GLYPH_FORMAT_VECTOR: {
                    draw_letter_outline(t, glyph_draw_dsc);
                }
                break;
#endif
            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_sw_fill(t, fill_draw_dsc, fill_area);
    }
}

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG

/*
 * Renders the vectors paths representing a glyph with ThorVG
 * the result is then blended into the draw buffer
 */
static void draw_letter_outline(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc)
{

    lv_draw_sw_letter_outlines_t * glyph_paths;
    lv_draw_vector_dsc_t * vector_dsc;
    lv_draw_buf_t * draw_buf;
    lv_matrix_t matrix;
    lv_layer_t layer;

    glyph_paths = (lv_draw_sw_letter_outlines_t *) glyph_dsc->glyph_data;
    LV_ASSERT_NULL(glyph_paths);

    int32_t cf;
    int32_t w;
    int32_t h;
    uint32_t stride;
    float scale;
    lv_area_t buf_area;

    cf = LV_COLOR_FORMAT_ARGB8888;

    scale = LV_FREETYPE_F26DOT6_TO_FLOAT(lv_freetype_outline_get_scale(glyph_dsc->g->resolved_font));
    w = (int32_t)((float) glyph_dsc->g->box_w + glyph_dsc->outline_stroke_width * 2 * scale);
    h = (int32_t)((float) glyph_dsc->g->box_h + glyph_dsc->outline_stroke_width * 2 * scale);
    buf_area.x1 = 0;
    buf_area.y1 = 0;
    lv_area_set_width(&buf_area, w);
    lv_area_set_height(&buf_area, h);

    stride = lv_draw_buf_width_to_stride(w, cf);
    draw_buf = lv_draw_buf_create(w, h, cf, stride);
    lv_draw_buf_clear(draw_buf, NULL);

    lv_memzero(&layer, sizeof(lv_layer_t));
    layer.draw_buf = draw_buf;
    layer.color_format = cf;
    layer.buf_area = buf_area;
    layer.phy_clip_area = buf_area;
    layer._clip_area = buf_area;

    lv_memzero(&matrix, sizeof(lv_matrix_t));

    lv_matrix_identity(&matrix);

    vector_dsc = lv_draw_vector_dsc_create(&layer);

    int32_t offset_x;
    int32_t offset_y;

    offset_x = (int32_t)((float) glyph_dsc->g->ofs_x - glyph_dsc->outline_stroke_width * scale);
    offset_y = (int32_t)((float) glyph_dsc->g->ofs_y - glyph_dsc->outline_stroke_width * scale);

    /*Invert Y-Axis - Freetype's origin point is in the bottom left corner*/
    lv_matrix_scale(&matrix, 1, -1);
    lv_matrix_translate(&matrix, -offset_x, -h - offset_y);
    lv_matrix_scale(&matrix, scale, scale);
    lv_draw_vector_dsc_set_transform(vector_dsc, &matrix);

    /*Set attributes color, line width etc*/
    if(cf == LV_COLOR_FORMAT_ARGB8888) {

        if(glyph_dsc->outline_stroke_width > 0) {
            lv_draw_vector_dsc_set_fill_color(vector_dsc, glyph_dsc->outline_stroke_color);
            lv_draw_vector_dsc_set_fill_opa(vector_dsc, glyph_dsc->outline_stroke_opa);
            lv_draw_vector_dsc_add_path(vector_dsc, glyph_paths->outside_path);
        }

        lv_draw_vector_dsc_set_fill_color(vector_dsc, glyph_dsc->color);
        lv_draw_vector_dsc_set_fill_opa(vector_dsc, glyph_dsc->opa);
        lv_draw_vector_dsc_add_path(vector_dsc, glyph_paths->inside_path);

    }
    else {
        LV_LOG_ERROR("Unsupported color format: %d", cf);
    }

    lv_area_t old_area;
    lv_area_t letter_coords;

    /*Render vector path(s) - set the clip area so that it matches
     *the size of the temporary buffer used to render the glyph path(s)*/
    lv_memcpy(&old_area, &t->clip_area, sizeof(lv_area_t));
    lv_memcpy(&t->clip_area, &buf_area, sizeof(lv_area_t));

    /*Can't call lv_draw_vector() as it would create a new draw task while
     *the main thread also can create draw tasks. So create a dummy draw task
     *manually to draw the outline*/
    if(vector_dsc->task_list) {
        vector_dsc->base.layer = vector_dsc->base.layer;
        lv_draw_task_t dummy_t;
        lv_memzero(&dummy_t, sizeof(lv_draw_task_t));
        dummy_t.area = vector_dsc->base.layer->_clip_area;
        dummy_t._real_area = vector_dsc->base.layer->_clip_area;
        dummy_t.clip_area = vector_dsc->base.layer->_clip_area;
        dummy_t.target_layer = vector_dsc->base.layer;
        dummy_t.type = LV_DRAW_TASK_TYPE_VECTOR;
        dummy_t.draw_dsc = vector_dsc;
        lv_draw_sw_vector(&dummy_t, dummy_t.draw_dsc);
    }

    /*Restore previous draw area of the entire text label*/
    lv_memcpy(&t->clip_area, &old_area, sizeof(lv_area_t));

    lv_memcpy(&letter_coords, glyph_dsc->letter_coords, sizeof(lv_area_t));
    lv_area_set_width(&letter_coords, w);
    lv_area_set_height(&letter_coords, h);

    lv_draw_image_dsc_t img_dsc;

    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.rotation = 0;
    img_dsc.scale_x = LV_SCALE_NONE;
    img_dsc.scale_y = LV_SCALE_NONE;
    img_dsc.opa = LV_OPA_100;
    img_dsc.src = draw_buf;
    lv_draw_sw_image(t, &img_dsc, &letter_coords);

    lv_draw_vector_dsc_delete(vector_dsc);
    lv_draw_buf_destroy(draw_buf);

}

/* Build the inside and outside vector paths for a glyph based
 * on the received outline events emitted by lv_freetype_outline.c */
static void freetype_outline_event_cb(lv_event_t * e)
{

    lv_fpoint_t pnt;
    lv_fpoint_t ctrl_pnt1;
    lv_fpoint_t ctrl_pnt2;
    lv_draw_sw_letter_outlines_t * glyph_paths;
    lv_vector_path_t * path;
    lv_freetype_outline_event_param_t * outline_event;

    outline_event = lv_event_get_param(e);
    pnt.x = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->to.x);
    pnt.y = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->to.y);
    glyph_paths = outline_event->outline;

    if(lv_event_get_code(e) == LV_EVENT_CREATE) {

        glyph_paths = lv_malloc_zeroed(sizeof(lv_draw_sw_letter_outlines_t));
        LV_ASSERT_MALLOC(glyph_paths);

        glyph_paths->cur_path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_HIGH);
        glyph_paths->inside_path = glyph_paths->cur_path;
        outline_event->outline = glyph_paths;
        return;

    }
    else if(lv_event_get_code(e) == LV_EVENT_DELETE) {

        if(glyph_paths->inside_path != NULL) {
            lv_vector_path_clear(glyph_paths->inside_path);
            lv_vector_path_delete(glyph_paths->inside_path);
        }

        if(glyph_paths->outside_path != NULL) {
            lv_vector_path_clear(glyph_paths->outside_path);
            lv_vector_path_delete(glyph_paths->outside_path);
        }

        lv_free(glyph_paths);
        return;

    }
    else if(outline_event->type == LV_FREETYPE_OUTLINE_BORDER_START) {

        /* Inside path is done - create the border path */
        lv_vector_path_close(glyph_paths->cur_path);
        glyph_paths->cur_path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_HIGH);
        glyph_paths->outside_path = glyph_paths->cur_path;
        return;
    }

    path = glyph_paths->cur_path;

    switch(outline_event->type) {

        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_vector_path_move_to(path, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_vector_path_line_to(path, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            ctrl_pnt1.x = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control1.x);
            ctrl_pnt1.y = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control1.y);
            ctrl_pnt2.x = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control2.x);
            ctrl_pnt2.y = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control2.y);
            lv_vector_path_cubic_to(path, &ctrl_pnt1, &ctrl_pnt2, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_CONIC_TO:
            ctrl_pnt1.x = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control1.x);
            ctrl_pnt1.y = LV_FREETYPE_F26DOT6_TO_FLOAT(outline_event->control1.y);
            lv_vector_path_quad_to(path, &ctrl_pnt1, &pnt);
            break;
        case LV_FREETYPE_OUTLINE_END:
        case LV_FREETYPE_OUTLINE_BORDER_START:
            /* It's not necessary to close the path and
             * border start is handled above
             */
            break;
    }
}

#endif /* LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC */

#endif /*LV_USE_DRAW_SW*/
