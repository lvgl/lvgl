.. _qnx:

===
QNX
===

What is QNX?
************

QNX is a commercial operating system first released in 1980. The operating
system is based on a micro-kernel design, with the file system(s), network
stack, and various other drivers each running in its own process with a separate
address space.

See www.qnx.com for more details.

Highlight of QNX
----------------

- 64-bit only, runs on x86_64 and ARMv8
- Requires an MMU as the design mandates separation among processes
- Support for thousands of processes and millions of threads
- Up to 64 cores, up to 16TB of RAM
- Virtualization support (as host and guest)
- Full POSIX compatibility
- Safety certification to various automotive, industrial and medical standards

How to run LVGL on QNX?
***********************

There are two ways to use LVGL in your QNX project. The first is similar to how
LVGL is used on other systems. The second is to build LVGL as either a shared or
a static library.

Include LVGL in Your Project
----------------------------

Follow the generic instructions for getting started with LVGL. After copying
`lv_conf_template.h` to  `lv_conf.h` make the following changes to the latter:

1. Enable QNX support:

.. code-block:: c

    #define LV_USE_QNX 1

2. Set colour depth to 32:

.. code-block:: c

    #define LV_COLOR_DEPTH 32

3. (Optional) Enable double-buffering:

.. code-block:: c

    #define LV_QNX_BUF_COUNT 2

Build LVGL as a Library
-----------------------

**Note that this method is an alternative to including LVGL in your project. If
you choose to build a library then you do not need to follow the instructions in
the previous section.**

The top-level `qnx` directory includes a recursive make file for building LVGL,
both as a shared library and as a static library for the supported
architectures. To build all libraries, simply invoke `make` in this directory:

.. code-block:: shell

    # cd $(LVGL_ROOT)/env_support/qnx
    # make

If you prefer to build for a specific architecture and variant, go to the
appropriate directory and run `make` there. For example, to build a shared
library for ARMv8:

.. code-block:: shell

    # cd $(LVGL_ROOT)/env_support/qnx/aarch64/so.le
    # make

As a general rule, if you only want to have one LVGL application in your system
then it is better to use a static library. If you have more than one, and
especially if they run concurrently, it is better to use the shared library.

Before building the library, you may wish to edit
`$(LVGL_ROOT)/env_support/qnx/lv_conf.h`, e.g. to add fonts or disable
double-buffering.

Writing a LVGL Application
--------------------------

To create a LVGL application for QNX, follow these steps in your code:

1. Initialize the library.
2. Create a window.
3. Add the input devices.
4. Create the UI.
5. Run the event loop.

Steps 2, 3 and 5 use QNX-specific calls, but the rest of the code should be
identical to that of a LVGL application written for any other platform.

The following code shows how to create a "Hello World" application:

.. code-block:: c

    #include <lvgl.h>

    int
    main(int argc, char **argv)
    {
        /* Initialize the library. */
        lv_init();

        /* Create a 800x480 window. */
        lv_display_t *disp = lv_qnx_window_create(800, 480);
        lv_qnx_window_set_title(disp, "LVGL Example");

        /* Add keyboard and mouse devices. */
        lv_qnx_add_keyboard_device(disp);
        lv_qnx_add_pointer_device(disp);

        /* Generate the UI. */
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Hello world");
        lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        /* Run the event loop until it exits. */
        return lv_qnx_event_loop(disp);
    }

Build the Application
---------------------

Building the application consists of compiling the source with the LVGL headers,
and then linking against the library. This can be done in many ways, using
different build systems. The following is a simple make file for the example
above, which builds for ARMv8 with the shared library:

.. code-block:: makefile

    CC=qcc -Vgcc_ntoaarch64le

    LVGL_ROOT=$(HOME)/src/lvgl
    CCFLAGS=-I$(LVGL_ROOT)/env_support/qnx -I$(LVGL_ROOT)
    LDFLAGS=-lscreen -llvgl -L$(LVGL_ROOT)/env_support/qnx/aarch64/so.le

    lvgl_example: lvgl_example.c
    	$(CC) $(CCFLAGS) -Wall -o $@ $< $(LDFLAGS)

    clean:
    	rm -f *.o *~ lvgl_example
