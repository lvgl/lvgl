.. _opengl_overview:

================
OpenGL Overview
================

Introduction
============

Overview
--------

OpenGL (Open Graphics Library) is a cross-platform, hardware-accelerated graphics API that provides a standardized interface for rendering 2D and 3D graphics.
Originally developed by Silicon Graphics in 1992, OpenGL has become one of the most widely adopted graphics APIs across desktop, mobile, and embedded platforms.

OpenGL Support in LVGL
----------------------

The OpenGL integration consists of two main components:

- OpenGL Display Drivers: Handle output by showing the LVGL rendered content on the display, window, or texture in OpenGL-based environments
- OpenGL Draw Unit (see :ref:`OpenGL Draw Unit <opengles_draw_unit>`): When enabled, LVGL uses OpenGL for hardware-accelerated rendering operations, including texture caching for improved performance

OpenGL support addresses several key use cases:

1. Performance optimization: Leverage GPU acceleration for rendering.
2. Cross-Platform compatibility: OpenGL's wide platform support enables LVGL UIs on desktop, mobile, and embedded systems.
3. Integration with existing applications: Embed LVGL UI elements into games, visualization tools, or other OpenGL applications.

Performance Considerations
--------------------------

The OpenGL implementation provides significant performance benefits in most scenarios:

- Best Performance: UIs with static or infrequently changing content benefit most from texture caching
- Moderate Performance: Dynamic content with predictable patterns still sees improvements
- Potential Overhead: Continuously changing content (e.g., random colors every frame) may perform similarly to software rendering

OpenGL Driver Options
======================


The following drivers can be used with OpenGL

The following drivers can be used and will automatically create and maintain an OpenGL context.

- :ref:`SDL driver <sdl_driver>`
- :ref:`Wayland driver <wayland_driver>`
- :ref:`DRM driver <linux_drm>`
- :ref:`GLFW driver <glfw_driver>`

All drivers except the GLFW driver use EGL (Embedded-System Graphics Library) under the hood.

Additionally, LVGL provides a generic OpenGL driver which the user may use to embed 
OpenGL textures in existing OpenGL applications. This driver assumes an existing OpenGL context
and generates textures with hardware acceleration for integration into custom graphics pipelines.

For complete implementation details, see :ref:`OpenGL driver <opengl_driver>`.

.. _3d_gltf_support:

3D/glTF Support
===============

The glTF module provides support for loading and rendering 3D models using the glTF 2.0 specification within LVGL applications.
This support requires OpenGL ES 2.0 with some extra extensions and provides comprehensive 3D rendering capabilities including PBR materials,
animations, and interactive camera controls for embedded 3D visualization.

For complete implementation details, see :ref:`glTF <gltf>`.

NanoVG Draw Unit
================

The NanoVG draw unit provides a hardware-accelerated 2D vector graphics rendering backend for LVGL.
It leverages GPU capabilities for antialiased path rendering, efficient image compositing, and text rendering.

For complete implementation details, see :ref:`NanoVG Draw Unit <nanovg_draw_unit>`.


