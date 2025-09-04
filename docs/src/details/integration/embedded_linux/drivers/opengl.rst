.. _opengl_driver:

==============
OpenGL Driver
==============

Overview
--------

The **OpenGL** display driver is a generic driver that creates textures for embedding 
LVGL content in other applications. The goal is to create textures that people can 
embed in other applications. The OpenGL context must be created by the user or they 
can use GLFW or EGL as backends.

Getting Started with OpenGL
---------------------------

Prerequisites
~~~~~~~~~~~~~

An OpenGL context must be created before using the OpenGL driver. You can create this using:

- GLFW (see :ref:`GLFW driver <glfw_driver>`)
- EGL (see :ref:`EGL driver <egl_driver>`)
- Your own OpenGL context management

Configure OpenGL Driver
~~~~~~~~~~~~~~~~~~~~~~~

1. Enable the OpenGL driver support in lv_conf.h, by cmake compiler define or by KConfig

    .. code-block:: c

        #define LV_USE_OPENGLES  1

Basic Usage
~~~~~~~~~~~

.. code-block:: c

    #include "lvgl/lvgl.h"

    #define WIDTH 640
    #define HEIGHT 480

    int main()
    {
        /* initialize lvgl */
        lv_init();

        /* NOTE: OpenGL context must be created before this point */

        /* create a display that flushes to a texture */
        lv_display_t * texture = lv_opengles_texture_create(WIDTH, HEIGHT);
        lv_display_set_default(texture);

        /* get the texture ID for use in your application */
        unsigned int texture_id = lv_opengles_texture_get_texture_id(texture);

        /* create Widgets on the screen */
        lv_demo_widgets();

        while (1)
        {
            uint32_t time_until_next = lv_timer_handler();
            if(time_until_next == LV_NO_TIMER_READY) time_until_next = LV_DEF_REFR_PERIOD;
            lv_delay_ms(time_until_next);
            
            /* use texture_id in your OpenGL rendering */
            your_opengl_render_function(texture_id);
        }

        return 0;
    }


.. _opengl_texture_caching_renderer:

OpenGL Texture Caching Renderer
-------------------------------

There is a renderer in LVGL which caches software-rendered areas as OpenGL textures.
The textures are retrieved from the cache and reused when there is a match.
The performance will be drastically improved in most cases.

.. code-block:: c

    #define LV_USE_DRAW_OPENGLES 1

Known Limitations
~~~~~~~~~~~~~~~~~

- Performance will be the same or slightly worse if the drawn areas are never found in the cache
  due to Widgets with continuously varying colors or shapes. One example is a label whose color
  is set to a random value every frame, as in the "Multiple labels" scene of the benchmark demo.
- Layers with transparent pixels and an overall layer transparency will not blend correctly.
  The effect can be observed in the "Containers with opa_layer" scene of the benchmark demo
  in the border corners.
- Layers with rotation are not currently supported. Images with rotation are fine.


