===
QNX
===

What is QNX?
------------

QNX is a commercial operating system first released in 1980. The operating
system is based on a micro-kernel design, with the file system(s), network
stack, and various other drivers each running in its own process with a separate
address space.

See www.qnx.com for more details.

Highlight of QNX
~~~~~~~~~~~~~~~~

- 64-bit only, runs on x86_64 and ARMv8
- Requires an MMU as the design mandates separation among processes
- Support for thousands of processes and millions of threads
- Up to 64 cores, up to 16TB of RAM
- Virtualization support (as host and guest)
- Full POSIX compatibility
- Safety certification to various automotive, industrial and medical standards

How to run LVGL on QNX?
-----------------------

Build
~~~~~

QNX uses standard development tools. Once you have installed the software
development platform, follow the LVGL instructions for building on any *NIX
system.

Example
~~~~~~~

The following example shows how to use the QNX LVGL driver to run one of the
LVGL examples:

.. code:: c

    #include "lvgl/lvgl.h"

    void lv_example_keyboard_2(void)
    {
        /* Copy the code from examples/widget/keyboard_2.c */
    }

    int
    main(int argc, char **argv)
    {
        /* Initialize the library. */
        lv_init();

        /* Create a 800x480 window. */
        lv_display_t *disp = lv_qnx_window_create(800, 480);

        /* Add a mouse device. */
        lv_qnx_add_pointer_device(disp);

        /* Generate the UI. */
        lv_example_keyboard_2();

        /* Run the event loop until it exits. */
        return lv_qnx_event_loop(disp);
    }
