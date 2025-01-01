.. _libpng:

==============
libpng decoder
==============

libpng is the official PNG reference library. It supports almost all PNG features, is extensible, and has been extensively tested for over 28 years.

Detailed introduction: http://www.libpng.org/pub/png/libpng.html

Install
-------

.. code-block:: bash

    sudo apt install libpng-dev

Add libpng to your project
--------------------------

.. code-block:: cmake

    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})

.. _libpng_usage:

Usage
-----

Enable :c:macro:`LV_USE_LIBPNG` in ``lv_conf.h``.

See the examples below.
It should be noted that each image of this decoder needs to consume ``width x height x 4`` bytes of RAM,
and it needs to be combined with the :ref:`overview_image_caching` feature to ensure that the memory usage is within a reasonable range.
The decoded image is stored in RGBA pixel format.

.. _libpng_example:

Example
-------

.. include:: ../../examples/libs/libpng/index.rst

.. _libpng_api:

API
---

:ref:`libpng`

