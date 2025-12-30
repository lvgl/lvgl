.. _indev_gestures:

========
Gestures
========


Overview
********

Pointer input devices can detect simple (up, down, left, right) and multi-touch
(swipe, pinch, scroll) gestures.

By default, most widgets send gestures to their parents so they can be detected on the
Screen widget in the form of an :cpp:enumerator:`LV_EVENT_GESTURE` event. For example:

To prevent passing the gesture event to the parent from a widget, use
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_GESTURE_BUBBLE)`.

Note that gestures are not triggered if a widget is being scrolled.

If you performed some action on a gesture, you can call
:cpp:expr:`lv_indev_wait_release(lv_indev_active())` in the event handler to prevent
LVGL from sending further input-device-related events.



Simple Gestures
***************

Simple gestures are always enabled and very easy to use:

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

To trigger a gesture, two things need to happen. The movement needs to be:

1. fast enough: the difference between the current and the previous point must be
   greater than ``indev->gesture_min_velocity``
2. large enough: the difference between the first and the current point must be
   greater than ``indev->gesture_limit``



Multi-touch Gestures
********************

LVGL has the ability to recognize multi-touch gestures. Currently, these multi-touch
gestures are supported:

- Two fingers pinch (up and down)
- Two fingers rotation
- Two fingers swipe (infinite)

To enable multi-touch gesture recognition, set the ``LV_USE_GESTURE_RECOGNITION``
option in the ``lv_conf.h`` file.


Usage
-----

The recognizers can be updated to recognize gestures by calling
:cpp:expr:`lv_indev_gesture_recognizers_update(indev, touches, touch_cnt)`. This must
be done in the user-defined indev ``read_cb``. This will iterate over the recognizers
and stop once it detects a recognized or ended gesture. Currently, only one
multi-touch gesture can be recognized or ended at a time.

Once the recognizers are updated, calling
:cpp:expr:`lv_indev_gesture_recognizers_set_data(indev, data)` will update the
:cpp:expr:`lv_indev_data_t` structure. This should also be done in the indev
``read_cb``.

The way the touch points are collected varies based on the hardware and drivers used.

Here is a generic example of the ``read_cb``:

.. code-block:: c

   static void touch_read_callback(lv_indev_t * indev, lv_indev_data_t * data)
   {
       /* Stores the collected touch events */
       lv_indev_touch_data_t touches[10];

       /* Store the current touch (finger) count */
       int32_t touch_cnt;

       /* Get the touch points */
       touch_cnt = my_read_touch_points(touches);

       lv_indev_gesture_recognizers_update(indev, touches, touch_cnt);
       lv_indev_gesture_recognizers_set_data(indev, data);

       /* Also process normal touch */
       if(touch_cnt > 0) {
           data->point.x = touches[0].point.x;
           data->point.y = touches[0].point.y;
           data->state = LV_INDEV_STATE_PRESSED;
       } else {
           data->state = LV_INDEV_STATE_RELEASED;
       }
   }

LVGL sends events if the gestures are in one of the following states:

- ``LV_INDEV_GESTURE_STATE_RECOGNIZED``: The gesture has been recognized and is now
  active.
- ``LV_INDEV_GESTURE_STATE_ENDED``: The gesture has ended.



Events
------

Once a gesture is recognized or ended, a ``LV_EVENT_GESTURE`` is sent. The user can
use these functions to gather more information about the gesture:

- :cpp:expr:`lv_event_get_gesture_type(lv_event_t * e)`: Get the type of the gesture.
  Use this to check which multi-touch gesture is currently reported.
- :cpp:expr:`lv_event_get_gesture_state(lv_event_t * e, lv_indev_gesture_type_t type)`:
  Get the state of the gesture as :cpp:type:`lv_indev_gesture_state_t`. It can be one
  of:

  - :cpp:enumerator:`LV_INDEV_GESTURE_STATE_NONE`: The gesture is not active.
  - :cpp:enumerator:`LV_INDEV_GESTURE_STATE_RECOGNIZED`: The gesture is recognized and
    can be used.
  - :cpp:enumerator:`LV_INDEV_GESTURE_STATE_ENDED`: The gesture ended.

The user can then request the gesture values with the following functions:

- :cpp:expr:`lv_event_get_pinch_scale(lv_event_t * e)`: Get the pinch scale. Only
  relevant for pinch gestures.
- :cpp:expr:`lv_event_get_rotation(lv_event_t * e)`: Get the rotation in radians. Only
  relevant for rotation gestures.
- :cpp:expr:`lv_event_get_two_fingers_swipe_distance(lv_event_t * e)`: Get the
  distance in pixels from the gesture starting center. Only relevant for two-finger
  swipe gestures.
- :cpp:expr:`lv_event_get_two_fingers_swipe_dir(lv_event_t * e)`: Get the direction
  from the starting center. Only relevant for two-finger swipe gestures.

This allows the user to react to the gestures and use the gesture values. An example
of such an application is available in the source tree:
``examples/others/gestures/lv_example_gestures.c``.



Thresholds
----------

The gesture recognizers can be configured to modify the gesture thresholds:

- :cpp:expr:`lv_indev_set_pinch_up_threshold(lv_indev_t * indev, float threshold)`:
  Set the pinch-up (zoom in) threshold in pixels.
- :cpp:expr:`lv_indev_set_pinch_down_threshold(lv_indev_t * indev, float threshold)`:
  Set the pinch-down (zoom out) threshold in pixels.
- :cpp:expr:`lv_indev_set_rotation_rad_threshold(lv_indev_t * indev, float threshold)`:
  Set the rotation angle threshold in radians.



libinput Example
----------------

In the case of ``libinput``, touch events are received asynchronously. To handle it,
the touch array and touch count need to be global variables:

.. code-block:: c

   /* Stores the collected touch events */
   static lv_indev_touch_data_t touches[10];

   /* Store the current touch (finger) count SINCE THE LAST READ */
   static int32_t touch_cnt;

so the flow is:

1. Save the touch events asynchronously in the ``touches`` array (one event per finger)
2. Process the touch data in the ``read_cb``
3. Forget the touches and wait for new touch events by setting ``touch_cnt = 0`` at
   the end of the ``read_cb``

.. code-block:: c

   /**
    * @brief Convert the libinput event to LVGL's representation of a touch event
    * @param ev a pointer to the libinput event
    */
   static void touch_process(struct libinput_event *ev)
   {
      int type = libinput_event_get_type(ev);
      struct libinput_event_touch *touch_ev = libinput_event_get_touch_event(ev);
      int id = libinput_event_touch_get_slot(touch_ev);
      uint32_t time = libinput_event_touch_get_time(touch_ev);

      /* Get the last event for the contact point */
      lv_indev_touch_data_t *cur = NULL;

      /* Find if the touch is already stored in the array by its ID.
       * If there are 2 active touches it's enough to check those. */
      for (int i = 0; i < touch_cnt; i++) {
          if (touches[i].id == id) {
              cur = &touches[i];
              break;
          }
      }

      if (cur && cur->timestamp == time) {
          /* Previous event has the same timestamp - ignore duplicate event */
          return;
      }

      /* Save the new touch */
      if (cur == NULL ||
              type == LIBINPUT_EVENT_TOUCH_UP ||
              type == LIBINPUT_EVENT_TOUCH_DOWN) {

          cur = &touches[touch_cnt];
          cur->id = id; /* It will be needed to identify the touches in the next events */
          touch_cnt++;
      }

      /* Process the event */
      switch (type) {
          case LIBINPUT_EVENT_TOUCH_DOWN:
          case LIBINPUT_EVENT_TOUCH_MOTION:
              cur->point.x = (int) libinput_event_touch_get_x_transformed(touch_ev, SCREEN_WIDTH);
              cur->point.y = (int) libinput_event_touch_get_y_transformed(touch_ev, SCREEN_HEIGHT);
              cur->state = LV_INDEV_STATE_PRESSED;
              break;

          case LIBINPUT_EVENT_TOUCH_UP:
              cur->state = LV_INDEV_STATE_RELEASED;
              cur->point.x = 0;
              cur->point.y = 0;
              break;
      }

      cur->timestamp = time;
   }

   /**
    * @brief Filter out libinput events that are not related to touches
    * @param ev a pointer to the libinput event
    */
   static void process_libinput_event(struct libinput_event *ev)
   {
      int type = libinput_event_get_type(ev);

      switch (type) {
          case LIBINPUT_EVENT_TOUCH_MOTION:
          case LIBINPUT_EVENT_TOUCH_DOWN:
          case LIBINPUT_EVENT_TOUCH_UP:
              /* Filter only touch events */
              touch_process(ev);
              break;
          default:
              /* Skip unrelated libinput events */
              return;
      }
   }
