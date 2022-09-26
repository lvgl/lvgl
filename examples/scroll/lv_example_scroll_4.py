#
# Styling the scrollbars
#
obj = lv.obj(lv.scr_act())
obj.set_size(200, 100)
obj.center()

label = lv.label(obj)
label.set_text(
"""
Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Etiam dictum, tortor vestibulum lacinia laoreet, mi neque consectetur neque, vel mattis odio dolor egestas ligula.
Sed vestibulum sapien nulla, id convallis ex porttitor nec.
Duis et massa eu libero accumsan faucibus a in arcu.
Ut pulvinar odio lorem, vel tempus turpis condimentum quis. Nam consectetur condimentum sem in auctor.
Sed nisl augue, venenatis in blandit et, gravida ac tortor.
Etiam dapibus elementum suscipit.
Proin mollis sollicitudin convallis.
Integer dapibus tempus arcu nec viverra.
Donec molestie nulla enim, eu interdum velit placerat quis.
Donec id efficitur risus, at molestie turpis.
Suspendisse vestibulum consectetur nunc ut commodo.
Fusce molestie rhoncus nisi sit amet tincidunt.
Suspendisse a nunc ut magna ornare volutpat.
""")


# Remove the style of scrollbar to have clean start
obj.remove_style(None, lv.PART.SCROLLBAR | lv.STATE.ANY)

# Create a transition the animate the some properties on state change
props = [lv.STYLE.BG_OPA, lv.STYLE.WIDTH, 0]
trans = lv.style_transition_dsc_t()
trans.init(props, lv.anim_t.path_linear, 200, 0, None)

# Create a style for the scrollbars
style = lv.style_t()
style.init()
style.set_width(4)               # Width of the scrollbar
style.set_pad_right(5)           # Space from the parallel side
style.set_pad_top(5)             # Space from the perpendicular side

style.set_radius(2)
style.set_bg_opa(lv.OPA._70)
style.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_border_color(lv.palette_darken(lv.PALETTE.BLUE, 3))
style.set_border_width(2)
style.set_shadow_width(8)
style.set_shadow_spread(2)
style.set_shadow_color(lv.palette_darken(lv.PALETTE.BLUE, 1))

style.set_transition(trans)

# Make the scrollbars wider and use 100% opacity when scrolled
style_scrolled = lv.style_t()
style_scrolled.init()
style_scrolled.set_width(8)
style_scrolled.set_bg_opa(lv.OPA.COVER)

obj.add_style(style, lv.PART.SCROLLBAR)
obj.add_style(style_scrolled, lv.PART.SCROLLBAR | lv.STATE.SCROLLED)

