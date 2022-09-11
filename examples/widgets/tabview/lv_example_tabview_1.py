# Create a Tab view object
tabview = lv.tabview(lv.scr_act(), lv.DIR.TOP, 50)

# Add 3 tabs (the tabs are page (lv_page) and can be scrolled
tab1 = tabview.add_tab("Tab 1")
tab2 = tabview.add_tab("Tab 2")
tab3 = tabview.add_tab("Tab 3")

# Add content to the tabs
label = lv.label(tab1)
label.set_text("""This the first tab

If the content
of a tab
becomes too
longer
than the
container
then it
automatically
becomes
scrollable.



Can you see it?""")

label = lv.label(tab2)
label.set_text("Second tab")

label = lv.label(tab3)
label.set_text("Third tab");

label.scroll_to_view_recursive(lv.ANIM.ON)

