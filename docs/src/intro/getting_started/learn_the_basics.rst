.. _learn_the_basics:

================
Learn the Basics
================

Overview
********

LVGL (Light and Versatile Graphics Library) is a free and open-source graphics
library providing everything you need to create an embedded GUI with easy-to-use
mobile-phone-like graphical elements, beautiful visual effects, and a low memory
footprint.

You can think of LVGL as a collection of C and H files that can be dropped into
any project to add UI capabilities to the product.

With the help of consistent and easy-to-learn API functions you can create widgets
(buttons, sliders, charts, etc), style them, add events, layouts, or animations.

Based on these settings LVGL will render an image (either by using its built-in
software rendering engine or a GPU) and will call a callback function to show
the rendered image on the display. This callback function is the main interface
between LVGL and the display. Most of the porting-related work is focused on
writing such a callback in an effective way.

This chapter will show the basics to give an idea about how LVGL works and how it can be used.
For more details about each feature visit that feature's dedicated documentation page.

.. _basic_data_flow:

Basic Data Flow
---------------

1. **Driver Initialization**: It's the user's responsibility to set up the clock, timers, peripherals, etc.
2. **Call lv_init()**: It initializes LVGL itself
3. **Create display and input devices**: Create display(s) (:cpp:type:`lv_display_t`) and input device(s)  (:cpp:type:`lv_indev_t`) and set up their callbacks
4. **Create the UI**: Call LVGL functions to create screens, widgets, styles, animations, events, etc.
5. **Call lv_timer_handler() in a loop**: It handles all the LVGL-related tasks:
    - refresh display(s),
    - read input devices,
    - fire events based on user input (and other things),
    - run any animations, and
    - run user-created timers.

Integration example
-------------------

This is just a brief example of how to add LVGL to a new project. For more details
check out :ref:`adding_lvgl_to_your_project`.

.. code-block:: c

    void main(void)
    {
        your_driver_init();

        lv_init();

        lv_tick_set_cb(my_get_millis);

        lv_display_t * display = lv_display_create(320, 240);

        /*LVGL will render to this 1/10 screen sized buffer for 2 bytes/pixel*/
        static uint8_t buf[320 * 240 / 10 * 2];
        lv_display_set_buffers(display, buf, NULL, LV_DISPLAY_RENDER_MODE_PARTIAL);

        /*This callback will display the rendered image*/
        lv_display_set_flush_cb(display, my_flush_cb);

        /*Create widgets*/
        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Hello LVGL!");

        /*Make LVGL periodically execute its tasks*/
        while(1) {
            lv_timer_handler();
            my_sleep(5);  /*Wait 5 milliseconds before processing LVGL timer again*/
        }
    }

    /*Return the elapsed milliseconds since startup.
     *It needs to be implemented by the user*/
    uint32_t my_get_millis(void)
    {
        return my_tick_ms;
    }

    /*Copy the rendered image to the screen.
     *It needs to be implemented by the user*/
    void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf)
    {
        /*Show the rendered image on the display*/
        my_display_update(area, px_buf);

        /*Indicate that the buffer is available.
         *If DMA were used, call in the DMA complete interrupt*/
        lv_display_flush_ready();
    }


.. _basics_displays:

Displays
********

*Display* refers to the actual hardware. In order to connect LVGL to the hardware an :cpp:expr:`lv_display_t`
object needs to be created and initialized.

LVGL has built-in support for many :ref:`built-in drivers <drivers>`, but it's easy to initialize a
display from scratch as well (as shown above).

LVGL also handles multiple displays at once.

.. _basics_screens:

Screens
*******

A *Screen* is an LVGL widget created on a *Display*. It's a logical container for other widgets. A display can
have multiple screens, but there is always one active screen, which can be retrieved by using :cpp:expr:`lv_screen_active()`.
It returns an `lv_obj_t *` pointer. See :ref:`active_screen` for more information.

The most common way to create a screen is by creating a :ref:`Base widget <base_widget>` with a ``NULL`` parent. E.g.

