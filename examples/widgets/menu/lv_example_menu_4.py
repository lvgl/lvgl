btn_cnt = 1

def float_btn_event_cb(e):
    global btn_cnt
    btn_cnt += 1

    sub_page = lv.menu_page(menu, None)

    cont = lv.menu_cont(sub_page)
    label = lv.label(cont)
    label.set_text("Hello, I am hiding inside {:d}".format(btn_cnt))

    cont = lv.menu_cont(main_page)
    label = lv.label(cont)
    label.set_text("Item {:d}".format(btn_cnt))
    menu.set_load_page_event(cont, sub_page)

# Create a menu object
menu = lv.menu(lv.scr_act())
menu.set_size(320, 240)
menu.center()

# Create a sub page
sub_page = lv.menu_page(menu, None)

cont = lv.menu_cont(sub_page)
label = lv.label(cont)
label.set_text("Hello, I am hiding inside the first item")

# Create a main page
main_page = lv.menu_page(menu, None)

cont = lv.menu_cont(main_page)
label = lv.label(cont)
label.set_text("Item 1")
menu.set_load_page_event(cont, sub_page)

menu.set_page(main_page)

float_btn = lv.btn(lv.scr_act())
float_btn.set_size(50, 50)
float_btn.add_flag(lv.obj.FLAG.FLOATING)
float_btn.align(lv.ALIGN.BOTTOM_RIGHT, -10, -10)
float_btn.add_event_cb(float_btn_event_cb, lv.EVENT.CLICKED, None)
float_btn.set_style_radius(lv.RADIUS.CIRCLE, 0)
float_btn.set_style_bg_img_src(lv.SYMBOL.PLUS, 0)
float_btn.set_style_text_font(lv.theme_get_font_large(float_btn), 0)