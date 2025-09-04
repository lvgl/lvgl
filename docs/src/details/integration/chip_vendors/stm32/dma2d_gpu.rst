.. _dma2d:

===========
STM32 DMA2D
===========

STM32 DMA2D is also called Chrom-ART. It will be referred to as DMA2D
on this page.


What is DMA2D
*************

Many STM32s come with a dedicated hardware peripheral for accelerating
pixel data manipulation. It works asynchronously with the CPU. If you
know what DMA is, it's similar to a memory-to-memory DMA engine and can even
be used like one, but allows specifying source and destination stride
and converting the data it's moving from one pixel format to another.
It can blend foregrounds together with backgrounds controlled by a constant
alpha value or from a pixel alpha channel.

LVGL integrates DMA2D as a :ref:`draw unit <draw units>`. During rendering,
the LVGL DMA2D draw unit will take tasks that it can perform. The ones
it cannot perform will be done in parallel by the software draw unit.
The draw tasks that the LVGL DMA2D draw unit can render are fills
and images. In general, the DMA2D draw unit can handle fills and images
that use standard color formats, no gradient, no transforms, and no radius.


Usage
*****

To use DMA2D in your project now, set ``LV_USE_DRAW_DMA2D``
to ``1`` in ``lv_conf.h``. You will need to specify the header for LVGL
to include internally for DMA2D definitions. Set ``LV_DRAW_DMA2D_HAL_INCLUDE``
to the corresponding header. E.g., if your STM32 model is an STM32U5, the
header name will likely be ``"stm32u5xx_hal.h"``. If you're using a framework,
ensure it will not be in contention with LVGL over the DMA2D peripheral.
LVGL uses direct memory register access to manipulate the DMA2D peripheral.
It does not use the STM32 HAL.

Nothing more is required for DMA2D to start working immediately, but
you may want to make some other considerations.

Since DMA2D is implemented in LVGL as a draw unit, it can work in parallel
with other draw units (typically only the software draw unit). By default,
any time that DMA2D is working on a draw task that LVGL depends on the
completion of before continuing, LVGL will spin waiting for
DMA2D to complete. If you would like the CPU to sleep or be scheduled to
other RTOS tasks while a DMA2D transfer is ongoing, do the following:

1. An RTOS/OS must be present. Set ``LV_USE_OS`` to one of the supported values
   in ``lv_conf_template.h`` corresponding to the RTOS you're using.
2. Set ``LV_USE_DRAW_DMA2D_INTERRUPT`` to ``1`` in ``lv_conf.h``.
3. You must call
   :cpp:expr:`lv_draw_dma2d_transfer_complete_interrupt_handler()`
   when you receive the global interrupt that signals
   a DMA2D transfer has completed.


Interop with LTDC and NeoChrom
******************************

DMA2D usage can be freely mixed with LTDC usage as long as ``LV_ST_LTDC_USE_DMA2D_FLUSH``
is **not** enabled. LTDC will use the DMA2D peripheral for flushing, if that is enabled.

NeoChrom and DMA2D may be enabled at the same time. They are both draw units
and they will both independently accept draw tasks.


API
***

.. API startswith:  lv_draw_dma2d_

