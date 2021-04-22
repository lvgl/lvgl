```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/calendar.md
```
# Calendar (lv_calendar)

## Overview

The Calendar object is a classic calendar which can:
- highlight the current day
- highlight any user-defined dates
- display the name of the days
- go the next/previous month by button click
- highlight the clicked day


## Parts and Styles
The calendar's main part is called `LV_CALENDAR_PART_BG`. It draws a background using the typical background style properties.

Besides the following virtual parts exist:
- `LV_CALENDAR_PART_HEADER` The upper area where the current year and month's name is shown. It also has buttons to move the next/previous month. 
It uses typical background properties plus padding to adjust its size and margin to set the distance from the top of the calendar and the day names below it.
- `LV_CALENDAR_PART_DAY_NAMES` Shows the name of the days below the header. It uses the *text* style properties padding to keep some distance from the background (left, right), header (top) and dates (bottom).
- `LV_CALENDAR_PART_DATES` Show the date numbers from 1..28/29/30/31 (depending on current month). Different "state" of the states are drawn according to the states defined in this part:
  - normal dates: drawn with `LV_STATE_DEFAULT` style
  - pressed date: drawn with `LV_STATE_PRESSED` style
  - today: drawn with `LV_STATE_FOCUSED` style
  - highlighted dates: drawn with `LV_STATE_CHECKED` style   


## Usage


## Overview

To set and get dates in the calendar, the `lv_calendar_date_t` type is used which is a structure with `year`, `month` and `day` fields.


### Current date
To set the current date (today), use the `lv_calendar_set_today_date(calendar, &today_date)` function.

### Shown date
To set the shown date, use `lv_calendar_set_shown_date(calendar, &shown_date)`;

### Highlighted days
The list of highlighted dates should be stored in a `lv_calendar_date_t` array loaded by `lv_calendar_set_highlighted_dates(calendar, &highlighted_dates)`.  
Only the arrays pointer will be saved so the array should be a static or global variable.

### Name of the days
The name of the days can be adjusted with `lv_calendar_set_day_names(calendar, day_names)` where `day_names` looks like `const char * day_names[7] = {"Su", "Mo", ...};`

### Name of the months
Similarly to `day_names`, the name of the month can be set with `lv_calendar_set_month_names(calendar, month_names_array)`.

## Events
Besides the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the calendars:
**LV_EVENT_VALUE_CHANGED** is sent when the current month has changed.

In *Input device related* events, `lv_calendar_get_pressed_date(calendar)` tells which day is currently being pressed or return `NULL` if no date is pressed.

## Keys
No *Keys* are processed by the object type.

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
