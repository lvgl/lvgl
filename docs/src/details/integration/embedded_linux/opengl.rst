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
- OpenGL Draw Unit: When enabled, LVGL uses OpenGL for hardware-accelerated rendering operations, including texture caching for improved performance

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

LVGL provides three OpenGL drivers to suit different application needs and platforms:


1. GLFW Driver
--------------

The GLFW display/input driver offers support for creating
LVGL displays and keyboard/mouse inputs that can be used in an OpenGL context.
The GLFW driver provides automatic window creation and context management with comprehensive input handling for rapid development on PC-like platforms.

For complete implementation details, see :ref:`GLFW driver <glfw_driver>`.

2. EGL Driver
-------------

The EGL display driver offers support for creating
LVGL displays using the EGL (Embedded-System Graphics Library) API that can be used in an OpenGL context.
This driver provides lower-level hardware integration with direct driver access, supporting both DRM-based systems and standalone implementations for embedded platforms.

For complete implementation details, see :ref:`EGL driver <egl_driver>`.

3. Generic OpenGL Driver
------------------------

The generic OpenGL driver offers support for creating
LVGL displays as OpenGL textures that can be embedded in existing OpenGL applications.
This driver assumes an existing OpenGL context and generates textures with hardware acceleration 
for integration into custom graphics pipelines.

For complete implementation details, see :ref:`OpenGL driver <opengl_driver>`.

3D/glTF Support
===============

The glTF extension provides support for loading and rendering 3D models using the glTF 2.0 specification within LVGL applications.
This extension requires OpenGL ES 3.0 and provides comprehensive 3D rendering capabilities including PBR materials,
animations, and interactive camera controls for embedded 3D visualization.

For complete implementation details, see :ref:`glTF <gltf>`.

