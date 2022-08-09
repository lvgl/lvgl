# Arm-2D GPU

Arm-2D is not a GPU but **an abstraction layer for 2D GPUs dedicated to Microcontrollers**. It supports all Cortex-M processors ranging from Cortex-M0 to the latest Cortex-M85. 

Arm-2D is an open-source project on Github. For more, please refer to: https://github.com/ARM-software/Arm-2D.



## How to Use

In general, you can set the macro `LV_USE_GPU_ARM2D` to `1`in `lv_conf.h` to enable Arm-2D acceleration for LVGL.

If you are using **[CMSIS-Pack](https://github.com/lvgl/lvgl/tree/master/env_support/cmsis-pack)** to deploy the LVGL. You don't have to define the macro `LV_USE_GPU_ARM2D` manually, instead, please select the component `GPU Arm-2D` in the **RTE** dialog. This step will define the macro for us. 



## Design Considerations

As mentioned before, Arm-2D is an abstraction layer for 2D GPU; hence if there is no accelerator or dedicated instruction set (such as Helium or ACI) available for Arm-2D, it provides negligible performance boost for LVGL (sometimes worse) for regular Cortex-M processors. 

**We highly recommend you enable Arm-2D acceleration for LVGL** when:

- The target processors are **Cortex-M55** and/or **Cortex-M85**
- The target processors support **[Helium](https://developer.arm.com/documentation/102102/0103/?lang=en)**.
- The device vendor provides an arm-2d compliant driver for their propriotory 2D accelerators and/or customized instruction set.
- The target device contains [DMA-350](https://community.arm.com/arm-community-blogs/b/internet-of-things-blog/posts/arm-corelink-dma-350-next-generation-direct-memory-access-for-endpoint-ai)



## Examples

- [A Cortex-M55 (supports Helium) based MDK Project, PC emulation is available.](https://github.com/lvgl/lv_port_an547_cm55_sim)
