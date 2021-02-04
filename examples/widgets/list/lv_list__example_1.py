def event_handler(obj, event):
    if event == lv.EVENT.CLICKED:
        print("Clicked: %s" % lv.list.get_btn_text(obj))

# Create a list
list1 = lv.list(lv.scr_act())
list1.set_size(160, 200)
list1.align(None, lv.ALIGN.CENTER, 0, 0)

# Add buttons to the list

list_btn = list1.add_btn(lv.SYMBOL.FILE, "New")
list_btn.set_event_cb(event_handler)

list_btn = list1.add_btn(lv.SYMBOL.DIRECTORY, "Open")
list_btn.set_event_cb(event_handler)

list_btn = list1.add_btn(lv.SYMBOL.CLOSE, "Delete")
list_btn.set_event_cb(event_handler)

list_btn = list1.add_btn(lv.SYMBOL.EDIT, "Edit")
list_btn.set_event_cb(event_handler)

list_btn = list1.add_btn(lv.SYMBOL.SAVE, "Save")
list_btn.set_event_cb(event_handler)