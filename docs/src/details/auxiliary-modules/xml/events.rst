.. _xml_events:

======
Events
======

Overview
********

``<lv_event>`` tags can be added as a child of any Widget to react to user inputs or other value changes.

Right now, only a single event type is supported to call user-defined callbacks.

Usage
*****

Call function
-------------

User-defined functions can be called like this:

.. code-block:: xml

    <view>
      <lv_button width="200" height="100">
        <lv_event-call_function callback="my_callback_1" trigger="clicked" user_data="some_text"/>
        <lv_label text="Hello"/>
      </lv_button>
    </view>

When the XML is loaded at runtime, the callback name needs to be mapped to a function by using:
``lv_xml_register_event_cb("my_callback_1", an_event_handler)``

The callback should have the standard LVGL event callback signature:
``void an_event_handler(lv_event_t * e);``

In the exported C code, it is assumed that there is a function with the exact name specified as the callback name.
For example, ``callback="my_callback_1"`` will be exported as:

.. code-block:: c

    void my_callback_1(lv_event_t * e); /* At the beginning of the exported file */

    lv_obj_add_event_cb(obj, my_callback_1, LV_EVENT_CLICKED, "some_text");

For triggers, all LVGL event types are supported with straightforward mapping:

- :cpp:expr:`LV_EVENT_ALL`: ``"all"``
- :cpp:expr:`LV_EVENT_CLICKED`: ``"clicked"``
- :cpp:expr:`LV_EVENT_PRESSED`: ``"pressed"``
- etc.

The ``user_data`` is optional. If omitted, ``NULL`` will be passed.
