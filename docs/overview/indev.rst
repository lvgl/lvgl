.. _indev:

=============
Input devices
=============

An input device usually means:

- Pointer-like input device like touchpad or mouse
- Keypads like a normal keyboard or simple numeric keypad
- Encoders with left/right turn and push options
- External hardware buttons which are assigned to specific points on the screen

:important: Before reading further, please read the `Porting </porting/indev>`__ section of Input devices

Pointers
********

.. _indev_cursor:

Cursor
------

Pointer input devices (like a mouse) can have a cursor.

.. code:: c

   ...
   lv_indev_t * mouse_indev = lv_indev_create();
   ...
   LV_IMAGE_DECLARE(mouse_cursor_icon);                          /*Declare the image source.*/
   lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());      /*Create an image object for the cursor */
   lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
   lv_indev_set_cursor(mouse_indev, cursor_obj);               /*Connect the image  object to the driver*/

Note that the cursor object should have
:cpp:expr:`lv_obj_remove_flag(cursor_obj, LV_OBJ_FLAG_CLICKABLE)`. For images,
*clicking* is disabled by default.

.. _indev_gestures:

Gestures
--------

Pointer input devices can detect basic gestures. By default, most of the
widgets send the gestures to its parent, so finally the gestures can be
detected on the screen object in a form of an :cpp:enumerator:`LV_EVENT_GESTURE`
event. For example:

.. code:: c

   void my_event(lv_event_t * e)
   {
     lv_obj_t * screen = lv_event_get_current_target(e);
     lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
     switch(dir) {
       case LV_DIR_LEFT:
         ...
         break;
       case LV_DIR_RIGHT:
         ...
         break;
       case LV_DIR_TOP:
         ...
         break;
       case LV_DIR_BOTTOM:
         ...
         break;
     }
   }

   ...

   lv_obj_add_event_cb(screen1, my_event, LV_EVENT_GESTURE, NULL);

To prevent passing the gesture event to the parent from an object use
:cpp:expr:`lv_obj_remove_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE)`.

Note that, gestures are not triggered if an object is being scrolled.

If you did some action on a gesture you can call
:cpp:expr:`lv_indev_wait_release(lv_indev_active())` in the event handler to
prevent LVGL sending further input device related events.

.. _indev_keypad_and_encoder:

Keypad and encoder
******************

You can fully control the user interface without a touchpad or mouse by
using a keypad or encoder(s). It works similar to the *TAB* key on the
PC to select an element in an application or a web page.

.. _indev_groups:

Groups
------

Objects you want to control with a keypad or encoder need to be added to
a *Group*. In every group there is exactly one focused object which
receives the pressed keys or the encoder actions. For example, if a
:ref:`Text area <lv_textarea>` is focused and you press some letter
on a keyboard, the keys will be sent and inserted into the text area.
Similarly, if a :ref:`Slider <lv_slider>` is focused and you press
the left or right arrows, the slider's value will be changed.

You need to associate an input device with a group. An input device can
send key events to only one group but a group can receive data from more
than one input device.

To create a group use :cpp:expr:`lv_group_t * g = lv_group_create()` and to add
an object to the group use :cpp:expr:`lv_group_add_obj(g, obj)`.

To associate a group with an input device use
:cpp:expr:`lv_indev_set_group(indev, g)`.

.. _indev_keys:

Keys
^^^^

There are some predefined keys which have special meaning:

