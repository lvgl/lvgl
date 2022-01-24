from utime import ticks_ms
import gc

ITEM_CNT = 200

def draw_event_cb(e):
    obj = e.get_target()
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    # If the cells are drawn...
    if dsc.part == lv.PART.ITEMS:
        chk = obj.has_cell_ctrl(dsc.id, 0, lv.table.CELL_CTRL.CUSTOM_1)

        rect_dsc = lv.draw_rect_dsc_t()
        rect_dsc.init()

        if chk:
            rect_dsc.bg_color = lv.theme_get_color_primary(obj)
        else:
            rect_dsc.bg_color = lv.palette_lighten(lv.PALETTE.GREY,2)
            
        rect_dsc.radius = lv.RADIUS.CIRCLE

        sw_area = lv.area_t()
        sw_area.x1 = dsc.draw_area.x2 - 50;
        sw_area.x2 = sw_area.x1 + 40;
        sw_area.y1 =  dsc.draw_area.y1 + dsc.draw_area.get_height() // 2 - 10
        sw_area.y2 = sw_area.y1 + 20;
        lv.draw_rect(sw_area, dsc.clip_area, rect_dsc)
         
        rect_dsc.bg_color = lv.color_white()
        
        if chk:
            sw_area.x2 -= 2
            sw_area.x1 = sw_area.x2 - 16
        else:
            sw_area.x1 += 2
            sw_area.x2 = sw_area.x1 + 16
        sw_area.y1 += 2;
        sw_area.y2 -= 2;
        lv.draw_rect(sw_area, dsc.clip_area, rect_dsc)

def change_event_cb(e):
    obj = e.get_target()
    row = lv.C_Pointer()
    col = lv.C_Pointer()
    table.get_selected_cell(row, col)
    # print("row: ",row.uint_val)

    chk = table.has_cell_ctrl(row.uint_val, 0, lv.table.CELL_CTRL.CUSTOM_1)
    if chk:
        table.clear_cell_ctrl(row.uint_val, 0, lv.table.CELL_CTRL.CUSTOM_1)
    else:
        table.add_cell_ctrl(row.uint_val, 0, lv.table.CELL_CTRL.CUSTOM_1)

#
# A very light-weighted list created from table
#

# Measure memory usage
gc.enable()
gc.collect()
mem_free = gc.mem_free()
print("mem_free: ",mem_free)
t = ticks_ms()
print("ticks: ", t)
table = lv.table(lv.scr_act())

# Set a smaller height to the table. It'll make it scrollable
table.set_size(150, 200)

table.set_col_width(0, 150)
table.set_row_cnt(ITEM_CNT)  # Not required but avoids a lot of memory reallocation lv_table_set_set_value
table.set_col_cnt(1)

# Don't make the cell pressed, we will draw something different in the event
table.remove_style(None, lv.PART.ITEMS | lv.STATE.PRESSED)

for i in range(ITEM_CNT):
    table.set_cell_value(i, 0, "Item " + str(i+1))

table.align(lv.ALIGN.CENTER, 0, -20);

# Add an event callback to to apply some custom drawing
table.add_event_cb(draw_event_cb, lv.EVENT.DRAW_PART_END, None)
table.add_event_cb(change_event_cb, lv.EVENT.VALUE_CHANGED, None)

gc.collect()
mem_used = mem_free - gc.mem_free()
elaps = ticks_ms()-t

label = lv.label(lv.scr_act())
label.set_text(str(ITEM_CNT) + " items were created in " + str(elaps) + " ms\n using " + str(mem_used) + " bytes of memory")
#label.set_text(str(ITEM_CNT) + " items were created in " + str(elaps) + " ms")

label.align(lv.ALIGN.BOTTOM_MID, 0, -10)

