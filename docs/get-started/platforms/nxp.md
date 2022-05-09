# NXP
NXP has integrated LVGL into the MCUXpresso SDK packages for several of their general
purpose and crossover microcontrollers, allowing easy evaluation and migration into your
product design. [Download an SDK for a supported board](https://www.nxp.com/design/software/embedded-software/littlevgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?&tid=vanLITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
today and get started with your next GUI application.

## Creating new project with LVGL
Downloading the MCU SDK example project is recommended as a starting point. It comes fully
configured with LVGL (and with PXP support if module is present), no additional integration
work is required.

## Adding HW acceleration for NXP iMX RT platforms using PXP (PiXel Pipeline) engine for existing projects
Several drawing features in LVGL can be offloaded to the PXP engine. The CPU is available for other operations while the PXP is running. An RTOS is required to block the LVGL drawing thread and switch to another task or suspend the CPU for power savings.

#### Features supported:
  - RGB565 color format
  - Area fill + optional transparency
  - BLIT (BLock Image Transfer) + optional transparency
  - Color keying + optional transparency
  - Recoloring (color tint) + optional transparency
  - RTOS integration layer
  - Default FreeRTOS and bare metal code provided

#### Basic configuration:
  - Select NXP PXP engine in lv_conf.h: Set `LV_USE_GPU_NXP_PXP` to 1
  - Enable default implementation for interrupt handling, PXP start function and automatic initialization: Set `LV_USE_GPU_NXP_PXP_AUTO_INIT` to 1
  - If `FSL_RTOS_FREE_RTOS` symbol is defined, FreeRTOS implementation will be used, otherwise bare metal code will be included

#### Basic initialization:
  - If `LV_USE_GPU_NXP_PXP_AUTO_INIT` is enabled, no user code is required; PXP is initialized automatically in `lv_init()`
  - For manual PXP initialization, default configuration structure for callbacks can be used. Initialize PXP before calling `lv_init()`
```c
      #if LV_USE_GPU_NXP_PXP
        #include "lv_gpu/lv_gpu_nxp_pxp.h"
        #include "lv_gpu/lv_gpu_nxp_pxp_osa.h"
      #endif
      . . .
      #if LV_USE_GPU_NXP_PXP
        if (lv_gpu_nxp_pxp_init(&pxp_default_cfg) != LV_RES_OK) {
            PRINTF("PXP init error. STOP.\n");
            for ( ; ; ) ;
        }
      #endif
```

#### Project setup:
  - Add PXP related files to project:
    - lv_gpu/lv_gpu_nxp.c, lv_gpu/lv_gpu_nxp.h: low level drawing calls for LVGL
    - lv_gpu/lv_gpu_nxp_osa.c, lv_gpu/lv_gpu_osa.h: default implementation of OS-specific functions (bare metal and FreeRTOS only)
        - optional, required only if `LV_USE_GPU_NXP_PXP_AUTO_INIT` is set to 1
  - PXP related code depends on two drivers provided by MCU SDK. These drivers need to be added to project:
      - fsl_pxp.c, fsl_pxp.h: PXP driver
      - fsl_cache.c, fsl_cache.h: CPU cache handling functions

#### Advanced configuration:
  - Implementation depends on multiple OS-specific functions. The struct `lv_nxp_pxp_cfg_t` with callback pointers is used
    as a parameter for the `lv_gpu_nxp_pxp_init()` function. Default implementation for FreeRTOS and baremetal is provided in lv_gpu_nxp_osa.c
      - `pxp_interrupt_init()`: Initialize PXP interrupt (HW setup, OS setup)
      - `pxp_interrupt_deinit()`: Deinitialize PXP interrupt (HW setup, OS setup)
      - `pxp_run()`: Start PXP job. Use OS-specific mechanism to block drawing thread. PXP must finish drawing before leaving this function.
  - There are configurable area thresholds which are used to decide whether the area will be processed by CPU, or by PXP. Areas smaller than a
    defined value will be processed by CPU and those bigger than the threshold will be processed by PXP. These thresholds may be defined as
    preprocessor variables. Default values are defined lv_gpu/lv_gpu_nxp_pxp.h
      - `GPU_NXP_PXP_BLIT_SIZE_LIMIT`: size threshold for image BLIT, BLIT with color keying, and BLIT with recolor (OPA > LV_OPA_MAX)
      - `GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT`: size threshold for image BLIT and BLIT with color keying with transparency (OPA < LV_OPA_MAX)
      - `GPU_NXP_PXP_FILL_SIZE_LIMIT`: size threshold for fill operation (OPA > LV_OPA_MAX)
      - `GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT`: size threshold for fill operation with transparency (OPA < LV_OPA_MAX)
