def event_handler(evt):
    code = evt.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = evt.get_target()
        date = lv.calendar_date_t()
        lv.calendar.get_pressed_date(source,date)
        if date:
             print("Clicked date: %02d.%02d.%02d"%(date.day, date.month, date.year))
                

calendar = lv.calendar(lv.scr_act())
calendar.set_size(200, 200)
calendar.align(lv.ALIGN.CENTER, 0, 20)
calendar.add_event_cb(event_handler, lv.EVENT.ALL, None)

calendar.set_today_date(2021, 02, 23)
calendar.set_showed_date(2021, 02)

# Highlight a few days
highlighted_days=[]
for i in range(3):
    highlighted_days.append(lv.calendar_date_t())

highlighted_days[0].year=2021
highlighted_days[0].month=02
highlighted_days[0].day=6

highlighted_days[1].year=2021
highlighted_days[1].month=02
highlighted_days[1].day=11

highlighted_days[2].year=2022
highlighted_days[2].month=02
highlighted_days[2].day=22

calendar.set_highlighted_dates(highlighted_days, 3)

header = lv.calendar_header_dropdown(lv.scr_act(),calendar)
# header = lv.calendar_header_arrow(lv.scr_act(),calendar,25)


