#
# Demonstrate flex grow.
#

cont = lv.obj(lv.scr_act())
cont.set_size(300, 220)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.ROW)

obj = lv.obj(cont)
obj.set_size(40, 40)           # Fix size

obj = lv.obj(cont)
obj.set_height(40)
obj.set_flex_grow(1)           # 1 portion from the free space

obj = lv.obj(cont)
obj.set_height(40)
obj.set_flex_grow(2)           # 2 portion from the free space

obj = lv.obj(cont)
obj.set_size(40, 40)           # Fix size. It is flushed to the right by the "grow" items

