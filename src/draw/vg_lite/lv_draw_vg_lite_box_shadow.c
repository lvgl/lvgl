/**
 * @file lv_draw_vg_lite_box_shadow.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"
#include "../../misc/lv_area_private.h"
#include "../sw/blend/lv_draw_sw_blend.h"
#include "../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/*SW can render the shadow into a temporary buffer for VG-Lite to blit*/
#define SW_SHADOW_AVAILABLE (LV_USE_DRAW_SW && LV_DRAW_SW_COMPLEX && LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void get_core_area(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, lv_area_t * core_area);
static void get_shape_area(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, int32_t dist,
                           lv_area_t * shape_area);
static int32_t get_radius_at(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist);
static bool obj_is_inside_shape(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, int32_t dist);
static bool obj_surely_inside_shape(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist);
static float get_profile(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist);
static void warn_sw_fallback(void);
static int32_t get_clamped_radius(int32_t radius, const lv_area_t * area);
#if SW_SHADOW_AVAILABLE
static void draw_with_sw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, const lv_area_t * shadow_area);
#endif
static void get_bg_area(const lv_area_t * coords, lv_area_t * bg_area);
static void fill_ring(lv_draw_vg_lite_unit_t * u, const lv_area_t * clip_area, const lv_draw_box_shadow_dsc_t * dsc,
                      lv_opa_t opa, const lv_area_t * outer, int32_t outer_radius,
                      const lv_area_t * inner, int32_t inner_radius);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool sw_fallback_warned;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_vg_lite_box_shadow_is_supported(const lv_draw_box_shadow_dsc_t * dsc, lv_color_format_t layer_cf)
{
    bool concentric = dsc->ofs_x == 0 && dsc->ofs_y == 0;
    if(dsc->bg_cover || concentric || obj_surely_inside_shape(dsc, -(dsc->width / 2))) return true;

    /*Otherwise the shadow would be drawn under the translucent object. Let SW draw it if it can.*/
#if LV_USE_DRAW_SW
    if(!lv_draw_sw_blend_is_dest_cf_supported(layer_cf)) return true;

    warn_sw_fallback();
    return false;
#else
    LV_UNUSED(layer_cf);
    return true;
#endif
}

void lv_draw_vg_lite_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                                const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    /*Calculate the rectangle which is blurred to get the shadow in `shadow_area`*/
    lv_area_t core_area;
    get_core_area(dsc, coords, &core_area);

    /*Calculate the bounding box of the shadow*/
    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - dsc->width / 2 - 1;
    shadow_area.x2 = core_area.x2 + dsc->width / 2 + 1;
    shadow_area.y1 = core_area.y1 - dsc->width / 2 - 1;
    shadow_area.y2 = core_area.y2 + dsc->width / 2 + 1;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &shadow_area, clip_area)) return;

    LV_PROFILER_DRAW_BEGIN;

    bool concentric = dsc->ofs_x == 0 && dsc->ofs_y == 0;
    int32_t half_w = dsc->width / 2;
    /*Where the object's edge is, measured outwards from the edge of the core*/
    int32_t obj_dist = -dsc->spread;
    /*As in the SW renderer, the outermost pixels of the object get shadow too.
     *A concentric shadow further inside is under the object.*/
    int32_t inner_dist = concentric ? LV_MAX(-half_w, obj_dist - 1) : -half_w;
    bool obj_inside = obj_is_inside_shape(dsc, coords, inner_dist);

#if SW_SHADOW_AVAILABLE
    /*SW can't draw into this layer (see lv_draw_vg_lite_box_shadow_is_supported()), but it can keep the shadow
     *from under the translucent object in a buffer of its own*/
    if(!dsc->bg_cover && !obj_inside) {
        draw_with_sw(draw_unit, layer, clip_area, dsc, coords, &shadow_area);
        LV_PROFILER_DRAW_END;
        return;
    }
