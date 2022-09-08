import fs_driver
import sys

LV_FONT_DEFAULT = lv.font_montserrat_14
# Create an image from the png file
try:
    with open('../../assets/emoji/emoji_F617.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find emoji_F617.png")
    sys.exit()

img_emoji_F617_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

def get_imgfont_path(font, img_src, len, unicode, unicode_next) :
    print("get_imgfont_path")
    if unicode == 0xF617: 
        img_src = img_emoji_F617_argb 
    else :
        path = img_src
        path="../lvgl/examples/assets/emoji/{:04x}.png".format(unicode)
        print("Image path: ",path)
        # snprintf(path, len, "%s/%04X.%s", "A:lvgl/examples/assets/emoji", unicode, "png");

    return True

#
# draw img in label or span obj
#
imgfont = lv.imgfont_create(80, get_imgfont_path, None)
if imgfont == None:
    print("imgfont init error")

imgfont.fallback = LV_FONT_DEFAULT

label1 = lv.label(lv.scr_act())
label1.set_text("12\uF600\uF617AB")
label1.set_style_text_font(imgfont, lv.PART.MAIN)
label1.center()
'''
#else

void lv_example_imgfont_1(void)
{
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "imgfont is not installed");
    lv_obj_center(label);
}
'''

