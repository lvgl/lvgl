.. _using images:

============
Using Images
============

The simplest way to use an image in LVGL is to display it with an
:ref:`lv_image` Widget:

.. code-block:: c

    lv_obj_t * icon = lv_image_create(lv_screen_active());

    /* From variable */
    lv_image_set_src(icon, &my_img_dsc);

    /* From file */
    lv_image_set_src(icon, "S:my_icon.bin");

    /* From Unicode string */
    lv_image_set_src(icon, LV_SYMBOL_BATTERY_FULL);

If the image was converted to a variable with the :ref:`online <images_online_converter>`
or :ref:`offline converter <images_offline_converter>`, you should use
:cpp:expr:`LV_IMAGE_DECLARE(my_img_dsc)` to declare the image in the file where
you want to use it.



.. _using_images_examples:

Examples
********

.. include:: /examples/widgets/image/index.rst

.. include:: /examples/libs/bmp/index.rst

.. include:: /examples/libs/libpng/index.rst

