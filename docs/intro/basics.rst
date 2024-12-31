.. _lvgl_basics:

===========
LVGL Basics
===========

LVGL (Light and Versatile Graphics Library) is a free and open-source graphics
library providing everything you need to create an embedded GUI with easy-to-use
graphical elements, beautiful visual effects, and a low memory footprint.

How does it do this?



.. _basic_data_flow:

Overview of LVGL's Data Flow
****************************

.. figure:: /misc/intro_data_flow.png
    :scale: 75 %
    :alt:  LVGL Data Flow
    :align:  center
    :figwidth:  image

    Overview of LVGL Data Flow


You create one :ref:`display` for each physical display panel, create
:ref:`basics_screen_widgets` on them, add :ref:`basics_widgets` onto those Screens.
To handle touch, mouse, keypad, etc., you :ref:`create an Input Device <indev_creation>`
for each.  The :ref:`tick_interface` tells LVGL what time is it.  :ref:`timer_handler`
drives LVGL's timers which, in turn, perform all of LVGL's time-related tasks:

- periodically refreshes displays,
- reads input devices,
- fires events,
- runs any animations, and
- runs user-created timers.


.. _applications_job:

Application's Job
-----------------

After initialization, the application's job is merely to create Widget Trees when
they are needed, manage events those Widgets generate (by way of user interaction
and other things), and delete them when they are no longer needed.  LVGL takes care
of the rest.



.. _basics_major_concepts:

Major Concepts
**************


.. _display-vs-screen:

Display vs Screen
-----------------
Before we get into any details about Widgets, let us first clarify the difference
between two terms that you will hereafter see frequently:

- A **Display** or **Display Panel** is the physical hardware displaying the pixels.
- A :ref:`display` object is an object in RAM that represents a **Display** meant
  to be used by LVGL.
- A **Screen** is the "root" Widget in the Widget Trees mentioned above, and are
  "attached to" a particular :ref:`display`.


Default Display
---------------
When the first :ref:`display` object is created, it becomes the Default Display.
Many functions related to Screen Widgets use the default display.
See :ref:`default_display` for more information.


.. _basics_screen_widgets:

Screen Widgets
--------------
In this documentation, the term "Screen Widget" is frequently shortened to just
"Screen".  But it is important to understand that a "Screen" is simply any
:ref:`Widget <widgets>` created without a parent --- the "root" of each Widget Tree.

See :ref:`screens` for more details.


Active Screen
-------------

The Active Screen is the screen (and its child Widgets) currently being displayed.
See :ref:`active_screen` for more information.


.. _basics_widgets:

Widgets
-------
After LVGL is initialized (see :ref:`initializing_lvgl`), to create an interactive
user interface, an application next creates a tree of Widgets that LVGL can render to
the associated display, and with which the user can interact.

Widgets are "intelligent" LVGL graphical elements such as :ref:`Base Widgets
<base_widget_overview>` (simple rectangles and :ref:`screens`), Buttons, Labels,
Checkboxes, Switches, Sliders, Charts, etc.  Go to :ref:`widgets` to see the full
list.

To build this Widget Tree, the application first acquires a pointer to a Screen Widget.
A system designer is free to use the default Screen created with the :ref:`display`
and/or create his own.  To create a new Screen Widget, simply create a Widget passing
NULL as the parent argument.  Technically, this can be any type of Widget, but in
most cases it is a :ref:`base_widget_overview`.  (An example of another type of
Widget being used as a Screen is an :ref:`lv_image` Widget to supply an image for the
background.)

The application then adds Widgets to this Screen as children in the tree.  Widgets
are automatically added as children to their parent Widgets at time of creation ---
the Widget's parent is passed as the first argument to the function that creates
the Widget.  After being so added, we say that the parent Widget "contains" the
child Widget.

Any Widget can contain other Widgets.  For example, if you want a Button to have
text, create a Label Widget and add it to the Button as a child.

Each child Widget becomes "part of" its parent Widget.  Because of this relationship:

- when the parent Widget moves, its children move with it;
- when the parent Widget is deleted, its children are deleted with it;
- a child Widget is only visible within its parent's boundaries; any part of a child
  outside its parent's boundaries is clipped (i.e. not rendered).

Screens (and their child Widgets) can be created and deleted at any time *except*
when the Screen is the :ref:`active_screen`.  If you want to delete the current Screen
as you load a new one, call :cpp:func:`lv_screen_load_anim` and pass ``true`` for the
``auto_del`` argument.  If you want to keep the current Screen in RAM when you load a
new Screen, pass ``false`` for the ``auto_del`` argument, or call
:cpp:func:`lv_screen_active` to load the new screen.

A system designer is free to keep any number of Screens (and their child Widgets) in
RAM (e.g. for quick re-display again later).  Doing so:

- requires more RAM, but
- can save the time of repeatedly creating the Screen and its child Widgets;
- can be handy when a Screen is complex and/or can be made the :ref:`active_screen` frequently.

If multiple Screens are maintained in RAM simultaneously, it is up to the system
designer as to how they are managed.


.. _basics_creating_widgets:

Creating Widgets
~~~~~~~~~~~~~~~~
Widgets are created by calling functions that look like this::

    lv_<type>_create(parent)

The call will return an :cpp:type:`lv_obj_t` ``*`` pointer that can be used later to
reference the Widget to set its attributes.

For example:

.. code-block:: c

    lv_obj_t * slider1 = lv_slider_create(lv_screen_active());


.. _basics_modifying_widgets:

Modifying Widgets
~~~~~~~~~~~~~~~~~
Attributes common to all Widgets are set by functions that look like this::

    lv_obj_set_<attribute_name>(widget, <value>)

For example:

.. code-block:: c

    lv_obj_set_x(slider1, 30);
    lv_obj_set_y(slider1, 10);
    lv_obj_set_size(slider1, 200, 50);

Along with these attributes, widgets can have type-specific attributes which are
set by functions that look like this::

    lv_<type>_set_<attribute_name>(widget, <value>)

For example:

.. code-block:: c

    lv_slider_set_value(slider1, 70, LV_ANIM_ON);

To see the full API visit the documentation of the Widget in question under
:ref:`widgets` or study its related header file in the source code, e.g.

- lvgl/src/widgets/slider/lv_slider.h

or view it on GitHub, e.g.

- https://github.com/lvgl/lvgl/blob/master/src/widgets/slider/lv_slider.h .


.. _basics_deleting_widgets:

Deleting Widgets
~~~~~~~~~~~~~~~~
To delete any widget and its children::

    lv_obj_delete(lv_obj_t * widget)



.. _basics_events:

Events
------

Events are used to inform the application that something has happened with a Widget.
You can assign one or more callbacks to a Widget which will be called when the
Widget is clicked, released, dragged, being deleted, etc.

A callback is assigned like this:

.. code-block:: c

   lv_obj_add_event_cb(btn, my_btn_event_cb, LV_EVENT_CLICKED, NULL);

   ...

   void my_btn_event_cb(lv_event_t * e)
   {
       printf("Clicked\n");
   }

:cpp:enumerator:`LV_EVENT_ALL` can be used instead of :cpp:enumerator:`LV_EVENT_CLICKED`
to invoke the callback for all events.  (Beware:  there are a LOT of events!  This can
be handy for debugging or learning what events occur for a given Widget, or indeed
if the application needs to process all events for some reason.)

Event callbacks receive the argument :cpp:expr:`lv_event_t * e` containing the
current event code and other event-related information.  The current event code can
be retrieved with:

.. code-block:: c

    lv_event_code_t code = lv_event_get_code(e);

The Widget that triggered the event can be retrieved with:

.. code-block:: c

    lv_obj_t * obj = lv_event_get_target(e);

To learn all features of the events go to the :ref:`events` section.


.. _basics_parts:

Parts
-----

Widgets are built from one or more *parts*.  For example, a button
has only one part called :cpp:enumerator:`LV_PART_MAIN`. However, a
:ref:`lv_slider` has :cpp:enumerator:`LV_PART_MAIN`, :cpp:enumerator:`LV_PART_INDICATOR`
and :cpp:enumerator:`LV_PART_KNOB`.

By using parts you can apply different styles to sub-elements of a widget.  (See below.)

Read the Widget's documentation to learn which parts it uses.


.. _basics_states:

States
------

Widgets can be in a combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: Focused via keypad or encoder or clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: Focused via keypad or encoder but not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: Edit by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: Hovered by mouse
- :cpp:enumerator:`LV_STATE_PRESSED`: Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: Disabled

For example, if you press a Widget it will automatically go to the
:cpp:enumerator:`LV_STATE_FOCUSED` and :cpp:enumerator:`LV_STATE_PRESSED` states and when you
release it the :cpp:enumerator:`LV_STATE_PRESSED` state will be removed while the
:cpp:enumerator:`LV_STATE_FOCUSED` state remains active.

To check if a Widget is in a given state use
:cpp:expr:`lv_obj_has_state(widget, LV_STATE_...)`. It will return ``true`` if the
Widget is currently in that state.

To manually add or remove states use:

.. code-block:: c

   lv_obj_add_state(widget, LV_STATE_...);
   lv_obj_remove_state(widget, LV_STATE_...);


.. _basics_styles:

Styles
------

A style instance contains properties such as background color, border
width, font, etc. that describe the appearance of Widgets.