.. code-block:: c

    lv_obj_t * my_screen = lv_obj_create(NULL);

A screen can be loaded like this: :cpp:expr:`lv_screen_load(my_screen)`

.. _basics:widgets:

Widgets
*******

Widgets are the basic building blocks of the UI. For example:
:ref:`lv_button`, :ref:`lv_slider`, :ref:`lv_dropdown`, :ref:`lv_chart`, etc.

Widgets can be created dynamically by calling their respective create functions. The
create function returns an ``lv_obj_t *`` pointer which can be used to configure the widget later.

Each create function has a single ``parent`` argument that defines which widget the new one will be added to.

For example:

.. code-block:: c

    lv_obj_t * my_button1 = lv_button_create(lv_screen_active());
    lv_obj_t * my_label1 = lv_label_create(my_button1);

If a widget or screen is no longer needed, it can be removed by calling
:cpp:expr:`lv_obj_delete(my_button1)`

To change the properties of widgets, two sets of functions can be used:

- ``lv_obj_...()`` functions for common properties, e.g. :cpp:expr:`lv_obj_set_width()`, :cpp:expr:`lv_obj_add_style()`, etc. These are covered in :ref:`common_widget_features`.

- ``lv_<widget_type>_...()`` functions for type-specific properties, e.g.  :cpp:expr:`lv_label_set_text()`, :cpp:expr:`lv_slider_set_value()`, etc.

Here is an example that also shows some non-pixel units for sizes:

