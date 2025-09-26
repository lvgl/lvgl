.. _indev_groups:

======
Groups
======



Overview
********

Groups are a "group of widgets" that can be focused by :ref:`Keypad <indev_keypad>`
or :ref:`Encoder <indev_encoder>` input devices.

First, a group needs to be created. After that, the group needs to be assigned to an
input device. Finally, widgets need to be added to the group.

In practice, this means when the input device wants to focus the next or previous
widget (:cpp:enumerator:`LV_KEY_NEXT` and :cpp:enumerator:`LV_KEY_PREV`), it will
search for the widget in the assigned group.

Only the selected widgets will be focusable, and the order of receiving focus will be
the order in which the widgets were added.

The focused object will receive the keys and other related events. For example, if a
:ref:`Text Area <lv_textarea>` has focus and you press a letter on a keyboard, the
key will be sent and inserted into the text area (as :cpp:enumerator:`LV_EVENT_KEY`).
Similarly, if a :ref:`Slider <lv_slider>` has focus and you press the left or right
arrows, the slider's value will be changed.



Usage
*****

To create a group, use :cpp:expr:`lv_group_t * g = lv_group_create()`, and to add a
widget to the group, use :cpp:expr:`lv_group_add_obj(g, widget)`.

Once a widget has been added to a group, you can find out what group it belongs to
using :cpp:expr:`lv_obj_get_group(widget)`.

To find out which widget in a group has focus, if any, call
:cpp:expr:`lv_group_get_focused(group)`. If a widget in that group has focus, it will
return a pointer to it; otherwise, it will return NULL.

To associate a group with an input device, use :cpp:expr:`lv_indev_set_group(indev, g)`.



Edit and Navigate Mode
**********************

Since a :ref:`Keypad <indev_keypad>` has plenty of keys, it's easy to navigate between
widgets and edit them using the keypad. But :ref:`Encoders <indev_encoder>` have a
limited number of "keys", making it difficult to navigate and adjust widgets.

*Navigate* and *Edit* modes are used to solve this problem with encoders.

In **Navigate** mode, an encoder's step is translated to
:cpp:enumerator:`LV_KEY_NEXT` or :cpp:enumerator:`LV_KEY_PREV`. Therefore, the next or
previous object will be selected by turning the encoder. Pressing the encoder's button
will switch to *Edit* mode if the widget is "editable".

In *Edit* mode, the encoder's step is usually used to modify the focused widget.
Depending on the widget's type, a short or long press changes back to *Navigate* mode.
Usually, a widget that cannot be pressed (like a :ref:`Slider <lv_slider>`) exits *Edit*
mode upon a short click. But for widgets where a short click has meaning (e.g., a
:ref:`Button <lv_button>`), a long press is required.

Whether a widget is editable is set in the widget's class. See, for example,
``.editable = LV_OBJ_CLASS_EDITABLE_TRUE`` in
`lv_slider.c <https://github.com/lvgl/lvgl/blob/master/src/widgets/slider/lv_slider.c>`__.



Default Group
*************

Interactive widgets (such as Buttons, Checkboxes, Sliders, etc.) can be automatically
added to a default group. Just create a group with
:cpp:expr:`lv_group_t * g = lv_group_create()` and set the default group with
:cpp:expr:`lv_group_set_default(g)`.

Don't forget to assign one or more input devices to the default group using
:cpp:expr:`lv_indev_set_group(my_indev, g)`.



Multiple Groups
***************

Multiple groups can also be created. Imagine an instrument (e.g., a power supply) with
4 channels and "left", "right", and "OK" buttons for each channel to adjust them. On
the display, all 4 channels are shown below each other.

To manage the channels separately:

- Create a group for each channel
- Add the UI elements of each channel to its group
- Create 4 input devices for the 4 channels
- Connect each group to its respective input device

This way, the channels can be managed separately in an elegant manner.
