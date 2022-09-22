# Create an AZERTY keyboard map
kb_map = ["A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P", lv.SYMBOL.BACKSPACE, "\n",
          "Q", "S", "D", "F", "G", "J", "K", "L", "M",  lv.SYMBOL.NEW_LINE, "\n",
          "W", "X", "C", "V", "B", "N", ",", ".", ":", "!", "?", "\n",
          lv.SYMBOL.CLOSE, " ",  " ", " ", lv.SYMBOL.OK, None]

# Set the relative width of the buttons and other controls
kb_ctrl = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6,
           4, 4, 4, 4, 4, 4, 4, 4, 4, 6,
           4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
           2, lv.btnmatrix.CTRL.HIDDEN | 2, 6, lv.btnmatrix.CTRL.HIDDEN | 2, 2]

# Create a keyboard and add the new map as USER_1 mode
kb = lv.keyboard(lv.scr_act())

kb.set_map(lv.keyboard.MODE.USER_1, kb_map, kb_ctrl)
kb.set_mode(lv.keyboard.MODE.USER_1)

# Create a text area. The keyboard will write here
ta = lv.textarea(lv.scr_act())
ta.align(lv.ALIGN.TOP_MID, 0, 10)
ta.set_size(lv.pct(90), 80)
ta.add_state(lv.STATE.FOCUSED)

kb.set_textarea(ta)

