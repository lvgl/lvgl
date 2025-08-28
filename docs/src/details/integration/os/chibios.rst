=======
ChibiOS
=======

How to integrate LVGL with ChibiOS
**********************************

1. Create a thread for the lv_timer_handler
*******************************************

This thread is responsible for calling the lv_timer_handler() function at the calculated interval.
The interval is determined by the LVGL library based on the current state of the GUI and the timers that are running.

.. code-block:: c

    static THD_WORKING_AREA(waLV_TIMER_HANDLER, 256);
    static THD_FUNCTION(LV_TIMER_HANDLER, arg)
    {
        (void)arg;
        chRegSetThreadName("LV_TIMER_HANDLER");
        uint32_t sleep = 0;
        while (true)
            {
                sleep = lv_timer_handler();
                if (sleep == LV_NO_TIMER_READY) {
                    sleep = LV_DEF_REFR_PERIOD;
                }
                if (sleep <= 0) {
                    sleep = 1;
                }
                chThdSleepMilliseconds(sleep);
            }
    }

2. Timer for acurate tick measurement
*************************************

This tells LVGL how often ticks happen in the system.
Its best to use a timer since those are not affected by the CPU load or thread priorites.
The two safest options would to use a virtual timer that is based on the internal systick or a dedicated hardware timer.


Using hardware timers via the GPT driver.
Most accurate way, but requires a dedicated timer.

.. code-block:: c

    static void gpt_lvgl_tick(GPTDriver *gptp) {
        (void)gptp;
        lv_tick_inc(1);
    }

    static const GPTConfig gptcfg = {
        .frequency = 100000,
        .callback  = gpt_lvgl_tick,
        .cr2       = 0,
        .dier      = 0
    };
    gptStart(&GPTD1, &gptcfg);
    gptStartContinuous(&GPTD1, 100);

Using virtual timers which are based on the systems systick.

.. code-block:: c

    virtual_timer_t vt;

    static void tick_inc_callback(virtual_timer_t vtp, void *p)
    {
        (void)vtp;
        (void)p;

        lv_tick_inc(1);
    }

    chVTSetContinuous(&vt, TIME_MS2I(1), tick_inc_callback, NULL);

3. LTDC driver initialization and start
***************************************

This is the LTDC driver initialization and start.
Configure the configs according to your display, create the required framebuffers.
Please note that if you are using direct mode (see the LVGL LTDC Driver wiki page) , you can use a single framebuffer.
When using partial mode, you will need atleast one fullsize framebuffer and one partial draw buffer. The size of the draw buffer is usually best at 1/10th the size of the framebuffer, but it can be smaller if no performance regression is noticed.


.. code-block:: c

    #include "hal_stm32_ltdc.h"
    #include "../drivers/display/st_ltdc_chibios/lv_st_ltdc_chibios.h" // Required for the reload_event_callback_handler

    uint8_t frame_buffer[600 * 1024 *
                        2] __attribute__((section(".sdram"), aligned(1))); /**< Frame buffer for LTDC. Height * Width * Bytes_per_pixel, additional settings can be applied like section(.sdram) */

    static const ltdc_window_t ltdc_fullscreen_wincfg = {
        .hstart = 0,
        .hstop = 1024 - 1,
        .vstart = 0,
        .vstop = 600 - 1,
    };

    static const ltdc_frame_t ltdc_view_frmcfg1 = {
        .bufferp = frame_buffer,
        .fmt = LTDC_FMT_RGB565,
        .height = 600,
        .width = 1024,
        .pitch = (1024 * 2) /**< Width * bytes per pixel */
    };

    static const ltdc_laycfg_t ltdc_view_laycfg1 = {
        .frame = &ltdc_view_frmcfg1,
        .window = &ltdc_fullscreen_wincfg,
        .def_color = LTDC_COLOR_FUCHSIA,
        .const_alpha = 0xFF,
        .key_color = 0xFFFFFF,
        .pal_colors = NULL,
        .pal_length = 0,
        .blending = 255,
        .flags = (LTDC_BLEND_FIX1_FIX2 | LTDC_LEF_ENABLE)
    };

    static const LTDCConfig ltdc_cfg = {
        /* Display specifications.*/
        .screen_width = 1024, /**< Screen pixel width.*/
        .screen_height = 600, /**< Screen pixel height.*/
        .hsync_width = 1,     /**< Horizontal sync pixel width.*/
        .vsync_height = 3,    /**< Vertical sync pixel height.*/
        .hbp_width = 46,      /**< Horizontal back porch pixel width.*/
        .vbp_height = 23,     /**< Vertical back porch pixel height.*/
        .hfp_width = 40,      /**< Horizontal front porch pixel width.*/
        .vfp_height = 10,     /**< Vertical front porch pixel height.*/
        .flags = 0,           /**< Driver configuration flags.*/

        /* ISR callbacks.*/
        .line_isr = NULL,  /**< Line Interrupt ISR, or @p NULL.*/
        .rr_isr = (ltdc_isrcb_t)reload_event_callback_handler,    /**< Register Reload ISR, or @p NULL.*/
        .fuerr_isr = NULL, /**< FIFO Underrun ISR, or @p NULL.*/
        .terr_isr = NULL,  /**< Transfer Error ISR, or @p NULL.*/

        /* Color and layer settings.*/
        .clear_color = LTDC_COLOR_TEAL,
        .bg_laycfg = &ltdc_view_laycfg1,
        .fg_laycfg = NULL,
    };
    /* Start the LTDC driver.
        Place this in your main code.
        */
    ltdcInit();
    ltdcStart(&LTDCD1, &ltdc_cfg);

