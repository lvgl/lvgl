import fs_driver
import sys

LV_FONT_DEFAULT = lv.font_montserrat_14

# get the directory in which the script is running
try:
    script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
except NameError: 
    script_path = ''
    
# Create an image from the png file

try:
    with open(script_path + '/../../assets/emoji/F600.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find F600.png")
    sys.exit()

img_emoji_F600_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

try:
    with open(script_path + '/../../assets/emoji/emoji_F617.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find emoji_F617.png")
    sys.exit()

img_emoji_F617_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

def get_imgfont_path(font, img_src, length, unicode, unicode_next,user_data) :
    path = bytes(script_path + "/../../assets/emoji/emoji_{:04X}.png".format(unicode) + "\0","ascii")
    # print("image path: ",path)
    length = len(path)
    img_src.__dereference__(len(path))[0:len(path)] = path

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

