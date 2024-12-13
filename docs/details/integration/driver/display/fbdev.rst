========================
Linux Framebuffer Driver
========================

Overview
--------

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

.. code-block:: c

	#define LV_USE_LINUX_FBDEV           1
	#define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL

Usage
-----

To set up a framebuffer-based display, first create a display with ``lv_linux_fbdev_create``. Afterwards set the framebuffer device
node on the display (usually this is ``/dev/fb0``).

.. code-block:: c

	lv_display_t *disp = lv_linux_fbdev_create();
	lv_linux_fbdev_set_file(disp, "/dev/fb0");

If your screen stays black or only draws partially, you can try enabling direct rendering via ``LV_DISPLAY_RENDER_MODE_DIRECT``. Additionally,
you can activate a force refresh mode with ``lv_linux_fbdev_set_force_refresh(true)``. This usually has a performance impact though and shouldn't
be enabled unless really needed.


Common mistakes
---------------

Default resolution issue
^^^^^^^^^^^^^^^^^^^^^^^^

When the Linux kernel initializes, it sets up subsystems like the framebuffer 
(fbdev) to manage display output. If an HDMI display is connected, the kernel 
detects it and allocates a portion of RAM as the framebuffer, which holds the 
pixel data for rendering images.

However, a common issue arises when the display is not powered on during the 
boot process. If the board is powered on before the screen, the kernel may 
incorrectly configure the framebuffer resolution. As a result, both the visible 
and virtual resolutions can be incorrect, leading to display problems.

This issue often occurs with HDMI connections where the display is powered up 
after the system has already booted.

.. code-block::	
	
	fbset -fb /dev/fb0 


