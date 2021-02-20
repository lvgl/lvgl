#include "../../../lvgl.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

static void set_value(lv_obj_t *bar, lv_anim_value_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

static void event_cb(lv_obj_t * obj, lv_event_t e)
{
    if(e == LV_EVENT_DRAW_POST_END) {
        lv_bar_t * bar = (lv_bar_t *)obj;

        lv_draw_label_dsc_t dsc;
        lv_draw_label_dsc_init(&dsc);
        dsc.font = LV_THEME_FONT_NORMAL;

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", lv_bar_get_value(obj));

        lv_point_t txt_size;
        lv_txt_get_size(&txt_size, buf, dsc.font, dsc.letter_space, dsc.line_space, LV_COORD_MAX, dsc.flag);

        lv_area_t txt_area;
        /*If the indicator is long enough put the text inside on the right*/
        if(lv_area_get_width(&bar->indic_area) > txt_size.x + 20) {
            txt_area.x2 = bar->indic_area.x2 - 5;
            txt_area.x1 = txt_area.x2 - txt_size.x + 1;
            dsc.color = LV_COLOR_WHITE;
        }
        /*If the indicator is still short put the text out of it on the right */
        else {
            txt_area.x1 = bar->indic_area.x2 + 5;
            txt_area.x2 = txt_area.x1 + txt_size.x - 1;
            dsc.color = LV_COLOR_BLACK;
        }

        txt_area.y1 = bar->indic_area.y1 + (lv_area_get_height(&bar->indic_area) - txt_size.y) / 2;
        txt_area.y2 = txt_area.y1 + txt_size.y - 1;

        const lv_area_t * clip_area = lv_event_get_param();
        lv_draw_label(&txt_area, clip_area, &dsc, buf, NULL);
    }
}

/**
 * Custom drawer on bar to display the current value
 */
void lv_example_bar_6(void)
{
    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    lv_style_set_content_ofs_y(&style_bg, -3);
    lv_style_set_content_align(&style_bg, LV_ALIGN_OUT_TOP_MID);

    lv_obj_t * bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_add_event_cb(bar, event_cb, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) set_value);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

}

#endif
