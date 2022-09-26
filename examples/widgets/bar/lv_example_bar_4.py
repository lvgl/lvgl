#
# get an icon
#
def get_icon(filename,xres,yres):
    try:
        sdl_filename = "../../assets/" + filename + "_" + str(xres) + "x" + str(yres) + "_argb8888.fnt"
        print("file name: ", sdl_filename)
        with open(sdl_filename,'rb') as f:
            icon_data = f.read()
    except:
        print("Could not find image file: " + filename)
        return None

    icon_dsc = lv.img_dsc_t(
        {
            "header": {"always_zero": 0, "w": xres, "h": yres, "cf": lv.img.CF.TRUE_COLOR_ALPHA},
            "data": icon_data,
            "data_size": len(icon_data),
        }
    )
    return icon_dsc

#
# Bar with stripe pattern and ranged value
#

img_skew_strip_dsc = get_icon("img_skew_strip",80,20)
style_indic = lv.style_t()

style_indic.init()
style_indic.set_bg_img_src(img_skew_strip_dsc)
style_indic.set_bg_img_tiled(True)
style_indic.set_bg_img_opa(lv.OPA._30)

bar = lv.bar(lv.scr_act())
bar.add_style(style_indic, lv.PART.INDICATOR)

bar.set_size(260, 20)
bar.center()
bar.set_mode(lv.bar.MODE.RANGE)
bar.set_value(90, lv.ANIM.OFF)
bar.set_start_value(20, lv.ANIM.OFF)



