.. _sdl_draw_unit:

==============
SDL Draw Unit
==============

************
Introduction
************

Overview
========

The SDL Draw Unit provides a hardware-accelerated rendering backend for LVGL that leverages SDL2's texture system.
It uses software rendering to create SDL textures which are then cached and efficiently blended together by the GPU to compose the final UI.
This approach combines the flexibility of software rendering with the performance benefits of hardware-accelerated texture blending.

Key Features
------------

- **Texture Caching**: Rendered elements are cached as SDL textures, reducing redundant rendering operations
- **Hardware Blending**: SDL's GPU-accelerated texture blending provides smooth compositing
- **Cross-Platform**: Works on any platform that supports SDL2
- **Easy Integration**: Seamless integration with SDL-based LVGL applications

Performance Characteristics
----------------------------

The SDL Draw Unit excels in scenarios with:

- **Best Performance**: Static or infrequently changing UI elements that benefit from texture caching
- **Good Performance**: UIs with moderate animation and updates
- **Consider Alternatives**: Heavily dynamic content that changes every frame may not benefit as much from caching



*************
Prerequisites
*************

- SDL2 library installed (see :ref:`SDL Driver <sdl_driver>` for installation instructions)
- LVGL configured with SDL support (``LV_USE_SDL 1``)



*************
Configuration
*************

Enable in lv_conf.h
===================

.. code-block:: c

    #define LV_USE_SDL 1
    #define LV_USE_DRAW_SDL 1
    #define LV_USE_DRAW_SW 1

The SDL Draw Unit automatically integrates with the SDL display driver when both are enabled.
