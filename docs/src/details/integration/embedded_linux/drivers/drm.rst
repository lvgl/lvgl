.. _linux_drm:

===
DRM
===

Overview
--------

The **DRM** (Direct Rendering Manager) display driver provides support for rendering
LVGL directly to Linux framebuffer devices through the DRM/KMS subsystem.  
It enables running LVGL without a windowing system such as X11 or Wayland,
making it suitable for embedded devices, single-board computers, and direct-to-display
applications.

The DRM driver interacts directly with the GPU or display controller through
``/dev/dri/cardX`` nodes.

Getting Started with DRM
------------------------

Prerequisites
~~~~~~~~~~~~~

The DRM driver requires:

- A Linux system with DRM/KMS support enabled in the kernel.
- Access to a DRM device node, typically ``/dev/dri/card0``.
- Proper permissions to access DRM devices (e.g. running as root or adding the user
  to the ``video`` group).

On Debian/Ubuntu-based systems:

.. code-block:: shell

    sudo apt-get install libdrm-dev

Configure DRM Driver
~~~~~~~~~~~~~~~~~~~~

1. Enable the DRM driver support in ``lv_conf.h``, by CMake compiler define, or by KConfig:

   .. code-block:: c

       #define LV_USE_LINUX_DRM   1

2. Link against ``libdrm`` when building.

.. _linux_drm_basic_usage:

Basic Usage
~~~~~~~~~~~

.. code-block:: c

    #include "lvgl/lvgl.h"
    #include "lvgl/demos/lv_demos.h"

    int main(void)
    {
        /* Initialize LVGL */
        lv_init();

        /* DRM device node */
        const char *device = "/dev/dri/card0";

        /* Create a DRM display */
        lv_display_t *disp = lv_linux_drm_create();

        /* Set DRM device file and connector */
        /* The 2nd argument is the DRM device path */
        /* The 3rd argument is the connector_id (-1 = auto-select first available) */
        lv_linux_drm_set_file(disp, device, -1);

        /* Create demo widgets */
        lv_demo_widgets();

        /* Handle LVGL tasks */
        while (1) {
            uint32_t time_until_next = lv_timer_handler();
            if(time_until_next == LV_NO_TIMER_READY) {
                time_until_next = LV_DEF_REFR_PERIOD;
            }
            lv_delay_ms(time_until_next);
        }

        return 0;
    }

Notes
~~~~~

- ``connector_id`` specifies which display output (HDMI, eDP, DP, etc.) should be used.  
  If ``-1`` is passed, the DRM driver will try to automatically pick the first available connector.
- DRM requires proper modesetting. By default, LVGL will select a preferred display mode.


In order to avoid hard codding the device card path, you can ask LVGL to find a connected one for you using :cpp:func:`lv_linux_drm_find_device_path`.
It will return the first connected card it can find.

.. code-block:: c

    lv_display_t * disp = lv_linux_drm_create();

    /* Find the first connected card in /sys/class/drm */
    char * device = lv_linux_drm_find_device_path();
    lv_linux_drm_set_file(disp, device, -1);
    /* Free the path pointer */
    lv_free(device);


Using DRM with GBM
------------------

The DRM driver can optionally use **GBM** (Generic Buffer Management) for buffer allocation.  
This allows the driver to use GPU-friendly buffer objects instead of simple dumb framebuffers.

1. Enable the following option in your ``lv_conf.h`` (or via Kconfig/CMake):

.. code-block:: c

    #define LV_USE_LINUX_DRM_GBM_BUFFERS 1

2. Link against ``libgbm`` when building.

When this option is enabled:

- Buffers will be allocated using GBM.
- This can improve performance and compatibility on platforms where GBM is supported.



Using DRM with EGL
------------------

The DRM driver can also be combined with :ref:`egl_driver` for hardware-accelerated
rendering via EGL/GLES.

To enable this, set the following options in your ``lv_conf.h`` (or via Kconfig/CMake):

.. code-block:: c

    #define LV_USE_LINUX_DRM             1
    #define LV_USE_LINUX_DRM_GBM_BUFFERS 1
    #define LV_LINUX_DRM_USE_EGL         1
    #define LV_USE_OPENGLES              1
    #define LV_USE_DRAW_OPENGLES         1   /* optional but recommended for performance */

