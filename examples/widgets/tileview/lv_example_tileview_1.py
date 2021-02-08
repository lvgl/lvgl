valid_pos = [{"x":0, "y": 0}, {"x": 0, "y": 1}, {"x": 1,"y": 1}]

# resolution of the screen
HOR_RES = lv.disp_get_hor_res(lv.disp_get_default())
VER_RES = lv.disp_get_ver_res(lv.disp_get_default())

tileview = lv.tileview(lv.scr_act())
tileview.set_valid_positions(valid_pos, len(valid_pos))
tileview.set_edge_flash(True)

tile1 = lv.obj(tileview)
tile1.set_size(HOR_RES, VER_RES)
tile1.set_style(lv.style_pretty)
tileview.add_element(tile1)

# Tile1: just a label
label = lv.label(tile1)
label.set_text("Tile 1")
label.align(None, lv.ALIGN.CENTER, 0, 0)

# Tile2: a list
lst = lv.list(tileview)
lst.set_size(HOR_RES, VER_RES)
lst.set_pos(0, VER_RES)
lst.set_scroll_propagation(True)
lst.set_sb_mode(lv.SB_MODE.OFF)
tileview.add_element(lst)

list_btn = lst.add_btn(None, "One")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Two")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Three")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Four")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Five")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Six")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Seven")
tileview.add_element(list_btn)

list_btn = lst.add_btn(None, "Eight")
tileview.add_element(list_btn)

# Tile3: a button
tile3 = lv.obj(tileview, tile1)
tile3.set_pos(HOR_RES, VER_RES)
tileview.add_element(tile3)

btn = lv.btn(tile3)
btn.align(None, lv.ALIGN.CENTER, 0, 0)

label = lv.label(btn)
label.set_text("Button")