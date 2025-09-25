.. _indev_creation:

=============
Input devices
=============

What is an indev?
*****************

Input devices (or ``indev``) refer to all the modules and features related to user input handling.

This includes touch, mouse, cursor, keyboard, external buttons, encoder, navigation, and many more.

Input devices can trigger a wide variety of events such as press, release, click, double or triple click,
scroll, hover, and others. To see the full list, check out :ref:`indev_events`.

Input devices can also change the :ref:`State <widget_states>` of widgets. Some of the state changes are
indev-type specific. However, the most basic one is that pressed widgets will automatically have
:cpp:enumerator:`LV_STATE_PRESSED`. See :ref:`styles` for more details.

Creating an Input Device
************************

To create an input device, only two things are required:

1. a type: pointer, keypad, etc.
2. a read callback: to read the current touch point or pressed key

.. code-block:: c

    lv_indev_t * indev = lv_indev_create();        /* Create input device */
    lv_indev_set_type(indev, LV_INDEV_TYPE_...);   /* Set the device type */
    lv_indev_set_read_cb(indev, my_input_read);    /* Set the read callback */

The ``type`` member can be:

- :cpp:enumerator:`LV_INDEV_TYPE_POINTER`: touchpad or mouse
- :cpp:enumerator:`LV_INDEV_TYPE_KEYPAD`: keyboard or keypad
- :cpp:enumerator:`LV_INDEV_TYPE_ENCODER`: encoder with left/right turn and push options
- :cpp:enumerator:`LV_INDEV_TYPE_BUTTON`: external buttons virtually pressing the screen

``my_input_read`` is a function pointer that will be called periodically to
report the current state of the input device to LVGL.

Input devices are connected to a :ref:`Display <display>`, so make sure to create a display before
creating indevs.

If you have multiple displays, you must ensure the default display is set
to the one your input device is "connected to" before making the above calls.

Reading Data
************

Polling
-------

By default, a :ref:`timer` is created that calls the ``read_cb`` periodically. The default read period is set by
:c:macro:`LV_DEF_REFR_PERIOD` in ``lv_conf.h``.

:cpp:expr:`lv_indev_get_read_timer()` returns the read timer so that :ref:`timer`-related functions can be called on it
to adjust its behavior.

Buffered Reading
----------------

By default, LVGL calls ``read_cb`` periodically. Because of this
intermittent polling, there is a chance that some user events are
missed.

To solve this, you can write an event-driven driver for your input device
that buffers measured data. In ``read_cb``, you can report the buffered
data instead of directly reading the input device. Setting the
``data->continue_reading`` flag will tell LVGL there is more data to
read and it should call ``read_cb`` again.

If the driver can provide precise timestamps for buffered events, it can
overwrite ``data->timestamp``. By default, this is initialized to
:cpp:func:`lv_tick_get()` just before invoking ``read_cb``.

.. _indev event mode:

Event-Driven Mode
-----------------

Normally, an input device is read every :c:macro:`LV_DEF_REFR_PERIOD`
milliseconds (set in ``lv_conf.h``). However, in some cases, you might
need more control over when to read the input device. For example, you
might need to read it by polling a file descriptor.

You can do this by:

.. code-block:: c

   /* Update the input device's running mode to LV_INDEV_MODE_EVENT */
   lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);

   ...

   /* Call this anywhere you want to read the input device */
   lv_indev_read(indev);

.. note:: :cpp:func:`lv_indev_read`, :cpp:func:`lv_timer_handler`, and :cpp:func:`_lv_display_refr_timer` cannot run at the same time.

.. note:: For devices in event-driven mode, `data->continue_reading` is ignored.

Pausing the Indev Timer
-----------------------

It's not always possible to take an indev reading directly inside
a raw interrupt handler. Typically, a flag would be set inside the interrupt handler
which would be checked and reset inside the indev read callback where the reading
would actually be taken. This works fine, but the indev read callback is constantly
polling a flag which may go for long periods unset. We cannot use :ref:`indev event mode`
because :cpp:func:`lv_indev_read` should not be called in an interrupt handler.

For this situation, you can use the timer-based indev read callback as usual but
pause the indev timer if there hasn't been an interrupt in a while.
Resuming a timer is typically safe in an interrupt handler.
Care must be taken to avoid race conditions.

.. code-block:: c

    volatile bool interrupt_occurred;
    lv_timer_t * volatile indev_timer;

    void interrupt_handler(void)
    {
        interrupt_occurred = true;
        if(indev_timer) lv_timer_resume(indev_timer);
    }

    uint32_t last_interrupt_tick;

    void my_input_read(lv_indev_t * indev, lv_indev_data_t * data)
    {
        uint32_t tick_now = lv_tick_get();

        /* If no interrupt has happened in the past 100 ms, pause the indev timer */
        if(lv_tick_diff(tick_now, last_interrupt_tick) > 100) {
            lv_timer_pause(indev_timer);
        }

        if(interrupt_occurred) {
            interrupt_occurred = false;
            last_interrupt_tick = tick_now;

            /*
             * Ensure the timer is running in case an interrupt occurred
             * just after the timer was paused. Without this, a race condition
             * could leave the timer paused and input events would not be processed.
             */
            lv_timer_resume(indev_timer);
        }

        /* Perform the reading */
        /* ... */
    }

.. code-block:: c

    /* In your setup code */
    indev_timer = lv_indev_get_read_timer(indev);

.. admonition:: Further Reading

    - `lv_port_indev_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_indev_template.c>`__
      for a template for your own input-device driver.

