.. _vg_lite_tvg:

=================
VG-Lite Simulator
=================

LVGL integrates a VG-Lite simulator based on ThorVG.
Its purpose is to simplify the debugging of VG-Lite adaptation and reduce the time spent debugging and locating problems on hardware devices.

It has been integrated into the CI automated compilation and testing process to ensure that the VG-Lite rendering backend can be fully tested after each PR modification.

How It Works
************

Sort out the APIs in the ``vg_lite.h`` header file provided by the vendor, re-implement the APIs using `ThorVG <https://github.com/thorvg/thorvg>`_,
and simulate the same rendering images as the real hardware on the simulator.

Configuration
*************

1. Enable VG-Lite rendering backend, see :ref:`vg_lite`.

2. Enable ThorVG and turn on the configuration :c:macro:`LV_USE_THORVG_INTERNAL` or :c:macro:`LV_USE_THORVG_EXTERNAL`.
   It is recommended to use the internal ThorVG library to ensure uniform rendering results.

3. Enable :c:macro:`LV_USE_VG_LITE_THORVG` and set :c:macro:`LV_DRAW_BUF_ALIGN` to 64. The rest of the options can remain default.
   Make sure :c:macro:`LV_VG_LITE_USE_GPU_INIT` is enabled, because the thorvg drawing context needs to be initialized before it can be used.
