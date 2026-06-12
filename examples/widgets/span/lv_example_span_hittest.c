#include "../../lv_examples.h"
#if LV_USE_SPAN && LV_BUILD_EXAMPLES

static void click_event_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_event_get_indev(e);
    lv_point_t point;
    lv_indev_get_point(indev, &point);
#if LV_USE_LOG
    lv_obj_t * spans = lv_event_get_target_obj(e);
    lv_span_t * span = lv_spangroup_get_span_by_point(spans, &point);
    LV_LOG_USER("Clicked span: %s", span ? lv_span_get_text(span) : "none");
#endif
}

/**
 * @title Resolve the clicked span
 * @brief Map a click point to the span under it with lv_spangroup_get_span_by_point.
 *
 * A clickable `lv_spangroup` holds three spans, colored only so the segments
 * are easy to tell apart. On `LV_EVENT_CLICKED` the handler reads the pointer
 * position and calls `lv_spangroup_get_span_by_point` to find which span was
 * hit, then logs its text — the example isolates the hit-test, not styling.
 */
void lv_example_span_hittest(void)
{
    lv_obj_t * spans = lv_spangroup_create(lv_screen_active());
    lv_obj_set_width(spans, 240);
    lv_obj_set_height(spans, LV_SIZE_CONTENT);
    lv_obj_center(spans);
    lv_obj_add_flag(spans, LV_OBJ_FLAG_CLICKABLE);

    lv_span_t * span = lv_spangroup_add_span(spans);
    lv_span_set_text(span, "Click any ");
    lv_style_set_text_color(lv_span_get_style(span), lv_palette_main(LV_PALETTE_RED));

    span = lv_spangroup_add_span(spans);
    lv_span_set_text(span, "word in ");
    lv_style_set_text_color(lv_span_get_style(span), lv_palette_main(LV_PALETTE_GREEN));

    span = lv_spangroup_add_span(spans);
    lv_span_set_text(span, "this group.");
    lv_style_set_text_color(lv_span_get_style(span), lv_palette_main(LV_PALETTE_BLUE));

    lv_spangroup_refresh(spans);

    lv_obj_add_event_cb(spans, click_event_cb, LV_EVENT_CLICKED, NULL);
}

#endif