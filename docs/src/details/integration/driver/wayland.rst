.. _wayland_driver:

=============================
Wayland Display/Inputs driver
=============================

Overview
--------

| The **Wayland** `driver <https://github.com/lvgl/lvgl/tree/master/src/drivers/wayland>`__ offers support for simulating the LVGL display and keyboard/mouse inputs in a desktop window.
| It is an alternative to **X11** or **SDL2**

The main purpose for this driver is for testing/debugging the LVGL application, it can also be used to run applications in 'kiosk mode'

Dependencies
------------

The wayland driver requires some dependencies.

On Ubuntu

.. code:: bash

   sudo apt-get install libwayland-dev libxkbcommon-dev libwayland-bin wayland-protocols

On Fedora

.. code:: bash

   sudo dnf install wayland-devel libxkbcommon-devel wayland-utils wayland-protocols-devel


Configuring the wayland driver
------------------------------

1. Enable the wayland driver in ``lv_conf.h``

.. code:: c

   #define LV_USE_WAYLAND  1

2. Optional configuration options:

- Enable window decorations, only required on GNOME because out of all the available wayland compositors
  **only** Mutter/GNOME enforces the use of client side decorations

.. code:: c

   #define LV_WAYLAND_WINDOW_DECORATIONS 1

- Enable support for the deprecated 'wl_shell', Only useful when the BSP on the target has weston ``9.x``

.. code:: c

   #define LV_WAYLAND_WL_SHELL  1

Example
-------

An example simulator is available in this `repo <https://github.com/lvgl/lv_port_linux/>`__

Usage
-----

#. In ``main.c`` ``#include "lv_drivers/wayland/wayland.h"``
#. Enable the Wayland driver in ``lv_conf.h`` with ``LV_USE_WAYLAND 1``

#. ``LV_COLOR_DEPTH`` should be set either to ``32`` or ``16`` in ``lv_conf.h``

#. Add a display using ``lv_wayland_window_create()``,
   possibly with a close callback to track the status of each display:

.. code:: c

    #define H_RES (800)
    #define V_RES (480)

    /* Create a display */
    lv_disp_t * disp = lv_wayland_create_window(H_RES, V_RES, "Window Title", close_cb);


As part of the above call, the Wayland driver will register four input devices
for each display:

* a KEYPAD connected to Wayland keyboard events
* a POINTER connected to Wayland touch events
* a POINTER connected to Wayland pointer events
* an ENCODER connected to Wayland pointer axis events

Handles for input devices of each display can be obtained using
``lv_wayland_get_indev_keyboard()``, ``lv_wayland_get_indev_touchscreen()``,
``lv_wayland_get_indev_pointer()`` and ``lv_wayland_get_indev_pointeraxis()`` respectively.

Fullscreen mode
^^^^^^^^^^^^^^^

To programmatically fullscreen the window,
use the ``lv_wayland_window_set_fullscreen()`` function respectively with ``true``
or ``false`` for the ``fullscreen`` argument.

Maximized mode
^^^^^^^^^^^^^^

To programmatically maximize the window,
use the ``lv_wayland_window_set_maximized()`` function respectively with ``true``
or ``false`` for the ``maximized`` argument.


Custom timer handler
^^^^^^^^^^^^^^^^^^^^

Always call ``lv_wayland_timer_handler()`` in your timer loop instead of the regular ``lv_timer_handler()``.

**Note:** ``lv_wayland_timer_handler()`` internally calls ``lv_timer_handler()``

This allows the wayland client to work on well on weston, resizing shared memory buffers during
a commit does not work well on weston.

Wrapping the call to ``lv_timer_hander()`` is a necessity to have more control over
when the LVGL flush callback is called.

The custom timer handler returns ``false`` if the frame from previous cycle is not rendered.
When this happens, it usually means that the application is minimized or hidden behind another window.
Causing the driver to wait until the arrival of any message on the wayland socket, the process is in interruptible sleep.

Building the wayland driver
---------------------------

An example simulator is available in this `repo <https://github.com/lvgl/lv_port_linux/>`__

If there is a need to use driver with another build system. The source and header files for the XDG shell
must be generated from the definitions for the XDG shell protocol.

In the example Cmake is used to perform the operation by invoking the ``wayland-scanner`` utility

To achieve this manually,

Make sure the dependencies listed at the start of the article are installed.

The wayland protocol is defined using XML files which are present in ``/usr/share/wayland-protocols``

To generate the required files run the following commands:

.. code-block:: sh

   wayland-scanner client-header </usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > wayland_xdg_shell.h
   wayland-scanner private-code </usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > wayland_xdg_shell.c

The resulting files can then be integrated into the project, it's better to re-run ``wayland-scanner`` on
each build to ensure that the correct versions are generated, they must match the version of the ``wayland-client``
dynamically linked library installed on the system.


Current state and objectives
----------------------------

* Add direct rendering mode
* Refactor the shell integrations to avoid excessive conditional compilation
* Technically, the wayland driver allows to create multiple windows - but this feature is experimental.
* Eventually add enhanced support for XDG shell to allow the creation of desktop apps on Unix-like platforms,
  similar to what the win32 driver does.
* Add a support for Mesa, currently wl_shm is used and it's not the most effective technique.


Bug reports
-----------

The wayland driver is currently under construction, bug reports, contributions and feedback is always welcome.

It is however important to create detailed issues when a problem is encountered, logs and screenshots of the problem are of great help.

Please enable ``LV_USE_LOG`` and launch the simulator executable like so

.. code::

  WAYLAND_DEBUG=1 ./path/to/simulator_executable > /tmp/debug 2>&1

This will create a log file called ``debug`` in the ``/tmp`` directory, copy-paste the content of the file in the github issue.
The log file contains LVGL logs and the wayland messages.

Be sure to replicate the problem quickly otherwise the logs become too big

