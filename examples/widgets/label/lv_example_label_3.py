import fs_driver
#
# Show mixed LTR, RTL and Chinese label
#

ltr_label = lv.label(lv.scr_act())
ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).");
# ltr_label.set_style_text_font(ltr_label, &lv_font_montserrat_16, 0);

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

try:
    ltr_label.set_style_text_font(ltr_label, lv.font_montserrat_16, 0)
except:
    font_montserrat_16 = lv.font_load("S:../../assets/font/montserrat-16.fnt")
    ltr_label.set_style_text_font(font_montserrat_16, 0)
    
ltr_label.set_width(310)
ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

rtl_label = lv.label(lv.scr_act())
rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
rtl_label.set_width(310)
rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

font_simsun_16_cjk = lv.font_load("S:../../assets/font/lv_font_simsun_16_cjk.fnt")

cz_label = lv.label(lv.scr_act())
cz_label.set_style_text_font(font_simsun_16_cjk, 0)
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")    
cz_label.set_width(310)
cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

