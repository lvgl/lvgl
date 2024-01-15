==================================================================================================
LCD driver SPI bus I/O implementation example for STM32 devices using the STM32Cube HAL SPI driver
==================================================================================================

This is an example implementation of an SPI bus driver using STM32CubeIDE and the ST HAL libraries, tested on a Nucleo-F746ZG board. This code example only implements
the LCD driver specific parts, so you still have to configure the hardware using STM32CubeMX or STM32CubeIDE. It is not meant as the best possible implementation,
but since it uses DMA for the pixel transfer, it has a good performance.

To use this code without change you need to name the appropriate GPIO pins as follows:

.. code:: c

	LCD_RESET	/* Reset */
	LCD_CS		/* Chip Select */
	LCD_DCX		/* Data/Command Select */

The example code uses the SPI1 port. The SPI controller of the STM32F746 is capable of 16-bit transfers, and it can swap the 16-bit pixel data bytes on the fly, so
there is no need to do this in software. This improves the performance considerably.

This code implements a rather simple locking mechanism using a global variable :cpp:var:`my_disp_bus_busy` to prevent accessing the controller while there is a DMA transfer
going on in the background. In a more sophisticated implementation this could be replaced with a semaphore or a transaction queue.

.. code:: c

	#include "stm32f7xx_hal.h"
	#include "lvgl.h"
	
	...
	
	lv_display_t *my_disp;
	volatile int my_disp_bus_busy = 0;

	...
	
	/* DMA transfer ready callback */
	static void my_lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi)
	{
		/* CS high */
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
		my_disp_bus_busy = 0;
		lv_display_flush_ready(my_disp);
	}

	/* Initialize LCD I/O bus, reset LCD */
	static int32_t my_lcd_io_init(void)
	{
		/* Register SPI Tx Complete Callback */
		HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_COMPLETE_CB_ID, stm32_lcd_color_transfer_ready_cb);

		/* reset LCD */
		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

		return HAL_OK;
	}

	/* Send short command to the LCD. This function shall wait until the transaction finishes. */
	static void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
	{
		LV_UNUSED(disp);
		/* Set the SPI in 8-bit mode */
		hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
		HAL_SPI_Init(&hspi1);
		/* DCX low (command) */
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);
		/* CS low */
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		/* send command */
		if (HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
			/* DCX high (data) */
			HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
			/* for short data blocks we use polling transfer */
			HAL_SPI_Transmit(&hspi1, (uint8_t *)param, (uint16_t)param_size, BUS_SPI1_POLL_TIMEOUT);
			/* CS high */
			HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
		}
	}

	/* Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
	static void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
	{
		LV_UNUSED(disp);
		while (my_disp_bus_busy);	/* wait until previous transfer is finished */
		/* Set the SPI in 8-bit mode */
		hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
		HAL_SPI_Init(&hspi1);
		/* DCX low (command) */
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);
		/* CS low */
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		/* send command */
		if (HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
			/* DCX high (data) */
			HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
			/* for color data use DMA transfer */
			/* Set the SPI in 16-bit mode to match endianess */
			hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
			HAL_SPI_Init(&hspi1);
			my_disp_bus_busy = 1;
			HAL_SPI_Transmit_DMA(&hspi1, param, (uint16_t)param_size / 2);

			/* NOTE: CS will be reset in the transfer ready callback */
		}
	}