When ``LV_LINUX_DRM_USE_EGL`` is enabled, the DRM driver will automatically initialize EGL.  
No special setup is required beyond the basic DRM initialization shown in :ref:`linux_drm_basic_usage`.

For a detailed overview of EGL usage and configuration, see :ref:`egl_driver`.


Selecting Display Mode
----------------------

.. note::
    Custom mode selection is currently only supported when using DRM with EGL 
    (``LV_LINUX_DRM_USE_EGL`` enabled). When using DRM without EGL, the driver 
    will always use the preferred display mode.

By default, the DRM driver automatically selects the preferred display mode for the connected display. However, you can customize this behavior by providing a mode selection callback.

Custom Mode Selection
~~~~~~~~~~~~~~~~~~~~~

To implement custom mode selection logic, define a callback function and register it with :cpp:func:`lv_linux_drm_set_mode_cb`:

.. code-block:: c

    #include "lvgl/lvgl.h"

    /* Custom mode selection callback */
    size_t my_mode_selector(lv_display_t * disp, const lv_linux_drm_mode_t * modes, size_t mode_count)
    {
        /* Example: Select the first 1920x1080@60Hz mode */
        for(size_t i = 0; i < mode_count; i++) {
            int32_t width = lv_linux_drm_mode_get_horizontal_resolution(&modes[i]);
            int32_t height = lv_linux_drm_mode_get_vertical_resolution(&modes[i]);
            int32_t refresh = lv_linux_drm_mode_get_refresh_rate(&modes[i]);
            
            if(width == 1920 && height == 1080 && refresh == 60) {
                return i;  /* Return the index of the selected mode */
            }
        }
        
        /* Fallback: return the first mode */
        return 0;
    }

    int main(void)
    {
        lv_init();
        
        lv_display_t * disp = lv_linux_drm_create();
        
        /* Set custom mode selection callback */
        lv_linux_drm_set_mode_cb(disp, my_mode_selector);
        
        lv_linux_drm_set_file(disp, "/dev/dri/card0", -1);
        
        /* ... rest of your application ... */
    }

The callback receives an array of available modes and must return the index of the desired mode.

Mode Information API
~~~~~~~~~~~~~~~~~~~~

The following functions are available to query mode properties:

- :cpp:func:`lv_linux_drm_mode_get_horizontal_resolution` - Get width in pixels
- :cpp:func:`lv_linux_drm_mode_get_vertical_resolution` - Get height in pixels
- :cpp:func:`lv_linux_drm_mode_get_refresh_rate` - Get refresh rate in Hz
- :cpp:func:`lv_linux_drm_mode_is_preferred` - Check if mode is the display's preferred/native mode

Example: Selecting Preferred Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    size_t select_preferred_mode(lv_display_t * disp, const lv_linux_drm_mode_t * modes, size_t mode_count)
    {
        /* Find and select the preferred mode */
        for(size_t i = 0; i < mode_count; i++) {
            if(lv_linux_drm_mode_is_preferred(&modes[i])) {
                return i;
            }
        }
        
        /* If no preferred mode found, return the first mode */
        return 0;
    }

Example: Selecting Highest Resolution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    size_t select_highest_resolution(lv_display_t * disp, const lv_linux_drm_mode_t * modes, size_t mode_count)
    {
        size_t best_index = 0;
        int32_t max_pixels = 0;
        
        for(size_t i = 0; i < mode_count; i++) {
            int32_t width = lv_linux_drm_mode_get_horizontal_resolution(&modes[i]);
            int32_t height = lv_linux_drm_mode_get_vertical_resolution(&modes[i]);
            int32_t pixels = width * height;
            
            if(pixels > max_pixels) {
                max_pixels = pixels;
                best_index = i;
            }
        }
        
        return best_index;
    }

Notes
~~~~~

- The mode selection callback is called before the display is initialized.
- If no callback is set, the driver uses the preferred mode by default.
- Ensure the callback always returns a valid index (0 to ``mode_count - 1``).
- To restore default behavior, call :cpp:func:`lv_linux_drm_set_mode_cb` with ``NULL`` as the callback.
