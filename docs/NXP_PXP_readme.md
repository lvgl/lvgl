
# HW acceleration for NXP iMX RT platforms using PXP (PiXel Pipeline) engine 

Several drawing features in LVGL can be offloaded to PXP engine. In order to use CPU time while PXP is running, RTOS is required to block LVGL drawing thread and switch to another task, or simply to idle task, where CPU could be suspended to save power. 


#### Features supported:
- RGB565 format only
- Area fill + optional transparency
- BLIT (image transfer) + optional transparency
- Color keying + optional transparency
- Recoloring (color tint) + optional transparency
- RTOS integration layer
- default FreeRTOS and bare metal code provided

#### Basic configuration:
  - enable GPU support in lv_conf.h: Set LV_USE_GPU to 1
  - select NXP PXP engine: Set LV_USE_GPU_NXP_PXP to 1 
  - enable default implementation for interrupt handling and PXP start function: Set LV_USE_GPU_NXP_PXP_DEFAULT_OSA to 1
    - if FSL_RTOS_FREE_RTOS symbol is defined, FreeRTOS implementation will be selected, otherwise bare metal code will be 
      included. 
    
#### Basic initialization: 
  - Initialize PXP module before LVGL library. Default configuration structure for callbacks can be used: 
 
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
        - optional, required only if LV_USE_GPU_NXP_PXP_DEFAULT_OSA is set to 1 
  - PXP related code has dependency on two drivers provided by MCU SDK. These drivers need to be added to project:
      - fsl_pxp.c, fsl_pxp.h: PXP driver 
      - fsl_cache.c, fsl_cache.h: CPU cache handling functions 
      
#### Advanced configuration: 
  - Implementation require multiple OS-specific functions that has to be provided by user. Structure with callbacks is used as a parameter for lv_gpu_nxp_pxp_init() function. Default implmenetation for FreeRTOS and baremetal is provided in lv_gpu_nxp_osa.c. 
      - pxp_interrupt_init(): Initialize PXP interrupt (HW setup, OS setup)
      - pxp_interrupt_deinit(): Deinitialize PXP interrupt (HW setup, OS setup)
      - pxp_run(): Start PXP job (use OS-specific mechanism to block drawing thread, or simply wait for PXP complete interrupt)
  - There are configurable area thresholds which are used to decide whether the area will be processed by CPU, or by PXP. Area smaller then defined value will be processed by CPU, areas bigger than the threshold will be processed by PXP. These thresholds may be defined as a preprocessor variables, otherwise default values will be used from lv_gpu/lv_gpu_nxp_pxp.h
      - GPU_NXP_PXP_BLIT_SIZE_LIMIT: size threshold for image BLIT, BLIT with color keying, and BLIT with recolor (OPA > LV_OPA_MAX)
      - GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT: size threshold for image BLIT and BLIT with color keying with transparency (OPA < LV_OPA_MAX)
      - GPU_NXP_PXP_FILL_SIZE_LIMIT: size threshold for fill operation (OPA > LV_OPA_MAX)
      - GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT: size threshold for fill operation with transparency (OPA < LV_OPA_MAX)
 
