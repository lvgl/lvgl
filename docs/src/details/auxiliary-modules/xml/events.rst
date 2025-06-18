.. _xml_events:

======
Events
======

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

Call function
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
:cpp:expr:`lv_xml_register_event_cb("my_callback_1", an_event_handler);`.

The callback should follow the standard LVGL event callback signature:
``void an_event_handler(lv_event_t * e);``

In the exported C code, it is assumed that a function with the exact name exists.
For example, ``callback="my_callback_1"`` will be exported as:

.. code-block:: c

    void my_callback_1(lv_event_t * e); /* At the beginning of the exported file */

    lv_obj_add_event_cb(obj, my_callback_1, LV_EVENT_CLICKED, "some_text");

The ``user_data`` is optional. If omitted, ``NULL`` will be passed.

Set subject value
*****************

It's possible to set a :ref:`Subject <observer_subject>` value on user interaction by adding a special child to any widget:

.. code-block:: xml

    <view>
        <lv_button width="200" height="100">
            <subject_set_int trigger="clicked" subject="subject1" value="10"/>
            <subject_set_string trigger="clicked" subject="subject2" value="Hello"/>
            <lv_label text="Set to 10"/>
        </lv_button>
    </view>

The usage is straightforward: the specified ``subject`` will be set to the given ``value`` when the ``trigger`` occurs.

Increment subject value
***********************

Incrementing or decrementing a :ref:`Subject <observer_subject>` value can be defined as follows:

.. code-block:: xml

    <view>
        <lv_button width="200" height="100">
            <subject_increment trigger="clicked" subject="subject1" step="10"/>
            <subject_increment trigger="clicked" subject="subject2" step="-10" min="0" max="50"/>
        </lv_button>
    </view>

The ``<subject_increment>`` element defines a ``step`` to be added to the subject's current value when the ``trigger`` occurs.
Optionally, ``min`` and/or ``max`` can be set to limit the subject's value.

If ``step`` is **negative**, the subject's value will be decremented.

**Note:** Only integer subjects are supported by ``<subject_increment>``.