#endif

    /*Nested shapes `dist` px outside the edge of the core (negative: inside), from the outermost inwards.
     *Each one adds to the opacity of everything inside it, so together they follow the blurred profile without
     *seams between neighbouring shapes. The innermost one also covers the center.
     *Cut the object out of them where possible: required under a translucent object (otherwise SW can't draw into
     *this layer and the shadow shows through), saves filling the hidden area under a covering one.*/
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;
    lv_area_t bg_area;
    get_bg_area(coords, &bg_area);
    float opa_max = (float)dsc->opa / 255.0f;
    float drawn = 0.0f;
    int32_t dist;
    int32_t outer_dist = LV_MAX(dsc->width - half_w - 1, inner_dist);
    for(dist = outer_dist; dist >= inner_dist; dist--) {
        float target = dist == -half_w ? opa_max : get_profile(dsc, dist) * opa_max;
        if(target <= drawn) continue;

        lv_opa_t opa = (lv_opa_t)((target - drawn) / (1.0f - drawn) * 255.0f + 0.5f);
        if(opa <= LV_OPA_MIN) continue;
        drawn = 1.0f - (1.0f - drawn) * (1.0f - (float)opa / 255.0f);

        lv_area_t shape_area;
        get_shape_area(dsc, coords, dist, &shape_area);
        if(lv_area_get_width(&shape_area) <= 0 || lv_area_get_height(&shape_area) <= 0) continue;

        fill_ring(u, clip_area, dsc, opa, &shape_area, get_radius_at(dsc, dist + 1),
                  obj_inside ? &bg_area : NULL, dsc->radius);
    }

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void get_core_area(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, lv_area_t * core_area)
{
    core_area->x1 = coords->x1  + dsc->ofs_x - dsc->spread;
    core_area->x2 = coords->x2  + dsc->ofs_x + dsc->spread;
    core_area->y1 = coords->y1  + dsc->ofs_y - dsc->spread;
    core_area->y2 = coords->y2  + dsc->ofs_y + dsc->spread;
}

/*The shape whose edge is `dist` px outside the edge of the core (negative: inside), i.e. the core grown by `dist + 1`*/
static void get_shape_area(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, int32_t dist,
                           lv_area_t * shape_area)
{
    get_core_area(dsc, coords, shape_area);
    lv_area_increase(shape_area, dist + 1, dist + 1);
}

/**
 * The corner radius of the shadow's contour `dist` px outside the edge of the core (negative: inside).
 * It's the shadow's radius between the core and the object, and grows (or shrinks) by 1 px per px beyond them,
 * so the contour at the object's edge matches the object.
 */
static int32_t get_radius_at(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist)
{
    int32_t obj_dist = -dsc->spread;
    int32_t outer = LV_MAX(0, obj_dist);
    int32_t inner = LV_MIN(0, obj_dist);
    int32_t r;
    if(dist >= outer) r = dsc->radius + dist - outer;
    else if(dist >= inner) r = dsc->radius;
    else r = dsc->radius + dist - inner;
    return LV_MAX(r, 0);
}

/*Cutting the object (its bg area) out of a shape with the even-odd rule works only if it's inside the shape:
 *its corners may be sharper, so it needs a margin of the radius difference.*/
static bool obj_is_inside_shape(const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, int32_t dist)
{
    lv_area_t shape_area;
    get_shape_area(dsc, coords, dist, &shape_area);
    if(lv_area_get_width(&shape_area) <= 0 || lv_area_get_height(&shape_area) <= 0) return false;

    int32_t shape_radius = get_clamped_radius(get_radius_at(dsc, dist + 1), &shape_area);
    lv_area_t bg_area;
    get_bg_area(coords, &bg_area);
    int32_t margin = shape_radius - get_clamped_radius(dsc->radius, &bg_area);
    margin = LV_MAX(margin, 0);
    lv_area_increase(&bg_area, margin, margin);
    return lv_area_is_in(&bg_area, &shape_area, 0);
}

/**
 * Like obj_is_inside_shape() without the object's size, so without knowing how its radius gets clamped:
 * assume the largest margin. The object (its bg area is 1 px smaller) must stay that far inside the shape
 * on each side despite the offset.
 */
static bool obj_surely_inside_shape(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist)
{
    int32_t margin = get_radius_at(dsc, dist + 1);
    int32_t room = dsc->spread + dist + 2 - margin;
    return LV_ABS(dsc->ofs_x) <= room && LV_ABS(dsc->ofs_y) <= room;
}