.. code-block:: c

    lv_obj_t * my_button1 = lv_button_create(lv_screen_active());
    /*Set parent-sized width, and content-sized height*/
    lv_obj_set_size(my_button1, lv_pct(100), LV_SIZE_CONTENT);
    /*Align to the right center with 20px offset horizontally*/
    lv_obj_align(my_button1, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_t * my_label1 = lv_label_create(my_button1);
    lv_label_set_text_fmt(my_label1, "Click me!");
    lv_obj_set_style_text_color(my_label1, lv_color_hex(0xff0000), 0);
    /*Make the text red*/

To see the full API for any widget, see its documentation at :ref:`widgets`, or check
its related header file in the source code.


.. _basics_events:

Events
******

Events are used to inform the application that something has happened with a Widget.
You can assign one or more callbacks to a Widget which will be called when the Widget
is clicked, released, dragged, being deleted, etc.

A callback is assigned like this:

.. code-block:: c

   lv_obj_add_event_cb(btn, my_btn_event_cb, LV_EVENT_CLICKED, NULL);

   ...

   void my_btn_event_cb(lv_event_t * e)
   {
       printf("Clicked\n");
   }

:cpp:enumerator:`LV_EVENT_ALL` can be used instead of :cpp:enumerator:`LV_EVENT_CLICKED`
to invoke the callback for all events.

Event callbacks receive the argument :cpp:expr:`lv_event_t * e` containing the
current event code and other event-related information. The current event code can
be retrieved with:

.. code-block:: c

    lv_event_code_t code = lv_event_get_code(e);

The Widget that triggered the event can be retrieved with:

.. code-block:: c

    lv_obj_t * widget = lv_event_get_target_obj(e);

Learn all about Events in the :ref:`events` section.

Parts and States
****************

.. _basics_parts:

Parts
-----

Widgets are built from one or more *parts*. For example, a button
has only one part called :cpp:enumerator:`LV_PART_MAIN`. However, a
:ref:`lv_slider` has :cpp:enumerator:`LV_PART_MAIN`, :cpp:enumerator:`LV_PART_INDICATOR`
and :cpp:enumerator:`LV_PART_KNOB`.

By using parts you can apply different :ref:`styles <basics_styles>` to the parts
of a widget.

Read the Widget's documentation to learn which parts it uses.

.. _basics_states:

States
------

Widgets can be in a combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: Focused via keypad or encoder or clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: Focused via keypad or encoder but not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: Edited by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: Hovered by mouse
- :cpp:enumerator:`LV_STATE_PRESSED`: Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: Disabled

For example, if you press a Widget it will automatically go to the
:cpp:enumerator:`LV_STATE_FOCUSED` and :cpp:enumerator:`LV_STATE_PRESSED` states. When you
release it, the :cpp:enumerator:`LV_STATE_PRESSED` state will be removed while the
:cpp:enumerator:`LV_STATE_FOCUSED` state remains active.

To check if a Widget is in a given state use
:cpp:expr:`lv_obj_has_state(widget, LV_STATE_...)`. It will return ``true`` if the
Widget is currently in that state.

To programmatically add or remove states use:

.. code-block:: c

   lv_obj_add_state(widget, LV_STATE_...);
   lv_obj_remove_state(widget, LV_STATE_...);

.. _basics_styles:

Styles
******

Initializing styles
-------------------

Styles are carried in :cpp:struct:`lv_style_t` objects. They contain properties such as
background color, border width, font, etc.

The styles can be added to a widget's given :ref:`Part <basics_parts>` and :ref:`State <basics_states>`.
Only their pointer is saved in the Widgets so they need to be defined as static or global variables.

Before using a style it needs to be initialized with :cpp:expr:`lv_style_init(&style1)`.
After that, properties can be added to configure the style. For example:

.. code-block:: c

    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_bg_color(&style1, lv_color_hex(0xa03080));
    lv_style_set_border_width(&style1, 2);

See :ref:`style_properties_overview` for more details.

See :ref:`style_properties` to see the full list.

Adding styles to the widgets
----------------------------

After that it can be added to widgets:

.. code-block:: c

    lv_obj_add_style(my_button1, &style1, 0); /*0 means add to the main part and default state*/
    lv_obj_add_style(my_checkbox1, &style1, LV_STATE_DISABLED); /*Add to checkbox's disabled state*/
    lv_obj_add_style(my_slider1, &style1, LV_PART_KNOB | LV_STATE_PRESSED); /*Add to the slider's knob pressed state*/

Inheritance
-----------

Some properties (particularly the text-related ones) can be inherited. This
means if a property is not set in a Widget it will be searched for in
its parents. For example, you can set the font once in the screen's
style and all text on that screen will inherit it by default, unless the
font is specified on the widget or one of its parents.

Local styles
------------

Local style properties also can be added to Widgets. This creates a
style which resides inside the Widget and is used only by that Widget:

.. code-block:: c

    lv_obj_set_style_bg_color(slider1, lv_color_hex(0x2080bb), LV_PART_INDICATOR | LV_STATE_PRESSED);

See :ref:`styles` for full details.

.. _basics_subjects:

Subjects and Observers
**********************

Subjects and Observers are powerful tools to easily create data bindings.

Subjects are global :cpp:expr:`lv_subject_t` variables that store integer, color, string, etc. values.

Either the UI or the application can subscribe to these subjects by creating *observer callbacks* that
are notified when the subject changes.

A widget can also subscribe to a subject. This way, when the widget is deleted, it will be automatically unsubscribed.

For some widgets, helper functions make it simple to connect them to subjects. E.g.:
:cpp:expr:`lv_slider_bind_value()`, :cpp:expr:`lv_label_bind_text()`.

In general, using subjects and observers is a way to connect various parts of the UI and make them dynamically
react to application data changes—or allow the application to react to UI changes.

.. code-block:: c

    static void label_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        lv_obj_t * label = lv_observer_get_target_obj(observer);
        lv_label_set_text_fmt(label, "Progress: %d", lv_subject_get_int(subject));
    }

    ...

    static lv_subject_t subject1;
    lv_subject_init_int(&subject1, 10);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    /*lv_label_bind_text could have been used too*/
    lv_subject_add_observer_obj(&subject1, label_observer_cb, label1, NULL);

    lv_obj_t * slider1 = lv_slider_create(lv_screen_active());
    lv_slider_bind_value(slider1, &subject1);
    lv_obj_set_y(slider1, 30);

    lv_subject_set_int(&subject1, 30);

Learn more on the documentation page of :ref:`Observers <observer_how_to_use>`.

