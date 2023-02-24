#include "../lv_examples.h"

#if LV_BUILD_EXAMPLES

static uint32_t size = 0;
static bool size_dec = false;

static void timer_cb(lv_timer_t * timer)
{
    lv_obj_invalidate(timer->user_data);
    if(size_dec) size--;
    else size++;

    if(size == 50) size_dec = true;
    else if(size == 0) size_dec = false;
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->class_p == &lv_obj_class && dsc->part == LV_PART_MAIN) {
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_color = lv_color_hex(0xffaaaa);
        draw_dsc.radius = LV_RADIUS_CIRCLE;
        draw_dsc.border_color = lv_color_hex(0xff5555);
        draw_dsc.border_width = 2;
        draw_dsc.outline_color = lv_color_hex(0xff0000);
        draw_dsc.outline_pad = 3;
        draw_dsc.outline_width = 2;

        lv_area_t a;
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = size;
        a.y2 = size;
        lv_area_align(&obj->coords, &a, LV_ALIGN_CENTER, 0, 0);

        lv_draw_rect(dsc->draw_ctx, &draw_dsc, &a);
    }
}

/**
 * Demonstrate the usage of draw event
 */
void lv_example_event_4(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 200, 200);
    lv_obj_center(cont);
    lv_obj_add_event(cont, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_timer_create(timer_cb, 30, cont);
}

#endif
