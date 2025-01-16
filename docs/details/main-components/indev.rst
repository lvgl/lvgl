.. _indev:

=======================
Input Device (lv_indev)
=======================


.. _indev_creation:

Creating an Input Device
************************

To create an input device on the :ref:`default_display`:

.. code-block:: c

    /* Create and set up at least one display before you register any input devices. */
    lv_indev_t * indev = lv_indev_create();        /* Create input device connected to Default Display. */
    lv_indev_set_type(indev, LV_INDEV_TYPE_...);   /* Touch pad is a pointer-like device. */
    lv_indev_set_read_cb(indev, my_input_read);    /* Set driver function. */

If you have multiple displays, you will need to ensure the Default Display is set
to the display your input device is "connected to" before making the above calls.

The ``type`` member can be:

- :cpp:enumerator:`LV_INDEV_TYPE_POINTER`: touchpad or mouse
- :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`: keyboard or keypad
- :cpp:enumerator:`LV_INDEV_TYPE_ENCODER`: encoder with left/right turn and push options
- :cpp:enumerator:`LV_INDEV_TYPE_BUTTON`: external buttons virtually pressing the screen

``my_input_read`` is a function pointer which will be called periodically to
report the current state of an input device to LVGL.



Touchpad, Touch-Screen, Mouse or Any Pointer
--------------------------------------------

Input devices that can click points on the display belong to the POINTER
category.  Here is an example of a simple input-device Read Callback function:

.. code-block:: c

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    ...

    void my_input_read(lv_indev_t * indev, lv_indev_data_t * data)
    {
        if(touchpad_pressed) {
            data->point.x = touchpad_x;
            data->point.y = touchpad_y;
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }


.. _indev_cursor:

Mouse Cursor
~~~~~~~~~~~~

Pointer input devices (like a mouse) can have a cursor.

.. code-block:: c

   ...
   lv_indev_t * mouse_indev = lv_indev_create();
   ...
   LV_IMAGE_DECLARE(mouse_cursor_icon);                          /* Declare the image source. */
   lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());  /* Create image Widget for cursor. */
   lv_image_set_src(cursor_obj, &mouse_cursor_icon);             /* Set image source. */
   lv_indev_set_cursor(mouse_indev, cursor_obj);                 /* Connect image to Input Device. */

Note that the cursor object should have
:cpp:expr:`lv_obj_remove_flag(cursor_obj, LV_OBJ_FLAG_CLICKABLE)`.
For images, *clicking* is disabled by default.


.. _indev_gestures:

Gestures
~~~~~~~~

Pointer input devices can detect basic gestures.  By default, most Widgets send
gestures to their parents so they can be detected on the Screen Widget in the
form of an :cpp:enumerator:`LV_EVENT_GESTURE` event.  For example:

.. code-block:: c

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

To prevent passing the gesture event to the parent from a Widget, use
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_GESTURE_BUBBLE)`.

Note that, gestures are not triggered if a Widget is being scrolled.

If you did some action on a gesture you can call
:cpp:expr:`lv_indev_wait_release(lv_indev_active())` in the event handler to
prevent LVGL sending further input-device-related events.

.. _indev_crown:

Crown Behavior
~~~~~~~~~~~~~~

A "Crown" is a rotary device typically found on smart watches.

When the user clicks somewhere and after that turns the rotary
the last clicked widget will be either scrolled or it's value will be incremented/decremented
(e.g. in case of a slider).

As this behavior is tightly related to the last clicked widget, the crown support is
an extension of the pointer input device.  Just set ``data->diff`` to the number of
turned steps and LVGL will automatically send the :cpp:enumerator:`LV_EVENT_ROTARY`
event or scroll the widget based on the ``editable`` flag in the widget's class.
Non-editable widgets are scrolled and for editable widgets the event is sent.

To get the steps in an event callback use ``int32_t diff = lv_event_get_rotary_diff(e)``

The rotary sensitivity can be adjusted on 2 levels:

