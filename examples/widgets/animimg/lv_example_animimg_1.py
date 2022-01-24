from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

anim_imgs = [None]*3
# Create an image from the png file
try:
    with open('../../assets/animimg001.png','rb') as f:
        anim001_data = f.read()
except:
    print("Could not find animimg001.png")
    sys.exit()
    
anim_imgs[0] = lv.img_dsc_t({
  'data_size': len(anim001_data),
  'data': anim001_data 
})

try:
    with open('../../assets/animimg002.png','rb') as f:
        anim002_data = f.read()
except:
    print("Could not find animimg002.png")
    sys.exit()
    
anim_imgs[1] = lv.img_dsc_t({
  'data_size': len(anim002_data),
  'data': anim002_data 
})

try:
    with open('../../assets/animimg003.png','rb') as f:
        anim003_data = f.read()
except:
    print("Could not find animimg003.png")
    sys.exit()
    
anim_imgs[2] = lv.img_dsc_t({
  'data_size': len(anim003_data),
  'data': anim003_data 
})

animimg0 = lv.animimg(lv.scr_act())
animimg0.center()
animimg0.set_src(anim_imgs, 3)
animimg0.set_duration(1000)
animimg0.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
animimg0.start()



