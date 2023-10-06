#
# Create a 2x2 tile view and allow scrolling only in an "L" shape.
# Demonstrate scroll chaining with a long list that
# scrolls the tile view when it can't be scrolled further.
#
tv = lv.tileview(lv.scr_act())

# Tile1: just a label
tile1 = tv.add_tile(0, 0, lv.DIR.BOTTOM)
label = lv.label(tile1)
label.set_text("Scroll down")
label.center()

# Tile2: a button
tile2 = tv.add_tile(0, 1, lv.DIR.TOP | lv.DIR.RIGHT)

button = lv.button(tile2)

label = lv.label(button)
label.set_text("Scroll up or right")

button.set_size(lv.SIZE_CONTENT, lv.SIZE_CONTENT)
button.center()

# Tile3: a list
tile3 = tv.add_tile(1, 1, lv.DIR.LEFT)
list = lv.list(tile3)
list.set_size(lv.pct(100), lv.pct(100))

list.add_button(None, "One")
list.add_button(None, "Two")
list.add_button(None, "Three")
list.add_button(None, "Four")
list.add_button(None, "Five")
list.add_button(None, "Six")
list.add_button(None, "Seven")
list.add_button(None, "Eight")
list.add_button(None, "Nine")
list.add_button(None, "Ten")