1.  in the input device by the ``indev->rotary_sensitivity`` element (1/256 unit), and
2.  by the ``rotary_sensitivity`` style property in the widget (1/256 unit).

The final diff is calculated like this:

``diff_final = diff_in * (indev_sensitivity / 256) +  (widget_sensitivity / 256);``


For example, if both the indev and widget sensitivity is set to 128 (0.5), the input
diff will be multiplied by 0.25.  The value of the Widget will be incremented by that
value or the Widget will be scrolled that amount of pixels.

Multi-touch gestures
====================

LVGL has the ability to recognize multi-touch gestures, when a gesture
is detected a ``LV_EVENT_GESTURE`` is passed to the object on which the
gesture occurred. Currently, only the pinch gesture is supported
more gesture types will be implemented soon.

To enable the multi-touch gesture recognition set the
``LV_USE_GESTURE_RECOGNITION`` option in the ``lv_conf.h`` file.

Touch event collection
~~~~~~~~~~~~~~~~~~~~~~

The driver or application collects touch events until the indev read callback
is called. It is the responsibility of the driver to call
the gesture recognition function of the appropriate type. For example
to recognise pinch gestures call ``lv_indev_gesture_detect_pinch``.

After calling the gesture detection function, it's necessary to call
the ``lv_indev_set_gesture_data`` function to set the ``gesture_data``
and ``gesture_type`` fields of the structure ``lv_indev_data_t``

.. code-block::

   /* The recognizer keeps the state of the gesture */
   static lv_indev_gesture_recognizer_t recognizer;

   /* An array that stores the collected touch events */
   static lv_indev_touch_data_t touches[10];

   /* A counter that needs to be incremented each time a touch event is received */
   static uint8_t touch_cnt;

   static void touch_read_callback(lv_indev_t * drv, lv_indev_data_t * data)
   {

        lv_indev_touch_data_t * touch;
        uint8_t i;


        touch = &touches[0];
        lv_indev_gesture_detect_pinch(recognizer, &touches[0],
                                      touch_cnt);

        touch_cnt = 0;

        /* Set the gesture information, before returning to LVGL */
        lv_indev_set_gesture_data(data, recognizer);

   }

A touch event is represented by the ``lv_indev_touch_data_t`` structure, the fields
being 1:1 compatible with events emitted by the `libinput <https://wayland.freedesktop.org/libinput/doc/latest/>`_ library

Handling touch events
~~~~~~~~~~~~~~~~~~~~~

Touch events are handled like any other event. First, setup a listener for the ``LV_EVENT_GESTURE`` event type by defining and setting the callback function.

The state or scale of the pinch gesture can be retrieved by
calling the ``lv_event_get_pinch_scale`` and ``lv_indev_get_gesture_state`` from within the 
callback.

An example of such an application is available in
the source tree ``examples/others/gestures/lv_example_gestures.c``

Keypad or Keyboard
------------------

Full keyboards with all the letters or simple keypads with a few navigation buttons
belong in the keypad category.

You can fully control the user interface without a touchpad or mouse by using a
keypad or encoder.  It works similar to the *TAB* key on the PC to select an element
in an application or web page.

To use a keyboard or keypad:

