# NXP
NXP has integrated LVGL into the MCUXpresso SDK packages for several of their general purpose and crossover
microcontrollers, allowing easy evaluation and migration into your product design.
[Download an SDK for a supported board](https://www.nxp.com/design/software/embedded-software/littlevgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?&tid=vanLITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
today and get started with your next GUI application.

## Creating new project with LVGL
Downloading the MCU SDK example project is recommended as a starting point. It comes fully configured with LVGL (and
with PXP/VGLite support if the modules are present), no additional integration work is required.

## HW acceleration for NXP iMX RT platforms
Depending on the RT platform used, the acceleration can be done by NXP PXP (PiXel Pipeline) and/or the Verisilicon GPU
through an API named VGLite. There is a single NXP draw context that covers both GPUs allowing to have enabled either
one or even both at the same time. While enableing both 2D accelerators, the VGLite can be used to accelerate widget
drawing while the PXP accelerated blit and fill operations.

Supported draw callbacks are available in "src/draw/nxp/lv_gpu_nxp.c":
```c
    nxp_draw_ctx->base_draw.draw_arc = lv_draw_nxp_arc;
    nxp_draw_ctx->base_draw.draw_rect = lv_draw_nxp_rect;
    nxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_nxp_img_decoded;
    nxp_draw_ctx->blend = lv_draw_nxp_blend;
```

If enabled both GPUs, the PXP is the preffered one to be used for drawing operation. A fallback mechanism is
implemented so that if the feature is not supported by PXP (or if PXP fails), the VGLite will take over to handle the
task. At the end, the CPU will assure that every widget drawing is fully covered (if not already done by GPU).

### PXP accelerator
Several drawing features in LVGL can be offloaded to the PXP engine. The VGLite (if supported) and CPU are available for
other operations while the PXP is running. An RTOS is required to block the LVGL drawing thread and switch to another
task or suspend the CPU for power savings.

#### Features supported:
  - RGB565 and ARGB8888 color formats
  - Area fill + optional transparency
  - BLIT (BLock Image Transfer) + optional transparency
  - Color keying + optional transparency
  - Recoloring (color tint) + optional transparency
  - Image Rotation (90, 180, 270 degree) + optional transparency
  - Recoloring (color tint) + Image Rotation (90, 180, 270 degree) + optional transparency
  - Screen Rotation (90, 180, 270 degree)
  - RTOS integration layer
  - Default FreeRTOS and bare metal code provided

  - Combination of recolor and/or rotation + color key/alpha blend/transparency is supported but PXP needs two steps.
  First step is to recolor/rotate the image to a temporarly buffer (please check LV_MEM_SIZE value for allocation limit)
  and another step is required to handle color keying, alpha chanel or to apply transparency.

#### Known limitations:
  - Rotation is not supported for images unaligned to blocks of 16x16 pixels.
PXP is set to process 16x16 blocks to optimize the system for memory bandwidth and image processing time.
The output engine essentially truncates any output pixels after the desired number of pixels has been written.
When rotating a source image and the output is not divisible by the block size, the incorrect pixels could be truncated
and the final output image can look shifted.

#### Basic configuration:
  - Select NXP PXP engine in lv_conf.h: Set `LV_USE_GPU_NXP_PXP` to 1
  - Enable default implementation for interrupt handling, PXP start function and automatic initialization:
  Set `LV_USE_GPU_NXP_PXP_AUTO_INIT` to 1
  - If `SDK_OS_FREE_RTOS` symbol is defined, FreeRTOS implementation will be used, otherwise bare metal code will be
  included

#### Basic initialization:
  - If `LV_USE_GPU_NXP_PXP_AUTO_INIT` is enabled, no user code is required; PXP is initialized automatically in
  `lv_init()`
  - For manual PXP initialization, default configuration structure for callbacks can be used. Initialize PXP before
  calling `lv_init()`
```c
      #if LV_USE_GPU_NXP_PXP
        #include "src/draw/nxp/pxp/lv_gpu_nxp_pxp.h"
      #endif
      . . .
      #if LV_USE_GPU_NXP_PXP
        PXP_COND_STOP(!lv_gpu_nxp_pxp_init(), "PXP init failed.");
      #endif
```

#### Project setup:
  - Add PXP related files to project:
    - src/draw/nxp/pxp/lv_gpu_nxp_pxp.c, src/draw/nxp/pxp/lv_gpu_nxp_pxp.h: init, uninit, run/wait PXP device, log/trace
    - src/draw/nxp/pxp/lv_draw_pxp_blend.c, src/draw/nxp/pxp/lv_draw_pxp_blend.h: fill and blit (w/o transformation)
    - src/draw/nxp/pxp/lv_gpu_nxp_osa.c, src/draw/nxp/pxp/lv_gpu_osa.h: default implementation of OS-specific functions
    (bare metal and FreeRTOS only)
        - optional, required only if `LV_USE_GPU_NXP_PXP_AUTO_INIT` is set to 1
  - PXP related code depends on two drivers provided by MCU SDK. These drivers need to be added to project:
      - fsl_pxp.c, fsl_pxp.h: PXP driver
      - fsl_cache.c, fsl_cache.h: CPU cache handling functions

#### Logging:
  - By default, LV_GPU_NXP_PXP_LOG_ERRORS is enabled so that any PXP error will be seen on LVGL output
  - For tracing logs about the PXP limitations or size thresholds, the user can enable LV_GPU_NXP_PXP_LOG_TRACES

#### Advanced configuration:
  - Implementation depends on multiple OS-specific functions. The struct `lv_nxp_pxp_cfg_t` with callback pointers is
  used as a parameter for the `lv_gpu_nxp_pxp_init()` function. Default implementation for FreeRTOS and baremetal is
  provided in lv_gpu_nxp_osa.c
      - `pxp_interrupt_init()`: Initialize PXP interrupt (HW setup, OS setup)
      - `pxp_interrupt_deinit()`: Deinitialize PXP interrupt (HW setup, OS setup)
      - `pxp_run()`: Start PXP job. Use OS-specific mechanism to block drawing thread. PXP must finish drawing before
      leaving this function.
  - There are configurable area thresholds which are used to decide whether the area will be processed by CPU or by PXP.
  Areas smaller than a defined value will be processed by CPU and those bigger than the threshold will be processed by
  PXP. These thresholds may be defined as preprocessor variables. Default values are defined in lv_draw_pxp_blend.h
      - `LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT`: size threshold for image BLIT, BLIT with color keying, BLIT with recolor and
      BLIT with rotation (OPA >= LV_OPA_MAX)
      - `LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT`: size threshold for image BLIT, BLIT with color keying, BLIT with recolor
      and BLIT with rotation and transparency (OPA < LV_OPA_MAX)
      - `LV_GPU_NXP_PXP_FILL_SIZE_LIMIT`: size threshold for fill operation (OPA >= LV_OPA_MAX)
      - `LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT`: size threshold for fill operation with transparency (OPA < LV_OPA_MAX)

### VGLite accelerator
Extra drawing features in LVGL can be handled by the VGLite engine. The PXP (if supported) and CPU are available for
other operations while the VGLite is running. An RTOS is required to block the LVGL drawing thread and switch to another
task or suspend the CPU for power savings.

#### Features supported:
  - RGB565 and ARGB8888 color formats
  - Area fill + optional transparency
  - BLIT (BLock Image Transfer) + optional transparency
  - Image Rotation (any degree with decimal) + optional transparency
  - Image Scale + optional transparency
  - Draw background rectangle with radius or gradient
  - Draw arc
  - RTOS integration layer

#### Basic configuration:
  - Select NXP VGLite engine in lv_conf.h: Set `LV_USE_GPU_NXP_VG_LITE` to 1
  - `SDK_OS_FREE_RTOS` symbol needs to be defined so that the FreeRTOS implementation will be used

#### Basic initialization:
  - Initialize VGLite before calling `lv_init()` by specifying the width/height of tessellation window. Value should be
  a multiple of 16; minimum value is 16 pixels, maximum cannot be greater than frame width. If less than or equal to 0,
  then no tessellation buffer is created, in which case the function is used for a blit init.
```c
      #if LV_USE_GPU_NXP_VG_LITE
        #include "vg_lite.h"
      #endif
      . . .
      #if LV_USE_GPU_NXP_VG_LITE
        VG_LITE_COND_STOP(vg_lite_init(64, 64) != VG_LITE_SUCCESS, "VGLite init failed.");
      #endif
```

#### Project setup:
  - Add VGLite related files to project:
    - src/draw/nxp/vglite/lv_gpu_nxp_vglite.c, src/draw/nxp/vglite/lv_gpu_nxp_vglite.h: buffer init, log/trace
    - src/draw/nxp/vglite/lv_draw_vglite_blend.c, src/draw/nxp/vglite/lv_draw_vglite_blend.h: fill and blit
    (w/o transformation)
    - src/draw/nxp/vglite/lv_draw_vglite_rect.c, src/draw/nxp/vglite/lv_draw_vglite_rect.h: rectangle draw
    - src/draw/nxp/vglite/lv_draw_vglite_arc.c, src/draw/nxp/vglite/lv_draw_vglite_arc.h: arc draw

#### Logging:
  - By default, LV_GPU_NXP_VG_LITE_LOG_ERRORS is enabled so that any VGLite error will be seen on LVGL output
  - For tracing logs about the VGLite limitations, size thresholds or stride alignment, the user can enable
  LV_GPU_NXP_VG_LITE_LOG_TRACES

#### Advanced configuration:
  - There are configurable area thresholds which are used to decide whether the area will be processed by CPU or by
  VGLite. Areas smaller than a defined value will be processed by CPU and those bigger than the threshold will be
  processed by VGLite. These thresholds may be defined as preprocessor variables. Default values are defined in
  lv_draw_vglite_blend.h
      - `LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT`: size threshold for image BLIT, BLIT with scale and BLIT with rotation
      (OPA >= LV_OPA_MAX)
      - `LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT`: size threshold for image BLIT, BLIT with scale and BLIT with rotation
      and transparency (OPA < LV_OPA_MAX)
      - `LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT`: size threshold for fill operation (OPA >= LV_OPA_MAX)
      - `LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT`: size threshold for fill operation with transparency (OPA < LV_OPA_MAX)