Styles are carried in :cpp:struct:`lv_style_t` objects.  Only their pointer is saved
in the Widgets so they need to be defined as static or global variables.  Before
using a style it needs to be initialized with :cpp:expr:`lv_style_init(&style1)`.
After that, properties can be added to configure the style.  For example:

.. code-block:: c

    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_bg_color(&style1, lv_color_hex(0xa03080))
    lv_style_set_border_width(&style1, 2))

See :ref:`style_properties_overview` for more details.

See :ref:`style_properties` to see the full list.

Styles are assigned using the OR-ed combination of a Widget's part and
state. For example to use this style on the slider's indicator when the
slider is pressed:

.. code-block:: c

    lv_obj_add_style(slider1, &style1, LV_PART_INDICATOR | LV_STATE_PRESSED);

If the *part* is :cpp:enumerator:`LV_PART_MAIN` it can be omitted:

.. code-block:: c

    lv_obj_add_style(btn1, &style1, LV_STATE_PRESSED); /* Equal to LV_PART_MAIN | LV_STATE_PRESSED */

Similarly, :cpp:enumerator:`LV_STATE_DEFAULT` can be omitted:

.. code-block:: c

   lv_obj_add_style(slider1, &style1, LV_PART_INDICATOR); /* Equal to LV_PART_INDICATOR | LV_STATE_DEFAULT */

For :cpp:enumerator:`LV_STATE_DEFAULT` | :cpp:enumerator:`LV_PART_MAIN` simply pass ``0``:

.. code-block:: c

   lv_obj_add_style(btn1, &style1, 0); /* Equal to LV_PART_MAIN | LV_STATE_DEFAULT */

Styles can be cascaded (similarly to CSS). This means you can add more
styles to a part of a Widget. For example ``style_btn`` can set a
default button appearance, and ``style_btn_red`` can overwrite the
background color to make the button red:

.. code-block:: c

   lv_obj_add_style(btn1, &style_btn, 0);
   lv_obj_add_style(btn1, &style1_btn_red, 0);

If a property is not set for the current state, the style with
:cpp:enumerator:`LV_STATE_DEFAULT` will be used. A default value is used if the
property is not defined in the default state.

Some properties (particularly the text-related ones) can be inherited. This
means if a property is not set in a Widget it will be searched for in
its parents. For example, you can set the font once in the screen's
style and all text on that screen will inherit it by default.

Local style properties also can be added to Widgets. This creates a
style which resides inside the Widget and is used only by that Widget:

.. code-block:: c

    lv_obj_set_style_bg_color(slider1, lv_color_hex(0x2080bb), LV_PART_INDICATOR | LV_STATE_PRESSED);

To learn all the features of styles see :ref:`styles`.


.. _basics_themes:

Themes
------

Themes are the default styles for Widgets. Styles from a theme are
applied automatically when Widgets are created.

The theme for your application is a compile time configuration set in
``lv_conf.h``.


.. _basics_micropython:

MicroPython
-----------

LVGL can even be used with :ref:`micropython`.

.. code-block:: python

    # Initialize
    import display_driver
    import lvgl as lv

    # Create a button with a label
    scr = lv.obj()
    btn = lv.button(scr)
    btn.align(lv.ALIGN.CENTER, 0, 0)
    label = lv.label(btn)
    label.set_text('Hello World!')
    lv.screen_load(scr)



.. _going_deeper:

Going Deeper
*************

There are several good ways ways to gain deeper knowledge of LVGL.  Here is one
recommended order of documents to read and things to play with while you are
advancing your knowledge:

1. If not already read, start with :ref:`introduction` page of
   the documentation.  (5 minutes)
2. Check out the `Online Demos`_ to see LVGL in action.  (3 minutes)
3. If not already done, read the :ref:`lvgl_basics` (above).  (15 minutes)
4. Set up an LVGL :ref:`simulator`.  (10 minutes)
5. Have a look at some :ref:`examples` and their code.
6. Add LVGL to your project.  See :ref:`add_lvgl_to_your_project` or check out
   the `ready-to-use Projects`_.
7. Read the :ref:`main_components` pages to get a better understanding of the library. (2-3 hours)
8. Skim the documentation of :ref:`widgets` to see what is available.
9. If you have questions go to the `Forum`_.
10. Read the :ref:`contributing` guide to see how you can help to improve LVGL. (15 minutes)


.. _online demos:           https://lvgl.io/demos
.. _ready-to-use projects:  https://github.com/lvgl?q=lv_port_&type=&language=
.. _forum:                  https://forum.lvgl.io/



.. _basics_examples:

Basic Examples
**************

Below are several basic examples.  They include the application code that produces
the Widget Tree needed to make LVGL render the examples shown.  Each example assumes
a LVGL has undergone normal initialization, meaning that a ``lv_display_t`` object
was created and therefore has an :ref:`active_screen`.


.. include:: ../examples/get_started/index.rst