- Register a Read Callback function for your device and set its type to
  :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`.
- Create a Widget Group (``lv_group_t * g = lv_group_create()``) and add Widgets to
  it with :cpp:expr:`lv_group_add_obj(g, widget)`.
- Assign the group to an input device: :cpp:expr:`lv_indev_set_group(indev, g)`.
- Use ``LV_KEY_...`` to navigate among the Widgets in the group.  See
  ``lv_core/lv_group.h`` for the available keys.

.. code-block:: c

   lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);

   ...

   void keyboard_read(lv_indev_t * indev, lv_indev_data_t * data){
     data->key = last_key();            /* Get the last pressed or released key */

     if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else data->state = LV_INDEV_STATE_RELEASED;
   }


Encoder
-------

A common example of an encoder is a device with a turning knob that tells the hosting
device *when* the knob is being turned, and *in which direction*.

With an encoder your application can receive events from the following:

1.  press of its button,
2.  oong-press of its button,
3.  turn left, and
4.  turn right.

In short, the Encoder input devices work like this:

- By turning the encoder you can focus on the next/previous object.
- When you press the encoder on a simple object (like a button), it will be clicked.
- If you press the encoder on a complex object (like a list, message box, etc.)
  the Widget will go to edit mode whereby you can navigate inside the
  object by turning the encoder.
- To leave edit mode, long press the button.

To use an Encoder (similar to the *Keypads*) the Widgets should be added to a group.

.. code-block:: c

   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);

   ...

   void encoder_read(lv_indev_t * indev, lv_indev_data_t * data){
     data->enc_diff = enc_get_new_moves();

     if(enc_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else data->state = LV_INDEV_STATE_RELEASED;
   }


.. _indev_groups:

Widget Groups
-------------
When input focus needs to be managed among a set of Widgets (e.g. to capture user
input from a keypad or encoder), that set of Widgets is placed in a group which
thereafter manages how input focus moves from Widget to Widget.

In each group there is exactly one object with focus which receives the pressed keys
or the encoder actions.  For example, if a :ref:`Text Area <lv_textarea>` has focus
and you press some letter on a keyboard, the keys will be sent and inserted into the
text area.  Similarly, if a :ref:`Slider <lv_slider>` has focus and you press the
left or right arrows, the slider's value will be changed.

You need to associate an input device with a group.  An input device can
send key events to only one group but a group can receive data from more
than one input device.

To create a group use :cpp:expr:`lv_group_t * g = lv_group_create()` and to add
a Widget to the group use :cpp:expr:`lv_group_add_obj(g, widget)`.

Once a Widget has been added to a group, you can find out what group it is in
using :cpp:expr:`lv_obj_get_group(widget)`.

To find out if a Widget in a group has focus, call :cpp:expr:`lv_obj_is_focused(widget)`.
If the Widget is not part of a group, this function will return ``false``.

To associate a group with an input device use :cpp:expr:`lv_indev_set_group(indev, g)`.



.. _indev_keys:

Keys
~~~~

There are some predefined keys which have special meaning:

- :cpp:enumerator:`LV_KEY_NEXT`: Move focus to next object
- :cpp:enumerator:`LV_KEY_PREV`: Move focus to previous object
- :cpp:enumerator:`LV_KEY_ENTER`: Triggers :cpp:enumerator:`LV_EVENT_PRESSED`, :cpp:enumerator:`LV_EVENT_CLICKED`, or :cpp:enumerator:`LV_EVENT_LONG_PRESSED` etc. events
- :cpp:enumerator:`LV_KEY_UP`: Increase value or move up
- :cpp:enumerator:`LV_KEY_DOWN`: Decrease value or move down
- :cpp:enumerator:`LV_KEY_RIGHT`: Increase value or move to the right
- :cpp:enumerator:`LV_KEY_LEFT`: Decrease value or move to the left
- :cpp:enumerator:`LV_KEY_ESC`: Close or exit (e.g. close a :ref:`Drop-Down List <lv_dropdown>`)
- :cpp:enumerator:`LV_KEY_DEL`: Delete (e.g. a character on the right in a :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_BACKSPACE`: Delete (e.g. a character on the left in a :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_HOME`: Go to the beginning/top (e.g. in a :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_END`: Go to the end (e.g. in a :ref:`Text Area <lv_textarea>`)

The most important special keys in your :cpp:func:`read_cb` function are:

- :cpp:enumerator:`LV_KEY_NEXT`
- :cpp:enumerator:`LV_KEY_PREV`
- :cpp:enumerator:`LV_KEY_ENTER`
- :cpp:enumerator:`LV_KEY_UP`
- :cpp:enumerator:`LV_KEY_DOWN`
- :cpp:enumerator:`LV_KEY_LEFT`
- :cpp:enumerator:`LV_KEY_RIGHT`

You should translate some of your keys to these special keys to support navigation
in a group and interact with selected Widgets.

Usually, it's enough to use only :cpp:enumerator:`LV_KEY_LEFT` and :cpp:enumerator:`LV_KEY_RIGHT` because most
Widgets can be fully controlled with them.

With an encoder you should use only :cpp:enumerator:`LV_KEY_LEFT`, :cpp:enumerator:`LV_KEY_RIGHT`,
and :cpp:enumerator:`LV_KEY_ENTER`.

Edit and Navigate Mode
~~~~~~~~~~~~~~~~~~~~~~

Since a keypad has plenty of keys, it's easy to navigate between Widgets
and edit them using the keypad. But encoders have a limited number of
"keys" and hence it is difficult to navigate using the default options.
*Navigate* and *Edit* modes are used to avoid this problem with
encoders.

In *Navigate* mode, an encoder's :cpp:enumerator:`LV_KEY_LEFT` or :cpp:enumerator:`LV_KEY_RIGHT` is translated to
:cpp:enumerator:`LV_KEY_NEXT` or :cpp:enumerator:`LV_KEY_PREV`. Therefore, the next or previous object will be
selected by turning the encoder. Pressing :cpp:enumerator:`LV_KEY_ENTER` will change
to *Edit* mode.

In *Edit* mode, :cpp:enumerator:`LV_KEY_NEXT` and :cpp:enumerator:`LV_KEY_PREV` is usually used to modify an
object. Depending on the Widget's type, a short or long press of
:cpp:enumerator:`LV_KEY_ENTER` changes back to *Navigate* mode. Usually, a Widget
which cannot be pressed (like a :ref:`Slider <lv_slider>`) leaves
*Edit* mode upon a short click. But with Widgets where a short click has
meaning (e.g. :ref:`Button <lv_button>`), a long press is required.

Default Group
~~~~~~~~~~~~~

Interactive widgets (such as Buttons, Checkboxes, Sliders, etc.) can
be automatically added to a default group. Just create a group with
:cpp:expr:`lv_group_t * g = lv_group_create()` and set the default group with
:cpp:expr:`lv_group_set_default(g)`

Don't forget to assign one or more input devices to the default group
with :cpp:expr:`lv_indev_set_group(my_indev, g)`.

Styling
-------

When a Widget receives focus either by clicking it via touchpad or by navigating to
it with an encoder or keypad, it goes to the :cpp:enumerator:`LV_STATE_FOCUSED`
state.  Hence, focused styles will be applied to it.

If a Widget switches to edit mode it enters the
:cpp:expr:`LV_STATE_FOCUSED | LV_STATE_EDITED` states so any style properties
assigned to these states will be shown.

See :ref:`styles` for more details.




Using Buttons with Encoder Logic
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to standard encoder behavior, you can also utilize its logic
to navigate(focus) and edit widgets using buttons. This is especially
handy if you have only few buttons available, or you want to use other
buttons in addition to an encoder wheel.

You need to have 3 buttons available:

- :cpp:enumerator:`LV_KEY_ENTER`: will simulate press or pushing of the encoder button.
- :cpp:enumerator:`LV_KEY_LEFT`: will simulate turning encoder left.
- :cpp:enumerator:`LV_KEY_RIGHT`: will simulate turning encoder right.
- other keys will be passed to the focused widget.

If you hold the keys it will simulate an encoder advance with period
specified in ``indev_drv.long_press_repeat_time``.

.. code-block:: c


   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);

   ...

   void encoder_with_keys_read(lv_indev_t * indev, lv_indev_data_t * data){
     data->key = last_key();            /* Get the last pressed or released key */
                                        /* use LV_KEY_ENTER for encoder press */
     if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else {
         data->state = LV_INDEV_STATE_RELEASED;
         /* Optionally you can also use enc_diff, if you have encoder */
         data->enc_diff = enc_get_new_moves();
     }
   }


