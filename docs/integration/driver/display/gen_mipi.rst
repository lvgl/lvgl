=================================================
Generic MIPI DCS compatible LCD Controller driver
=================================================

Overview
-------------

From the `Wikipedia <https://en.wikipedia.org/wiki/MIPI_Alliance>`__:

	`MIPI Allience <https://www.mipi.org/>`__ is a global business alliance that develops technical specifications
	for the mobile ecosystem, particularly smart phones but including mobile-influenced industries. MIPI was founded in 2003 by Arm, Intel, Nokia, Samsung,
	STMicroelectronics and Texas Instruments.

MIPI Allience published a series of specifications related to display devices, including DBI (Display Bus Interface), DSI (Display Serial Interface) and DCS
(Display Command Set). Usually when one talks about a MIPI-compatible display, one thinks of a device with a DSI serial interface. However, the Display Bus Interface specification
includes a number of other, legacy interfaces, like SPI serial, or i8080-compatible parallel interface, which are often used to interface LCD displays to lower-end microcontrollers.
Furthermore, the DCS specification contains a standard command set, which is supported by a large number of legacy TFT LCD controllers, including the popular Sitronix
(ST7735, ST7789, ST7796) and Ilitek (ILI9341) SOCs. These commands provide a common interface to configure display orientation, color resolution, various power modes, and provide generic video memory access. On top
of that standard command set each LCD controller chip has a number of vendor-specific commands to configure voltage generator levels, timings, or gamma curves.

.. note::

	It is important to understand that this generic MIPI LCD driver is not a hardware driver for displays with the DSI ("MIPI") serial interface. Instead, it implements the MIPI DCS command set used in many LCD controllers with an SPI or i8080 bus, and provides a common framework for chip-specific display controllers.

.. tip::
	Although this is a generic driver, it can be used to support compatible chips which do not have a specific driver.


Prerequisites
-------------

There are no prerequisites.

Configuring the driver
----------------------

Enable the generic MIPI LCD driver support in lv_conf.h, by cmake compiler define or by KConfig

.. code:: c

	#define LV_USE_GENERIC_MIPI  1

.. note::
	``LV_USE_GENERIC_MIPI`` is automatically enabled when a compatible driver is enabled.

Usage
-----

You need to implement two platform-dependent functions:

