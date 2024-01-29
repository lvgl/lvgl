========================
Linux Framebuffer Driver
========================

Overview
-------------

The Linux framebuffer (fbdev) is a linux subsystem used to display graphics. It is a hardware-independent API that gives user space software
access to the framebuffer (the part of a computer's video memory containing a current video frame) using only the Linux kernel's own basic
facilities and its device file system interface, avoiding the need for libraries that implement video drivers in user space. 

Prerequisites
-------------

Your system has a framebuffer device configured (usually under ``/dev/fb0``).

Configuring the driver
----------------------

Enable the framebuffer driver support in lv_conf.h, by cmake compiler define or by KConfig. Additionally you may configure the rendering
mode.

.. code:: c

	#define LV_USE_LINUX_FBDEV           1
	#define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL

Usage
-----

To set up a framebuffer-based display, first create a display with ``lv_linux_fbdev_create``. Afterwards set the framebuffer device
node on the display (usually this is ``/dev/fb0``).

.. code:: c

	lv_display_t *disp = lv_linux_fbdev_create();
	lv_linux_fbdev_set_file(disp, "/dev/fb0");

If your screen stays black or only draws partially, you can try enabling direct rendering via ``LV_DISPLAY_RENDER_MODE_DIRECT``. Additionally,
you can activate a force refresh mode with ``lv_linux_fbdev_set_force_refresh(true)``. This usually has a performance impact though and shouldn't
be enabled unless really needed.