Hardware Button
---------------

A *Hardware Button* here is an external button (switch) typically next to the screen
which is assigned to specific coordinates of the screen.  If a button is pressed it
will simulate the pressing on the assigned coordinate, similar to a touchpad.

To assign Hardware Buttons to coordinates use ``lv_indev_set_button_points(my_indev,
points_array)``. ``points_array`` should look like ``const lv_point_t points_array[]
= { {12,30}, {60,90}, ...}``

.. admonition::  Important:

    ``points_array`` cannot be allowed to go out of scope.  Either declare it as a
    global variable or as a static variable inside a function.

.. code-block:: c

   lv_indev_set_type(indev, LV_INDEV_TYPE_BUTTON);

   ...

   void button_read(lv_indev_t * indev, lv_indev_data_t * data){
       static uint32_t last_btn = 0;   /* Store the last pressed button */
       int btn_pr = my_btn_read();     /* Get the ID (0,1,2...) of the pressed button */
       if(btn_pr >= 0) {               /* Is there a button press? (E.g. -1 indicated no button was pressed) */
          last_btn = btn_pr;           /* Save the ID of the pressed button */
          data->state = LV_INDEV_STATE_PRESSED;  /* Set the pressed state */
       } else {
          data->state = LV_INDEV_STATE_RELEASED; /* Set the released state */
       }

       data->btn_id = last_btn;         /* Save the last button */
   }

