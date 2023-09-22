==============
libpng decoder
==============

libpng is the official PNG reference library. It supports almost all PNG features, is extensible, and has been extensively tested for over 28 years.
Detailed introduction: `libpng <http://www.libpng.org/pub/png/libpng.html>`__.

Install
-------

.. code:: bash

    sudo apt install libpng-dev

Add libpng to your project
--------------------------

.. code:: cmake

    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})

Usage
-----

Enable :c:macro:`LV_USE_LIBPNG` in ``lv_conf.h``.

See the examples below.
It should be noted that each image of this decoder needs to consume ``image width x image height x 4`` bytes of RAM, 
and it needs to be combined with the ref:`image-caching` feature to ensure that the memory usage is within a reasonable range.

Example
-------

.. include:: ../examples/libs/libpng/index.rst

API
---

:ref:`libpng`

