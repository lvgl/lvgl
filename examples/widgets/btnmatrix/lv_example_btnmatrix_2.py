from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image from the png file
try:
    with open('../../assets/star.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find star.png")
    sys.exit()
    
img_star_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data 
})

def event_cb(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.DRAW_PART_BEGIN:
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        # Change the draw descriptor the 2nd button
        if dsc.id == 1:
            dsc.rect_dsc.radius = 0;
            if obj.get_selected_btn() == dsc.id:
                dsc.rect_dsc.bg_color = lv.palette_darken(lv.PALETTE.GREY, 3)
            else:
                dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE)

            dsc.rect_dsc.shadow_width = 6
            dsc.rect_dsc.shadow_ofs_x = 3
            dsc.rect_dsc.shadow_ofs_y = 3
            dsc.label_dsc.color = lv.color_white()

        # Change the draw descriptor the 3rd button

        elif dsc.id == 2:
            dsc.rect_dsc.radius = lv.RADIUS.CIRCLE
            if obj.get_selected_btn() == dsc.id:
                dsc.rect_dsc.bg_color = lv.palette_darken(lv.PALETTE.RED, 3)
            else:
                dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.RED)
                
                dsc.label_dsc.color = lv.color_white()
        elif dsc.id == 3:
            dsc.label_dsc.opa = lv.OPA.TRANSP  # Hide the text if any

    if code == lv.EVENT.DRAW_PART_END:
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())

        # Add custom content to the 4th button when the button itself was drawn
        if dsc.id == 3:
            # LV_IMG_DECLARE(img_star);
            header = lv.img_header_t()
            res = lv.img.decoder_get_info(img_star_argb, header)
            if res != lv.RES.OK:
                print("error when getting image header")
                return
            else:
                a = lv.area_t()
                a.x1 = dsc.draw_area.x1 + (dsc.draw_area.get_width() - header.w) // 2
                a.x2 = a.x1 + header.w - 1;
                a.y1 = dsc.draw_area.y1 + (dsc.draw_area.get_height() - header.h) // 2
                a.y2 = a.y1 + header.h - 1;
                img_draw_dsc = lv.draw_img_dsc_t()
                img_draw_dsc.init()
                img_draw_dsc.recolor = lv.color_black()
                if obj.get_selected_btn() == dsc.id:
                    img_draw_dsc.recolor_opa = lv.OPA._30

                lv.draw_img(a, dsc.clip_area, img_star_argb, img_draw_dsc)

#
# Add custom drawer to the button matrix to c
#
btnm = lv.btnmatrix(lv.scr_act())
btnm.add_event_cb(event_cb, lv.EVENT.ALL, None)
btnm.center()

