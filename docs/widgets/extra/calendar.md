```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/calendar.md
```
# Calendar (lv_calendar)

## Overview

The Calendar object is a classic calendar which can:
- can show the days of any month in a 7x7 matrix 
- Show the name of the days 
- highlight the current day (today)
- highlight any user-defined dates

The Calendar is added to the default group (if it is set). Besides the Calendar is an editable object to allow selecting and clicking the dates with encoder navigation too.

To make the Calendar flexible, by default it doesn't show the current year or month. Instead, there area external "headers" that can be attached to the calendar.

## Parts and Styles
The calendar object uses the [Button matrix](/widgets/core/btnmatrix) object under the hood to arrange the days into a matrix.
- `LV_PART_MAIN` The background of the calendar. It uses all the background related style properties. 
- `LV_PART_ITEMS` Refers to the dates and day names. Button matrix control flags are set the to differentiate the buttons and a custom drawer event is added modify the properties of the buttons
   - day names have no border, no background and drawn with a gray color
   - days of the previous and next month have `LV_BTNMATRIX_CTRL_DISABLED` flag
   - today has a ticker border with the themes primary color
   - highlighted day have some opacity with the themes primary color.

## Usage

Some functions uses  the `lv_calendar_date_t` type which is a structure with `year`, `month` and `day` fields.

### Current date
To set the current date (today), use the `lv_calendar_set_today_date(calendar, year, month, day)` function. `month` needs to be in 1..12 range and `day` in 1..31 range.

### Shown date
To set the shown date, use `lv_calendar_set_shown_date(calendar, year, month)`;

### Highlighted days

The list of highlighted dates should be stored in a `lv_calendar_date_t` array loaded by `lv_calendar_set_highlighted_dates(calendar, highlighted_dates, date_num)`.  
Only the arrays pointer will be saved so the array should be a static or global variable.

### Name of the days
The name of the days can be adjusted with `lv_calendar_set_day_names(calendar, day_names)` where `day_names` looks like `const char * day_names[7] = {"Su", "Mo", ...};`
Only the pointer of the day names is saved  so the elements should be static, global or constant variables.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent is a data is clicked. `lv_calendar_get_pressed_date(calendar, &date)` tells which day is currently being pressed. Returns `LV_RES_OK` if theres is valid pressed data, else `LV_RES_INV`. 

Learn more about [vent](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP/LEFT/RIGHT` To navigate among the buttons to dates
- `LV_KEY_ENTER` To press/release the selected date

Learn more about [Keys](/overview/indev).

## Headers

### Arrow buttons

`lv_calendar_header_arrow_create(parent, calendar, button_size)` creates a header that contains a left and right arrow on the sides and a text with the current year and month between them.


### Drop-down
`lv_calendar_header_dropdown_create(parent, calendar)` creates a header that contains 2 drop-drown lists: one for the year and an other for the month.
 


## Example

```eval_rst

.. include:: ../../../examples/widgets/calendar/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_calendar.h
  :project: lvgl

```
