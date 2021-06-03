#
# Style a button from scratch
#

# Init the style for the default state
style = lv.style_t()
style.init()

style.set_radius(3)

style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_bg_grad_color(lv.palette_darken(lv.PALETTE.BLUE, 2))
style.set_bg_grad_dir(lv.GRAD_DIR.VER)

style.set_border_opa(lv.OPA._40)
style.set_border_width(2)
style.set_border_color(lv.palette_main(lv.PALETTE.GREY))

style.set_shadow_width(8)
style.set_shadow_color(lv.palette_main(lv.PALETTE.GREY))
style.set_shadow_ofs_y(8)

style.set_outline_opa(lv.OPA.COVER)
style.set_outline_color(lv.palette_main(lv.PALETTE.BLUE))

style.set_text_color(lv.color_white())
style.set_pad_all(10)

# Init the pressed style
style_pr = lv.style_t()
style_pr.init()

# Add a large outline when pressed
style_pr.set_outline_width(30)
style_pr.set_outline_opa(lv.OPA.TRANSP)

style_pr.set_translate_y(5)
style_pr.set_shadow_ofs_y(3)
style_pr.set_bg_color(lv.palette_darken(lv.PALETTE.BLUE, 2))
style_pr.set_bg_grad_color(lv.palette_darken(lv.PALETTE.BLUE, 4))

# Add a transition to the the outline
trans = lv.style_transition_dsc_t()
props = [lv.STYLE.OUTLINE_WIDTH, lv.STYLE.OUTLINE_OPA, 0]
trans.init(props, lv.anim_t.path_linear, 300, 0, None)

style_pr.set_transition(trans)

btn1 = lv.btn(lv.scr_act())
btn1.remove_style_all()                          # Remove the style coming from the theme
btn1.add_style(style, 0)
btn1.add_style(style_pr, lv.STATE.PRESSED)
btn1.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
btn1.center()

label = lv.label(btn1)
label.set_text("Button")
label.center()

