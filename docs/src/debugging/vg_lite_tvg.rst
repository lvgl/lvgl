.. _vg_lite_tvg:

=====================
VG-Lite GPU Simulator
=====================

LVGL integrates a VG-Lite GPU simulator based on ThorVG.  Its purpose is to simplify
the debugging of VG-Lite GPU adaptation and reduce the time spent debugging and
locating problems on hardware devices.

It has been integrated into the CI automated compilation and testing process to ensure
that the VG-Lite rendering backend can be fully tested after each Pull Request (PR) is
merged into the repository.



How It Works
************

Using the ``vg_lite.h`` header file, ThorVG re-implements the VG-Lite API, generating
the same rendered images as the real VG-Lite GPU hardware.



Configuration
*************

1. Enable VG-Lite rendering backend, see :ref:`vg_lite`.

2. Enable ThorVG and turn on the configuration :c:macro:`LV_USE_THORVG_INTERNAL` or :c:macro:`LV_USE_THORVG_EXTERNAL`.
   It is recommended to use the internal ThorVG library to ensure uniform rendering results.

3. Enable :c:macro:`LV_USE_VG_LITE_THORVG` and set :c:macro:`LV_DRAW_BUF_ALIGN` to 64. The rest of the options can remain default.
   Make sure :c:macro:`LV_VG_LITE_USE_GPU_INIT` is enabled, because the thorvg drawing context needs to be initialized before it can be used.
