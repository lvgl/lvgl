# Create a drop UP list by applying auto realign

# Create a drop down list
ddlist = lv.ddlist(lv.scr_act())
ddlist.set_options("\n".join([
                    "Apple",
                    "Banana",
                    "Orange",
                    "Melon",
                    "Grape",
                    "Raspberry"]))


ddlist.set_fix_width(150)
ddlist.set_fix_height(150)
ddlist.set_draw_arrow(True)

# Enable auto-realign when the size changes.
# It will keep the bottom of the ddlist fixed
ddlist.set_auto_realign(True)

# It will be called automatically when the size changes
ddlist.align(None, lv.ALIGN.IN_BOTTOM_MID, 0, -20)
