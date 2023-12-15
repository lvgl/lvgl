def back_event_handler(e):
    obj = e.get_target_obj()
    if menu.back_button_is_root(obj):
        mbox1 = lv.msgbox(lv.screen_active())
        mbox1.add_title("Hello")
        mbox1.add_text("Root back button click.")
        mbox1.add_close_button()

# Create a menu object
menu = lv.menu(lv.screen_active())
menu.set_mode_root_back_button(lv.menu.ROOT_BACK_BUTTON.ENABLED)
menu.add_event_cb(back_event_handler, lv.EVENT.CLICKED, None)
menu.set_size(320, 240)
menu.center()

# Create a sub page
sub_page = lv.menu_page(menu, None)
cont = lv.menu_cont(sub_page)
label = lv.label(cont)
label.set_text("Hello, I am hiding here")

# Create a main page
main_page = lv.menu_page(menu, None)

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 1")

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 2")

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 3 (Click me!)")
menu.set_load_page_event(cont, sub_page)

menu.set_page(main_page)