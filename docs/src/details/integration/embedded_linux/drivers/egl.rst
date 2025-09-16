.. _egl_driver:

===
EGL
===

Overview
--------

The **EGL** driver provides support for creating LVGL displays using the EGL (Embedded-System Graphics Library) API. 
EGL is a lower-level API that is more closely tied to the underlying drivers of the platform. 
The OpenGL support in LVGL is intended to be portable between different APIs. Currently, there is support for GLFW and EGL. 
Using EGL requires some additional platform integration.

:cpp:func:`lv_opengles_egl_window_create` can be used to create a :cpp:type:`lv_opengles_window_t`
which can be used with the same generic LVGL OpenGL APIs as a GLFW window.

EGL with DRM
------------

EGL can be used together with the DRM driver for hardware-accelerated rendering.

When ``LV_LINUX_DRM_USE_EGL`` is enabled, the DRM driver will automatically set up EGL.  
No additional initialization is required beyond the normal DRM setup.

See :ref:`linux_drm` for configuration and a basic usage example.

EGL without DRM (Experimental)
------------------------------

.. warning::

    This feature is experimental and the API is private. Expect breaking changes.

If you want to use EGL without being tied to DRM, you can enable ``LV_USE_EGL`` using a compiler definition.
This API is currently private and experimental, and people should expect breaking changes.

.. code-block:: bash

    # Enable standalone EGL (experimental)
    -DLV_USE_EGL=1

This allows you to use EGL with your own context management or other platforms, but the API may change 
without notice in future versions.

Render Direct to Window
-----------------------

.. warning::

    This feature is incomplete and has bugs.

Performance can be improved if the LVGL OpenGL driver renders its cached textures directly to the window 
(and :c:macro:`LV_USE_DRAW_OPENGLES` is enabled). This can be done by creating the display with 
:cpp:func:`lv_opengles_window_display_create` instead of :cpp:func:`lv_opengles_texture_create` + 
:cpp:func:`lv_opengles_texture_get_texture_id` + :cpp:func:`lv_opengles_window_add_texture`.
Performance should be better with GLFW than EGL. EGL currently has issues when used this way.

Improving Performance
---------------------

There is a renderer in LVGL which caches software-rendered areas as OpenGL textures.
See :ref:`opengl_texture_caching_renderer` to learn more about it.
