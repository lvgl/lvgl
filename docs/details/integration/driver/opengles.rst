.. _opengl_es_driver:

===============================
OpenGL ES Display/Inputs Driver
===============================

Overview
--------

| The **OpenGL ES** display/input `driver <https://github.com/lvgl/lvgl/src/drivers/opengles>`__ offers support for simulating the LVGL display and keyboard/mouse inputs in an desktop window created via GLFW.
| It is an alternative to **Wayland**, **XCB**, **SDL** or **Qt**.

The main purpose for this driver is for testing/debugging the LVGL application in an **OpenGL** simulation window.

Prerequisites
-------------

The OpenGL driver uses GLEW GLFW to access the OpenGL window manager.

1. Install GLEW and GLFW: ``sudo apt-get install libglew-dev libglfw3-dev``

Configure OpenGL Driver
-----------------------

1. Required linked libraries: -lGL -lGLEW -lglfw
2. Enable the OpenGL driver support in lv_conf.h, by cmake compiler define or by KConfig
    .. code-block:: c

        #define LV_USE_OPENGLES  1

Basic Usage
-----------

.. code-block:: c

    #include "lvgl/lvgl.h"
    #include "lvgl/examples/lv_examples.h"
    #include "lvgl/demos/lv_demos.h"

    #define WIDTH 640
    #define HEIGHT 480

    int main()
    {
        /* initialize lvgl */
        lv_init();

        /* create a window and initialize OpenGL */
        lv_glfw_window_t * window = lv_glfw_window_create(WIDTH, HEIGHT, true);

        /* create a display that flushes to a texture */
        lv_display_t * texture = lv_opengles_texture_create(WIDTH, HEIGHT);
        lv_display_set_default(texture);

        /* add the texture to the window */
        unsigned int texture_id = lv_opengles_texture_get_texture_id(texture);
        lv_glfw_texture_t * window_texture = lv_glfw_window_add_texture(window, texture_id, WIDTH, HEIGHT);

        /* get the mouse indev of the window texture */
        lv_indev_t * mouse = lv_glfw_texture_get_mouse_indev(window_texture);

        /* add a cursor to the mouse indev */
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(mouse, cursor_obj);

        /* create Widgets on the screen */
        lv_demo_widgets();

        while (1)
        {
            uint32_t time_until_next = lv_timer_handler();
            lv_delay_ms(time_until_next);
        }

        return 0;
    }

Advanced Usage
--------------

The OpenGL driver can draw textures from the user. A third-party library could be
used to add content to a texture and the driver will draw the texture in the window.

.. code-block:: c

    #include "lvgl/lvgl.h"
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>

    #define WIDTH 640
    #define HEIGHT 480

    void custom_texture_example(void)
    {
        /*****************
        *  MAIN WINDOW
        *****************/

        /* create a window and initialize OpenGL */
        /* multiple windows can be created */
        lv_glfw_window_t * window = lv_glfw_window_create(WIDTH, HEIGHT, true);

        /****************************
        *   OPTIONAL MAIN TEXTURE
        ****************************/

        /* create a main display that flushes to a texture */
        lv_display_t * main_texture = lv_opengles_texture_create(WIDTH, HEIGHT);
        lv_display_set_default(main_texture);

        /* add the main texture to the window */
        unsigned int main_texture_id = lv_opengles_texture_get_texture_id(main_texture);
        lv_glfw_texture_t * window_main_texture = lv_glfw_window_add_texture(window, main_texture_id, WIDTH, HEIGHT);

        /* get the mouse indev of this main texture */
        lv_indev_t * main_texture_mouse = lv_glfw_texture_get_mouse_indev(window_main_texture);

        /* add a cursor to the mouse indev */
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(main_texture_mouse, cursor_obj);

        /* create Widgets on the screen of the main texture */
        lv_demo_widgets();

        /**********************
        *   ANOTHER TEXTURE
        **********************/

        /* create a sub display that flushes to a texture */
        const int32_t sub_texture_w = 300;
        const int32_t sub_texture_h = 300;
        lv_display_t * sub_texture = lv_opengles_texture_create(sub_texture_w, sub_texture_h);

        /* add the sub texture to the window */
        unsigned int sub_texture_id = lv_opengles_texture_get_texture_id(sub_texture);
        lv_glfw_texture_t * window_sub_texture = lv_glfw_window_add_texture(window, sub_texture_id, sub_texture_w, sub_texture_h);

        /* create Widgets on the screen of the sub texture */
        lv_display_set_default(sub_texture);
        lv_example_keyboard_2();
        lv_display_set_default(main_texture);

        /* position the sub texture within the window */
        lv_glfw_texture_set_x(window_sub_texture, 250);
        lv_glfw_texture_set_y(window_sub_texture, 150);

        /* optionally change the opacity of the sub texture */
        lv_glfw_texture_set_opa(window_sub_texture, LV_OPA_80);

        /*********************************************
        *   USE AN EXTERNAL OPENGL TEXTURE IN LVGL
        *********************************************/

        unsigned int external_texture_id;
        glGenTextures(1, &external_texture_id);
        glBindTexture(GL_TEXTURE_2D, external_texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        LV_IMAGE_DECLARE(img_cogwheel_argb);
    #if LV_COLOR_DEPTH == 8
        const int texture_format = GL_R8;
    #elif LV_COLOR_DEPTH == 16
        const int texture_format = GL_RGB565;
    #elif LV_COLOR_DEPTH == 24
        const int texture_format = GL_RGB;
    #elif LV_COLOR_DEPTH == 32
        const int texture_format = GL_RGBA;
    #else
    #error("Unsupported color format")
    #endif
        glTexImage2D(GL_TEXTURE_2D, 0, texture_format, img_cogwheel_argb.header.w, img_cogwheel_argb.header.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, img_cogwheel_argb.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        /* add the external texture to the window */
        lv_glfw_texture_t * window_external_texture = lv_glfw_window_add_texture(window, external_texture_id, img_cogwheel_argb.header.w, img_cogwheel_argb.header.h);

        /* set the position and opacity of the external texture within the window */
        lv_glfw_texture_set_x(window_external_texture, 20);
        lv_glfw_texture_set_y(window_external_texture, 20);
        lv_glfw_texture_set_opa(window_external_texture, LV_OPA_70);

        /*********************************************
        *   USE AN LVGL TEXTURE IN ANOTHER LIBRARY
        *********************************************/

        lv_refr_now(sub_texture);

        /* the texture is drawn on by LVGL and can be used by anything that uses OpenGL textures */
        third_party_lib_use_texture(sub_texture_id);
    }

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


.. Comment:  The above blank line is necessary for Sphinx to not complain,
    since it looks for the blank line after a bullet list.
