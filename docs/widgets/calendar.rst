.. _lv_calendar:

======================
Calendar (lv_calendar)
======================

Overview
********

The Calendar object is a classic calendar which can: - show the days of
any month in a 7x7 matrix - Show the name of the days - highlight the
current day (today) - highlight any user-defined dates

The Calendar is added to the default group (if it is set). Calendar is
an editable object which allow selecting and clicking the dates with
encoder navigation too.

To make the Calendar flexible, by default it doesn't show the current
year or month. Instead, there are optional "headers" that can be
attached to the calendar.

.. _lv_calendar_parts_and_styles:

Parts and Styles
****************

The calendar object uses the `Button matrix </widgets/buttonmatrix>`__
object under the hood to arrange the days into a matrix.

- :cpp:enumerator:`LV_PART_MAIN` The background of the calendar. Uses all the background related style properties.
- :cpp:enumerator:`LV_PART_ITEMS` Refers to the dates and day names. Button matrix control flags are set to differentiate the
  buttons and a custom drawer event is added modify the properties of the buttons as follows:

  - day names have no border, no background and drawn with a gray color
  - days of the previous and next month have :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_DISABLED` flag
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

Shown date
----------

To set the shown date, use
:cpp:expr:`lv_calendar_set_shown_date(calendar, year, month)`

Highlighted days
----------------

The list of highlighted dates should be stored in a
:cpp:struct:`lv_calendar_date_t` array loaded by
:cpp:expr:`lv_calendar_set_highlighted_dates(calendar, highlighted_dates, date_num)`.
Only the array's pointer will be saved so the array should be a static
or global variable.

Name of the days
----------------

The name of the days can be adjusted with
:cpp:expr:`lv_calendar_set_day_names(calendar, day_names)` where ``day_names``
looks like ``const char * day_names[7] = {"Su", "Mo", ...};`` Only the
pointer of the day names is saved so the elements should be static,
global or constant variables.

Custom year list
----------------

Sets a custom year list with :cpp:expr:`lv_calendar_header_dropdown_set_year_list(calendar, years_list)`
where ``years_list`` is a pointer to the custom years list. It can be a constant string
like ``static const char * years = "2023\n2022\n2021\n2020\n2019";``, 
or can be generated dynamically into a buffer as well.

.. _lv_calendar_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent if a date is clicked.
   :cpp:expr:`lv_calendar_get_pressed_date(calendar, &date)` set ``date`` to the
   date currently being pressed. Returns :cpp:enumerator:`LV_RES_OK` if there is a
   valid pressed date, else :cpp:enumerator:`LV_RES_INVALID`.

Learn more about :ref:`events`.

.. _lv_calendar_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons to dates
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected date

Learn more about :ref:`indev_keys`.

.. _lv_calendar_header:

Headers
*******

**From v8.1 the header is added directly into the Calendar widget and
the API of the headers has been changed.**

Arrow buttons
-------------

:cpp:expr:`lv_calendar_header_arrow_create(calendar)` creates a header that
contains a left and right arrow on the sides and a text with the current
year and month between them.

Drop-down
---------

:cpp:expr:`lv_calendar_header_dropdown_create(calendar)` creates a header that
contains 2 drop-drown lists: one for the year and another for the month.

.. _lv_calendar_example:

Example
*******

.. include:: ../examples/widgets/calendar/index.rst

.. _lv_calendar_api:

API
***
