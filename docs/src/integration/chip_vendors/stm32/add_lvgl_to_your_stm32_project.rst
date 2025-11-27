================================
Use LVGL in an STM32 HAL Project
================================

Create the STM32 HAL Project
****************************

If you have an STM32 board with a display mounted to it,
it's best to initialize your project with STM32CubeIDE or STM32CubeMX
by finding your board in the board selector. Sometimes 3rd party boards
have a sample STM32Cube project. The common component of these projects
is the ``.ioc`` file. It describes all the peripheral configurations and pin mappings.
A board with a display is much easier to get started with if the
LCD is already configured in the ``.ioc`` file. If the board LCD is
controlled by the LTDC (LCD-TFT Display Controller) peripheral,
you can use the :ref:`LVGL LTDC driver <stm32 ltdc driver>`.


Add LVGL to your Project
************************

- ``git clone`` or copy LVGL into your project directory.
- In the STM32CubeIDE **Project Explorer** pane: right click on the
  LVGL folder and select **Add/remove include path…**. If
  this doesn't appear or doesn't work, you can review your project
  include paths under the **Project** -> **Properties** menu, and then
  navigating to **C/C++ Build** -> **Settings** -> **Include paths**, and
  ensuring that the LVGL directory is listed.

Now that the source files are included in your project, follow the instructions to
:ref:`add LVGL to your project <adding_lvgl_to_your_project>` and to create the
``lv_conf.h`` file and initialize the display.
Before manually initializing your display though, check to see
if your project uses the LTDC (LCD-TFT Display Controller) peripheral. If it
does, you can simply :ref:`use LVGL's LTDC driver <stm32 ltdc driver>`.


Using LVGL with STM32 HAL
*************************

After reviewing :ref:`Adding LVGL to your project <adding_lvgl_to_your_project>`,
you will appreciate the specifics of using LVGL with an STM32 HAL project
covered here.

tick
----

You will want to set the tick callback to ``HAL_GetTick``.

.. code-block:: c

    lv_tick_set_cb(HAL_GetTick);

In your timer handler loop, you should delay with ``HAL_Delay``.
A good value to delay for is ``2`` ms. Because of the way ``HAL_Delay``
works, a delay of ``1`` actually only delays for between 0 and 1 ms.

.. code-block:: c

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        lv_timer_handler();

        HAL_Delay(2);
    }
    /* USER CODE END 3 */


Display Driver
--------------

Before manually initializing your display, check to see
if your project uses the LTDC (LCD-TFT Display Controller) peripheral. If it
does, you can simply :ref:`use LVGL's LTDC driver <stm32 ltdc driver>`.

If your display is :ref:`one that LVGL has a driver for <display controllers>`,
you should use it so that you have less driver logic to implement in
your project.

:ref:`See this guide for creating an SPI display driver. <lcd_stm32_guide>`


RTOS
----

If you enable FreeRTOS or CMSIS RTOS in your STM32 HAL project, you
can enable LVGL's support for the corresponding RTOS and LVGL will use the
thread creation and synchronization primitives of the respective RTOS when
rendering, depending on the renderers enabled.

Set ``LV_USE_OS`` to ``LV_OS_FREERTOS`` or ``LV_OS_CMSIS_RTOS2``.
