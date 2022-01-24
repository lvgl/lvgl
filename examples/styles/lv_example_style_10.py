#
# Creating a transition
#

props = [lv.STYLE.BG_COLOR, lv.STYLE.BORDER_COLOR, lv.STYLE.BORDER_WIDTH, 0]

# A default transition
# Make it fast (100ms) and start with some delay (200 ms)

trans_def =  lv.style_transition_dsc_t()
trans_def.init(props, lv.anim_t.path_linear, 100, 200, None)

# A special transition when going to pressed state
# Make it slow (500 ms) but start  without delay

trans_pr = lv.style_transition_dsc_t()
trans_pr.init(props, lv.anim_t.path_linear, 500, 0, None)

style_def = lv.style_t()
style_def.init()
style_def.set_transition(trans_def)

style_pr = lv.style_t()
style_pr.init()
style_pr.set_bg_color(lv.palette_main(lv.PALETTE.RED))
style_pr.set_border_width(6)
style_pr.set_border_color(lv.palette_darken(lv.PALETTE.RED, 3))
style_pr.set_transition(trans_pr)

# Create an object with the new style_pr
obj = lv.obj(lv.scr_act())
obj.add_style(style_def, 0)
obj.add_style(style_pr, lv.STATE.PRESSED)

obj.center()

