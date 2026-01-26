.. include:: /include/substitutions.txt
.. _xml_images:

======
Images
======

Overview
********

In XML, images are considered external resources that need to be named in order to be referenced in
XML files. Below is how to map images with names.

Registering Images
******************

From File
---------

If the images are used as files (e.g., PNG images loaded from a file system),
they can be simply added to ``globals.xml``:

.. code-block:: xml

    <images>
        <file name="avatar" src_path="images/avatar1.png"/>
        <file name="logo" src_path="images/path/to/my_logo.png"/>
    </images>

When registering ``globals.xml`` with
:cpp:expr:`lv_xml_register_component_from_file("A:path/to/globals.xml")`,
names are automatically mapped to the path.

The images can have relative paths in ``globals.xml``. Before registering ``globals.xml``,
:cpp:expr:`lv_xml_set_default_asset_path("path/prefix/")` can be called to set the parent folder.
The path of the image files will be appended to the path set here.

From Data
---------

If the images are converted to arrays and compiled into the firmware, they need to be
registered explicitly using:

.. code-block:: cpp

   lv_xml_register_image(NULL, "image_name", &my_image)

where ``my_image`` is an :cpp:type:`lv_image_dsc_t`.

After that, it can be used identically to image files:

.. code-block:: xml

    <lv_image src="image_name" align="center"/>

Usage in XML
************

After registration, the images can be referenced by their name:

.. code-block:: xml

    <lv_image src="avatar" align="center"/>

Notes for the UI Editor
***********************

When using LVGL's UI Editor, images can be added to ``globals.xml`` using the
``<data>`` tag instead of ``<file>``.

In this case, the Editor will generate the C array (:cpp:type:`lv_image_dsc_t`)
and also generate code to register the images.

It's also possible to set the target ``color_format="..."`` of the images. All typical
color formats are supported, like i1...i8, a1...a8, rgb565, rgb888, argb8888, etc.
