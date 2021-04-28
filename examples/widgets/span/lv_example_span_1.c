#include "../../lv_examples.h"
#if LV_USE_SPAN && LV_BUILD_EXAMPLES

/**
 * Create span.
 */
void lv_example_span_1(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_border_width(&style, 1);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_ORANGE));
    lv_style_set_pad_all(&style, 2);

    lv_obj_t * spans = lv_spangroup_create(lv_scr_act());
    lv_obj_set_width(spans, 300);
    lv_obj_set_height(spans,300);
    lv_obj_center(spans);
    lv_obj_add_style(spans, &style, 0);

    lv_span_set_align(spans, LV_TEXT_ALIGN_LEFT);
    lv_span_set_overflow(spans, LV_SPAN_OVERFLOW_CLIP);
    lv_span_set_indent(spans, 20);
    lv_span_set_mode(spans, LV_SPAN_MODE_BREAK);

    lv_span_t * span = lv_span_create(spans);
    lv_span_set_text(span, "china is a beautiful country.");
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_STRIKETHROUGH | LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_text_opa(&span->style, LV_OPA_30);

    span = lv_span_create(spans);
    lv_span_set_text_static(span, "good good study, day day up.");
#if LV_FONT_MONTSERRAT_24
    lv_style_set_text_font(&span->style,  &lv_font_montserrat_24);
#endif
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));

    span = lv_span_create(spans);
    lv_span_set_text_static(span, "LVGL is an open-source graphics library.");
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_BLUE));

    span = lv_span_create(spans);
    lv_span_set_text_static(span, "the boy no name.");
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));
#if LV_FONT_MONTSERRAT_20
    lv_style_set_text_font(&span->style, &lv_font_montserrat_20);
#endif
    lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_UNDERLINE);

    span = lv_span_create(spans);
    lv_span_set_text(span, "I have a dream that hope to come true.");

    lv_span_refr_mode(spans);

    //lv_span_del(spans, span);
    //lv_obj_del(spans);
}

#endif
