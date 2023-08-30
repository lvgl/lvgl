# Calendar (lv_calendar)

**From v8.1 the header is added directly into the Calendar widget and the API of the headers has been changed.**

## Overview

The Calendar object is a classic calendar which can:
- show the days of any month in a 7x7 matrix
- Show the name of the days
- highlight the current day (today)
- highlight any user-defined dates

The Calendar is added to the default group (if it is set). Calendar is an editable object which allow selecting and clicking the dates with encoder navigation too.

To make the Calendar flexible, by default it doesn't show the current year or month. Instead, there are optional "headers" that can be attached to the calendar.

## Parts and Styles
The Calendar is composed of 3 widegets
- Container: A rectangle which is a container for the *Header* and the *Days*. Uses only `LV_PART_MAIN` where all the background related style properties are working.
- Days: It's a [Button matrix](/widgets/core/btnmatrix) object under the hood to arrange the days into a matrix. `lv_calendar_get_btnmatrix(calendar)` can be used to get it.
    - `LV_PART_MAIN` The background of the calendar. Uses all the background related style properties.
    - `LV_PART_ITEMS` Refers to the dates and day names. Button matrix control flags are set to differentiate the buttons and a custom drawer event is added modify the properties of the buttons as follows:
       - day names have no border, no background and drawn with a gray color
       - days of the previous and next month have `LV_BTNMATRIX_CTRL_DISABLED` flag
       - today has a thicker border with the theme's primary color
       - highlighted days have some opacity with the theme's primary color.
- Header: Not created by default, the details are up to the given header.

## Usage

Some functions use the `lv_calendar_date_t` type which is a structure with `year`, `month` and `day` fields.

### Current date
To set the current date (today), use the `lv_calendar_set_today_date(calendar, year, month, day)` function. `month` needs to be in 1..12 range and `day` in 1..31 range.

### Shown date
To set the shown date, use `lv_calendar_set_shown_date(calendar, year, month)`;

### Highlighted days

The list of highlighted dates should be stored in a `lv_calendar_date_t` array loaded by `lv_calendar_set_highlighted_dates(calendar, highlighted_dates, date_num)`.
Only the array's pointer will be saved so the array should be a static or global variable.

### Name of the days
The name of the days can be adjusted with `lv_calendar_set_day_names(calendar, day_names)` where `day_names` looks like `const char * day_names[7] = {"Su", "Mo", ...};`
Only the pointer of the day names is saved so the elements should be static, global or constant variables.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent if a date is clicked. `lv_calendar_get_pressed_date(calendar, &date)` set `date` to the date currently being pressed. Returns `LV_RES_OK` if there is a valid pressed date, else `LV_RES_INV`.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP/LEFT/RIGHT` To navigate among the buttons to dates
- `LV_KEY_ENTER` To press/release the selected date

Learn more about [Keys](/overview/indev).

## Headers

### Arrow buttons

`lv_calendar_header_arrow_create(calendar)` creates a header that contains a left and right arrow on the sides and a text with the current year and month between them.


### Drop-down
`lv_calendar_header_dropdown_create(calendar)` creates a header that contains 2 drop-drown lists: one for the year and another for the month.


## Example

```eval_rst

.. include:: ../../../examples/widgets/calendar/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_calendar.h
  :project: lvgl

```
