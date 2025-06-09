.. _xml_images:

======
Images
======

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

An ``<images>`` section can be added to ``globals.xml`` files.  If present, it
describes how to map images with names.

(Later, it might also be possible in Components and Widgets to define local images to
keep the global space cleaner.)

Currently ``<file>`` is the only supported child element, and ``<convert>`` is not
yet implemented.



Usage
*****

.. code-block:: xml

    <images>
        <file name="avatar" src_path="avatar1.png">
            <convert path="raw/avatar.svg" width="100px" color_format="L8"/>
        </file>

        <data name="logo" src_path="logo1.png" color-format="rgb565" memory="RAM2">
            <convert path="https://foo.com/image.png" width="50%" height="80%"
            color_format="RGB565"/>
        </data>
    </images>

- ``<file>`` means that the image source is used as a file path:
- ``<data>`` means that the image is converted to a C array on export.

In both cases in the exported C code global ``const void * <image_name>`` variables are created and in the
initialization function of the Component Library (e.g. ``my_lib_init_gen()``) either the path or
the pointer to the converted :cpp:type:`lv_image_dsc_t` pointers are assigned to that variable.

In :cpp:expr:`lv_image_set_src(image, image_name)` ``image_name`` is used
instead of the path or :cpp:type:`lv_image_dsc_t` pointer.


For simplicity, in the UI |nbsp| Editor preview, images are always loaded as files.

If the UI is created from XML at runtime and a ``globals.xml`` is parsed, the ``<data>`` tags are skipped
because it is assumed that the user manually created the mapping.  This is because the XML parser cannot
automatically map an image like:

.. code-block:: c

   lv_image_dsc_t my_logo;

to

.. code-block:: xml

   <data name="my_logo"/>


Constants
---------

Constants can be used with images as well.

.. code-block:: xml

    <consts>
        <int name="icon_size" value="32">
            <variant name="size" case="small" value="16"/>
        </int>
    </consts>

    <images>
        <data name="icon_apply" src_path="apply.png">
            <convert path="raw/apply.png" width="#icon_size"/>
        </data>
    </images>



