
.. _indev_pointer:


==================
Touchpad and Mouse
==================



Overview
********

Input devices that can click points on the display belong to the *Pointer* category.
This includes any kind of touchpad or mouse.

Pointer input devices can:

- scroll widgets
- send a wide variety of :ref:`Events <events>` (including click, long press,
  double click, and many more)
- trigger and handle :ref:`Gestures <indev_gestures>`



Example
*******

Here is an example of a simple input-device read callback function:

.. code-block:: c

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
   lv_indev_set_read_cb(indev, touch_read);

   ...

   void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
   {
       if(my_touch_is_pressed()) {
           data->point.x = touchpad_x;
           data->point.y = touchpad_y;
           data->state = LV_INDEV_STATE_PRESSED;
       } else {
           data->state = LV_INDEV_STATE_RELEASED;
       }
   }



.. _indev_cursor:


Cursor
******

Pointer input devices can have a cursor too.

.. code-block:: c

   LV_IMAGE_DECLARE(mouse_cursor_icon);                          /* Declare the image source. */
   lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());  /* Create image widget for cursor. */
   lv_image_set_src(cursor_obj, &mouse_cursor_icon);             /* Set image source. */
   lv_indev_set_cursor(mouse_indev, cursor_obj);                 /* Connect image to input device. */

When the cursor is added, it will be placed on the system layer
(:cpp:expr:`lv_layer_sys()`) and positioned at the last coordinate read.



Multi-touch
***********

Multi-touch can refer to two things:

1. Processing gestures, like swipe, pinch, etc. For this, see
   :ref:`Gestures <indev_gestures>`
2. Supporting multiple simultaneous touch points: just create multiple input
   devices. For example, to support 2 touches, create 2 pointer input devices.
   Use the first touch point from the touch driver in the first indev, and use
   the second point in the second indev.



.. _extending_click_area:


Extending the Click Area
************************

By default, widgets can be clicked only within their bounding area. However,
especially with small widgets, it can be helpful to make a widget's "clickable"
area larger. You can do this with
:cpp:expr:`lv_obj_set_ext_click_area(widget, size)`.



Parameters
**********

The timing and limits of input devices can be configured via the following fields
of :cpp:expr:`lv_indev_t`:

- :cpp:expr:`lv_indev_set_long_press_time(indev, ms)`:
  Time required to send :cpp:enumerator:`LV_EVENT_LONG_PRESSED` (in milliseconds)
- :cpp:expr:`lv_indev_set_long_press_repeat_time(indev, ms)`:
  Interval between :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT` events (in
  milliseconds)
- :cpp:expr:`lv_indev_set_scroll_limit(indev, pixels)`:
  Number of pixels to move before scrolling the widget
- :cpp:expr:`lv_indev_set_scroll_throw(indev, percent)`:
  Scroll throw (momentum) slow-down in [%]. A greater value means faster
  slow-down.



.. _indev_crown:


Crown Mode
**********

A "Crown" is a rotary device typically found on smartwatches.

When the user clicks somewhere and then turns the rotary, the last clicked widget
will either be scrolled or its value will be incremented/decremented (e.g., in the
case of a slider).

As this behavior is tightly related to the last clicked widget, crown support is
an extension of the pointer input device. Just set ``data->enc_diff`` to the number
of rotary steps, and LVGL will automatically send the
:cpp:enumerator:`LV_EVENT_ROTARY` event or scroll the widget based on the
``editable`` flag in the widget's class. Non-editable widgets are scrolled, and for
editable widgets, the event is sent.

To get the steps in an event callback, use:
``int32_t diff = lv_event_get_rotary_diff(e);``

The rotary sensitivity can be adjusted at two levels:

1. In the input device using the ``indev->rotary_sensitivity`` field (1/256 unit)
2. In the widget using the ``rotary_sensitivity`` style property (1/256 unit)

The final diff is calculated as:

``diff_final = diff_in * (indev_sensitivity / 256) * (widget_sensitivity / 256);``

For example, if both the indev and widget sensitivity are set to 128 (0.5), the
input diff will be multiplied by 0.25. The value of the widget will be incremented
by that amount or the widget will be scrolled by that many pixels.
