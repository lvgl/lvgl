===============================
OpenGL ES Display/Inputs driver
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

Configure OpenGL driver
-----------------------

1. Required linked libraries: -lGL -lGLEW -lglfw
2. Enable the OpenGL driver support in lv_conf.h, by cmake compiler define or by KConfig
    .. code:: c

        #define LV_USE_OPENGLES  1

Usage
-----

.. code:: c

    #include "lvgl/lvgl.h"
    #include "lvgl/examples/lv_examples.h"
    #include "lvgl/demos/lv_demos.h"

    int main()
    {
        lv_init();

        lv_display_t * disp = lv_glfw_window_create(480, 272);

        lv_indev_t * mouse = lv_glfw_mouse_create();
        lv_indev_set_group(mouse, lv_group_get_default());
        lv_indev_set_display(mouse, disp);

        lv_display_set_default(disp);

        LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
        lv_obj_t * cursor_obj;
        cursor_obj = lv_image_create(lv_screen_active());   /*Create an image object for the cursor */
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);   /*Set the image source*/
        lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

        lv_demo_widgets();

        while (1)
        {
            uint32_t time_till_next = lv_timer_handler();
            lv_delay_ms(time_till_next);
        }

        return 0;
    }

Custom Textures
---------------

The OpenGL driver can draw textures from the user. A third-party library could be
used to add content to a texture and the driver will draw the texture in the window.

For this example, ensure ``LV_COLOR_DEPTH`` is set to ``32`` and build the examples
so that ``img_cogwheel_argb`` is available.

.. code:: c

    #include "lvgl/lvgl.h"
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>

    void custom_texture_example(void)
    {
        LV_IMAGE_DECLARE(img_cogwheel_argb);

        /* only important with multiple displays */
        lv_glfw_window_make_context_current(lv_display_get_default());

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        /* set the texture wrapping/filtering options (on the currently bound texture object) */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /* load and generate the texture */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_cogwheel_argb.header.w, img_cogwheel_argb.header.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, img_cogwheel_argb.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        lv_glfw_texture_t * tex = lv_glfw_texture_add(lv_display_get_default(), texture, img_cogwheel_argb.header.w, img_cogwheel_argb.header.h);
        lv_glfw_texture_set_x(tex, 150);
        lv_glfw_texture_set_y(tex, 100);
    }
