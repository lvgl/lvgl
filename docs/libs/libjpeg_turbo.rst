.. _libjpeg:

=====================
libjpeg-turbo decoder
=====================

**libjpeg-turbo** is a JPEG image codec that uses SIMD instructions to accelerate baseline JPEG compression and decompression on x86, 
x86-64, Arm, PowerPC, and MIPS systems, as well as progressive JPEG compression on x86, x86-64, and Arm systems.

Detailed introduction: https://libjpeg-turbo.org

Library source: https://github.com/libjpeg-turbo/libjpeg-turbo

.. _libjpeg_install:

Install
-------

.. code:: bash

    sudo apt install libjpeg-turbo8-dev

Add libjpeg-turbo to your project
---------------------------------

.. code:: cmake

    find_package(JPEG REQUIRED)
    include_directories(${JPEG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${JPEG_LIBRARIES})

.. _libjpeg_usage:

Usage
-----

Enable :c:macro:`LV_USE_LIBJPEG_TURBO` in ``lv_conf.h``.

See the examples below.
It should be noted that each image of this decoder needs to consume ``image width x image height x 3`` bytes of RAM, 
and it needs to be combined with the :ref:`overview_image_caching` feature to ensure that the memory usage is within a reasonable range.

.. _libjpeg_example:

Example
-------

.. include:: ../examples/libs/libjpeg_turbo/index.rst

.. _libjpeg_api:

API
---

:ref:`libjpeg_turbo`

