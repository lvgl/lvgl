import urandom

currentButton = None
list1 = None

def event_handler(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED:
        if currentButton == obj:
            currentButton = None
        else:
            currentButton = obj
        parent = obj.get_parent()
        for i in range( parent.get_child_cnt()):
            child = parent.get_child(i)
            if child == currentButton:
                child.add_state(lv.STATE.CHECKED)
            else:
                child.clear_state(lv.STATE.CHECKED)

def event_handler_top(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED:
        if currentButton == None:
            return
        currentButton.move_background()
        currentButton.scroll_to_view( lv.ANIM.ON)

def event_handler_up(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        if currentButton == None:
            return
        index = currentButton.get_index()
        if index <= 0:
            return
        currentButton.move_to_index(index - 1)
        currentButton.scroll_to_view(lv.ANIM.ON)

def event_handler_center(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        if currentButton == None:
            return
        parent = currentButton.get_parent()
        pos = parent.get_child_cnt() // 2
        currentButton.move_to_index(pos)
        currentButton.scroll_to_view(lv.ANIM.ON)

def event_handler_dn(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        if currentButton == None:
            return
        index = currentButton.get_index()
        currentButton.move_to_index(index + 1)
        currentButton.scroll_to_view(lv.ANIM.ON)

def event_handler_bottom(evt):
    global currentButton
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        if currentButton == None:
            return
        currentButton.move_foreground()
        currentButton.scroll_to_view(lv.ANIM.ON)

def event_handler_swap(evt):
    global currentButton
    global list1
    code = evt.get_code()
    obj = evt.get_target()
    if code == lv.EVENT.CLICKED:
        cnt = list1.get_child_cnt()
        for i in range(100):
            if cnt > 1:
                obj = list1.get_child(urandom.getrandbits(32) % cnt )
                obj.move_to_index(urandom.getrandbits(32) % cnt)
        if currentButton != None:
            currentButton.scroll_to_view(lv.ANIM.ON)

#Create a list with buttons that can be sorted
list1 = lv.list(lv.scr_act())
list1.set_size(lv.pct(60), lv.pct(100))
list1.set_style_pad_row( 5, 0)

for i in range(15):
    btn = lv.btn(list1)
    btn.set_width(lv.pct(100))
    btn.add_event_cb( event_handler, lv.EVENT.CLICKED, None)
    lab = lv.label(btn)
    lab.set_text("Item " + str(i))

#Select the first button by default
currentButton = list1.get_child(0)
currentButton.add_state(lv.STATE.CHECKED)

#Create a second list with up and down buttons
list2 = lv.list(lv.scr_act())
list2.set_size(lv.pct(40), lv.pct(100))
list2.align(lv.ALIGN.TOP_RIGHT, 0, 0)
list2.set_flex_flow(lv.FLEX_FLOW.COLUMN)

btn = list2.add_btn(None, "Top")
btn.add_event_cb(event_handler_top, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)

btn = list2.add_btn(lv.SYMBOL.UP, "Up")
btn.add_event_cb(event_handler_up, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)

btn = list2.add_btn(lv.SYMBOL.LEFT, "Center")
btn.add_event_cb(event_handler_center, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)

btn = list2.add_btn(lv.SYMBOL.DOWN, "Down")
btn.add_event_cb(event_handler_dn, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)

btn = list2.add_btn(None, "Bottom")
btn.add_event_cb(event_handler_bottom, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)

btn = list2.add_btn(lv.SYMBOL.SHUFFLE, "Shuffle")
btn.add_event_cb(event_handler_swap, lv.EVENT.ALL, None)
lv.group_remove_obj(btn)
