# Create a menu object
menu = lv.menu(lv.scr_act())
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