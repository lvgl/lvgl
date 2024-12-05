#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

#include "../../../misc/lv_area_private.h"

void lv_draw_dave2d_arc(lv_draw_dave2d_unit_t * u, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{

    uint32_t                flags = 0;
    int32_t sin_start;
    int32_t cos_start;
    int32_t sin_end;
    int32_t cos_end;
    d2_s32 result;
    lv_area_t clipped_area;
    lv_area_t buffer_area;
    lv_point_t arc_centre;
    int32_t x;
    int32_t y;

    if(!lv_area_intersect(&clipped_area, coords, u->base_unit.clip_area)) return;

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    buffer_area = u->base_unit.target_layer->buf_area;

    arc_centre = dsc->center;
    arc_centre.x = arc_centre.x - buffer_area.x1;
    arc_centre.y = arc_centre.y - buffer_area.y1;

    lv_area_move(&clipped_area, x, y);
    lv_area_move(&buffer_area, x, y);

    //
    // If both angles are equal (e.g. 0 and 0 or 180 and 180) nothing has to be done
    //
    if(dsc->start_angle == dsc->end_angle) {
        return;                      // Nothing to do, no angle - no arc
    }

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

#if D2_RENDER_EACH_OPERATION
    d2_selectrenderbuffer(u->d2_handle, u->renderbuffer);
#endif

    //
    // Generate render operations
    //
    d2_framebuffer_from_layer(u->d2_handle, u->base_unit.target_layer);

    d2_setalpha(u->d2_handle, dsc->opa);

    d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(dsc->color));

    result = d2_cliprect(u->d2_handle, (d2_border)clipped_area.x1, (d2_border)clipped_area.y1, (d2_border)clipped_area.x2,
                         (d2_border)clipped_area.y2);
    LV_ASSERT(D2_OK == result);

    if(360 <= LV_ABS(dsc->start_angle - dsc->end_angle)) {
        d2_rendercircle(u->d2_handle,
                        (d2_point)D2_FIX4(arc_centre.x),
                        (d2_point) D2_FIX4(arc_centre.y),
                        (d2_width) D2_FIX4(dsc->radius - dsc->width / 2),
                        (d2_width) D2_FIX4(dsc->width));
    }
    else { //An ARC, not a full circle
        //
        // If the difference between both is larger than 180 degrees we must use the concave flag
        //
        /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
        if((LV_ABS(dsc->start_angle - dsc->end_angle) > 180) || ((dsc->end_angle < dsc->start_angle) &&
                                                                 (LV_ABS(dsc->start_angle - (dsc->end_angle + 360)) > 180))) {
            flags = d2_wf_concave;
        }
        else {
            flags = 0;
        }

        sin_start = lv_trigo_sin((int16_t)dsc->start_angle);
        cos_start = lv_trigo_cos((int16_t)dsc->start_angle);

        sin_end   = lv_trigo_sin((int16_t)dsc->end_angle);
        cos_end   = lv_trigo_cos((int16_t)dsc->end_angle);

        bool draw_arc;
        lv_area_t arc_area;
        lv_area_t clip_arc;
        lv_point_t start_point;
        lv_point_t end_point;

        start_point.x = arc_centre.x + (int16_t)(((dsc->radius) * cos_start) >> LV_TRIGO_SHIFT);
        start_point.y = arc_centre.y + (int16_t)(((dsc->radius) * sin_start) >> LV_TRIGO_SHIFT);

        end_point.x = arc_centre.x + (int16_t)(((dsc->radius) * cos_end) >> LV_TRIGO_SHIFT);
        end_point.y = arc_centre.y + (int16_t)(((dsc->radius) * sin_end) >> LV_TRIGO_SHIFT);

        arc_area.x1 = LV_MIN3(start_point.x, end_point.x, arc_centre.x);
        arc_area.y1 = LV_MIN3(start_point.y, end_point.y, arc_centre.y);

        arc_area.x2 = LV_MAX3(start_point.x, end_point.x, arc_centre.x);
        arc_area.y2 = LV_MAX3(start_point.y, end_point.y, arc_centre.y);

        /* 0 degrees */
        if((dsc->end_angle < dsc->start_angle) || ((dsc->start_angle < 360) && (dsc->end_angle > 360))) {
            arc_area.x2 = arc_centre.x + dsc->radius;
        }

        /* 90 degrees */
        if(((dsc->end_angle > 90) && (dsc->start_angle < 90)) || ((dsc->start_angle < 90) &&
                                                                  (dsc->end_angle < dsc->start_angle))) {
            arc_area.y2 = arc_centre.y + dsc->radius;
        }

        /* 180 degrees */
        if(((dsc->end_angle > 180) && (dsc->start_angle < 180)) || ((dsc->start_angle < 180) &&
                                                                    (dsc->end_angle < dsc->start_angle))) {
            arc_area.x1 = arc_centre.x - dsc->radius;
        }

        /* 270 degrees */
        if(((dsc->end_angle > 270) && (dsc->start_angle < 270)) || ((dsc->start_angle < 270) &&
                                                                    (dsc->end_angle < dsc->start_angle))) {
            arc_area.y1 = arc_centre.y - dsc->radius;
        }

        draw_arc = lv_area_intersect(&clip_arc, &arc_area, &clipped_area);

        if(draw_arc) {

            result = d2_renderwedge(u->d2_handle,
                                    (d2_point)D2_FIX4(arc_centre.x),
                                    (d2_point) D2_FIX4(arc_centre.y),
                                    (d2_width) D2_FIX4(dsc->radius - dsc->width / 2),
                                    (d2_width) D2_FIX4(dsc->width),
                                    -(d2_s32)(sin_start << 1),
                                    (d2_s32)(cos_start << 1),
                                    (d2_s32)(sin_end << 1),
                                    -(d2_s32)(cos_end << 1),
                                    flags);
            LV_ASSERT(D2_OK == result);

            if(dsc->rounded) {
                lv_point_t start_coord;
                lv_point_t end_coord;

                start_coord.x = arc_centre.x + (int16_t)(((dsc->radius - dsc->width / 2) * cos_start) >> LV_TRIGO_SHIFT);
                start_coord.y = arc_centre.y + (int16_t)(((dsc->radius - dsc->width / 2) * sin_start) >> LV_TRIGO_SHIFT);

                /** Render a circle. */
                d2_rendercircle(u->d2_handle,
                                (d2_point) D2_FIX4((uint16_t)(start_coord.x)),
                                (d2_point) D2_FIX4((uint16_t)(start_coord.y)),
                                (d2_width) D2_FIX4(dsc->width / 2), 0);

                end_coord.x = arc_centre.x + (int16_t)(((dsc->radius - dsc->width / 2) * cos_end) >> LV_TRIGO_SHIFT);
                end_coord.y = arc_centre.y + (int16_t)(((dsc->radius - dsc->width / 2) * sin_end) >> LV_TRIGO_SHIFT);

                /** Render a circle. */
                d2_rendercircle(u->d2_handle,
                                (d2_point) D2_FIX4((uint16_t)(end_coord.x)),
                                (d2_point) D2_FIX4((uint16_t)(end_coord.y)),
                                (d2_width) D2_FIX4(dsc->width / 2), 0);
            }
        }
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
