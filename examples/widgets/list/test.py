#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.CLICKED:
            print("Clicked: list1." + list1.get_button_text(obj))

# Create a list
list1 = lv.list(lv.scr_act())
list1.set_size(180, 220)
list1.center()

# Add buttons to the list
list1.add_text("File")
button_new = list1.add_button(lv.SYMBOL.FILE, "New")
button_new.add_event(event_handler,lv.EVENT.ALL, None)
button_open = list1.add_button(lv.SYMBOL.DIRECTORY, "Open")
button_open.add_event(event_handler,lv.EVENT.ALL, None)
button_save = list1.add_button(lv.SYMBOL.SAVE, "Save")
button_save.add_event(event_handler,lv.EVENT.ALL, None)
button_delete = list1.add_button(lv.SYMBOL.CLOSE, "Delete")
button_delete.add_event(event_handler,lv.EVENT.ALL, None)
button_edit = list1.add_button(lv.SYMBOL.EDIT, "Edit")
button_edit.add_event(event_handler,lv.EVENT.ALL, None)

list1.add_text("Connectivity")
button_bluetooth = list1.add_button(lv.SYMBOL.BLUETOOTH, "Bluetooth")
button_bluetooth.add_event(event_handler,lv.EVENT.ALL, None)
button_navig = list1.add_button(lv.SYMBOL.GPS, "Navigation")
button_navig.add_event(event_handler,lv.EVENT.ALL, None)
button_USB = list1.add_button(lv.SYMBOL.USB, "USB")
button_USB.add_event(event_handler,lv.EVENT.ALL, None)
button_battery = list1.add_button(lv.SYMBOL.BATTERY_FULL, "Battery")
button_battery.add_event(event_handler,lv.EVENT.ALL, None)

list1.add_text("Exit")
button_apply = list1.add_button(lv.SYMBOL.OK, "Apply")
button_apply.add_event(event_handler,lv.EVENT.ALL, None)
button_close = list1.add_button(lv.SYMBOL.CLOSE, "Close")
button_close.add_event(event_handler,lv.EVENT.ALL, None)

