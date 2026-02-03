=====
evdev
=====

Overview
--------

The Linux event device (evdev) is a hardware-independent API that gives access to input events from,
for example, a mouse or touchscreen. It is exposed via the Linux device file system interface.

Prerequisites
-------------

Your system has an input device configured (usually under ``/dev/input/`` such as ``/dev/input/event0``).

Configuring the driver
----------------------

Enable the Linux LVGL evdev driver support in ``lv_conf.h``.

.. code-block:: c

    #define LV_USE_EVDEV 1

Usage
-----

To set up an event input, first create an input device with ``lv_edev_create`` setting it to the correct Linux event device.
Then link this to the LVGL display with ``lv_indev_set_display``.

.. code-block:: c

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    lv_indev_set_display(touch, disp);

Ensure that an ``lv_display_t`` object is already created for ``disp``. An example for this is shown below, using the Linux framebuffer driver.

.. code-block:: c

    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");


Locating your input device
--------------------------

If you can't determine your input device, first run

```$cat /proc/bus/input/devices```

This should show input devices and there will be entries with the word ``event`` which give a clue as to the device to use eg. ``event1`` would be ``/dev/input/event1``.

You can use ``evtest`` to show data from that event source to see if it is actually the one you want.

Try:

``$evtest /dev/input/event1`` replacing ``eventX`` with your event device from above.

Automatic input device discovery
--------------------------------

There is support for automatically finding and adding input devices in ``/dev/input/``. New devices will automatically be added
when they are connected. To enable this feature, you can simply call :cpp:expr:`lv_evdev_discovery_start(NULL, NULL)`.

You may want to react to a new device being added so that a cursor image can be applied, for example. You can provide a callback
function which will be called when a new device is added.

.. code-block:: c

    #include "lvgl/src/core/lv_global.h"

    static void indev_deleted_cb(lv_event_t * e)
    {
        if(LV_GLOBAL_DEFAULT()->deinit_in_progress) return;
        lv_obj_t * cursor_obj = lv_event_get_user_data(e)
        lv_obj_delete(cursor_obj);
    }

    static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
    {
        LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" :
                                                  type == LV_EVDEV_TYPE_ABS ? "ABS" :
                                                  type == LV_EVDEV_TYPE_KEY ? "KEY" :
                                                  "unknown");

        if(type == LV_EVDEV_TYPE_REL) {
            LV_IMAGE_DECLARE(mouse_cursor_icon);
            lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
            lv_image_set_src(cursor_obj, &mouse_cursor_icon);
            lv_indev_set_cursor(indev, cursor_obj);
            lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
        }
    }

    int main()
    {
        /* ... */
        lv_evdev_discovery_start(discovery_cb, NULL);
        /* ... */
    }

At the time of writing, this feature is not supported in BSD.
