.. _nanovg_draw_unit:

================
NanoVG Draw Unit
================

Introduction
============

NanoVG is a lightweight, antialiased 2D vector graphics library built on top of OpenGL/OpenGL ES.
The NanoVG draw unit integrates NanoVG as a hardware-accelerated rendering backend for LVGL,
providing GPU-accelerated drawing for all standard LVGL widgets and graphics primitives.

Unlike the software renderer, NanoVG leverages the GPU for:

- Antialiased path rendering (rectangles, arcs, lines, triangles)
- Hardware-accelerated image compositing with rotation and scaling
- Efficient text rendering with font texture caching
- Box shadows and gradients
- Vector graphics support

Requirements
============

- OpenGL 2.0+ / OpenGL ES 2.0+ / OpenGL ES 3.0+
- An initialized OpenGL context (via GLFW, EGL, or custom setup)
- Stencil buffer support (8-bit recommended)

Configuration
=============

Enable the NanoVG draw unit in ``lv_conf.h``:

.. code-block:: c

    /* Enable NanoVG library */
    #define LV_USE_NANOVG      1

    /* Enable NanoVG draw unit */
    #define LV_USE_DRAW_NANOVG 1

    /* Select OpenGL backend (choose one):
     * - LV_NANOVG_BACKEND_GL2:   OpenGL 2.0
     * - LV_NANOVG_BACKEND_GL3:   OpenGL 3.0+
     * - LV_NANOVG_BACKEND_GLES2: OpenGL ES 2.0
     * - LV_NANOVG_BACKEND_GLES3: OpenGL ES 3.0+
     */
    #define LV_NANOVG_BACKEND   LV_NANOVG_BACKEND_GLES2

    /* Optional: Adjust cache sizes */
    #define LV_NANOVG_IMAGE_CACHE_CNT  32  /* Image texture cache entries */
    #define LV_NANOVG_FBO_CACHE_CNT    8   /* Framebuffer object cache entries */

Supported Features
==================

The NanoVG draw unit supports all standard LVGL drawing operations:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Feature
     - Description
   * - Fill
     - Solid colors, gradients (linear/radial)
   * - Border
     - Rounded rectangles with customizable width
   * - Box Shadow
     - Hardware-accelerated shadow rendering
   * - Images
     - Rotation, scaling, tiling, recoloring
   * - Labels
     - Font rendering with texture atlas caching
   * - Lines
     - Antialiased lines with configurable width
   * - Arcs
     - Antialiased arc segments
   * - Triangles
     - Filled triangles
   * - Masks
     - Rectangle masks for clipping
   * - Layers
     - Off-screen rendering with FBO
   * - Canvas
     - Direct drawing to canvas buffers
   * - Vector Graphics
     - SVG-style path rendering (requires ``LV_USE_VECTOR_GRAPHIC``)

Supported Image Formats
=======================

NanoVG supports zero-copy texture upload for these LVGL color formats:

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - LVGL Format
     - GL Processing
     - Notes
   * - ``LV_COLOR_FORMAT_A8``
     - Alpha texture
     - Color tinting via shader
   * - ``LV_COLOR_FORMAT_ARGB8888``
     - BGR→RGB swizzle
     - Premultiplication handled in shader
   * - ``LV_COLOR_FORMAT_XRGB8888``
     - BGR→RGB + alpha=1
     - X channel ignored
   * - ``LV_COLOR_FORMAT_RGB888``
     - BGR→RGB swizzle
     - No alpha channel
   * - ``LV_COLOR_FORMAT_RGB565``
     - Direct upload
     - Note: LVGL uses BGR565 layout

Performance Tips
================

1. **Minimize Layer Usage**: Each layer requires a framebuffer object (FBO) switch
2. **Use Premultiplied Alpha**: Set ``LV_IMAGE_FLAGS_PREMULTIPLIED`` for pre-processed images
3. **Cache Static Content**: NanoVG caches textures automatically; avoid recreating images
4. **Batch Similar Operations**: Group widgets with similar styles for better GPU batching

Limitations
===========

- **Blur**: Not natively supported; Using this style will not affect the rendering results.
- **Complex Gradients**: Limited to 2-color gradients (LVGL supports multi-stop)
- **Layer Readback**: ``glReadPixels`` for canvas/layer is relatively slow
