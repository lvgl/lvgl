.. _indev_keypad:

===================
Keypad and Keyboard
===================



Overview
********

Full keyboards with all the letters or simple keypads with a few navigation buttons
belong in the *Keypad* category.

You can fully control the user interface without a touchpad or mouse by using only a
keypad. It works similarly to the *TAB* key on a PC to select an element in an
application or web page.

Only widgets added to a group can be selected by a keyboard. Learn more at
:ref:`indev_groups`.



Example
*******

.. code-block:: c

   /*Create a group and add widgets to it so they can be selected by the keypad*/
   lv_group_t * g = lv_group_create();

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
   lv_indev_set_read_cb(indev, keyboard_read);
   lv_indev_set_group(indev, g);

   ...

   void keyboard_read(lv_indev_t * indev, lv_indev_data_t * data) {
     if(key_pressed()) {
        data->key = my_last_key();            /* Get the last pressed or released key */
        data->state = LV_INDEV_STATE_PRESSED;
     } else {
        data->state = LV_INDEV_STATE_RELEASED;
     }
   }




.. _indev_keys:

Keys
****

There are some predefined keys which have special meaning:

- :cpp:enumerator:`LV_KEY_NEXT`: Move focus to the next object
- :cpp:enumerator:`LV_KEY_PREV`: Move focus to the previous object
- :cpp:enumerator:`LV_KEY_ENTER`: Triggers :cpp:enumerator:`LV_EVENT_PRESSED`,
  :cpp:enumerator:`LV_EVENT_CLICKED`, or :cpp:enumerator:`LV_EVENT_LONG_PRESSED`
  events
- :cpp:enumerator:`LV_KEY_UP`: Increase value or move up
- :cpp:enumerator:`LV_KEY_DOWN`: Decrease value or move down
- :cpp:enumerator:`LV_KEY_RIGHT`: Increase value or move to the right
- :cpp:enumerator:`LV_KEY_LEFT`: Decrease value or move to the left
- :cpp:enumerator:`LV_KEY_ESC`: Close or exit (e.g., close a :ref:`Drop-Down List
  <lv_dropdown>`)
- :cpp:enumerator:`LV_KEY_DEL`: Delete (e.g., a character on the right in a
  :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_BACKSPACE`: Delete (e.g., a character on the left in a
  :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_HOME`: Go to the beginning/top (e.g., in a
  :ref:`Text Area <lv_textarea>`)
- :cpp:enumerator:`LV_KEY_END`: Go to the end (e.g., in a
  :ref:`Text Area <lv_textarea>`)

The most important special keys in your :cpp:func:`read_cb` function are:

- :cpp:enumerator:`LV_KEY_NEXT`
- :cpp:enumerator:`LV_KEY_PREV`
- :cpp:enumerator:`LV_KEY_ENTER`
- :cpp:enumerator:`LV_KEY_UP`
- :cpp:enumerator:`LV_KEY_DOWN`
- :cpp:enumerator:`LV_KEY_LEFT`
- :cpp:enumerator:`LV_KEY_RIGHT`

You should translate some of the read keys to these special keys to support navigation
in a group and interact with selected widgets.


Key Events
**********

If needed you can easily catch the keys sent by an input device by subscribing to
an indev event like this:


.. code-block:: c

    void key_event_cb(lv_event_t *e)
    {
        lv_indev_t * indev = (lv_indev_t *)lv_event_get_target(e); /*Same as lv_indev_active()*/
        if(lv_indev_get_state(indev) == LV_INDEV_STATE_PRESSED) {
            LV_LOG_USER("%c", (char) lv_indev_get_key(indev));
        }
    }

    ...

    lv_indev_add_event_cb(keyboard, key_event_cb, LV_EVENT_KEY, NULL);



Key Remapping
*************

Some applications require assigning physical buttons to multiple key events depending
on the context: arrow keys may be used in a screen to navigate across widgets, and
used to interact with one widget in another screen. Key remapping functionality of
indev can be used to implement this behavior.

You must first define a remapping callback, then invoke
:cpp:func:`lv_indev_set_key_remap_cb` to enable it.

.. code-block:: c

    lv_key_t remap_left_right_into_prev_next(lv_indev_t * indev, lv_key_t key)
    {
        LV_UNUSED(indev);

        switch (key) {
        case LV_KEY_LEFT:
            return LV_KEY_PREV;
        case LV_KEY_RIGHT:
            return LV_KEY_NEXT;
        default:
            return key;
        }
    }

.. code-block:: c

    /* in your setup code */
    lv_indev_set_key_remap_cb(indev, remap_left_right_into_prev_next);

    /* later, to disable remapping */
    lv_indev_set_key_remap_cb(indev, NULL);

.. note::
   The remap callback is invoked only for keypad input devices. It will not remap keys from other input device types.
