def event_handler(obj, event):
    if event == lv.EVENT.VALUE_CHANGED:
        print("Value: %s" % obj.get_text())
    elif event == lv.EVENT.LONG_PRESSED_REPEAT:
        # For simple test: Long press the Text are to add the text below
        ta1.add_text("\n\nYou can scroll it if the text is long enough.\n")

ta1 = lv.ta(lv.scr_act())
ta1.set_size(200, 100)
ta1.align(None, lv.ALIGN.CENTER, 0, 0)
ta1.set_cursor_type(lv.CURSOR.BLOCK)
ta1.set_text("A text in a Text Area")     # Set an initial text
ta1.set_event_cb(event_handler)