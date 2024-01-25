.. _porting_indev:

======================
Input device interface
======================

Types of input devices
**********************

To create an input device use

.. code:: c

   /*Register at least one display before you register any input devices*/
   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_...);   /*See below.*/
   lv_indev_set_read_cb(indev, read_cb);  /*See below.*/

The ``type`` member can be:

- :cpp:enumerator:`LV_INDEV_TYPE_POINTER`: touchpad or mouse
- :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`: keyboard or keypad
- :cpp:enumerator:`LV_INDEV_TYPE_ENCODER`: encoder with left/right turn and push options
- :cpp:enumerator:`LV_INDEV_TYPE_BUTTON`: external buttons virtually pressing the screen

``read_cb`` is a function pointer which will be called periodically to
report the current state of an input device.

Visit :ref:`Input devices <indev>` to learn more about input
devices in general.

Touchpad, mouse or any pointer
------------------------------

Input devices that can click points on the screen belong to this
category.

.. code:: c

   lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
   ...

   void my_input_read(lv_indev_t * indev, lv_indev_data_t*data)
   {
     if(touchpad_pressed) {
       data->point.x = touchpad_x;
       data->point.y = touchpad_y;
       data->state = LV_INDEV_STATE_PRESSED;
     } else {
       data->state = LV_INDEV_STATE_RELEASED;
     }
   }

To set a mouse cursor use :cpp:expr:`lv_indev_set_cursor(indev, &img_cursor)`.

Keypad or keyboard
------------------

Full keyboards with all the letters or simple keypads with a few
navigation buttons belong here.

To use a keyboard/keypad:

- Register a ``read_cb`` function and use :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD` type.
- An object group has to be created: ``lv_group_t * g = lv_group_create()`` and objects have to be added to
  it with :cpp:expr:`lv_group_add_obj(g, obj)`
- The created group has to be assigned to an input device: :cpp:expr:`lv_indev_set_group(indev, g)`
- Use ``LV_KEY_...`` to navigate among the objects in the group. See
  ``lv_core/lv_group.h`` for the available keys.

.. code:: c


   lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);

   ...

   void keyboard_read(lv_indev_t * indev, lv_indev_data_t*data){
     data->key = last_key();            /*Get the last pressed or released key*/

     if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else data->state = LV_INDEV_STATE_RELEASED;
   }

Encoder
-------

With an encoder you can do the following:

1. Press its button
2. Long-press its button
3. Turn left
4. Turn right

In short, the Encoder input devices work like this:

- By turning the encoder you can focus on the next/previous object.
- When you press the encoder on a simple object (like a button), it will be clicked.
- If you press the encoder on a complex object (like a list, message box, etc.)
  the object will go to edit mode whereby you can navigate inside the
  object by turning the encoder.
- To leave edit mode, long press the button.

To use an *Encoder* (similarly to the *Keypads*) the objects should be
added to groups.

.. code:: c

   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);

   ...

   void encoder_read(lv_indev_t * indev, lv_indev_data_t*data){
     data->enc_diff = enc_get_new_moves();

     if(enc_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else data->state = LV_INDEV_STATE_RELEASED;
   }

Using buttons with Encoder logic
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In addition to standard encoder behavior, you can also utilize its logic
to navigate(focus) and edit widgets using buttons. This is especially
handy if you have only few buttons available, or you want to use other
buttons in addition to encoder wheel.

You need to have 3 buttons available:

- :cpp:enumerator:`LV_KEY_ENTER`: will simulate press or pushing of the encoder button
- :cpp:enumerator:`LV_KEY_LEFT`: will simulate turning encoder left
- :cpp:enumerator:`LV_KEY_RIGHT`: will simulate turning encoder right
- other keys will be passed to the focused widget

If you hold the keys it will simulate an encoder advance with period
specified in ``indev_drv.long_press_repeat_time``.

.. code:: c


   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);

   ...

   void encoder_with_keys_read(lv_indev_t * indev, lv_indev_data_t*data){
     data->key = last_key();            /*Get the last pressed or released key*/
                                        /* use LV_KEY_ENTER for encoder press */
     if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else {
         data->state = LV_INDEV_STATE_RELEASED;
         /* Optionally you can also use enc_diff, if you have encoder*/
         data->enc_diff = enc_get_new_moves();
     }
   }

