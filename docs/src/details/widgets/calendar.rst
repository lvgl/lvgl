.. _lv_calendar:

======================
Calendar (lv_calendar)
======================

Overview
********

The Calendar Widget is a classic calendar which can:

- show the days of any month in a 7x7 matrix;
- show the name of the days;
- highlight the current day (today);
- highlight any user-defined dates.

The Calendar is added to the default group (if one is set). Calendar is
an editable Widget which allow selecting and clicking the dates with
encoder or keyboard navigation as well as pointer-type input devices.

To make the Calendar flexible, by default it does not show the current
year or month. Instead, there are optional "headers" that can be
attached to the calendar.


.. _lv_calendar_parts_and_styles:

Parts and Styles
****************

The calendar Widget uses the :ref:`Button Matrix <lv_buttonmatrix>`
Widget under the hood to arrange the days into a matrix.

- :cpp:enumerator:`LV_PART_MAIN` Calendar background. Uses the :ref:`typical
  background style properties <typical bg props>`.
- :cpp:enumerator:`LV_PART_ITEMS` Refers to dates and day names. Button matrix
  control flags are set to differentiate the buttons and a custom drawer event is
  added to modify the properties of the buttons as follows:

  - day names have no border, no background and are drawn with a gray color
  - days of the previous and next month have the :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_DISABLED` flag
  - today has a thicker border with the theme's primary color - highlighted days have some opacity with the theme's primary color.


.. _lv_calendar_usage:

Usage
*****

Some functions use the :cpp:struct:`lv_calendar_date_t` type which is a structure
with ``year``, ``month`` and ``day`` fields.

Current date
------------

To set the current date (today), use the
:cpp:expr:`lv_calendar_set_today_date(calendar, year, month, day)` function.
``month`` needs to be in 1..12 range and ``day`` in 1..31 range.

Month shown
-----------

To set the shown date, use
:cpp:expr:`lv_calendar_set_month_shown(calendar, year, month)`

Highlighted days
----------------

The list of highlighted dates should be stored in a
:cpp:struct:`lv_calendar_date_t` array and applied to the Calendar by calling
:cpp:expr:`lv_calendar_set_highlighted_dates(calendar, highlighted_dates, date_num)`.
Only the array's pointer will be saved so the array should be have static or
global scope.

Names of days
-------------

The names of the days can be adjusted with
:cpp:expr:`lv_calendar_set_day_names(calendar, day_names)` where ``day_names``
looks like ``const char * day_names[7] = {"Su", "Mo", ...};`` Only the
pointer of the day names is saved so the array should have static or
global scope.

Custom year list
----------------

Set a custom year list with :cpp:expr:`lv_calendar_header_dropdown_set_year_list(calendar, years_list)`
where ``years_list`` is a pointer to the custom years list. It can be a constant string
like ``static const char * years = "2023\n2022\n2021\n2020\n2019";``,
or can be generated dynamically into a buffer as well.  Calendar stores these in a
Drop-Down List Widget via :cpp:func:`lv_dropdown_set_options` so the passed string
pointer can be supplied by a local variable or buffer and does not need to persist
beyond the call.


Chinese calendar
----------------

The Chinese calendar is a traditional cultural tool that integrates elements
such as the lunar calendar, solar terms, and traditional festivals. It is
widely used in Chinese social life, helping people understand the dates of
the lunar calendar, arrange festival activities, and inherit the excellent
traditional culture of the Chinese nation. Whether in families, businesses,
or education, the Chinese calendar plays an irreplaceable role, enabling
people to better understand and appreciate the charm of Chinese traditional
culture.

If you want to use the Chinese calendar, please
use :cpp:expr:`lv_calendar_set_chinese_mode(calendar, true)` to enable it.



.. _lv_calendar_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent if a date is clicked.
   :cpp:expr:`lv_calendar_get_pressed_date(calendar, &date)` to set ``date`` to the
   date currently being pressed. Returns :cpp:enumerator:`LV_RESULT_OK` if there is a
   valid pressed date; otherwise it returns :cpp:enumerator:`LV_RESULT_INVALID`.


.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_calendar_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons to dates
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected date

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_calendar_header:

Headers
*******

**From LVGL v8.1 onward, the header is added directly into the Calendar Widget and
the API of the headers has been changed.**

Arrow buttons
-------------

:cpp:expr:`lv_calendar_add_header_arrow(calendar)` creates a header that
contains a left and right arrow on the sides and text between the arrows showing the
current year and month.

Drop-down
---------

:cpp:expr:`lv_calendar_add_header_dropdown(calendar)` creates a header that
contains 2 Drop-Drown List Widgets for the year and month.

.. _lv_calendar_example:



Example
*******

.. include:: ../../examples/widgets/calendar/index.rst



.. _lv_calendar_api:

API
***
