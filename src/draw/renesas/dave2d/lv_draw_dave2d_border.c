#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

static void dave2d_draw_border_complex(lv_draw_dave2d_unit_t * draw_unit, const lv_area_t * outer_area,
                                       const lv_area_t * inner_area,
                                       int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa);

static void dave2d_draw_border_simple(lv_draw_dave2d_unit_t * draw_unit, const lv_area_t * outer_area,
                                      const lv_area_t * inner_area,
                                      lv_color_t color, lv_opa_t opa);

void lv_draw_dave2d_border(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->side & LV_BORDER_SIDE_LEFT) ? dsc->width : - (dsc->width + rout));
    area_inner.x2 -= ((dsc->side & LV_BORDER_SIDE_RIGHT) ? dsc->width : - (dsc->width + rout));
    area_inner.y1 += ((dsc->side & LV_BORDER_SIDE_TOP) ? dsc->width : - (dsc->width + rout));
    area_inner.y2 -= ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : - (dsc->width + rout));

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    if(rout == 0 && rin == 0) {
        dave2d_draw_border_simple(draw_unit, coords, &area_inner, dsc->color, dsc->opa);
    }
    else {
        dave2d_draw_border_complex(draw_unit, coords, &area_inner, rout, rin, dsc->color, dsc->opa);
    }

}

static void dave2d_draw_border_simple(lv_draw_dave2d_unit_t * u, const lv_area_t * outer_area,
                                      const lv_area_t * inner_area,
                                      lv_color_t color, lv_opa_t opa)

{

    lv_area_t clip_area;
    lv_area_t local_outer_area;
    lv_area_t local_inner_area;
    int32_t x;
    int32_t y;
    bool is_common;

    is_common = lv_area_intersect(&clip_area, outer_area, u->base_unit.clip_area);
    if(!is_common) return;

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    local_outer_area = *outer_area;
    local_inner_area = *inner_area;

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&clip_area, x, y);
    lv_area_move(&local_outer_area, x, y);
    lv_area_move(&local_inner_area, x, y);

#if D2_RENDER_EACH_OPERATION
    d2_selectrenderbuffer(u->d2_handle, u->renderbuffer);
#endif
    //
    // Generate render operations
    //

    d2_framebuffer_from_layer(u->d2_handle, u->base_unit.target_layer);

    d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(color));
    d2_setalpha(u->d2_handle, opa);
    d2_cliprect(u->d2_handle, (d2_border)clip_area.x1, (d2_border)clip_area.y1, (d2_border)clip_area.x2,
                (d2_border)clip_area.y2);

    lv_area_t a;

    bool top_side = local_outer_area.y1 <= local_inner_area.y1;
    bool bottom_side = local_outer_area.y2 >= local_inner_area.y2;
    bool left_side = local_outer_area.x1 <= local_inner_area.x1;
    bool right_side = local_outer_area.x2 >= local_inner_area.x2;

    /*Top*/
    a.x1 = local_outer_area.x1;
    a.x2 = local_outer_area.x2;
    a.y1 = local_outer_area.y1;
    a.y2 = local_inner_area.y1 - 1;
    if(top_side) {
        d2_renderbox(u->d2_handle, (d2_point)D2_FIX4(a.x1),
                     (d2_point)D2_FIX4(a.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&a)),
                     (d2_point)D2_FIX4(lv_area_get_height(&a)));
    }

    /*Bottom*/
    a.y1 = local_inner_area.y2 + 1;
    a.y2 = local_outer_area.y2;
    if(bottom_side) {
        d2_renderbox(u->d2_handle, (d2_point)D2_FIX4(a.x1),
                     (d2_point)D2_FIX4(a.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&a)),
                     (d2_point)D2_FIX4(lv_area_get_height(&a)));
    }

    /*Left*/
    a.x1 = local_outer_area.x1;
    a.x2 = local_inner_area.x1 - 1;
    a.y1 = (top_side) ? local_inner_area.y1 : local_outer_area.y1;
    a.y2 = (bottom_side) ? local_inner_area.y2 : local_outer_area.y2;
    if(left_side) {
        d2_renderbox(u->d2_handle, (d2_point)D2_FIX4(a.x1),
                     (d2_point)D2_FIX4(a.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&a)),
                     (d2_point)D2_FIX4(lv_area_get_height(&a)));
    }

    /*Right*/
    a.x1 = local_inner_area.x2 + 1;
    a.x2 = local_outer_area.x2;
    if(right_side) {
        d2_renderbox(u->d2_handle, (d2_point)D2_FIX4(a.x1),
                     (d2_point)D2_FIX4(a.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&a)),
                     (d2_point)D2_FIX4(lv_area_get_height(&a)));
    }

    //
    // Execute render operations
    //