- :cpp:enumerator:`LV_KEY_NEXT`: Focus on the next object
- :cpp:enumerator:`LV_KEY_PREV`: Focus on the previous object
- :cpp:enumerator:`LV_KEY_ENTER`: Triggers :cpp:enumerator:`LV_EVENT_PRESSED`, :cpp:enumerator:`LV_EVENT_CLICKED`, or :cpp:enumerator:`LV_EVENT_LONG_PRESSED` etc. events
- :cpp:enumerator:`LV_KEY_UP`: Increase value or move upwards
- :cpp:enumerator:`LV_KEY_DOWN`: Decrease value or move downwards
- :cpp:enumerator:`LV_KEY_RIGHT`: Increase value or move to the right
- :cpp:enumerator:`LV_KEY_LEFT`: Decrease value or move to the left
- :cpp:enumerator:`LV_KEY_ESC`: Close or exit (E.g. close a :ref:`Drop down list <lv_dropdown>`)
- :cpp:enumerator:`LV_KEY_DEL`: Delete (E.g. a character on the right in a :ref:`Text area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_BACKSPACE`: Delete a character on the left (E.g. in a :ref:`Text area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_HOME`: Go to the beginning/top (E.g. in a :ref:`Text area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_END`: Go to the end (E.g. in a :ref:`Text area <lv_textarea>`)

The most important special keys in your :cpp:func:`read_cb` function are:

- :cpp:enumerator:`LV_KEY_NEXT`
- :cpp:enumerator:`LV_KEY_PREV`
- :cpp:enumerator:`LV_KEY_ENTER`
- :cpp:enumerator:`LV_KEY_UP`
- :cpp:enumerator:`LV_KEY_DOWN`
- :cpp:enumerator:`LV_KEY_LEFT`
- :cpp:enumerator:`LV_KEY_RIGHT`

You should translate some of your keys to these special keys to support navigation
in a group and interact with selected objects.

Usually, it's enough to use only :cpp:enumerator:`LV_KEY_LEFT` and :cpp:enumerator:`LV_KEY_RIGHT` because most
objects can be fully controlled with them.

With an encoder you should use only :cpp:enumerator:`LV_KEY_LEFT`, :cpp:enumerator:`LV_KEY_RIGHT`,
and :cpp:enumerator:`LV_KEY_ENTER`.

Edit and navigate mode
^^^^^^^^^^^^^^^^^^^^^^

Since a keypad has plenty of keys, it's easy to navigate between objects
and edit them using the keypad. But encoders have a limited number of
"keys" and hence it is difficult to navigate using the default options.
*Navigate* and *Edit* modes are used to avoid this problem with
encoders.

In *Navigate* mode, an encoder's :cpp:enumerator:`LV_KEY_LEFT` or :cpp:enumerator:`LV_KEY_RIGHT` is translated to
:cpp:enumerator:`LV_KEY_NEXT` or :cpp:enumerator:`LV_KEY_PREV`. Therefore, the next or previous object will be
selected by turning the encoder. Pressing :cpp:enumerator:`LV_KEY_ENTER` will change
to *Edit* mode.

In *Edit* mode, :cpp:enumerator:`LV_KEY_NEXT` and :cpp:enumerator:`LV_KEY_PREV` is usually used to modify an
object. Depending on the object's type, a short or long press of
:cpp:enumerator:`LV_KEY_ENTER` changes back to *Navigate* mode. Usually, an object
which cannot be pressed (like a :ref:`Slider <lv_slider>`) leaves
*Edit* mode upon a short click. But with objects where a short click has
meaning (e.g. :ref:`Button <lv_button>`), a long press is required.

Default group
^^^^^^^^^^^^^

Interactive widgets - such as buttons, checkboxes, sliders, etc. - can
be automatically added to a default group. Just create a group with
:cpp:expr:`lv_group_t * g = lv_group_create()` and set the default group with
:cpp:expr:`lv_group_set_default(g)`

Don't forget to assign one or more input devices to the default group
with :cpp:expr:`lv_indev_set_group(my_indev, g)`.

Styling
-------

If an object is focused either by clicking it via touchpad or focused
via an encoder or keypad it goes to the :cpp:enumerator:`LV_STATE_FOCUSED` state.
Hence, focused styles will be applied to it.

If an object switches to edit mode it enters the
:cpp:expr:`LV_STATE_FOCUSED | LV_STATE_EDITED` states so these style properties
will be shown.

For a more detailed description read the
`Style <https://docs.lvgl.io/master/overview/style.html>`__ section.


.. _indev_api:

API
***
