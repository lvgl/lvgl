#
# Demonstrate how scrolling appears automatically
#
# Create an object with the new style
panel = lv.obj(lv.scr_act())
panel.set_size(200, 200)
panel.center()

child = lv.obj(panel)
child.set_pos(0, 0);
child.set_size(70, 70)
label = lv.label(child)
label.set_text("Zero")
label.center()

child = lv.obj(panel)
child.set_pos(160, 80)
child.set_size(80, 80)

child2 = lv.btn(child)
child2.set_size(100, 50)

label = lv.label(child2)
label.set_text("Right")
label.center()

child = lv.obj(panel)
child.set_pos(40, 160)
child.set_size(100, 70)
label = lv.label(child)
label.set_text("Bottom")
label.center()

