========================
Linux Edev Driver
========================

Overview
--------

The Linux event device (edev) is a hardware-independent API that gives user space software
access to input events from, for example, a mouse or touchscreen. It is exposed via the Linux device file system interface. 

Prerequisites
-------------

Your system has an input device configured (usually under ``/dev/input/event0``).

Configuring the driver
----------------------

Enable the Linux LVGL edev driver support in lv_conf.h.    

.. code:: c

	#define LV_USE_EVDEV 1

Usage
-----

To set up an edev input, first create a input device with ``lv_edev_create`` setting it to the correct Linux event device.
Then link this to the LVGL display with ``lv_indev_set_display``.   

.. code:: c

	lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
	lv_indev_set_display(touch, disp);

```disp``` is already created - if using the Linux framebuffer this could be:

.. code:: c

	lv_display_t * disp = lv_linux_fbdev  
	lv_linux_fbdev_set_file(disp, "/dev/fb0");_create();


Locating your input device
--------------------------

You can use evtest to find which input device you need.   
Try   

``evtest /dev/input/event0``

and each ``event`` input in turn until you get output.   
