#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

#include "../../../misc/lv_area_private.h"

void lv_draw_dave2d_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_area_t draw_area;
    lv_area_t coordinates;
    bool is_common;
    int32_t x;
    int32_t y;
    d2_u8 current_alpha_mode = 0;
    d2_s32 result;
    d2_u32 flags = 0;

    lv_point_t arc_centre;
    lv_draw_dave2d_unit_t * u = (lv_draw_dave2d_unit_t *)t->draw_unit;

    is_common = lv_area_intersect(&draw_area, coords, &t->clip_area);
    if(!is_common) return;

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    lv_area_copy(&coordinates, coords);

    x = 0 - t->target_layer->buf_area.x1;
    y = 0 - t->target_layer->buf_area.y1;

    lv_area_move(&draw_area, x, y);
    lv_area_move(&coordinates, x, y);

    d2_framebuffer_from_layer(u->d2_handle, t->target_layer);

    if(LV_GRAD_DIR_NONE != dsc->grad.dir) {
        float a1;
        float a2;

        float y1;
        float y2;

        float y3;
        float y0;
        int16_t y0_i ;
        int16_t y3_i ;

        if(LV_GRAD_DIR_VER == dsc->grad.dir) {
            a1 = dsc->grad.stops[0].opa;
            a2 = dsc->grad.stops[dsc->grad.stops_count - 1].opa;

            y1 = (float)LV_MIN(coordinates.y1, coordinates.y2);
            y2 = (float)LV_MAX(coordinates.y1, coordinates.y2);

            if(a1 < a2) {
                y0 = 0.0f;//silence the compiler warning
                y3 = 0.0f;

            }
            else {
                y0 = y2 - ((y2 - y1) / (a2 - a1) * (a2)); //point where alpha is 0
                y3 = y1 + ((y2 - y1) / (a2 - a1) * (255 - a1)); //point where alpha is 255
            }

            y0_i = (int16_t)y0;
            y3_i = (int16_t)y3;

            d2_setalphagradient(u->d2_handle, 0, (d2_point)D2_FIX4(0), (d2_point)D2_FIX4(y0_i), (d2_point)D2_FIX4(0),
                                (d2_point)D2_FIX4((y3_i - y0_i)));
        }
        else if(LV_GRAD_DIR_HOR == dsc->grad.dir) {

            float x1;
            float x2;

            float x3;
            float x0;
            int16_t x0_i ;
            int16_t x3_i ;

            a1 = dsc->grad.stops[0].opa;
            a2 = dsc->grad.stops[dsc->grad.stops_count - 1].opa;

            x1 = (float)LV_MIN(coordinates.x1, coordinates.x2);
            x2 = (float)LV_MAX(coordinates.x1, coordinates.x2);

            if(a1 < a2) {
                x0 = 0.0f;//silence the compiler warning
                x3 = 0.0f;

            }
            else {
                x0 = x2 - ((x2 - x1) / (a2 - a1) * (a2)); //point where alpha is 0
                x3 = x1 + ((x2 - x1) / (a2 - a1) * (255 - a1)); //point where alpha is 255
            }

            x0_i = (int16_t)x0;
            x3_i = (int16_t)x3;

            d2_setalphagradient(u->d2_handle, 0, (d2_point)D2_FIX4(x0_i), (d2_point)D2_FIX4(0), (d2_point)D2_FIX4(x3_i - x0_i),
                                (d2_point)D2_FIX4((0)));
        }

        current_alpha_mode = d2_getalphamode(u->d2_handle);
        d2_setfillmode(u->d2_handle, d2_fm_color);
        d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(dsc->grad.stops[0].color));
        d2_setalphamode(u->d2_handle, d2_am_gradient1);
    }
    else {
        d2_setfillmode(u->d2_handle, d2_fm_color); //default
        d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(dsc->color));
        d2_setalpha(u->d2_handle, dsc->opa);
    }

    d2_cliprect(u->d2_handle, (d2_border)draw_area.x1, (d2_border)draw_area.y1, (d2_border)draw_area.x2,
                (d2_border)draw_area.y2);

    if(dsc->radius == 0) {

        d2_renderbox(u->d2_handle, (d2_point)D2_FIX4(coordinates.x1),
                     (d2_point)D2_FIX4(coordinates.y1),
                     (d2_point)D2_FIX4(lv_area_get_width(&coordinates)),
                     (d2_point)D2_FIX4(lv_area_get_height(&coordinates)));
    }
    else {
        /*Get the real radius. Can't be larger than the half of the shortest side */
        int32_t coords_bg_w = lv_area_get_width(&coordinates);
        int32_t coords_bg_h = lv_area_get_height(&coordinates);
        int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
        int32_t radius = LV_MIN(dsc->radius, short_side >> 1);

        arc_centre.x = coordinates.x1 + radius;
        arc_centre.y = coordinates.y1 + radius;

        if(((2 * radius) == coords_bg_w)  && ((2 * radius) == coords_bg_h)) {
            result = d2_rendercircle(u->d2_handle,
                                     (d2_point)D2_FIX4(arc_centre.x),
                                     (d2_point) D2_FIX4(arc_centre.y),
                                     (d2_width) D2_FIX4(radius),
                                     (d2_width) D2_FIX4(0));
            LV_ASSERT(D2_OK == result);
        }
        else {

            lv_area_t arc_area;
            lv_area_t clip_arc;
            arc_centre.x = coordinates.x1 + radius;
            arc_centre.y = coordinates.y1 + radius;

            arc_area.x1 = coordinates.x1;
            arc_area.y1 = coordinates.y1;
            arc_area.x2 = coordinates.x1 + radius;
            arc_area.y2 = coordinates.y1 + radius;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {

                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);

                // d2_renderwedge internally changes the clip rectangle, only draw it if it is in side the current clip rectangle
                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(arc_centre.x),
                                        (d2_point) D2_FIX4(arc_centre.y),
                                        (d2_width) D2_FIX4(radius),
                                        (d2_width) D2_FIX4(0),
                                        (d2_s32) D2_FIX16(0), // 180 Degrees
                                        (d2_s32)  D2_FIX16((int16_t) -1),
                                        (d2_s32)  D2_FIX16((int16_t) -1),//( 270 Degrees
                                        (d2_s32) D2_FIX16(0),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }

            arc_centre.x = coordinates.x2 - radius;
            arc_centre.y = coordinates.y1 + radius;

            arc_area.x1 = coordinates.x2 - radius;
            arc_area.y1 = coordinates.y1;
            arc_area.x2 = coordinates.x2;
            arc_area.y2 = coordinates.y1 + radius;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {
                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);

                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(arc_centre.x),
                                        (d2_point) D2_FIX4(arc_centre.y),
                                        (d2_width) D2_FIX4(radius),
                                        (d2_width) D2_FIX4(0),
                                        (d2_s32) D2_FIX16((int16_t)1), // 270 Degrees
                                        (d2_s32)  D2_FIX16(0),
                                        (d2_s32)  D2_FIX16(0),// 0 degrees
                                        (d2_s32) D2_FIX16(-1),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }

            arc_centre.x = coordinates.x2 - radius;
            arc_centre.y = coordinates.y2 - radius;

            arc_area.x1 = coordinates.x2 - radius;
            arc_area.y1 = coordinates.y2 - radius;
            arc_area.x2 = coordinates.x2;
            arc_area.y2 = coordinates.y2;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {
                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);

                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(arc_centre.x),
                                        (d2_point) D2_FIX4(arc_centre.y),
                                        (d2_width) D2_FIX4(radius),
                                        (d2_width) D2_FIX4(0),
                                        (d2_s32) D2_FIX16(0),// 0 degrees
                                        (d2_s32)  D2_FIX16(1),
                                        (d2_s32)  D2_FIX16(1),// 90 degrees
                                        (d2_s32) D2_FIX16(0),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }

            arc_centre.x = coordinates.x1 + radius;
            arc_centre.y = coordinates.y2 - radius;

            arc_area.x1 = coordinates.x1;
            arc_area.y1 = coordinates.y2 - radius;
            arc_area.x2 = coordinates.x1 + radius;
            arc_area.y2 = coordinates.y2;

            if(lv_area_intersect(&clip_arc, &arc_area, &draw_area)) {
                d2_cliprect(u->d2_handle, (d2_border)clip_arc.x1, (d2_border)clip_arc.y1, (d2_border)clip_arc.x2,
                            (d2_border)clip_arc.y2);

                result = d2_renderwedge(u->d2_handle,
                                        (d2_point)D2_FIX4(arc_centre.x),
                                        (d2_point) D2_FIX4(arc_centre.y),
                                        (d2_width) D2_FIX4(radius),
                                        (d2_width) D2_FIX4(0),
                                        (d2_s32) D2_FIX16((int16_t) -1), //90 degrees
                                        (d2_s32)  D2_FIX16(0),
                                        (d2_s32)  D2_FIX16(0), //180 degrees
                                        (d2_s32) D2_FIX16(1),
                                        flags);
                LV_ASSERT(D2_OK == result);
            }

            /* reset the clip rectangle */
            d2_cliprect(u->d2_handle, (d2_border)draw_area.x1, (d2_border)draw_area.y1, (d2_border)draw_area.x2,
                        (d2_border)draw_area.y2);

            result = d2_renderbox(u->d2_handle,
                                  (d2_width)D2_FIX4(coordinates.x1 + radius),
                                  (d2_width)D2_FIX4(coordinates.y1),
                                  (d2_width)D2_FIX4(lv_area_get_width(&coordinates) - (2 * radius)),
                                  (d2_width)D2_FIX4(lv_area_get_height(&coordinates)));
            LV_ASSERT(D2_OK == result);

            result = d2_renderbox(u->d2_handle,
                                  (d2_width)D2_FIX4(coordinates.x1),
                                  (d2_width)D2_FIX4(coordinates.y1 + radius),
                                  (d2_width)D2_FIX4(radius),
                                  (d2_width)D2_FIX4(lv_area_get_height(&coordinates) - (2 * radius)));
            LV_ASSERT(D2_OK == result);

            result = d2_renderbox(u->d2_handle,
                                  (d2_width)D2_FIX4(coordinates.x2 - radius),
                                  (d2_width)D2_FIX4(coordinates.y1 + radius),
                                  (d2_width)D2_FIX4(radius),
                                  (d2_width)D2_FIX4(lv_area_get_height(&coordinates) - (2 * radius)));
            LV_ASSERT(D2_OK == result);
        }
    }

    if(LV_GRAD_DIR_NONE != dsc->grad.dir) {
        d2_setalphamode(u->d2_handle, current_alpha_mode);
        d2_setfillmode(u->d2_handle, d2_fm_color); //default
    }

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif
}

#endif /*LV_USE_DRAW_DAVE2D*/