#if D2_RENDER_EACH_OPERATION
    d2_executerenderbuffer(u->d2_handle, u->renderbuffer, 0);
    d2_flushframe(u->d2_handle);
#endif

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif
}

static void dave2d_draw_border_complex(lv_draw_dave2d_unit_t * u, const lv_area_t * orig_outer_area,
                                       const lv_area_t * orig_inner_area,
                                       int32_t rout, int32_t rin, lv_color_t color, lv_opa_t opa)
{
    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `coords`*/
    lv_area_t draw_area;
    lv_area_t outer_area;
    lv_area_t inner_area;
    int32_t x;
    int32_t y;
    d2_s32 result;
    d2_u32 flags = 0;

    outer_area = *orig_outer_area;
    inner_area = *orig_inner_area;

    if(!lv_area_intersect(&draw_area, &outer_area, u->base_unit.clip_area)) return;

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&draw_area, x, y);
    lv_area_move(&outer_area, x, y);
    lv_area_move(&inner_area, x, y);

#if D2_RENDER_EACH_OPERATION
    d2_selectrenderbuffer(u->d2_handle, u->renderbuffer);
#endif
    //
    // Generate render operations
    //

    d2_framebuffer_from_layer(u->d2_handle, u->base_unit.target_layer);

    d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(color));
    d2_setalpha(u->d2_handle, opa);
    d2_cliprect(u->d2_handle, (d2_border)draw_area.x1, (d2_border)draw_area.y1, (d2_border)draw_area.x2,
                (d2_border)draw_area.y2);

    lv_area_t blend_area;
    /*Calculate the x and y coordinates where the straight parts area are */
    lv_area_t core_area;
    core_area.x1 = LV_MAX(outer_area.x1 + rout, inner_area.x1);
    core_area.x2 = LV_MIN(outer_area.x2 - rout, inner_area.x2);
    core_area.y1 = LV_MAX(outer_area.y1 + rout, inner_area.y1);
    core_area.y2 = LV_MIN(outer_area.y2 - rout, inner_area.y2);

    bool top_side = outer_area.y1 <= inner_area.y1;
    bool bottom_side = outer_area.y2 >= inner_area.y2;

    /*No masks*/
    bool left_side = outer_area.x1 <= inner_area.x1;
    bool right_side = outer_area.x2 >= inner_area.x2;

    /*Draw the straight lines first */
    if(top_side) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = outer_area.y1;
        blend_area.y2 = inner_area.y1 - 1;
        d2_renderbox(u->d2_handle,
                     (d2_point)D2_FIX4(blend_area.x1),
                     (d2_point)D2_FIX4(blend_area.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&blend_area)),
                     (d2_point)D2_FIX4(lv_area_get_height(&blend_area)));
    }

    if(bottom_side) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = inner_area.y2 + 1;
        blend_area.y2 = outer_area.y2;
        d2_renderbox(u->d2_handle,
                     (d2_point)D2_FIX4(blend_area.x1),
                     (d2_point)D2_FIX4(blend_area.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&blend_area)),
                     (d2_point)D2_FIX4(lv_area_get_height(&blend_area)));
    }

    if(left_side) {
        blend_area.x1 = outer_area.x1;
        blend_area.x2 = inner_area.x1 - 1;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        d2_renderbox(u->d2_handle,
                     (d2_point)D2_FIX4(blend_area.x1),
                     (d2_point)D2_FIX4(blend_area.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&blend_area)),
                     (d2_point)D2_FIX4(lv_area_get_height(&blend_area)));
    }

    if(right_side) {
        blend_area.x1 = inner_area.x2 + 1;
        blend_area.x2 = outer_area.x2;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        d2_renderbox(u->d2_handle,
                     (d2_point)D2_FIX4(blend_area.x1),
                     (d2_point)D2_FIX4(blend_area.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&blend_area)),
                     (d2_point)D2_FIX4(lv_area_get_height(&blend_area)));
    }

    /*Draw the corners*/
    int32_t blend_w;
    /*Left corners*/
    blend_area.x1 = draw_area.x1;
    blend_area.x2 = LV_MIN(draw_area.x2, core_area.x1 - 1);

    blend_w = lv_area_get_width(&blend_area);

    if(blend_w > 0) {
        d2_s32 aa;
        aa = d2_getantialiasing(u->d2_handle);
        d2_setantialiasing(u->d2_handle, 0); //Don't blend with the background according to coverage value

        if(left_side || top_side) {
            lv_area_t arc_area;
            lv_area_t clip_arc;

            arc_area.x1 = core_area.x1 - rout;
            arc_area.y1 = core_area.y1 - rout;
            arc_area.x2 = core_area.x1;
            arc_area.y2 = core_area.y1;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {
                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);
                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(core_area.x1),
                                        (d2_point) D2_FIX4(core_area.y1),
                                        (d2_width) D2_FIX4(rout),
                                        (d2_width) D2_FIX4((rout - rin)),
                                        (d2_s32) D2_FIX16(0), // 180 Degrees
                                        (d2_s32)  D2_FIX16((int16_t) -1),
                                        (d2_s32)  D2_FIX16((int16_t) -1),//( 270 Degrees
                                        (d2_s32) D2_FIX16(0),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }

        }

        if(left_side || bottom_side) {
            lv_area_t arc_area;
            lv_area_t clip_arc;

            arc_area.x1 = core_area.x1 - rout;
            arc_area.y1 = core_area.y2;
            arc_area.x2 = core_area.x1;
            arc_area.y2 = core_area.y2 + rout;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {

                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);
                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(core_area.x1),
                                        (d2_point) D2_FIX4(core_area.y2),
                                        (d2_width) D2_FIX4(rout),
                                        (d2_width) D2_FIX4((rout - rin)),
                                        (d2_s32) D2_FIX16((int16_t) -1), //90 degrees
                                        (d2_s32)  D2_FIX16(0),
                                        (d2_s32)  D2_FIX16(0), //180 degrees
                                        (d2_s32) D2_FIX16(1),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }
        }

        /*Right corners*/
        blend_area.x1 = LV_MAX(draw_area.x1, blend_area.x2 + 1);    /*To not overlap with the left side*/
        blend_area.x1 = LV_MAX(draw_area.x1, core_area.x2 + 1);

        blend_area.x2 = draw_area.x2;
        blend_w = lv_area_get_width(&blend_area);

        if(blend_w > 0) {
            if(right_side || top_side) {

                lv_area_t arc_area;
                lv_area_t clip_arc;

                arc_area.x1 = core_area.x2;
                arc_area.y1 = core_area.y1 - rout;
                arc_area.x2 = core_area.x2 + rout;
                arc_area.y2 = core_area.y1;

                if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {

                    d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                                (d2_border)clip_arc.y2);
                    result = d2_renderwedge(u->d2_handle,
                                            (d2_point)D2_FIX4(core_area.x2),
                                            (d2_point) D2_FIX4(core_area.y1),
                                            (d2_width) D2_FIX4(rout),
                                            (d2_width) D2_FIX4((rout - rin)),
                                            (d2_s32) D2_FIX16((int16_t)1), // 270 Degrees
                                            (d2_s32)  D2_FIX16(0),
                                            (d2_s32)  D2_FIX16(0),// 0 degrees
                                            (d2_s32) D2_FIX16(-1),
                                            flags);
                    LV_ASSERT(D2_OK == result);
                }

            }

            if(right_side || bottom_side) {
                lv_area_t arc_area;
                lv_area_t clip_arc;

                arc_area.x1 = core_area.x2;
                arc_area.y1 = core_area.y2;
                arc_area.x2 = core_area.x2 + rout;
                arc_area.y2 = core_area.y2 + rout;

                if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {

                    d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                                (d2_border)clip_arc.y2);
                    result = d2_renderwedge(u->d2_handle,
                                            (d2_point)D2_FIX4(core_area.x2),
                                            (d2_point) D2_FIX4(core_area.y2),
                                            (d2_width) D2_FIX4(rout),
                                            (d2_width) D2_FIX4((rout - rin)),
                                            (d2_s32) D2_FIX16(0),// 0 degrees
                                            (d2_s32)  D2_FIX16(1),
                                            (d2_s32)  D2_FIX16(1),// 90 degrees
                                            (d2_s32) D2_FIX16(0),
                                            flags);
                    LV_ASSERT(D2_OK == result);
                }
            }
        }
        d2_setantialiasing(u->d2_handle, aa); //restore original setting
    }

    //
    // Execute render operations
    //
#if D2_RENDER_EACH_OPERATION
    d2_executerenderbuffer(u->d2_handle, u->renderbuffer, 0);
    d2_flushframe(u->d2_handle);
#endif

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

}

#endif /*LV_USE_DRAW_DAVE2D*/
