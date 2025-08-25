#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

#include "../../../misc/lv_area_private.h"

void lv_draw_dave2d_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc,
                              const lv_area_t * coords)
{
    lv_area_t clipped_area;
    lv_area_t coordinates;
    int32_t x;
    int32_t y;

    lv_draw_dave2d_unit_t * u = (lv_draw_dave2d_unit_t *)t->draw_unit;

    if(!lv_area_intersect(&clipped_area, coords, &t->clip_area)) return;

    x = 0 - t->target_layer->buf_area.x1;
    y = 0 - t->target_layer->buf_area.y1;

    coordinates = *coords;

    lv_area_move(&clipped_area, x, y);
    lv_area_move(&coordinates, x, y);

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    d2_framebuffer_from_layer(u->d2_handle, t->target_layer);

    d2_cliprect(u->d2_handle, (d2_border)clipped_area.x1, (d2_border)clipped_area.y1, (d2_border)clipped_area.x2,
                (d2_border)clipped_area.y2);

    d2_renderbox(u->d2_handle,
                 (d2_point)        D2_FIX4(coordinates.x1),
                 (d2_point)        D2_FIX4(coordinates.y1),
                 (d2_width)        D2_FIX4(lv_area_get_width(&coordinates)),
                 (d2_width)        D2_FIX4(lv_area_get_height(&coordinates)));

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif
}
#endif //LV_USE_DRAW_DAVE2D
