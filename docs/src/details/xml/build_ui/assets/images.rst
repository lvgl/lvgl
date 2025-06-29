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

Usage
*****

.. code-block:: xml

    <images>
        <file name="avatar" src_path="avatar1.png"/>
        <data name="logo" src_path="logo1.png" color-format="rgb565" />
    </images>

- ``<file>`` means that the image source is used as a file path:
- ``<data>`` means that the image is converted to a C array on export.

In both cases in the exported C code global ``const void * <image_name>`` variables are created and in the
initialization function of the Component Library (e.g. ``my_lib_init_gen()``) either the path or
the pointer to the converted :cpp:type:`lv_image_dsc_t` pointers are assigned to that variable.

In :cpp:expr:`lv_image_set_src(image, image_name)` ``image_name`` is used
instead of the path or :cpp:type:`lv_image_dsc_t` pointer.


Registering images
------------------

If the UI is created from XML at runtime and a ``globals.xml`` is parsed, the ``<data>`` tags are skipped
because it is assumed that the user manually created the mapping. This is because the XML parser cannot
automatically map an image like:

.. code-block:: c

   lv_image_dsc_t my_logo;

to

.. code-block:: xml

   <data name="my_logo"/>

To register an image path or data in the XML engine use:

.. code-block:: cpp

   lv_xml_register_image(scope, "image_name", data)

``scope`` is usually ``NULL`` to register the image in the global scope.
To register an image locally for a component you can get its scope with:

.. code-block:: cpp

   lv_xml_component_get_scope("component_name")

After calling this function, when ``"image_name"`` is used as an image source in XML, ``data``
(can be a path or a pointer to an image descriptor) will be used.


Notes for the UI Editor
-----------------------

For simplicity, in the UI |nbsp| Editor's preview, images are always loaded as files.
It makes the preview dynamic so no code export and compilation is needed when an image changes.

