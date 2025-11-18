.. _libjpeg:

=====================
libjpeg-turbo Decoder
=====================

**libjpeg-turbo** is an LVGL interface to the libjpeg-turbo library --- a JPEG image
codec that uses SIMD instructions to accelerate baseline JPEG compression and
decompression on x86, x86-64, Arm, PowerPC, and MIPS systems, as well as progressive
JPEG compression on x86, x86-64, and Arm systems.

On such systems, libjpeg-turbo is generally 2-6x as fast as libjpeg, all else being
equal.

For a detailed introduction, see:  https://libjpeg-turbo.org .

Library source:  https://github.com/libjpeg-turbo/libjpeg-turbo



.. _libjpeg_install:

Install
*******

.. code-block:: bash

    sudo apt install libjpeg-turbo8-dev



Adding  libjpeg-turbo to Your Project
*************************************

Cmake:

.. code-block:: cmake

    find_package(JPEG REQUIRED)
    include_directories(${JPEG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${JPEG_LIBRARIES})



.. _libjpeg_usage:

Usage
*****

Set :c:macro:`LV_USE_LIBJPEG_TURBO` in ``lv_conf.h`` to ``1``.

See the examples below.

.. |times|  unicode:: U+000D7 .. MULTIPLICATION SIGN

It should be noted that each image decoded needs to consume:

    image width |times| image height |times| 3

bytes of RAM, and it needs to be combined with the :ref:`overview_image_caching`
feature to ensure that the memory usage is within a reasonable range.



.. _libjpeg_example:

Example
*******

.. include:: ../../examples/libs/libjpeg_turbo/index.rst



.. _libjpeg_api:

API
***

:ref:`libs/libjpeg_turbo/lv_libjpeg_turbo.h`

