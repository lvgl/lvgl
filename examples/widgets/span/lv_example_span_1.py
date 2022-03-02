#
# Create span
#
style = lv.style_t()
style.init()
style.set_border_width(1)
style.set_border_color(lv.palette_main(lv.PALETTE.ORANGE))
style.set_pad_all(2)

spans = lv.spangroup(lv.scr_act())
spans.set_width(300)
spans.set_height(300)
spans.center()
spans.add_style(style, 0)

spans.set_align(lv.TEXT_ALIGN.LEFT)
spans.set_overflow(lv.SPAN_OVERFLOW.CLIP)
spans.set_indent(20)
spans.set_mode(lv.SPAN_MODE.BREAK)

span = spans.new_span()
span.set_text("china is a beautiful country.")
span.style.set_text_color(lv.palette_main(lv.PALETTE.RED))
span.style.set_text_decor(lv.TEXT_DECOR.STRIKETHROUGH | lv.TEXT_DECOR.UNDERLINE)
span.style.set_text_opa(lv.OPA._30)

span = spans.new_span()
span.set_text_static("good good study, day day up.")
#if LV_FONT_MONTSERRAT_24
#    lv_style_set_text_font(&span->style,  &lv_font_montserrat_24);
#endif
span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))

span = spans.new_span()
span.set_text_static("LVGL is an open-source graphics library.")
span.style.set_text_color(lv.palette_main(lv.PALETTE.BLUE))

span = spans.new_span()
span.set_text_static("the boy no name.")
span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))
#if LV_FONT_MONTSERRAT_20
#    lv_style_set_text_font(&span->style, &lv_font_montserrat_20);
#endif
span.style.set_text_decor(lv.TEXT_DECOR.UNDERLINE)

span = spans.new_span()
span.set_text("I have a dream that hope to come true.")

spans.refr_mode()

# lv_span_del(spans, span);
# lv_obj_del(spans);

