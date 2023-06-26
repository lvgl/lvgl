
# Create a Tab view object
tabview = lv.tabview(lv.scr_act(), lv.DIR.LEFT, 80)
tabview.set_style_bg_color(lv.palette_lighten(lv.PALETTE.RED, 2), 0)

tab_btns = tabview.get_tab_btns()
tab_btns.set_style_bg_color(lv.palette_darken(lv.PALETTE.GREY, 3), 0)
tab_btns.set_style_text_color(lv.palette_lighten(lv.PALETTE.GREY, 5), 0)
tab_btns.set_style_border_side(lv.BORDER_SIDE.RIGHT, lv.PART.ITEMS | lv.STATE.CHECKED)


# Add 3 tabs (the tabs are page (lv_page) and can be scrolled
tab1 = tabview.add_tab("Tab 1")
tab2 = tabview.add_tab("Tab 2")
tab3 = tabview.add_tab("Tab 3")
tab4 = tabview.add_tab("Tab 4")
tab5 = tabview.add_tab("Tab 5")

tab2.set_style_bg_color(lv.palette_lighten(lv.PALETTE.AMBER, 3), 0)
tab2.set_style_bg_opa(lv.OPA.COVER, 0)

# Add content to the tabs
label = lv.label(tab1)
label.set_text("First tab")

label = lv.label(tab2)
label.set_text("Second tab")

label = lv.label(tab3)
label.set_text("Third tab")

label = lv.label(tab4)
label.set_text("Forth tab")

label = lv.label(tab5)
label.set_text("Fifth tab")

tabview.get_content().clear_flag(lv.obj.FLAG.SCROLLABLE)

