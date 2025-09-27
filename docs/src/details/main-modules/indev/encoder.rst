.. _indev_encoder:

=======
Encoder
=======



Overview
********

An encoder is a device with a pressable turning knob that tells the hosting device:

- how much the knob was turned
- in which direction
- and the state of the button (pressed or released)

By using :ref:`Groups <indev_groups>`, even complex UIs can be navigated by a single
encoder.

In short, encoder input devices work like this:

1. By turning the encoder, you can focus on the next or previous object.
2. When you press and release the encoder on a simple "non-editable" object (like a
   button), it will be clicked.
3. If you press the encoder on a complex "editable" object (like a list, message box,
   etc.), the widget will go into *Edit mode*, allowing you to navigate inside the
   object by turning the encoder.
4. To leave edit mode, long-press the button.

This also means that the encoder will focus on and edit only the widgets added to a
:ref:`group <indev_groups>`.



Example
*******

This is an example to initialize an encoder:

.. code-block:: c

   /*Create a group and add widgets to it so they can be selected by the encoder*/
   lv_group_t * g = lv_group_create();

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
   lv_indev_set_read_cb(indev, encoder_read);

   ...

   void encoder_read(lv_indev_t * indev, lv_indev_data_t * data){
     data->enc_diff = enc_get_new_moves();  /* Negative for left, positive for right turns, 0 for no change */

     if(enc_pressed()) data->state = LV_INDEV_STATE_PRESSED;
     else data->state = LV_INDEV_STATE_RELEASED;
   }



Styles
******

When a widget receives focus either by clicking it via touchpad or by navigating to
it with an encoder or keypad, it enters the :cpp:enumerator:`LV_STATE_FOCUSED` state.

If a widget switches to edit mode, it enters the
:cpp:expr:`LV_STATE_FOCUSED | LV_STATE_EDITED` states, so any style properties
assigned to these states will be shown.



Buttons as Encoder
******************

In addition to standard encoder behavior, you can also utilize its logic to navigate
(focus) and edit widgets using buttons. This is especially handy if you have only a
few buttons available, or you want to use other buttons in addition to an encoder
wheel.

You need to have 3 buttons available:

- :cpp:enumerator:`LV_KEY_ENTER`: simulates pressing the encoder button.
- :cpp:enumerator:`LV_KEY_LEFT`: simulates turning the encoder left.
- :cpp:enumerator:`LV_KEY_RIGHT`: simulates turning the encoder right.
- Other keys will be passed to the focused widget as they are.

If you hold the keys, it will simulate encoder movement with the period specified in
``indev.long_press_repeat_time``.

In this case, in the ``read_cb``, set ``data->key`` to the pressed key.

.. code-block:: c

   /*Create a group and add widgets to it so they can be selected by the encoder*/
   lv_group_t * g = lv_group_create();

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
   lv_indev_set_read_cb(indev, encoder_with_keys_read);
   lv_indev_set_group(indev, g);

   ...

   void encoder_with_keys_read(lv_indev_t * indev, lv_indev_data_t * data){
     if(key_pressed()) {
         /* Get the last pressed or released key
          * use LV_KEY_ENTER for encoder press */
         data->key = my_last_key();
         data->state = LV_INDEV_STATE_PRESSED;
     } else {
         data->state = LV_INDEV_STATE_RELEASED;
     }
   }
