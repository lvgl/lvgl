import fs_driver
import sys

LV_FONT_DEFAULT = lv.font_montserrat_14

# get the directory in which the script is running
try:
    script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
except NameError: 
    script_path = ''
    
def get_imgfont_path(font, img_src, length, unicode, unicode_next,user_data) :
    path = bytes(script_path + "/../../assets/emoji/emoji_{:04X}.png".format(unicode) + "\0","ascii")
    # print("image path: ",path)
    img_src.__dereference__(length)[0:len(path)] = path
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
