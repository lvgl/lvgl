def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.VALUE_CHANGED:
        print("State: ", "On" if obj.has_state(lv.STATE.CHECKED) else "Off")


lv.scr_act().set_flex_flow(lv.FLEX_FLOW.COLUMN)
lv.scr_act().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)

sw = lv.switch(lv.scr_act())
sw.add_event(event_handler, lv.EVENT.ALL, None)
sw.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.CHECKED)
sw.add_event(event_handler, lv.EVENT.ALL, None)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.DISABLED)
sw.add_event(event_handler, lv.EVENT.ALL, None)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
sw.add_event(event_handler, lv.EVENT.ALL, None)

