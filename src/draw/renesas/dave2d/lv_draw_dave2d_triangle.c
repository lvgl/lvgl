#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

void lv_draw_dave2d_triangle(lv_draw_dave2d_unit_t * u, const lv_draw_triangle_dsc_t * dsc)
{
    lv_area_t clipped_area;
    d2_u32      flags = 0;
    d2_u8 current_alpha_mode = 0;
    int32_t x;
    int32_t y;

    lv_area_t tri_area;
    tri_area.x1 = LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    if(!_lv_area_intersect(&clipped_area, &tri_area, u->base_unit.clip_area)) return;

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }
#endif

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&clipped_area, x, y);

#if D2_RENDER_EACH_OPERATION
    d2_selectrenderbuffer(u->d2_handle, u->renderbuffer);
#endif

    lv_point_precise_t p[3];
    p[0] = dsc->p[0];
    p[1] = dsc->p[1];
    p[2] = dsc->p[2];

    /*Order the points like this:
     * [0]: top
     * [1]: right bottom
     * [2]: left bottom */

    if(dsc->p[0].y <= dsc->p[1].y && dsc->p[0].y <= dsc->p[2].y) {
        p[0] = dsc->p[0];
        if(dsc->p[1].x < dsc->p[2].x) {
            p[2] = dsc->p[1];
            p[1] = dsc->p[2];
        }
        else {
            p[2] = dsc->p[2];
            p[1] = dsc->p[1];
        }
    }
    else if(dsc->p[1].y <= dsc->p[0].y && dsc->p[1].y <= dsc->p[2].y) {
        p[0] = dsc->p[1];
        if(dsc->p[0].x < dsc->p[2].x) {
            p[2] = dsc->p[0];
            p[1] = dsc->p[2];
        }
        else {
            p[2] = dsc->p[2];
            p[1] = dsc->p[0];
        }
    }
    else {
        p[0] = dsc->p[2];
        if(dsc->p[0].x < dsc->p[1].x) {
            p[2] = dsc->p[0];
            p[1] = dsc->p[1];
        }
        else {
            p[2] = dsc->p[1];
            p[1] = dsc->p[0];
        }
    }

    p[0].x -= u->base_unit.target_layer->buf_area.x1;
    p[1].x -= u->base_unit.target_layer->buf_area.x1;
    p[2].x -= u->base_unit.target_layer->buf_area.x1;

    p[0].y -= u->base_unit.target_layer->buf_area.y1;
    p[1].y -= u->base_unit.target_layer->buf_area.y1;
    p[2].y -= u->base_unit.target_layer->buf_area.y1;

    p[1].y -= 1;
    p[2].y -= 1;

    current_alpha_mode = d2_getalphamode(u->d2_handle);

    if(LV_GRAD_DIR_NONE != dsc->bg_grad.dir) {
        float a1;
        float a2;

        float y1;
        float y2;

        float y3;
        float y0;
        int32_t y0_i ;
        int32_t y3_i ;

        if(LV_GRAD_DIR_VER == dsc->bg_grad.dir) {
            a1 = dsc->bg_grad.stops[0].opa;
            a2 = dsc->bg_grad.stops[dsc->bg_grad.stops_count - 1].opa;

            y1 = LV_MIN3(p[0].y, p[1].y, p[2].y);
            y2 = LV_MAX3(p[0].y, p[1].y, p[2].y);

            if(a1 < a2) {
                /* TODO */
                __BKPT(0);
                y0 = 0.0f;//silence the compiler warning
                y3 = 0.0f;

            }
            else {
                y0 = y2 - ((y2 - y1) / (a2 - a1) * (a2)); //point where alpha is 0
                y3 = y1 + ((y2 - y1) / (a2 - a1) * (255 - a1)); //point where alpha is 255
            }

            y0_i = (int16_t)y0;
            y3_i = (int16_t)y3;

            d2_setalphagradient(u->d2_handle, 0, D2_FIX4(0),  D2_FIX4(y0_i), D2_FIX4(0), D2_FIX4((y3_i - y0_i)));
        }
        else if(LV_GRAD_DIR_HOR == dsc->bg_grad.dir) {
            /* TODO */
            __BKPT(0);
        }

        d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(dsc->bg_grad.stops[0].color));
        d2_setalphamode(u->d2_handle, d2_am_gradient1);
    }
    else {
        d2_setalpha(u->d2_handle, dsc->bg_opa);
        d2_setalphamode(u->d2_handle, d2_am_constant);
        d2_setcolor(u->d2_handle, 0, lv_draw_dave2d_lv_colour_to_d2_colour(dsc->bg_color));

    }

    d2_framebuffer_from_layer(u->d2_handle, u->base_unit.target_layer);

    d2_cliprect(u->d2_handle, (d2_border)clipped_area.x1, (d2_border)clipped_area.y1, (d2_border)clipped_area.x2,
                (d2_border)clipped_area.y2);

    d2_rendertri(u->d2_handle,
                 (d2_point)      D2_FIX4(p[0].x),
                 (d2_point)      D2_FIX4(p[0].y),
                 (d2_point)      D2_FIX4(p[1].x),
                 (d2_point)      D2_FIX4(p[1].y),
                 (d2_point)      D2_FIX4(p[2].x),
                 (d2_point)      D2_FIX4(p[2].y),
                 flags);

    //
    // Execute render operations
    //
#if D2_RENDER_EACH_OPERATION
    d2_executerenderbuffer(u->d2_handle, u->renderbuffer, 0);
    d2_flushframe(u->d2_handle);
#endif

    d2_setalphamode(u->d2_handle, current_alpha_mode);

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }

#endif

}

#endif /*LV_USE_DRAW_DAVE2D*/
