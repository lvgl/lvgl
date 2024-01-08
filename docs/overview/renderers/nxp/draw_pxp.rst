PXP GPU (2d accelerator)
========================

Several drawing features in LVGL can be offloaded to the PXP engine. The
CPU is available for other operations while the PXP is running. RTOS is
required to block the LVGL drawing thread and switch to another task or
suspend the CPU for power savings.

Supported draw callbacks are available in "src/draw/nxp/pxp/lv_draw_pxp.c":

.. code:: c

    pxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_pxp_img_decoded;
    pxp_draw_ctx->blend = lv_draw_pxp_blend;
    pxp_draw_ctx->base_draw.wait_for_finish = lv_draw_pxp_wait_for_finish;


Features supported
------------------

All operations can be used in conjunction with optional transparency.

- RGB565 and ARGB8888 color formats
- Area fill with color
- BLIT (BLock Image Transfer)
- Screen Rotation (90, 180, 270 degree)
- Color keying
- Recoloring (color tint)
- Image Rotation (90, 180, 270 degree)
- RTOS integration layer
- Default FreeRTOS and bare metal code provided
- Combination of recolor and/or rotation + color key/alpha
  blend/transparency is supported. That is achieved by PXP in two
  steps:

   - First step is to recolor/rotate the image to a temporary buffer (statically allocated)
   - Second step is required to handle color keying, alpha channel or to apply transparency


Known limitations
-----------------

- Rotation is not supported for images unaligned to blocks of 16x16
  pixels. PXP is set to process 16x16 blocks to optimize the system for
  memory bandwidth and image processing time. The output engine
  essentially truncates any output pixels after the desired number of
  pixels has been written. When rotating a source image and the output
  is not divisible by the block size, the incorrect pixels could be
  truncated and the final output image can look shifted.


Basic configuration
-------------------

- Select NXP PXP engine in lv_conf.h: Set :c:macro:`LV_USE_GPU_NXP_PXP` to ``1``
- Enable default implementation for interrupt handling, PXP start
  function and automatic initialization: Set
  :c:macro:`LV_USE_GPU_NXP_PXP_AUTO_INIT` to ``1``
- If :c:macro:`SDK_OS_FREE_RTOS` symbol is defined, FreeRTOS implementation
  will be used, otherwise bare metal code will be included


Basic initialization
--------------------

- If :c:macro:`LV_USE_GPU_NXP_PXP_AUTO_INIT` is enabled, no user code is
  required; PXP is initialized automatically in :cpp:func:`lv_init`
- For manual PXP initialization, default configuration structure for
  callbacks can be used. Initialize PXP before calling :cpp:func:`lv_init`

.. code:: c

    #if LV_USE_GPU_NXP_PXP
    #include "src/draw/nxp/pxp/lv_gpu_nxp_pxp.h"
    #endif
    ...
    #if LV_USE_GPU_NXP_PXP
    PXP_COND_STOP(!lv_gpu_nxp_pxp_init(), "PXP init failed.");
    #endif


Project setup
-------------

- Add PXP related files to project:

   - src/draw/nxp/pxp/lv_draw_pxp.c[.h]: draw context callbacks
   - src/draw/nxp/pxp/lv_draw_pxp_blend.c[.h]: fill and blit (with optional transformation)
   - src/draw/nxp/pxp/lv_gpu_nxp_pxp.c[.h]: init, uninit, run/wait PXP device
   - src/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c[.h]: OS abstraction (FreeRTOS or bare metal)

      - optional, required only if :c:macro:`LV_USE_GPU_NXP_PXP_AUTO_INIT` is set to ``1``

- PXP related code depends on two drivers provided by MCU SDK. These
  drivers need to be added to project:

   - fsl_pxp.c[.h]: PXP driver
   - fsl_cache.c[.h]: CPU cache handling functions


Logging
-------

- By default, :c:macro:`LV_GPU_NXP_PXP_LOG_ERRORS` is enabled so that any PXP error will be seen on SDK debug console
- By default, :c:macro:`LV_GPU_NXP_PXP_LOG_TRACES` is disabled. Enable it for tracing logs (like PXP limitations)


Advanced configuration
----------------------

- Implementation depends on multiple OS-specific functions. The struct
  :cpp:struct:`lv_nxp_pxp_cfg_t` with callback pointers is used as a parameter
  for the :cpp:func:`lv_gpu_nxp_pxp_init` function. Default implementation
  for FreeRTOS and bare metal is provided in lv_gpu_nxp_pxp_osa.c

   - :cpp:func:`pxp_interrupt_init`: Initialize PXP interrupt (HW setup, OS setup)
   - :cpp:func:`pxp_interrupt_deinit`: Deinitialize PXP interrupt (HW setup, OS setup)
   - :cpp:func:`pxp_run`: Start PXP job. Use OS-specific mechanism to block drawing thread.
     PXP must finish drawing before leaving this function.

- Area threshold (size limit) is configurable and used to decide
  whether the area will be processed by PXP or not. Areas smaller than
  the defined value will be processed by CPU and those bigger than the
  threshold will be processed by PXP. The threshold is defined as a
  macro in lv_draw_pxp.c

   - :c:macro:`LV_GPU_NXP_PXP_SIZE_LIMIT`: size threshold for fill/blit (with optional transformation)


API
---

