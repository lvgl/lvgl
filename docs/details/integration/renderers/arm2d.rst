.. _arm2d:

==========
Arm-2D GPU
==========

Arm-2D is not a GPU but **an abstraction layer for 2D GPUs dedicated to
Microcontrollers**. It supports all Cortex-M processors ranging from
Cortex-M0 to the latest Cortex-M85.

Arm-2D accelerates LVGL9 with two modes: **Synchronous Mode** and
**Asynchronous Mode**.

- When **Helium** and **ACI (Arm Custom Instruction)** are available, it is recommend
  to use **Synchronous Mode** to accelerate LVGL.
- When Arm-2D backed 2D-GPUs are available, for example, **DMAC-350 based 2D
  GPUs**, it is recommend to use **Asynchronous Mode** to accelerate LVGL.

Arm-2D is an open-source project on GitHub. For more, please refer to:
https://github.com/ARM-software/Arm-2D.


How to Use
**********

In general:

- you can set the macro :c:macro:`LV_USE_DRAW_ARM2D_SYNC` to ``1`` and
  :c:macro:`LV_DRAW_SW_ASM` to ``LV_DRAW_SW_ASM_HELIUM`` in ``lv_conf.h`` to
  enable Arm-2D synchronous acceleration for LVGL.
- You can set
  the macro :c:macro:`LV_USE_DRAW_ARM2D_ASYNC` to ``1`` in ``lv_conf.h`` to enable
  Arm-2D Asynchronous acceleration for LVGL.

If you are using
`CMSIS-Pack <https://github.com/lvgl/lvgl/tree/master/env_support/cmsis-pack>`__
to deploy the LVGL. You don't have to define the macro
:c:macro:`LV_USE_DRAW_ARM2D_SYNC` manually, instead the lv_conf_cmsis.h will
check the environment and set the :c:macro:`LV_USE_DRAW_ARM2D_SYNC` accordingly.

Design Considerations
*********************

As mentioned before, Arm-2D is an abstraction layer for 2D GPU; hence if
there is no accelerator or dedicated instruction set (such as Helium or
ACI) available for Arm-2D, it provides negligible performance boost for
LVGL (sometimes worse) for regular Cortex-M processors.

**We highly recommend you enable Arm-2D acceleration for LVGL** when:

-  The target processors are **Cortex-M55**, **Cortex-M52** and **Cortex-M85**
-  The target processors support
   `Helium <https://developer.arm.com/documentation/102102/0103/?lang=en>`__.
-  The device vendor provides an arm-2d compliant driver for their
   proprietary 2D accelerators and/or ACI (Arm Customized Instruction).
-  The target device contains
   `DMAC-350 <https://community.arm.com/arm-community-blogs/b/internet-of-things-blog/posts/arm-corelink-dma-350-next-generation-direct-memory-access-for-endpoint-ai>`__

Examples
********

-  `A Cortex-M55 (supports Helium) based MDK Project, PC emulation is
   available. <https://github.com/lvgl/lv_port_an547_cm55_sim>`__

API
***

:ref:`lv_draw_sw_arm2d_h`

:ref:`lv_blend_arm2d_h`
