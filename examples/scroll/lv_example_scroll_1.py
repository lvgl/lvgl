#
# Demonstrate how scrolling appears automatically
#
# Create an object with the new style
panel = lv.obj(lv.scr_act())
panel.set_size(200, 200)
panel.center()

child = lv.obj(panel)
child.set_pos(0, 0)
label = lv.label(child)
label.set_text("Zero")
label.center()

child = lv.obj(panel)
child.set_pos(-40, 100)
label = lv.label(child)
label.set_text("Left")
label.center()

child = lv.obj(panel)
child.set_pos(90, -30)
label = lv.label(child)
label.set_text("Top")
label.center()

child = lv.obj(panel)
child.set_pos(150, 80)
label = lv.label(child)
label.set_text("Right")
label.center()

child = lv.obj(panel)
child.set_pos(60, 170)
label = lv.label(child)
label.set_text("Bottom")
label.center()

