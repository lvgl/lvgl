# NXP
NXP has integrated LVGL into the MCUXpresso SDK packages for general purpose and crossover microcontrollers, allowing
easy evaluation and migration into your product design.
[Download an SDK for a supported board](https://www.nxp.com/design/software/embedded-software/littlevgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?&tid=vanLITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
today and get started with your next GUI application.

## Creating new project with LVGL
Downloading the MCU SDK example project is recommended as a starting point. It comes fully configured with LVGL (and
with PXP/VGLite support if the modules are present), no additional integration work is required.

## HW acceleration for NXP iMX RT platforms
Depending on the RT platform used, the acceleration can be done by NXP PXP (PiXel Pipeline) and/or the Verisilicon GPU
through an API named VGLite. Each accelerator has its own context that allows them to be used individually as well
simultaneously (in LVGL multithreading mode).

### PXP accelerator
Several drawing features in LVGL can be offloaded to the PXP engine. The CPU is available for other operations while the
PXP is running. RTOS is required to block the LVGL drawing thread and switch to another task or suspend the CPU for
power savings.

Supported draw callbacks are available in "src/draw/nxp/pxp/lv_draw_pxp.c":
```c
    pxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_pxp_img_decoded;
    pxp_draw_ctx->blend = lv_draw_pxp_blend;
    pxp_draw_ctx->base_draw.wait_for_finish = lv_draw_pxp_wait_for_finish;
    pxp_draw_ctx->base_draw.buffer_copy = lv_draw_pxp_buffer_copy;
```

#### Features supported:
    All operations can be used in conjunction with optional transparency.

  - RGB565 and ARGB8888 color formats
  - Area fill with color
  - BLIT (BLock Image Transfer)
  - Screen Rotation (90, 180, 270 degree)
  - Color keying
  - Recoloring (color tint)
  - Image Rotation (90, 180, 270 degree)
  - Buffer copy
  - RTOS integration layer
  - Default FreeRTOS and bare metal code provided

  - Combination of recolor and/or rotation + color key/alpha blend/transparency is supported.
  That is achieved by PXP in two steps:
    - First step is to recolor/rotate the image to a temporary buffer (statically allocated)
    - Second step is required to handle color keying, alpha channel or to apply transparency

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
    - src/draw/nxp/pxp/lv_draw_pxp.c[.h]: draw context callbacks
    - src/draw/nxp/pxp/lv_draw_pxp_blend.c[.h]: fill and blit (with optional transformation)
    - src/draw/nxp/pxp/lv_gpu_nxp_pxp.c[.h]: init, uninit, run/wait PXP device
    - src/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c[.h]: OS abstraction (FreeRTOS or bare metal)
        - optional, required only if `LV_USE_GPU_NXP_PXP_AUTO_INIT` is set to 1
  - PXP related code depends on two drivers provided by MCU SDK. These drivers need to be added to project:
      - fsl_pxp.c[.h]: PXP driver
      - fsl_cache.c[.h]: CPU cache handling functions

#### Logging:
  - By default, `LV_GPU_NXP_PXP_LOG_ERRORS` is enabled so that any PXP error will be seen on SDK debug console
  - By default, `LV_GPU_NXP_PXP_LOG_TRACES` is disabled. Enable it for tracing logs (like PXP limitations)

#### Advanced configuration:
  - Implementation depends on multiple OS-specific functions. The struct `lv_nxp_pxp_cfg_t` with callback pointers is
  used as a parameter for the `lv_gpu_nxp_pxp_init()` function. Default implementation for FreeRTOS and bare metal is
  provided in lv_gpu_nxp_pxp_osa.c
      - `pxp_interrupt_init()`: Initialize PXP interrupt (HW setup, OS setup)
      - `pxp_interrupt_deinit()`: Deinitialize PXP interrupt (HW setup, OS setup)
      - `pxp_run()`: Start PXP job. Use OS-specific mechanism to block drawing thread. PXP must finish drawing before
      leaving this function.
  - Area threshold (size limit) is configurable and used to decide whether the area will be processed by PXP or not.
  Areas smaller than the defined value will be processed by CPU and those bigger than the threshold will be processed by
  PXP. The threshold is defined as a macro in lv_draw_pxp.c
      - `LV_GPU_NXP_PXP_SIZE_LIMIT`: size threshold for fill/blit (with optional transformation)

### VGLite accelerator
Extra drawing features in LVGL can be handled by the VGLite engine. The CPU is available for other operations while the
VGLite is running. An RTOS is required to block the LVGL drawing thread and switch to another task or suspend the CPU
for power savings.

Supported draw callbacks are available in "src/draw/nxp/vglite/lv_draw_vglite.c":
```c
    vglite_draw_ctx->base_draw.init_buf = lv_draw_vglite_init_buf;
    vglite_draw_ctx->base_draw.draw_line = lv_draw_vglite_line;
    vglite_draw_ctx->base_draw.draw_arc = lv_draw_vglite_arc;
    vglite_draw_ctx->base_draw.draw_rect = lv_draw_vglite_rect;
    vglite_draw_ctx->base_draw.draw_img_decoded = lv_draw_vglite_img_decoded;
    vglite_draw_ctx->blend = lv_draw_vglite_blend;
    vglite_draw_ctx->base_draw.wait_for_finish = lv_draw_vglite_wait_for_finish;
    vglite_draw_ctx->base_draw.buffer_copy = lv_draw_vglite_buffer_copy;
```

#### Features supported:
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
  - Buffer copy

#### Known limitations:
  - Source image alignment:
  The byte alignment requirement for a pixel depends on the specific pixel format. Both buffer address and buffer stride
  must be aligned. As general rule, the alignment is set to 16 pixels. This makes the buffer address alignment to be
  32 bytes for RGB565 and 64 bytes for ARGB8888.
  - For pixel engine (PE) destination, the alignment should be 64 bytes for all tiled (4x4) buffer layouts.
  The pixel engine has no additional alignment requirement for linear buffer layouts (`VG_LITE_LINEAR`).

#### Basic configuration:
  - Select NXP VGLite engine in lv_conf.h: Set `LV_USE_GPU_NXP_VG_LITE` to 1
  - `SDK_OS_FREE_RTOS` symbol needs to be defined so that the FreeRTOS implementation will be used

#### Basic initialization:
  - Initialize VGLite before calling `lv_init()` by specifying the width/height of tessellation window. Value should be
  a multiple of 16; minimum value is 16 pixels, maximum cannot be greater than the frame width. If less than or equal
  to 0, then no tessellation buffer is created, in which case VGLite is initialized only for blitting.
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
    - src/draw/nxp/vglite/lv_draw_vglite.c[.h]: draw context callbacks
    - src/draw/nxp/vglite/lv_draw_vglite_blend.c[.h]: fill and blit (with optional transformation)
    - src/draw/nxp/vglite/lv_draw_vglite_rect.c[.h]: draw rectangle
    - src/draw/nxp/vglite/lv_draw_vglite_arc.c[.h]: draw arc
    - src/draw/nxp/vglite/lv_draw_vglite_line.c[.h]: draw line
    - src/draw/nxp/vglite/lv_vglite_buf.c[.h]: init/get vglite buffer
    - src/draw/nxp/vglite/lv_vglite_utils.c[.h]: function helpers

#### Logging:
  - By default, `LV_GPU_NXP_VG_LITE_LOG_ERRORS` is enabled so that any VGLite error will be seen on SDK debug console
  - By default, `LV_GPU_NXP_VG_LITE_LOG_TRACES` is disabled. Enable it for tracing logs (like blit split workaround or
  VGLite fallback to CPU due to any error on the driver)

#### Advanced configuration:
  - Area threshold (size limit) is configurable and used to decide whether the area will be processed by VGLite or not.
  Areas smaller than the defined value will be processed by CPU and those bigger than the threshold will be processed by
  VGLite. The threshold is defined as a macro in lv_draw_vglite.c
      - `LV_GPU_NXP_VG_LITE_SIZE_LIMIT`: size threshold for fill/blit (with optional transformation)