/**
 * The blurred profile at the 1 px band `dist` px outside the edge of the core (negative: inside), 0..1.
 * A smoothstep from 1 at `width / 2` inside the edge to 0 at `width / 2` outside it,
 * shifted by 0.75 px to match the SW renderer's blur.
 */
static float get_profile(const lv_draw_box_shadow_dsc_t * dsc, int32_t dist)
{
    if(dsc->width <= 0) return dist < 0 ? 1.0f : 0.0f;

    float t = ((float)dsc->width / 2.0f - (float)dist - 0.75f) / (float)dsc->width;
    t = LV_CLAMP(0.0f, t, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static void warn_sw_fallback(void)
{
    if(sw_fallback_warned) return;
    sw_fallback_warned = true;

    LV_LOG_WARN("A box shadow with an offset would reach under a translucent object, "
                "so the SW renderer draws it (slower).");
}

static int32_t get_clamped_radius(int32_t radius, const lv_area_t * area)
{
    int32_t short_side = LV_MIN(lv_area_get_width(area), lv_area_get_height(area));
    return LV_MIN(radius, (short_side + 1) / 2);
}

#if SW_SHADOW_AVAILABLE
static void draw_with_sw(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                         const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords, const lv_area_t * shadow_area)
{
    lv_area_t buf_area;
    if(!lv_area_intersect(&buf_area, shadow_area, clip_area)) return;

    warn_sw_fallback();

    lv_draw_buf_t * buf = lv_draw_buf_create(lv_area_get_width(&buf_area), lv_area_get_height(&buf_area),
                                             LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED, LV_STRIDE_AUTO);
    if(buf == NULL) {
        LV_LOG_WARN("Failed to allocate the box shadow buffer");
        return;
    }
    lv_draw_buf_clear(buf, NULL);

    lv_layer_t sw_layer;
    lv_memzero(&sw_layer, sizeof(sw_layer));
    sw_layer.draw_buf = buf;
    sw_layer.buf_area = buf_area;
    sw_layer.color_format = LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
    lv_draw_sw_box_shadow(&sw_layer, &buf_area, dsc, coords);

    lv_draw_image_dsc_t image_dsc;
    lv_draw_image_dsc_init(&image_dsc);
    image_dsc.src = buf;
    lv_draw_vg_lite_img(draw_unit, layer, clip_area, &image_dsc, &buf_area, true);

    /*The GPU reads the buffer*/
    lv_vg_lite_finish((lv_draw_vg_lite_unit_t *)draw_unit);
    lv_draw_buf_destroy(buf);
}
#endif

/*1 px smaller than the object, as in the SW renderer, to be sure its edge is covered by the shadow*/
static void get_bg_area(const lv_area_t * coords, lv_area_t * bg_area)
{
    *bg_area = *coords;
    lv_area_increase(bg_area, -1, -1);
}

static void fill_ring(lv_draw_vg_lite_unit_t * u, const lv_area_t * clip_area, const lv_draw_box_shadow_dsc_t * dsc,
                      lv_opa_t opa, const lv_area_t * outer, int32_t outer_radius,
                      const lv_area_t * inner, int32_t inner_radius)
{
    lv_area_t clipped_area;
    if(!lv_area_intersect(&clipped_area, outer, clip_area)) return;

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, outer_radius == 0 && inner_radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bounding_box_area(path, &clipped_area);
    lv_vg_lite_path_append_rect(path,
                                outer->x1, outer->y1,
                                lv_area_get_width(outer), lv_area_get_height(outer),
                                outer_radius);
    if(inner && lv_area_get_width(inner) > 0 && lv_area_get_height(inner) > 0) {
        lv_vg_lite_path_append_rect(path,
                                    inner->x1, inner->y1,
                                    lv_area_get_width(inner), lv_area_get_height(inner),
                                    inner_radius);
    }
    lv_vg_lite_path_end(path);

    vg_lite_matrix_t matrix = u->global_matrix;
    lv_vg_lite_draw(
        &u->target_buffer,
        lv_vg_lite_path_get_path(path),
        VG_LITE_FILL_EVEN_ODD,
        &matrix,
        VG_LITE_BLEND_SRC_OVER,
        lv_vg_lite_color(dsc->color, opa, true));

    lv_vg_lite_path_drop(u, path);
}

#endif /*LV_USE_DRAW_VG_LITE*/
