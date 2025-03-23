.. _libpng:

==============
libpng Decoder
==============

**libpng** is an LVGL interface to the the official PNG reference library, which
supports almost all PNG features, is extensible, and has been extensively tested for
over 28 years.

For a detailed introduction, see:  http://www.libpng.org/pub/png/libpng.html .



Install
*******

.. code-block:: bash

    sudo apt install libpng-dev



Adding libpng to Your Project
*****************************

Cmake:

.. code-block:: cmake

    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})



.. _libpng_usage:

Usage
*****

Set :c:macro:`LV_USE_LIBPNG` in ``lv_conf.h`` to ``1``.

See the examples below.

.. |times|  unicode:: U+000D7 .. MULTIPLICATION SIGN

It should be noted that each image of this decoder needs to consume

    width |times| height |times| 4

bytes of RAM, and it needs to be combined with the :ref:`overview_image_caching` feature to
ensure that the memory usage is within a reasonable range. The decoded image is
stored in RGBA pixel format.



.. _libpng_example:

Example
*******

.. include:: ../../examples/libs/libpng/index.rst



.. _libpng_api:

API
***

.. API startswith:  lv_libpng_

