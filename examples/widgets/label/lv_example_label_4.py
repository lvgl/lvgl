MASK_WIDTH = 100
MASK_HEIGHT = 45

def add_mask_event_cb(e,mask_map):

    code = e.get_code()
    obj = e.get_target_obj()
    
    if code == lv.EVENT.COVER_CHECK :
        e.set_cover_res(lv.COVER_RES.MASKED)
    
    elif code == lv.EVENT.DRAW_MAIN_BEGIN:
        m = lv.draw_mask_map_param_t()
        obj_coords = lv.area_t()
        obj.get_coords(obj_coords)
        m.init(obj_coords, mask_map)
        mask_id = lv.draw_mask_add(m,None)

    elif code == lv.EVENT.DRAW_MAIN_END:
        try:
            m.free_param()
            mask_id.remove_id()
        except:
            pass
    
#
# Draw label with gradient color
#
# Create the mask of a text by drawing it to a canvas
mask_map = bytearray(MASK_WIDTH * MASK_HEIGHT * 4)
    
#  Create a "8 bit alpha" canvas and clear it
canvas = lv.canvas(lv.scr_act())
canvas.set_buffer(mask_map, MASK_WIDTH, MASK_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.fill_bg(lv.color_black(), lv.OPA.TRANSP)

# Draw a label to the canvas. The result "image" will be used as mask
label_dsc = lv.draw_label_dsc_t() 
label_dsc.init()
label_dsc.color = lv.color_white()
label_dsc.align = lv.TEXT_ALIGN.CENTER
canvas.draw_text(5, 5, MASK_WIDTH, label_dsc, "Text with gradient")

# The mask is reads the canvas is not required anymore
canvas.delete()

# Convert the mask to A8
# This is just a work around and will be changed later
mask8 = bytearray(MASK_WIDTH * MASK_HEIGHT)
for i in range(MASK_WIDTH * MASK_HEIGHT):
    #mask8[i] = lv.color_brightness(mask_c[i]);
    mask8[i] = mask_map[4*i]
    
# Create an object from where the text will be masked out.
# Now it's a rectangle with a gradient but it could be an image too
grad = lv.obj(lv.scr_act())
grad.set_size( MASK_WIDTH, MASK_HEIGHT)
grad.center()
grad.set_style_bg_color(lv.color_hex(0xff0000), 0)
grad.set_style_bg_grad_color(lv.color_hex(0x0000ff), 0)
grad.set_style_bg_grad_dir(lv.GRAD_DIR.HOR, 0)
grad.add_event(lambda e: add_mask_event_cb(e,mask8), lv.EVENT.ALL, None)

