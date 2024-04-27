===============================
OpenGL ES Display/Inputs driver
===============================

Overview
-------------

| The **OpenGL ES** display/input `driver <https://github.com/lvgl/lvgl/src/drivers/opengles>`__ offers support for simulating the LVGL display and keyboard/mouse inputs in an OpenGL desktop window.
| It is an alternative to **Wayland**, **XCB**, **SDL** or **Qt**.

The main purpose for this driver is for testing/debugging the LVGL application in an **OpenGL** simulation window.

Prerequisites
-------------

The OpenGL driver uses GLEW GLFW to access the OpenGL window manager.

1. Install GLEW: ``sudo apt-get install libglew-dev``
2. Install GLFW: ``sudo apt-get install libglfw3-dev``

Configure OpenGL driver
-----------------------

Enable the OpenGL driver support in lv_conf.h, by cmake compiler define or by KConfig
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

        lv_display_t * disp = lv_opengles_window_create(480, 272);

        lv_indev_t * mouse = lv_opengles_mouse_create();
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
