# Create styles for the keyboard
rel_style = lv.style_t()
pr_style  = lv.style_t()

lv.style_copy(rel_style, lv.style_btn_rel)
rel_style.body.radius = 0
rel_style.body.border.width = 1

lv.style_copy(pr_style, lv.style_btn_pr)
pr_style.body.radius = 0
pr_style.body.border.width = 1

# Create a keyboard and apply the styles
kb = lv.kb(lv.scr_act())
kb.set_cursor_manage(True)
kb.set_style(lv.kb.STYLE.BG, lv.style_transp_tight)
kb.set_style(lv.kb.STYLE.BTN_REL, rel_style)
kb.set_style(lv.kb.STYLE.BTN_PR, pr_style)

# Create a text area. The keyboard will write here
ta = lv.ta(lv.scr_act())
ta.align(None, lv.ALIGN.IN_TOP_MID, 0, 10)
ta.set_text("")

# Assign the text area to the keyboard
kb.set_ta(ta)