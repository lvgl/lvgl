def back_event_handler(e):
    obj = e.get_target()
    if menu.item_back_btn_is_root(obj):
        mbox1 = lv.msgbox(lv.scr_act(), "Hello", "Root back btn click.", None, True)
        mbox1.center()

def switch_handler(e):
    obj = e.get_target()
    if obj.has_state(lv.STATE.CHECKED):
        menu.set_mode_sidebar(lv.menu.MODE_SIDEBAR.ENABLED)
    else:
        menu.set_mode_sidebar(lv.menu.MODE_SIDEBAR.DISABLED)

# Create a menu
menu = lv.menu(lv.scr_act())
menu.set_mode_sidebar(lv.menu.MODE_SIDEBAR.ENABLED)
menu.add_event_cb(back_event_handler, lv.EVENT.CLICKED, None)
menu.set_size(320, 240)
menu.center()

sub_menu_display_items = menu.item_create()
menu.item_set_text(sub_menu_display_items, lv.SYMBOL.SETTINGS, "About")
menu.item_add_seperator(sub_menu_display_items, lv.menu.SECTION.OFF)
menu.item_add_slider(sub_menu_display_items, lv.menu.SECTION.ON, lv.SYMBOL.SETTINGS, "Brightness", 0, 150, 100)
 
sub_menu_sound_items = menu.item_create()
menu.item_set_text(sub_menu_sound_items, lv.SYMBOL.AUDIO, "Sound")
menu.item_add_seperator(sub_menu_sound_items, lv.menu.SECTION.OFF)
menu.item_add_switch(sub_menu_sound_items, lv.menu.SECTION.ON, lv.SYMBOL.AUDIO, "Sound", False)

sub_menu_mechanics_items = menu.item_create()
sub_menu_mechanics_menu_obj = menu.item_set_text(sub_menu_mechanics_items, lv.SYMBOL.SETTINGS, "Mechanics")
menu.item_add_seperator(sub_menu_mechanics_items, lv.menu.SECTION.OFF)
menu.item_add_slider(sub_menu_mechanics_items, lv.menu.SECTION.ON, lv.SYMBOL.SETTINGS, "Velocity", 0, 150, 120)
menu.item_add_slider(sub_menu_mechanics_items, lv.menu.SECTION.ON, lv.SYMBOL.SETTINGS, "Acceleration", 0, 150, 50)
menu.item_add_slider(sub_menu_mechanics_items, lv.menu.SECTION.ON, lv.SYMBOL.SETTINGS, "Weight limit", 0, 150, 80)

sub_menu_mode_items = menu.item_create()
menu.item_set_text(sub_menu_mode_items, lv.SYMBOL.SETTINGS, "Menu mode")
menu.item_add_seperator(sub_menu_mode_items, lv.menu.SECTION.OFF)
switch_cont = menu.item_add_switch(sub_menu_mode_items, lv.menu.SECTION.ON, lv.SYMBOL.SETTINGS, "Sidebar enable", True)
switch = switch_cont.get_child(2)
switch.add_event_cb(switch_handler, lv.EVENT.VALUE_CHANGED, None)

sub_menu_about_softwareinfo_items = menu.item_create()
menu.item_set_text(sub_menu_about_softwareinfo_items, None, "Software information")
menu.item_add_text(sub_menu_about_softwareinfo_items, lv.menu.SECTION.ON, None, "Version 1.0")

sub_menu_about_legalinfo_items = menu.item_create()
menu.item_set_text(sub_menu_about_legalinfo_items, None, "Legal information")
for i in range(15):
    menu.item_add_text(sub_menu_about_legalinfo_items, lv.menu.SECTION.ON, None, "This is a long long long long long long long long long text, if it is long enough it may scroll.")
   
sub_menu_about_items = menu.item_create()
menu.item_set_text(sub_menu_about_items, None, "About")
menu.item_add_seperator(sub_menu_about_items, lv.menu.SECTION.OFF)
menu.item_add_menu(sub_menu_about_items, lv.menu.SECTION.ON, sub_menu_about_softwareinfo_items)
menu.item_add_menu(sub_menu_about_items, lv.menu.SECTION.ON, sub_menu_about_legalinfo_items)



main_menu_items = menu.item_create()
menu.item_add_text(main_menu_items, lv.menu.SECTION.OFF, None, "Settings")
menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_mechanics_items)
menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_sound_items)
menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_display_items)
menu.item_add_text(main_menu_items, lv.menu.SECTION.OFF, None, "About")
menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_about_items)
menu.item_add_menu(main_menu_items, lv.menu.SECTION.ON, sub_menu_mode_items)

menu.set(main_menu_items)

lv.event_send(sub_menu_mechanics_menu_obj, lv.EVENT.CLICKED, None)