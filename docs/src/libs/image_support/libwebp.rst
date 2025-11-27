.. _libwebp:

=================
WebP Decoder
=================

**libwebp** is an LVGL interface to the WebP image format --- a modern image format that provides superior lossless and lossy compression for images on the web. WebP offers:

- Smaller file sizes compared to JPEG and PNG
- Lossy compression with transparency
- Lossless compression
- Animation support (not yet supported in the LVGL integration)

For more information, see: https://developers.google.com/speed/webp

Library source: https://github.com/webmproject/libwebp

.. _libwebp_install:

Install
*******

.. code-block:: bash

    # Linux
    sudo apt install libwebp-dev
    # macOS
    brew install webp

.. _libwebp_integration:

Adding libwebp to Your Project
******************************

CMake configuration:

.. code-block:: cmake

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WebP REQUIRED libwebp)
    include_directories(${WebP_INCLUDEDIR})
    target_link_libraries(main ${WebP_LINK_LIBRARIES})

.. _libwebp_usage:

Usage
*****

Set :c:macro:`LV_USE_LIBWEBP` in ``lv_conf.h`` to ``1``.

Memory requirements for WebP images:

- Lossy WebP: width × height × 4 bytes (ARGB8888 format)
- Lossless WebP: width × height × 4 bytes (ARGB8888 format)

For optimal memory usage, combine with LVGL's :ref:`image caching` feature.

.. _libwebp_example:

Example
*******

.. include:: /examples/libs/libwebp/index.rst

.. _libwebp_api:

API
***

.. API startswith:  lv_libwebp_
