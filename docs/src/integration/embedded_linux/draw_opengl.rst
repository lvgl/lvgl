.. _opengles_draw_unit:

===================
OpenGL ES Draw Unit
===================

************
Introduction
************

Overview
========

The OpenGL ES Draw Unit provides a hardware-accelerated rendering backend for LVGL that leverages OpenGL ES capabilities.

OpenGL ES (OpenGL for Embedded Systems) is a subset of OpenGL designed for embedded devices, mobile phones, and other resource-constrained platforms.
The OpenGL ES Draw Unit brings GPU-accelerated rendering to LVGL applications on these platforms.

Key Features
------------

- **Hardware Acceleration**: Direct GPU acceleration via OpenGL ES 2.0+
- **Texture Caching**: Rendered elements are cached as OpenGL textures for efficient reuse
- **GPU Blending**: Hardware-accelerated texture composition and blending
- **Embedded-Friendly**: Optimized for resource-constrained embedded systems
- **Wide Platform Support**: Works on mobile, embedded Linux, and other OpenGL ES-compatible platforms

Performance Characteristics
----------------------------

The OpenGL ES Draw Unit provides excellent performance for:

- **Best Performance**: Static UI elements that benefit from texture caching
- **Good Performance**: UIs with moderate animation and dynamic content
- **Embedded Optimization**: Efficient memory usage suitable for embedded systems



*************
Prerequisites
*************

- OpenGL ES 2.0 or higher support on your platform
- A driver which supports OpenGL configured (see :ref:`OpenGL Overview <opengl_overview>`)



*************
Configuration
*************

Enable in lv_conf.h
===================

.. code-block:: c

    #define LV_USE_OPENGLES 1

    #define LV_USE_DRAW_OPENGLES 1

    /* Configurable cache count. Bigger cache will improve performance */
    #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64



********
See Also
********

- :ref:`opengl_overview` - Complete OpenGL integration overview
- :ref:`egl_driver` - EGL Display Driver documentation
- :ref:`nanovg_draw_unit` - Vector graphics rendering option
