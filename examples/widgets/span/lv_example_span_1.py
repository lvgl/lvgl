#!/opt/bin/lv_micropython -i

import lvgl as lv
import display_driver

import fs_driver
fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

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

try:
    span.style.set_text_font(ltr_label, lv.font_montserrat_24)
except:
    fs_drv = lv.fs_drv_t()
    fs_driver.fs_register(fs_drv, 'S')
    print("montserrat-24 not enabled in lv_conf.h, dynamically loading the font")
    
    # get the directory in which the script is running
    try:
        script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
    except NameError:
        print("Could not find script path")
        script_path = ''
    if script_path != '':
        try:
            font_montserrat_24 = lv.font_load("S:" + script_path + "/../../assets/font/montserrat-24.fnt")
            span.style.set_text_font(font_montserrat_24)            
        except:
            print("Cannot load font file montserrat-24.fnt")

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
try:
    span.style.set_text_font(ltr_label, lv.font_montserrat_20)
except:
    fs_drv = lv.fs_drv_t()
    fs_driver.fs_register(fs_drv, 'S')
    print("montserrat-20 not enabled in lv_conf.h, dynamically loading the font")
    
    # get the directory in which the script is running
    try:
        script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
    except NameError:
        print("Could not find script path")
        script_path = ''
    if script_path != '':
        try:
            font_montserrat_20 = lv.font_load("S:" + script_path + "/../../assets/font/montserrat-20.fnt")
            span.style.set_text_font(font_montserrat_20)            
        except:
            print("Cannot load font file montserrat-20.fnt")

span.style.set_text_decor(lv.TEXT_DECOR.UNDERLINE)

span = spans.new_span()
span.set_text("I have a dream that hope to come true.")
span.style.set_text_decor(lv.TEXT_DECOR.STRIKETHROUGH)
spans.refr_mode()


