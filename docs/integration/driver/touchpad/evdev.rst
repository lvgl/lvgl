==================
Linux Evdev Driver
==================

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

	lv_display_t * disp = lv_linux_fbdev  
	lv_linux_fbdev_set_file(disp, "/dev/fb0");_create();


Locating your input device
--------------------------

If you can't determine your input device, first run   

```$cat /proc/bus/input/devices```

This should show input devices and there will be entries with the word ``event`` which give a clue as to the device to use eg. ``event1`` would be ``/dev/input/event1``.  

You can use ``evtest`` to show data from that event source to see if it is actually the one you want.

Try:   

``$evtest /dev/input/event1`` replacing ``eventX`` with your event device from above.   
