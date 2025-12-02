.. _wayland_driver:

=============================
Wayland Display/Inputs driver
=============================



Overview
********

The Wayland `driver <https://github.com/lvgl/lvgl/tree/master/src/drivers/wayland>`__ provides integration between LVGL and Wayland-based systems,
allowing LVGL applications to render directly into a Wayland surface and handle keyboard, mouse and touch input. This makes it suitable 
for production deployments, such as kiosk interfaces, control panels, or other embedded GUIs where Wayland is the display server.

In addition to production use, the driver is also useful for development on linux desktop environments, offering a convenient way to simulate and validate 
your LVGL applications without dedicated hardware. It serves as an alternative to the X11 and SDL2 drivers.



Dependencies
************

The wayland driver requires some dependencies.

On Ubuntu

.. code:: bash

   sudo apt-get install libwayland-dev libxkbcommon-dev libwayland-bin wayland-protocols

On Fedora

.. code:: bash

   sudo dnf install wayland-devel libxkbcommon-devel wayland-utils wayland-protocols-devel



Rendering Backends
******************

The Wayland driver supports multiple rendering backends, allowing you to choose the most appropriate rendering method for your hardware and use case.


SHM Backend (Default)
---------------------

The **SHM (Shared Memory)** backend is the default rendering backend used by the Wayland driver.
It uses Wayland's shared memory protocol (``wl_shm``) for rendering, which is supported across all Wayland compositors.

**Features:**

* Double-buffered direct rendering
* Universal compatibility with all Wayland compositors
* No special hardware requirements

**Limitations:**

* Rotation is not currently supported

**Usage:**

The SHM backend is enabled by default and requires no additional configuration.


G2D Backend
-----------

The G2D backend leverages NXP's hardware-accelerated 2D graphics engine available on i.MX processors. This backend provides improved performance on NXP platforms.

**Features:**

* Hardware-accelerated rendering
* Double-buffered direct rendering
* Rotation support (0°, 90°, 180°, 270°)

**Requirements:**

* Hardware with G2D support (e.g., NXP i.MX6/i.MX8 series)
* G2D library and drivers installed on the system

**Usage:**

Enable the G2D backend in ``lv_conf.h``:

.. code:: c

   #define LV_USE_DRAW_G2D  1

When ``LV_USE_DRAW_G2D`` is enabled, the Wayland driver will automatically use the G2D backend instead of the default SHM backend.



Configuring the wayland driver
******************************

1. Enable the wayland driver in ``lv_conf.h``

.. code:: c

   #define LV_USE_WAYLAND  1



Reference Project
*****************

`LVGL Linux port <https://github.com/lvgl/lv_port_linux/>`__ serves as a reference project for setting up wayland. It uses CMake to generate
the required protocols at build time.



Getting started
***************

#. In ``main.c`` ``#include "lv_drivers/wayland/wayland.h"``
#. Enable the Wayland driver in ``lv_conf.h`` with ``LV_USE_WAYLAND 1``

#. ``LV_COLOR_DEPTH`` should be set either to ``32`` or ``16`` in ``lv_conf.h``

#. Add a display using ``lv_wayland_window_create()``,
   possibly with a close callback to track the status of each display:

.. code:: c

    #define H_RES (800)
    #define V_RES (480)

    /* Create a display */
    lv_disp_t * disp = lv_wayland_window_create(H_RES, V_RES, "Window Title", close_cb);


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
---------------

To programmatically fullscreen the window, use the ``lv_wayland_window_set_fullscreen()``
function respectively with ``true`` or ``false`` for the ``fullscreen`` argument.


Maximized mode
--------------

To programmatically maximize the window,
use the ``lv_wayland_window_set_maximized()`` function respectively with ``true``
or ``false`` for the ``maximized`` argument.

Minimize window
---------------

To programmatically minimize the window, use the ``lv_wayland_window_set_minimized()`` function.


Physical display assignment
---------------------------

When using multiple physical displays, you can control which display a fullscreen window
appears on by assigning it to a specific physical display before entering fullscreen mode.

Use ``lv_wayland_assign_physical_display()`` to assign a window to a particular physical
display, where the ``display`` parameter specifies the physical display number (typically
0, 1, 2, etc.):

.. code-block:: c

    /* Assign to physical display 0 and full screen on that display */
    lv_wayland_assign_physical_display(disp, 0);  
    lv_wayland_window_set_fullscreen(window, true); 

To remove the physical display assignment and return to default behavior, use
``lv_wayland_unassign_physical_display()``:

.. code-block:: c

    lv_wayland_unassign_physical_display(disp);



Building the Wayland Driver
***************************

The `reference project <https://github.com/lvgl/lv_port_linux/>`__  uses CMakeLists to generate the necessary dependencies at build time.

Mainly, the project generates the necessary protocols with the ``wayland-scanner`` utility.

The wayland protocols are defined using XML files which are present in ``/usr/share/wayland-protocols``

By default, LVGL relies on the ``xdg-shell`` protocol for window management to generate it run the following commands:

.. code-block:: sh

   wayland-scanner client-header $SYSROOT/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml wayland_xdg_shell.h
   wayland-scanner private-code $SYSROOT/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml wayland_xdg_shell.c

When `LV_WAYLAND_USE_DMABUF` is set to `1`, the following protocols must also be generated:

.. code-block:: sh

   wayland-scanner client-header $SYSROOT/usr/share/wayland-protocols/stable/linux-dmabuf/linux-dmabuf-v1.xml wayland_linux_dmabuf.h
   wayland-scanner private-code $SYSROOT/usr/share/wayland-protocols/stable/linux-dmabuf/linux-dmabuf-v1.xml wayland_linux_dmabuf.c


The resulting files can then be integrated into the project, it's better to re-run ``wayland-scanner`` on
each build to ensure that the correct versions are generated, they must match the version of the ``wayland-client``
dynamically linked library installed on the system.



Window Decorations
******************

.. note::

    As of LVGL v9.5, the `LV_WAYLAND_WINDOW_DECORATIONS` option has been removed.

Window decorations (title bars, borders, close buttons, etc.) are now the responsibility of the application developer, not LVGL.
This change gives you full control over the appearance and behavior of your window decorations.

Creating Window Decorations
---------------------------

You can create your own window decorations using LVGL widgets. For example, use the :ref:`lv_win` widget to add a title bar with buttons,
or build custom decorations from basic widgets like containers, labels, and buttons.

This approach allows you to:
- Design decorations that match your application's style
- Add custom controls and functionality
- Maintain consistent UI across different Wayland compositors
- Have complete control over the look and feel

For applications that don't need decorations (fullscreen, kiosk mode, etc.), simply create your UI without them.



Current state and objectives
****************************

* EGL Support
* Server-side window decorations
* Rotation support for the SHM backend



Bug reports
***********

The wayland driver is currently under construction, bug reports, contributions and feedback are always welcome.

It is however important to create detailed issues when a problem is encountered, logs and screenshots of the problem are of great help.

Please enable ``LV_USE_LOG`` and launch the simulator executable like so

.. code::

  WAYLAND_DEBUG=1 ./path/to/simulator_executable > /tmp/debug 2>&1

This will create a log file called ``debug`` in the ``/tmp`` directory, copy-paste the content of the file in the github issue.
The log file contains LVGL logs and the wayland messages.

Be sure to replicate the problem quickly otherwise the logs become too big
