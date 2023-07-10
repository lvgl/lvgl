#include "../lv_examples.h"

#if LV_BUILD_EXAMPLES

static int n = 3;
static lv_obj_t * label = NULL;

static void timer_cb(lv_timer_t * timer)
{
    if(n < 3 || n > 32) {
        n = 3;
    }
    else {
        static uint32_t old_tick = 0;
        uint32_t tick = lv_tick_get();
        if(!old_tick) {
            old_tick = tick;
        }
        if(tick - old_tick > 3000) {
            n++;
            lv_label_set_text_fmt(label, "%d sides", n);
            old_tick = tick;
        }
    }
    lv_obj_invalidate(timer->user_data);
}

static void event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_rect_dsc_t draw_dsc;
    lv_draw_rect_dsc_init(&draw_dsc);
    draw_dsc.bg_color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
    draw_dsc.bg_opa = LV_OPA_COVER;
    lv_point_t points[32];
    int i, r = 150;
    uint32_t tick = lv_tick_get();
    for(i = 0; i < n; i++) {
        int angle = i * 360 / n + ((tick % 36000) / 100);
        lv_coord_t x = 150 + (r * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT), y =
                           150 + (r * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
        points[i].x = x;
        points[i].y = y;
    }
    lv_draw_polygon(draw_ctx, &draw_dsc, points, n);
}

/**
 * Demonstrate event bubbling
 */
void lv_example_event_4(void)
{

    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 300, 300);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "%d sides", n);
    lv_obj_center(label);

    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_DRAW_MAIN, NULL);
    lv_timer_create(timer_cb, 17, cont);
}

#endif
