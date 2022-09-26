# Create a menu object
menu = lv.menu(lv.scr_act())
menu.set_size(320, 240)
menu.center()

# Create sub pages
sub_page_1 = lv.menu_page(menu, "Page 1")

cont = lv.menu_cont(sub_page_1)
label = lv.label(cont)
label.set_text("Hello, I am hiding here")

sub_page_2 = lv.menu_page(menu, "Page 2")

cont = lv.menu_cont(sub_page_2)
label = lv.label(cont)
label.set_text("Hello, I am hiding here")

sub_page_3 = lv.menu_page(menu, "Page 3")

cont = lv.menu_cont(sub_page_3)
label = lv.label(cont)
label.set_text("Hello, I am hiding here")

# Create a main page
main_page = lv.menu_page(menu, None)

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 1 (Click me!)")
menu.set_load_page_event(cont, sub_page_1)

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 2 (Click me!)")
menu.set_load_page_event(cont, sub_page_2)

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 3 (Click me!)")
menu.set_load_page_event(cont, sub_page_3)

menu.set_page(main_page)