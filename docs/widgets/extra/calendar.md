```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/calendar.md
```
# Calendar (lv_calendar)

## Overview

The Calendar object is a classic calendar which can:
- can show the days of any month in a 7x7 matrix 
- Show the name of the days 
- highlight the current day
- highlight any user-defined dates

The Calendar is added to the deafult group (if it is set). Besides the Calendar is an editable object to allow selecting and clicing the dates with encoder navigation too.

To make the Calendar flexible, by default it doesn't show the curent year or month. Instead, there external "headers" that can be attached to the calendar.

## Parts and Styles
The calendar object uses the [Button matrix](/widgets/btnmatrix) object under the hood to arrange the days into a matrix.
- `LV_PART_MAIN`
- `LV_PART_ITEMS` Refers to the dates and day names. Button matrix control flags are set the to differentiate the buttons and a custom drawer event modifies the properties of the buttons
   - day names haev no border, no background, drawn with a grey color
   - days of the previous and next month have `LV_BTNMATRIX_CTRL_DISABLED` flag
   - today has a ticker border with the themes primary color
   - highlighted day have 40% opacity with the themes primary color.

## Usage

Some funnctions uses  the `lv_calendar_date_t` type is used which is a structure with `year`, `month` and `day` fields.

### Current date
To set the current date (today), use the `lv_calendar_set_today_date(calendar, year, month, day)` function. `month` needs to be in 1..12 range and `day` in 1..31 range

### Shown date
To set the shown date, use `lv_calendar_set_shown_date(calendar, year, month)`;

### Highlighted days

The list of highlighted dates should be stored in a `lv_calendar_date_t` array loaded by `lv_calendar_set_highlighted_dates(calendar, highlighted_dates, date_num)`.  
Only the arrays pointer will be saved so the array should be a static or global variable.

### Name of the days
The name of the days can be adjusted with `lv_calendar_set_day_names(calendar, day_names)` where `day_names` looks like `const char * day_names[7] = {"Su", "Mo", ...};`
Only the pointer of the day names is saved  so the array should be a static, global or constant variables.

## Headers

### Arrow buttons

`lv_calendar_header_arrow_create(parent, calendar, button_size)` creates a header that contains a left and right arrow on the sides and atext with the current year and month between them.

### Dropdown
`lv_calendar_header_dropdown_create(parent, calendar)` creates a header that contains 2 drop-drown lists: one for the year and an other for the month.
 
## Events
- `LV_EVENT_VALUE_CHANGED` Sent is a data is clicked. `lv_calendar_get_pressed_date(calendar, &date)` tells which day is currently being pressed. Returns `LV_RES_OK` if theres is valid pressed data, else `LV_RES_INV`. 

## Keys
- `LV_KEY_RIGHT/UP/LEFT/RIGHT` To navigate among the buttons to dates
- `LV_KEY_ENTER` To press/release the selected date

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_calendar/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_calendar.h
  :project: lvgl

```
