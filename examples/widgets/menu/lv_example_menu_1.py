# Create a menu object
menu = lv.menu(lv.scr_act())
menu.set_size(320, 240)
menu.center()

# Create a menu item
main_menu_items = menu.item_create()
menu.item_add_seperator(main_menu_items, lv.menu.SECTION.OFF)

cont = lv.menu_cont(menu)
cont.clear_flag(lv.obj.FLAG.CLICKABLE)
label = lv.label(cont)
label.set_text("Item 1")
menu.item_add_obj(main_menu_items, lv.menu.SECTION.ON, cont)

cont = lv.menu_cont(menu)
cont.clear_flag(lv.obj.FLAG.CLICKABLE)
label = lv.label(cont)
label.set_text("Item 2")
menu.item_add_obj(main_menu_items, lv.menu.SECTION.ON, cont)

# Create a sub menu item
sub_menu_items = menu.item_create()
cont = lv.menu_cont(menu)
label = lv.label(cont)
label.set_text("Item 3 (Click me!)")
menu.item_set_obj(sub_menu_items, cont)

cont = lv.menu_cont(menu)
cont.clear_flag(lv.obj.FLAG.CLICKABLE)
label = lv.label(cont)
label.set_text("Hello, I am hiding here")
menu.item_add_obj(sub_menu_items, lv.menu.SECTION.ON, cont)

menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_items)

menu.set(main_menu_items)