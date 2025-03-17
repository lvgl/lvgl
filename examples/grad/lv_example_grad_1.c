#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_LABEL

static void position_bullet(lv_event_t * e, lv_point_t * p)
{
    lv_indev_t * indev = lv_event_get_param(e);
    lv_indev_get_point(indev, p);

    lv_obj_t * bullet = lv_event_get_target(e);
    lv_obj_t * parent = lv_obj_get_parent(bullet);

    p->x -= lv_obj_get_x(parent);
    p->y -= lv_obj_get_y(parent);

    int32_t w = lv_obj_get_width(parent);
    int32_t h = lv_obj_get_height(parent);
    lv_obj_set_pos(bullet, LV_CLAMP(5, p->x, w - 20), LV_CLAMP(5, p->y, h - 20));
}

static void frac_1_event_cb(lv_event_t * e)
{
    lv_style_t * style = lv_event_get_user_data(e);
    lv_style_value_t v;
    if(lv_style_get_prop(style, LV_STYLE_BG_GRAD, &v) != LV_STYLE_RES_FOUND) {
        LV_LOG_WARN("style prop not found");
    }
    else {
        lv_grad_dsc_t * dsc = (lv_grad_dsc_t *)v.ptr;

        lv_point_t p;
        position_bullet(e, &p);

        lv_obj_t * bullet = lv_event_get_target(e);
        lv_obj_t * parent = lv_obj_get_parent(bullet);
        dsc->stops[0].frac = (uint8_t)LV_CLAMP(0, p.x * 255 / lv_obj_get_width(parent), 255);

        lv_obj_invalidate(parent);
    }
}

static void frac_2_event_cb(lv_event_t * e)
{
    lv_style_t * style = lv_event_get_user_data(e);
    lv_style_value_t v;
    if(lv_style_get_prop(style, LV_STYLE_BG_GRAD, &v) != LV_STYLE_RES_FOUND) {
        LV_LOG_WARN("style prop not found");
    }
    else {
        lv_grad_dsc_t * dsc = (lv_grad_dsc_t *)v.ptr;

        lv_point_t p;
        position_bullet(e, &p);

        lv_obj_t * bullet = lv_event_get_target(e);
        lv_obj_t * parent = lv_obj_get_parent(bullet);

        dsc->stops[1].frac = (uint8_t)LV_CLAMP(0, p.x * 255 / lv_obj_get_width(parent), 255);
        lv_obj_invalidate(parent);
    }
}

/**
 * Play with a simple horizontal gradient.
 * Adjust the stop positions of the gradient.
 */
void lv_example_grad_1(void)
{
    static const lv_color_t grad_colors[2] = {
        LV_COLOR_MAKE(0xff, 0x00, 0x00),
        LV_COLOR_MAKE(0x00, 0xff, 0x00),
    };

    static const lv_opa_t grad_opa[2] = {
        LV_OPA_100,
        LV_OPA_0,
    };

    static const uint8_t frac[2] = {
        20 * 255 / 100, /*20%*/
        80 * 255 / 100, /*80%*/
    };

    static lv_style_t style;
    lv_style_init(&style);

    static lv_grad_dsc_t grad_dsc;
    lv_grad_init_stops(&grad_dsc, grad_colors, grad_opa, frac, sizeof(grad_colors) / sizeof(lv_color_t));
    lv_grad_horizontal_init(&grad_dsc);

    /*Set gradient as background*/
    lv_style_set_bg_grad(&style, &grad_dsc);
    lv_style_set_border_width(&style, 2);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_radius(&style, 12);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_set_size(obj, lv_pct(80), lv_pct(80));
    lv_obj_center(obj);

    lv_obj_t * frac_1 = lv_button_create(obj);
    lv_obj_set_size(frac_1, 15, 15);
    lv_obj_set_style_bg_color(frac_1, lv_color_hex(0xff00ff), 0);
    lv_obj_add_event_cb(frac_1, frac_1_event_cb, LV_EVENT_PRESSING, &style);
    lv_obj_set_ext_click_area(frac_1, 5);
    lv_obj_set_pos(frac_1, lv_pct(20), lv_pct(50));

    lv_obj_t * frac_2 = lv_button_create(obj);
    lv_obj_set_size(frac_2, 15, 15);
    lv_obj_set_style_bg_color(frac_2, lv_color_hex(0xffff00), 0);
    lv_obj_add_event_cb(frac_2, frac_2_event_cb, LV_EVENT_PRESSING, &style);
    lv_obj_set_ext_click_area(frac_2, 5);
    lv_obj_set_pos(frac_2, lv_pct(80), lv_pct(50));
}

#endif
