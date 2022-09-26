from micropython import const

def create_text(parent, icon, txt, builder_variant):

    obj = lv.menu_cont(parent)

    img = None
    label = None

    if icon  :
        img = lv.img(obj)
        img.set_src(icon)

    if txt :
        label = lv.label(obj)
        label.set_text(txt)
        label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
        label.set_flex_grow(1)

    if builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 and icon and txt :
        img.add_flag(lv.OBJ_FLAG_FLEX_IN_NEW_TRACK)
        img.swap(label)

    return obj


def create_slider(parent, icon, txt, min, max, val) :

    obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2)

    slider = lv.slider(obj)
    slider.set_flex_grow(1)
    slider.set_range(min, max)
    slider.set_value(val, lv.ANIM.OFF)

    if icon == None :
        slider.add_flag(lv.obj.FLAG_FLEX.IN_NEW_TRACK)

    return obj

def create_switch(parent, icon, txt, chk) :

    obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1)

    sw = lv.switch(obj)
    if chk == lv.STATE.CHECKED:
        sw.add_state(chk )
    else:
        sw.add_state(0)

    return obj


def back_event_handler(e,menu):

    obj = e.get_target()
    # menu = lv_event_get_user_data(e);

    if menu.back_btn_is_root(obj) :
        mbox1 = lv.msgbox(None, "Hello", "Root back btn click.", None, True)
        mbox1.center()

def switch_handler(e,menu):

    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED :
        if obj.has_state(lv.STATE.CHECKED) :
            menu.set_page(None)
            menu.set_sidebar_page(root_page)
            # lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
            lv.event_send(menu.get_cur_sidebar_page().get_child(0).get_child(0),lv.EVENT.CLICKED,None)
        else :
            menu.set_sidebar_page(None)
            menu.clear_history()      #  Clear history because we will be showing the root page later 
            menu.set_page(root_page)

LV_MENU_ITEM_BUILDER_VARIANT_1 = const(0)
LV_MENU_ITEM_BUILDER_VARIANT_2 = const(1)

LV_HOR_RES = lv.scr_act().get_disp().driver.hor_res
LV_VER_RES = lv.scr_act().get_disp().driver.ver_res

menu = lv.menu(lv.scr_act())

bg_color = menu.get_style_bg_color(0)
if bg_color.color_brightness() > 127 :
    menu.set_style_bg_color(menu.get_style_bg_color(0).color_darken(10),0)
else :
    menu.set_style_bg_color(menu.get_style_bg_color(0).color_darken(50),0)


menu.set_mode_root_back_btn(lv.menu.ROOT_BACK_BTN.ENABLED)
menu.add_event_cb(lambda evt: back_event_handler(evt,menu), lv.EVENT.CLICKED, None)
menu.set_size(LV_HOR_RES, LV_VER_RES )
menu.center()

# Create sub pages
sub_mechanics_page = lv.menu_page(menu, None)
sub_mechanics_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
lv.menu_separator(sub_mechanics_page)
section = lv.menu_section(sub_mechanics_page);
create_slider(section,lv.SYMBOL.SETTINGS, "Velocity", 0, 150, 120)
create_slider(section,lv.SYMBOL.SETTINGS, "Acceleration", 0, 150, 50)
create_slider(section,lv.SYMBOL.SETTINGS, "Weight limit", 0, 150, 80)

sub_sound_page = lv.menu_page(menu, None)
sub_sound_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
lv.menu_separator(sub_sound_page)
section = lv.menu_section(sub_sound_page)
create_switch(section,lv.SYMBOL.AUDIO, "Sound", False)

sub_display_page = lv.menu_page(menu, None)
sub_display_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
lv.menu_separator(sub_display_page)
section = lv.menu_section(sub_display_page)
create_slider(section,lv.SYMBOL.SETTINGS, "Brightness", 0, 150, 100)

sub_software_info_page = lv.menu_page(menu, None)
sub_software_info_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
section = lv.menu_section(sub_software_info_page)
create_text(section,None, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1)

sub_legal_info_page = lv.menu_page(menu, None)
sub_legal_info_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)

section = lv.menu_section(sub_legal_info_page)

for i in range(15):
    create_text(section, None,
                "This is a long long long long long long long long long text, if it is long enough it may scroll.",
                LV_MENU_ITEM_BUILDER_VARIANT_1)

sub_about_page = lv.menu_page(menu, None)
sub_about_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)

lv.menu_separator(sub_about_page)
section = lv.menu_section(sub_about_page)
cont = create_text(section, None, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_software_info_page);
cont = create_text(section, None, "Legal information", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_legal_info_page)

sub_menu_mode_page = lv.menu_page(menu, None)
sub_menu_mode_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
lv.menu_separator(sub_menu_mode_page)
section = lv.menu_section(sub_menu_mode_page)
cont = create_switch(section, lv.SYMBOL.AUDIO, "Sidebar enable",True)
cont.get_child(2).add_event_cb(lambda evt: switch_handler(evt,menu), lv.EVENT.VALUE_CHANGED, None)

# Create a root page
root_page = lv.menu_page(menu, "Settings")
root_page.set_style_pad_hor(menu.get_main_header().get_style_pad_left(0),0)
section = lv.menu_section(root_page)
cont = create_text(section, lv.SYMBOL.SETTINGS, "Mechanics", LV_MENU_ITEM_BUILDER_VARIANT_1)
menu.set_load_page_event(cont, sub_mechanics_page);
cont = create_text(section, lv.SYMBOL.AUDIO, "Sound", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_sound_page)
cont = create_text(section, lv.SYMBOL.SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_display_page)

create_text(root_page, None, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
section = lv.menu_section(root_page)
cont = create_text(section, None, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_about_page)
cont = create_text(section, lv.SYMBOL.SETTINGS, "Menu mode", LV_MENU_ITEM_BUILDER_VARIANT_1);
menu.set_load_page_event(cont, sub_menu_mode_page)

menu.set_sidebar_page(root_page)

lv.event_send(menu.get_cur_sidebar_page().get_child(0).get_child(0),lv.EVENT.CLICKED,None)




