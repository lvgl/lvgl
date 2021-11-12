
def event_handler(evt):
    code = evt.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = evt.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:        
            calendar.set_today_date(date.year, date.month, date.day)
            print("Clicked date: %02d.%02d.%02d"%(date.day, date.month, date.year))
                

calendar = lv.calendar(lv.scr_act())
calendar.set_size(200, 200)
calendar.align(lv.ALIGN.CENTER, 0, 20)
calendar.add_event_cb(event_handler, lv.EVENT.ALL, None)

calendar.set_today_date(2021, 02, 23)
calendar.set_showed_date(2021, 02)

# Highlight a few days
highlighted_days=[
    lv.calendar_date_t({'year':2021, 'month':2, 'day':6}),
    lv.calendar_date_t({'year':2021, 'month':2, 'day':11}),
    lv.calendar_date_t({'year':2021, 'month':2, 'day':22})
]

calendar.set_highlighted_dates(highlighted_days, len(highlighted_days))

lv.calendar_header_dropdown(calendar)