Button
------

*Buttons* mean external "hardware" buttons next to the screen which are
assigned to specific coordinates of the screen. If a button is pressed
it will simulate the pressing on the assigned coordinate. (Similarly to a touchpad)

To assign buttons to coordinates use ``lv_indev_set_button_points(my_indev, points_array)``. ``points_array``
should look like ``const lv_point_t points_array[] = { {12,30},{60,90}, ...}``

:important: The points_array can't go out of scope. Either declare it as a global variable
            or as a static variable inside a function.`

.. code:: c


   lv_indev_set_type(indev, LV_INDEV_TYPE_BUTTON);

   ...

   void button_read(lv_indev_t * indev, lv_indev_data_t*data){
       static uint32_t last_btn = 0;   /*Store the last pressed button*/
       int btn_pr = my_btn_read();     /*Get the ID (0,1,2...) of the pressed button*/
       if(btn_pr >= 0) {               /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
          last_btn = btn_pr;           /*Save the ID of the pressed button*/
          data->state = LV_INDEV_STATE_PRESSED;  /*Set the pressed state*/
       } else {
          data->state = LV_INDEV_STATE_RELEASED; /*Set the released state*/
       }

       data->btn_id = last_btn;         /*Save the last button*/
   }

When the ``button_read`` callback in the example above changes the ``data->btn_id`` to ``0``
a press/release action at the first index of the ``points_array`` will be performed (``{12,30}``).

.. _porting_indev_other_features:

Other features
**************

Parameters
----------

The default value of the following parameters can be changed in :cpp:type:`lv_indev_t`:

- ``scroll_limit`` Number of pixels to slide before actually scrolling the object.
- ``scroll_throw`` Scroll throw (momentum) slow-down in [%]. Greater value means faster slow-down.
- ``long_press_time`` Press time to send :cpp:enumerator:`LV_EVENT_LONG_PRESSED` (in milliseconds)
- ``long_press_repeat_time`` Interval of sending :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT` (in milliseconds)
- ``read_timer`` pointer to the ``lv_timer`` which reads the input device. Its parameters
  can be changed by ``lv_timer_...()`` functions. :c:macro:`LV_DEF_REFR_PERIOD`
  in ``lv_conf.h`` sets the default read period.

Feedback
--------

Besides ``read_cb`` a ``feedback_cb`` callback can be also specified in
:cpp:type:`lv_indev_t`. ``feedback_cb`` is called when any type of event is sent
by the input devices (independently of its type). This allows generating
feedback for the user, e.g. to play a sound on :cpp:enumerator:`LV_EVENT_CLICKED`.

Associating with a display
--------------------------

Every input device is associated with a display. By default, a new input
device is added to the last display created or explicitly selected
(using :cpp:func:`lv_display_set_default`). The associated display is stored and
can be changed in ``disp`` field of the driver.

Buffered reading
----------------

By default, LVGL calls ``read_cb`` periodically. Because of this
intermittent polling there is a chance that some user gestures are
missed.

To solve this you can write an event driven driver for your input device
that buffers measured data. In ``read_cb`` you can report the buffered
data instead of directly reading the input device. Setting the
``data->continue_reading`` flag will tell LVGL there is more data to
read and it should call ``read_cb`` again.

Switching the input device to event-driven mode
-----------------------------------------------

Normally the input event is read every :c:macro:`LV_DEF_REFR_PERIOD`
milliseconds (set in ``lv_conf.h``).  However, in some cases, you might
need more control over when to read the input device. For example, you
might need to read it by polling file descriptor (fd).

You can do this in the following way:

.. code:: c

   /*Update the input device's running mode to LV_INDEV_MODE_EVENT*/
   lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);

   ...

   /*Call this anywhere you want to read the input device*/
   lv_indev_read(indev);

.. note:: that :cpp:func:`lv_indev_read`, :cpp:func:`lv_timer_handler` and :cpp:func:`_lv_display_refr_timer` can not run at the same time.

Further reading
***************

- `lv_port_indev_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_indev_template.c>`__ for a template for your own driver.
- `INdev features <indev>` to learn more about higher level input device features.

API
***