4. DMA2D driver initialization and start
****************************************

This is the DMA2D driver initialization and start.
The principal of initialization is the same as for LTDC.

.. code-block:: c

    #include "hal_stm32_dma2d.h"
    #include "../drivers/display/st_ltdc_chibios/lv_st_ltdc_chibios.h" // Required for the transfer_complete_callback_handler

    static const DMA2DConfig dma2d_cfg = {
        /* ISR callbacks.*/
        .cfgerr_isr = NULL,
        .paltrfdone_isr = NULL,
        .palacserr_isr = NULL,
        .trfwmark_isr = NULL,
        .trfdone_isr = (dma2d_isrcb_t)transfer_complete_callback_handler,
        .trferr_isr = NULL
    };

    dma2dInit();
    dma2dStart(&DMA2DD1, &dma2d_cfg);

5. Create the display using direct or partial mode
**************************************************

This is the display creation using direct or partial mode.
The principal of creation is the same as for LTDC.

Partial mode
~~~~~~~~~~~~

    .. code-block:: c

        uint8_t draw_buffer[600 * 1024 * 2] __attribute__((section(".sdram"), aligned(1))); /**< Frame buffer for LTDC. Height * Width * Bytes_per_pixel, additional settings can be applied like section(.sdram). The size doesnt need to be the full screen size. */
        uint8_t draw_buffer_optional[600 * 1024 * 2] __attribute__((section(".sdram"), aligned(1))); /**< Frame buffer for LTDC. Height * Width * Bytes_per_pixel, additional settings can be applied like section(.sdram). The size doesnt need to be the full screen size. */
        
        lv_display_t *disp = lv_st_ltdc_create_partial(draw_buffer, draw_buffer_optional, sizeof(draw_buffer), 0);

Direct mode
~~~~~~~~~~~

    .. code-block:: c

        uint8_t frame_buffer[600 * 1024 *2] __attribute__((section(".sdram"), aligned(1))); /**< Frame buffer for LTDC. Height * Width * Bytes_per_pixel, additional settings can be applied like section(.sdram) */
        uint8_t frame_buffer_optional[600 * 1024 *2] __attribute__((section(".sdram"), aligned(1))); /**< Frame buffer for LTDC. Height * Width * Bytes_per_pixel, additional settings can be applied like section(.sdram) */

        lv_display_t *disp = lv_st_ltdc_create_direct(frame_buffer1, frame_buffer_optional, 0);

6. Mutex handling
*****************

When using a different thread for hadling ui elements, use the LVGL mutex to ensure the main lvgl thread is not accessing the same element at the same time.

.. code-block:: c

    static THD_WORKING_AREA(wa_LVGL_GFX_THREAD, 256);
    static THD_FUNCTION(LVGL_GFX_THREAD, arg)
    {
        (void)arg;
        chRegSetThreadName("LV_TIMER_HANDLER");
        lv_lock();
        ui_init();
        lv_unlock();
        while (true)
        {
            lv_lock();
            update_ui_values();
            lv_unlock();
            chThdSleepMilliseconds(100);
        }
    }

7. Using delegates instead of mutexes
*************************************

If you are using a different thread for handling UI elements, you can use the delegates to handle the UI elements.
This is a more efficient way of handling the UI elements, but requires more work to implement.
Currently the way to use delegate functions on ChibiOS is chDelegateCallDirectX() where X is the number of arguments passed to the delegated function.
X can be a number from 0 to 4



Main thread
~~~~~~~~~~~

.. code-block:: c

   static thread_t *delegate_tp;

   static THD_WORKING_AREA(waLV_TIMER_HANDLER, 8192);
   static THD_FUNCTION(LV_TIMER_HANDLER, arg)
   {
       (void)arg;
       chRegSetThreadName("LV_TIMER_HANDLER");
       uint32_t sleep = 0;
       ui_init();
       while (true)
       {
           sleep = lv_timer_handler();
           if (sleep == LV_NO_TIMER_READY)
           {
               sleep = LV_DEF_REFR_PERIOD;
           }
           if (sleep <= 0)
           {
               sleep = 1;
           }
           chDelegateDispatch();
           chThdSleepMilliseconds(sleep);
       }
   }


Updater thread
~~~~~~~~~~~~~~

.. code-block:: c

   static THD_WORKING_AREA(waValue_Update_Thread, 8192);
   static THD_FUNCTION(Value_Update_Thread, arg)
   {
       (void)arg;
       chRegSetThreadName("Value_Update_Thread");
       uint32_t some_value = 0;
       while (true)
       {
           msg_t result = chDelegateCallDirect2(delegate_tp, update_values, (msg_t)pointer_to_screen_object, (msg_t)some_value);
           some_value++;
           if (some_value > 100)
           {
               some_value = 0;
           }
           chThdSleepMilliseconds(100);
       }
   }


Create the delegate thread
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   delegate_tp = chThdCreateStatic(waLV_TIMER_HANDLER, sizeof(waLV_TIMER_HANDLER), NORMALPRIO + 8, LV_TIMER_HANDLER, NULL);