import gc
import lvgl as lv
from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Measure memory usage
gc.enable()
gc.collect()
mem_free = gc.mem_free()

label = lv.label(lv.scr_act())
label.align(lv.ALIGN.BOTTOM_MID, 0, -10)
label.set_text(" memory free:" + str(mem_free/1024) + " kB")

# Create an image from the png file
try:
    with open('../../assets/star.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find star.png")
    sys.exit()

img_star = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

def event_cb(e, snapshot_obj):
    img = e.get_target()

    if snapshot_obj:
        # no need to free the old source for snapshot_obj, gc will free it for us.

        # take a new snapshot, overwrite the old one
        dsc = lv.snapshot_take(img.get_parent(), lv.img.CF.TRUE_COLOR_ALPHA)
        snapshot_obj.set_src(dsc)

    gc.collect()
    mem_used = mem_free - gc.mem_free()
    label.set_text("memory used:" + str(mem_used/1024) + " kB")

root = lv.scr_act()
root.set_style_bg_color(lv.palette_main(lv.PALETTE.LIGHT_BLUE), 0)

# Create an image object to show snapshot
snapshot_obj = lv.img(root)
snapshot_obj.set_style_bg_color(lv.palette_main(lv.PALETTE.PURPLE), 0)
snapshot_obj.set_style_bg_opa(lv.OPA.COVER, 0)
snapshot_obj.set_zoom(128)

# Create the container and its children
container = lv.obj(root)
container.align(lv.ALIGN.CENTER, 0, 0)
container.set_size(180, 180)
container.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
container.set_flex_align(lv.FLEX_ALIGN.SPACE_EVENLY, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
container.set_style_radius(50, 0)

for i in range(4):
    img = lv.img(container)
    img.set_src(img_star)
    img.set_style_bg_color(lv.palette_main(lv.PALETTE.GREY), 0)
    img.set_style_bg_opa(lv.OPA.COVER, 0)
    img.set_style_transform_zoom(400, lv.STATE.PRESSED)
    img.add_flag(img.FLAG.CLICKABLE)
    img.add_event_cb(lambda e: event_cb(e, snapshot_obj), lv.EVENT.PRESSED, None)
    img.add_event_cb(lambda e: event_cb(e, snapshot_obj), lv.EVENT.RELEASED, None)
