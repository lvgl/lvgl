VGLite GPU  (2d accelerator)
============================

Extra drawing features in LVGL can be handled by the VGLite engine. The
CPU is available for other operations while the VGLite is running. An
RTOS is required to block the LVGL drawing thread and switch to another
task or suspend the CPU for power savings.

Supported draw callbacks are available in "src/draw/nxp/vglite/lv_draw_vglite.c":

.. code:: c

    vglite_draw_ctx->base_draw.init_buf = lv_draw_vglite_init_buf;
    vglite_draw_ctx->base_draw.draw_line = lv_draw_vglite_line;
    vglite_draw_ctx->base_draw.draw_arc = lv_draw_vglite_arc;
    vglite_draw_ctx->base_draw.draw_rect = lv_draw_vglite_rect;
    vglite_draw_ctx->base_draw.draw_img_decoded = lv_draw_vglite_img_decoded;
    vglite_draw_ctx->blend = lv_draw_vglite_blend;
    vglite_draw_ctx->base_draw.wait_for_finish = lv_draw_vglite_wait_for_finish;


Features supported
------------------

All operations can be used in conjunction with optional transparency.

- RGB565 and ARGB8888 color formats
- Area fill with color
- BLIT (BLock Image Transfer)
- Image Rotation (any degree with decimal)
- Image Scale
- Draw rectangle background with optional radius or gradient
- Blit rectangle background image
- Draw rectangle border/outline with optional rounded corners
- Draw arc with optional rounded ending
- Draw line or dashed line with optional rounded ending


Known limitations
-----------------

- Source image alignment: The byte alignment requirement for a pixel
  depends on the specific pixel format. Both buffer address and buffer
  stride must be aligned. As general rule, the alignment is set to 16
  pixels. This makes the buffer address alignment to be 32 bytes for
  RGB565 and 64 bytes for ARGB8888.
- For pixel engine (PE) destination, the alignment should be 64 bytes
  for all tiled (4x4) buffer layouts. The pixel engine has no
  additional alignment requirement for linear buffer layouts
  (:c:macro:`VG_LITE_LINEAR`).


Basic configuration
-------------------

- Select NXP VGLite engine in lv_conf.h: Set :c:macro:`LV_USE_GPU_NXP_VG_LITE` to 1
- :c:macro:`SDK_OS_FREE_RTOS` symbol needs to be defined so that the FreeRTOS implementation will be used

Basic initialization
--------------------

- Initialize VGLite before calling :cpp:func:`lv_init` by specifying the
  width/height of tessellation window. Value should be a multiple of
  16; minimum value is 16 pixels, maximum cannot be greater than the
  frame width. If less than or equal to 0, then no tessellation buffer
  is created, in which case VGLite is initialized only for blitting.

.. code:: c

    #if LV_USE_GPU_NXP_VG_LITE
    #include "vg_lite.h"
    #endif
    ...
    #if LV_USE_GPU_NXP_VG_LITE
    VG_LITE_COND_STOP(vg_lite_init(64, 64) != VG_LITE_SUCCESS, "VGLite init failed.");
    #endif

Project setup
-------------

- Add VGLite related files to project:

   - src/draw/nxp/vglite/lv_draw_vglite.c[.h]: draw context callbacks
   - src/draw/nxp/vglite/lv_draw_vglite_blend.c[.h]: fill and blit (with optional transformation)
   - src/draw/nxp/vglite/lv_draw_vglite_rect.c[.h]: draw rectangle
   - src/draw/nxp/vglite/lv_draw_vglite_arc.c[.h]: draw arc
   - src/draw/nxp/vglite/lv_draw_vglite_line.c[.h]: draw line
   - src/draw/nxp/vglite/lv_vglite_buf.c[.h]: init/get vglite buffer
   - src/draw/nxp/vglite/lv_vglite_utils.c[.h]: function helpers

Logging
-------

- By default, :c:macro:`LV_GPU_NXP_VG_LITE_LOG_ERRORS` is enabled so that any VGLite error will be seen on SDK debug console
- By default, :c:macro:`LV_GPU_NXP_VG_LITE_LOG_TRACES` is disabled. Enable it
  for tracing logs (like blit split workaround or VGLite fallback to CPU due to any error on the driver)

Advanced configuration
----------------------

- Area threshold (size limit) is configurable and used to decide
  whether the area will be processed by VGLite or not. Areas smaller
  than the defined value will be processed by CPU and those bigger than
  the threshold will be processed by VGLite. The threshold is defined
  as a macro in lv_draw_vglite.c

   - :c:macro:`LV_GPU_NXP_VG_LITE_SIZE_LIMIT`: size threshold for fill/blit (with optional transformation)

API
---