.. code:: c

	/* Send short command to the LCD. This function shall wait until the transaction finishes. */
	int32_t my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
	{
		...
	}

	/* Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
	int32_t my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
	{
		...
	}

The only difference between the :cpp:func:`my_lcd_send_cmd()` and :cpp:func:`my_lcd_send_color()` functions is that :cpp:func:`my_lcd_send_cmd()` is used to send short commands and it is expected
complete the transaction when it returns (in other words, it should be blocking), while :cpp:func:`my_lcd_send_color()` is only used to send pixel data, and it is recommended to use
DMA to transmit data in the background. More sophisticated methods can be also implemented, like queuing transfers and scheduling them in the background.

Please note that while display flushing is handled by the driver, it is the user's responsibility to call :cpp:func:`lv_display_flush_ready()`
when the color transfer completes. In case of a DMA transfer this is usually done in a transfer ready callback.

.. note::
	While it is acceptable to use a blocking implementation for the pixel transfer as well, performance will suffer.

.. tip::
	Care must be taken to avoid sending a command while there is an active transfer going on in the background. It is the user's responsibility to implement this either
	by polling the hardware, polling a global variable (which is reset at the end of the transfer), or by using a semaphore or other locking mechanism.

Please also note that the driver does not handle the draw buffer allocation, because this may be platform-dependent, too. Thus you need to allocate the buffers and assign them
to the display object as usual by calling :cpp:func:`lv_display_set_buffers()`.

The driver can be used to create multiple displays. In such a configuration the callbacks must be able to distinguish between the displays. Usually one would
implement a separate set of callbacks for each display. Also note that the user must take care of arbitrating the bus when multiple devices are connected to it.

Example
-------

.. note::
	You can find a step-by-step guide and the actual implementation of the callbacks on an STM32F746 using STM32CubeIDE and the ST HAL libraries here: :ref:`lcd_stm32_guide`
	

.. code:: c

	#include "src/drivers/display/st7789/lv_st7789.h"

	#define LCD_H_RES		240
	#define LCD_V_RES		320
	#define LCD_BUF_LINES	60

	lv_display_t *my_disp;

	...

	/* Initialize LCD I/O bus, reset LCD */
	static int32_t my_lcd_io_init(void)
	{
		...
		return HAL_OK;
	}

	/* Send command to the LCD controller */
	static void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
	{
		...
	}

	/* Send pixel data to the LCD controller */
	static void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
	{
		...
	}

	int main(int argc, char ** argv)
	{
		...

		/* Initialize LVGL */
		lv_init();

		/* Initialize LCD bus I/O */
		if (my_lcd_io_init() != 0)
			return;

		/* Create the LVGL display object and the LCD display driver */
		my_disp = lv_lcd_generic_mipi_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);

		/* Set display orientation to landscape */
		lv_display_set_rotation(my_disp, LV_DISPLAY_ROTATION_90);

		/* Configure draw buffers, etc. */
		lv_color_t * buf1 = NULL;
		lv_color_t * buf2 = NULL;

		uint32_t buf_size = LCD_H_RES * LCD_BUF_LINES * lv_color_format_get_size(lv_display_get_color_format(my_disp));

		buf1 = lv_malloc(buf_size);
		if(buf1 == NULL) {
			LV_LOG_ERROR("display draw buffer malloc failed");
			return;
		}
		/* Allocate secondary buffer if needed */
		...

		lv_display_set_buffers(my_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

		ui_init(my_disp);

		while(true) {
			...

			/* Periodically call the lv_timer handler */
			lv_timer_handler();
		}
	}

Advanced topics
---------------

Create flags
^^^^^^^^^^^^

The third argument of the :cpp:func:`lv_lcd_generic_mipi_create()` function is a flag array. This can be used to configure the orientation and RGB ordering of the panel if the
default settings do not work for you. In particular, the generic MIPI driver accepts the following flags:

.. code:: c

	LV_LCD_FLAG_NONE
	LV_LCD_FLAG_MIRROR_X
	LV_LCD_FLAG_MIRROR_Y
	LV_LCD_FLAG_BGR

You can pass multiple flags by ORing them together, e.g., :c:macro:`LV_LCD_FLAG_MIRROR_X | LV_LCD_FLAG_BGR`.

Custom command lists
^^^^^^^^^^^^^^^^^^^^

While the chip-specific drivers do their best to initialize the LCD controller correctly, it is possible, that different TFT panels need different configurations.
In particular a correct gamma setup is crucial for good color reproduction. Unfortunately, finding a good set of parameters is not easy. Usually the manufacturer
of the panel provides some example code with recommended register settings.

You can use the ``my_lcd_send_cmd()`` function to send an arbitrary command to the LCD controller. However, to make it easier to send a large number of parameters
the generic MIPI driver supports sending a custom command list to the controller. The commands must be put into a 'uint8_t' array:

.. code:: c

	static const uint8_t init_cmd_list[] = {
		<command 1>, <number of parameters>, <parameter 1>, ... <parameter N>,
		<command 2>, <number of parameters>, <parameter 1>, ... <parameter N>,
		...
		LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF		/* terminate list: this is required! */
	};

	...

	lv_lcd_generic_mipi_send_cmd_list(my_disp, init_cmd_list);

You can add a delay between the commands by using the pseudo-command ``LV_LCD_CMD_DELAY_MS``, which must be followed by the delay given in 10ms units.
To terminate the command list you must use a delay with a value of ``LV_LCD_CMD_EOF``, as shown above.

See an actual example of sending a command list `here <https://github.com/lvgl/lvgl/src/drivers/display/st7789/lv_st7789.c>`__.
