.. _frogfs:

======
frogfs
======

`frogfs <https://github.com/jkent/frogfs>`_
is a read-only filesystem with customizable compression options.
It is suitable for packaging XML files into your app. See the intro
to :ref:`XML in LVGL <xml_intro>`.
It integrates effortlessly into CMake builds.

A copy of the frogfs source is inside LVGL. It has been modified
to use :cpp:func:`lv_malloc` instead of ``malloc``, and not depend
on any other system headers which may not always be available.
Additionally, an ``xml-filter`` has been added. It is a copy
of the ``html-filter`` but assumes stricter XML parsing rules.
Besides these changes, the upstream frogfs can be used interchangeably.


Setup
*****

Set ``LV_USE_FS_FROGFS`` to ``1`` in your ``lv_conf.h``. Also set
``LV_FS_FROGFS_LETTER`` to a letter like ``'A'`` if
``LV_FS_DEFAULT_DRIVER_LETTER`` is not set.

Create a ``frogfs.yaml`` file in your project directory.
Here is the minimum required content of the file.
``files`` in this case is a directory next to ``frogfs.yaml``
that has all the files you want to include in the filesystem.
The directory structure will be preserved.

.. code-block:: yaml

    collect:
      - files/*


With CMake
----------

However you have included LVGL in CMake, use the same path to
include the frogfs directory in your CMakeLists.txt.
Link the frogfs library to you application, and finally,
tell frogfs to generate a filesystem binary for your application.
This CMakeLists.txt should be in the same directory as ``frogfs.yaml``.

.. code-block:: cmake

    # If the path to LVGL is like this...
    include(${CMAKE_CURRENT_LIST_DIR}/../lvgl/CMakeLists.txt)

    # ...then include frogfs like this
    target_include_directories(lvgl PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/libs/frogfs/include)
    include(${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/libs/frogfs/cmake/standalone.cmake)

    # frogfs source is guarded by an #ifdef LV_BUILD_FROGFS so it won't be built by LVGL accidentally.
    # allow frogfs to build by defining LV_BUILD_FROGFS
    target_compile_definitions(frogfs PRIVATE -DLV_BUILD_FROGFS)


    # link frogfs to your application
    target_link_libraries(lvgl_workspace frogfs)

    # use this frogfs CMake command to tell frogfs to generate the filesystem
    # binary for your application
    target_add_frogfs(lvgl_workspace)


With a Script
-------------

You can invoke ``mkfrogfs.py`` manually. It takes 3 parameters. Use
the ``--help`` flag to show usage information.

.. code-block:: shell

    lvgl/src/libs/frogfs/tools/mkfrogfs.py frogfs.yaml build_dir frogfs.bin


Usage
*****

In your project use paths that refer to the frogfs filesystem
in any LVGL function that takes an LVGL virtual filesystem path.
E.g. if ``LV_FS_FROGFS_LETTER`` is ``'F'``:
:cpp:expr:`lv_image_set_src(img, "F:icon.bin")`, :cpp:expr:`lv_xml_load_all("F:")`,
etc.


Customization
*************

frogfs has various filters which files can be transformed by before they are
packed into the binary. Some filters do not require a decompression step at
runtime.

If using :ref:`XML files <xml_intro>`, try using the ``xml-minifier`` filter.
It is an example of a filter that does not require a decompression step at
runtime because "minification" simply reduces the size of the file while
preserving the way it will be parsed.
The only drawback of using this filter is the incorrect line number will be
logged if there is a parse failure.

.. code-block:: yaml

    collect:
      - files/*

    filter:
      '*.xml':
        - xml-minifier

``xml-minifier`` is a modification of ``html-minifier`` added in LVGL which
assumes more conservative XML parsing rules.

See the README and docs of the `frogfs repo <https://github.com/jkent/frogfs>`_
for more info about filters.