When the ``button_read`` callback in the example above changes the ``data->btn_id`` to ``0``
a press/release action at the first index of the ``points_array`` will be performed (``{12,30}``).


.. _indev_other_features:

Other Features
**************

Parameters
----------

The default value of the following parameters can be changed in :cpp:type:`lv_indev_t`:

- ``scroll_limit`` Number of pixels to slide before actually scrolling the Widget
- ``scroll_throw`` Scroll throw (momentum) slow-down in [%]. Greater value means faster slow-down.
- ``long_press_time`` Press time to send :cpp:enumerator:`LV_EVENT_LONG_PRESSED` (in milliseconds)
- ``long_press_repeat_time`` Interval of sending :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT` (in milliseconds)
- ``read_timer`` pointer to the ``lv_timer`` which reads the input device. Its parameters
  can be changed by calling ``lv_timer_...()`` functions. :c:macro:`LV_DEF_REFR_PERIOD`
  in ``lv_conf.h`` sets the default read period.

Feedback
--------

Besides ``read_cb`` a ``feedback_cb`` callback can be also specified in
:cpp:type:`lv_indev_t`. ``feedback_cb`` is called when any type of event is sent
by input devices (independently of their type).  This allows generating
feedback for the user, e.g. to play a sound on :cpp:enumerator:`LV_EVENT_CLICKED`.

Buffered Reading
----------------

By default, LVGL calls ``read_cb`` periodically. Because of this
intermittent polling there is a chance that some user gestures are
missed.

To solve this you can write an event driven driver for your input device
that buffers measured data. In ``read_cb`` you can report the buffered
data instead of directly reading the input device. Setting the
``data->continue_reading`` flag will tell LVGL there is more data to
read and it should call ``read_cb`` again.

Switching the Input Device to Event-Driven Mode
-----------------------------------------------

Normally an Input Device is read every :c:macro:`LV_DEF_REFR_PERIOD`
milliseconds (set in ``lv_conf.h``).  However, in some cases, you might
need more control over when to read the input device. For example, you
might need to read it by polling a file descriptor (fd).

You can do this by:

.. code-block:: c

   /* Update the input device's running mode to LV_INDEV_MODE_EVENT */
   lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);

   ...

   /* Call this anywhere you want to read the input device */
   lv_indev_read(indev);

.. note:: :cpp:func:`lv_indev_read`, :cpp:func:`lv_timer_handler` and :cpp:func:`_lv_display_refr_timer` cannot run at the same time.

.. note:: For devices in event-driven mode, `data->continue_reading` is ignored.


.. admonition::  Further Reading

    - `lv_port_indev_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_indev_template.c>`__
      for a template for your own Input-Device driver.



.. _indev_api:

API
***
