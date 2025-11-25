.. _xml_events:

=============
Events in XML
=============

Overview
********

There are several ways to define events for user interactions. These events can be added as children of any widget.

Triggers
********

In all event types, the ``trigger`` attribute defines what kind of user action should trigger the event.
All LVGL event types are supported with straightforward mapping:

- :cpp:enumerator:`LV_EVENT_ALL`: ``"all"``
- :cpp:enumerator:`LV_EVENT_CLICKED`: ``"clicked"``
- :cpp:enumerator:`LV_EVENT_PRESSED`: ``"pressed"``
- etc.

Call Function
*************

User-defined functions can be called like this:

.. code-block:: xml

    <view>
        <lv_button width="200" height="100">
            <event_cb callback="my_callback_1" trigger="clicked" user_data="some_text"/>
            <lv_label text="Hello"/>
        </lv_button>
    </view>

When the XML is loaded at runtime, the callback name needs to be mapped to a function using
:cpp:expr:`lv_xml_register_event_cb("my_callback_1", an_event_handler)`.

The callback should follow the standard LVGL event callback signature:
``void an_event_handler(lv_event_t * e);``

In the exported C code, it is assumed that a function with the exact name exists.
For example, ``callback="my_callback_1"`` will be exported as:

.. code-block:: c

    void my_callback_1(lv_event_t * e); /* At the beginning of the exported file */

    lv_obj_add_event_cb(obj, my_callback_1, LV_EVENT_CLICKED, "some_text");

The ``user_data`` is optional. If omitted, ``NULL`` will be passed.

.. _xml_events_screen:

Screen Load and Create events
*****************************

By using the ``<screen_load_event>`` and ``<screen_create_event>`` tags as children
of a widget or component, screens can be loaded or created on a trigger (e.g., click).

The difference between load and create is that:

- **load**: Just loads an already existing screen. After leaving the screen, it remains in memory,
  so all states are preserved.
- **create**: The screen is created dynamically, and when leaving the screen, it is deleted, so all changes are lost
  (unless they are saved in ``subjects``).

Both tags support the following optional attributes:

- ``trigger``: Event code that triggers the action (e.g., ``"clicked"``, ``"long_pressed"``, etc.). Default: ``"clicked"``.
- ``anim_type``: Describes how the screen is loaded (e.g., ``"move_right"``, ``"fade_in"``). Default: ``"none"``.
- ``duration``: Length of the animation in milliseconds. Default: ``0``. Only used if ``anim_type`` is not ``"none"``.
- ``delay``: Wait time before loading the screen in milliseconds. Default: ``0``.

This is a simple example of both load and create:

.. code-block:: xml

    <!-- screen1.xml -->
    <screen>
       <view style_bg_color="0xff7788">
           <lv_button>
               <lv_label text="Create"/>
               <!-- Create an instance of "screen2" and load it. -->
               <screen_create_event screen="screen2" anim_type="over_right" duration="500" delay="1000"/>
           </lv_button>
       </view>
    </screen>

    <!-- screen2.xml -->
    <screen>
       <view style_bg_color="0x77ff88">
           <lv_button>
               <lv_label text="Load"/>
               <!-- Load an already created instance of screen1.-->
               <screen_load_event screen="screen1"/>
           </lv_button>
       </view>
    </screen>

.. code-block:: c

    lv_xml_register_component_from_data("screen1", screen1_xml);
    lv_xml_register_component_from_data("screen2", screen2_xml);

    /* Create an instance of screen1 so that it can be loaded from screen2. */
    lv_obj_t * screen1 = lv_xml_create(NULL, "screen1", NULL);
    lv_screen_load(screen1);

.. _xml_events_set_subject_value:

Set Subject Value
*****************

It's possible to set a :ref:`Subject <observer_subject>` value on user interaction by adding a special child to any widget:

.. code-block:: xml

    <view>
        <lv_button width="200" height="100">
            <subject_set_int_event    trigger="clicked" subject="subject_int"    value="10"/>
            <subject_set_float_event  trigger="clicked" subject="subject_float"  value="12.34"/>
            <subject_set_string_event trigger="clicked" subject="subject_string" value="Hello"/>
            <lv_label text="Set the values"/>
        </lv_button>
    </view>

The usage is straightforward: the specified ``subject`` will be set to the given ``value`` when the ``trigger`` occurs.


.. _xml_events_increment_subject_value:

Increment Subject Value
***********************

Incrementing or decrementing a :ref:`Subject <observer_subject>` value can be defined as follows:

.. code-block:: xml

    <view>
        <lv_button width="200" height="100">
            <subject_increment_event trigger="clicked" subject="subject_int1" step="10"/>
            <subject_increment_event trigger="clicked" subject="subject_int2" step="-10" min_value="0" max_value="50"/>
            <subject_increment_event trigger="clicked" subject="subject_float1" step="2"/>
        </lv_button>
    </view>

The ``<subject_increment_event>`` element defines a ``step`` to be added to the subject's current value
when the ``trigger`` occurs.

``subject`` must be an ``int`` or ``float`` subject.

If ``step`` is **negative**, the subject's value will be decremented.
Only integer ``step`` values are supported now.

Optionally, ``min_value`` and/or ``max_value`` can be set to limit the subject's value.
The default min/max values are ``INT32_MIN`` (-2B) and ``INT32_MAX`` (+2B) respectively.

``rollover`` is also an optional property. If it's ``false`` (default) stop at the
min/max value, if ``true`` jump to the other end.
