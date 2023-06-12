# Renesas
The [HMI-Board](https://bit.ly/3I9nfUo) development board SDK now comes with LVGL integration for quick evaluation. Simply download the [SDK](https://github.com/RT-Thread-Studio/sdk-bsp-ra6m3-hmi-board/tree/main/projects/hmi-board-lvgl) for the supported motherboard and you’ll be on your way to creating your next GUI application in no time. For more information, check out the [Software design description](https://github.com/RT-Thread-Studio/sdk-bsp-ra6m3-hmi-board/blob/main/projects/hmi-board-lvgl/README.md).

## Creating new project with LVGL
It is recommended to start your project by downloading the HMI-Board SDK example project. It comes fully equipped with LVGL and dave-2d support (if the modules are present), so you won’t need to do any additional integration work.

## HW acceleration for Renesas RA6M3 platforms
For RA6M3 platforms, hardware acceleration can be achieved using the dave-2d GPU, depending on the platform used. Each accelerator has its own context, allowing them to be used individually or simultaneously in LVGL’s multithreading mode.

### Dave-2d accelerator
LVGL can offload several drawing features to the dave-2d engine, freeing up the CPU for other operations while dave-2d runs. An RTOS is required to block the LVGL drawing thread and switch to another task or suspend the CPU for power savings. Supported draw callbacks can be found in “src/draw/renesas/lv_gpu_d2_ra6m3.c”.

LVGL can offload several drawing features to the dave-2d engine, freeing up the CPU for other operations while dave-2d runs. An RTOS is required to block the LVGL drawing thread and switch to another task or suspend the CPU for power savings. Supported draw callbacks can be found in “src/draw/renesas/lv_gpu_d2_ra6m3.c”.

```c
	ra_2d_draw_ctx->blend = lv_draw_ra6m3_2d_blend;
    ra_2d_draw_ctx->base_draw.draw_img_decoded = lv_port_gpu_img_decoded;
    ra_2d_draw_ctx->base_draw.wait_for_finish = lv_port_gpu_wait;
    ra_2d_draw_ctx->base_draw.draw_letter = lv_draw_gpu_letter;
```

### Features supported:
    All operations can be used in conjunction with optional transparency.

  - RGB565 and ARGB8888 color formats
  - Area fill with color
  - BLIT (BLock Image Transfer)
  - Color conversion
  - Rotate and scale
  - Alpha blending
  - Bilinear filtering
  - RTOS integration layer
  - Default RT-Thread code provided
  - Subpixel exact placement

### Basic configuration:
  - Select Renesas dave-2d engine in lv_conf.h: Set `LV_USE_GPU_RA6M3_G2D` to 1
  - Set referenced header file in lv_conf.h: `#define LV_GPU_RA6M3_G2D_INCLUDE "hal_data.h"`

### RT-Thread Example:

```c
#define COLOR_BUFFER  (LV_HOR_RES_MAX * LV_VER_RES_MAX)

static lv_disp_drv_t disp_drv;

/*A static or global variable to store the buffers*/
static lv_color_t buf_1[COLOR_BUFFER];
```

- After initializing your peripherals (such as SPI, GPIOs, and LCD) in the `lv_port_disp_init()` function, you can initialize LVGL using [`lv_init()`.](https://docs.lvgl.io/master/API/core/lv_obj.html#_CPPv47lv_initv) Next, register the frame buffers using `lv_disp_draw_buf_init()` and create a new display driver using `lv_disp_drv_init()`.

```c
/*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
lv_disp_draw_buf_init(&disp_buf, buf_1, RT_NULL, COLOR_BUFFER);
lv_disp_drv_init(&disp_drv); /*Basic initialization*/

/*Set the resolution of the display*/
disp_drv.hor_res = LV_HOR_RES_MAX;
disp_drv.ver_res = LV_VER_RES_MAX;

/*Set a display buffer*/
disp_drv.draw_buf = &disp_buf;

/*Used to copy the buffer's content to the display*/
disp_drv.flush_cb = disp_flush;

/* Initialize GPU module */
lv_port_gpu_hw_init();

/*Finally register the driver*/
lv_disp_drv_register(&disp_drv);
```

* To run LVGL, you’ll need to create a thread. You can find examples of how to do this using RT-Thread in the `env_support/rt-thread/lv_rt_thread_port.c` file.

```c
static void lvgl_thread_entry(void *parameter)
{
#if LV_USE_LOG
    lv_log_register_print_cb(lv_rt_log);
#endif /* LV_USE_LOG */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_user_gui_init();

    /* handle the tasks of LVGL */
    while(1)
    {
        lv_task_handler();
        rt_thread_mdelay(LV_DISP_DEF_REFR_PERIOD);
    }
}

static int lvgl_thread_init(void)
{
    rt_err_t err;
	
    /* create lvgl thread */
    err = rt_thread_init(&lvgl_thread, "LVGL", lvgl_thread_entry, RT_NULL,
           &lvgl_thread_stack[0], sizeof(lvgl_thread_stack), PKG_LVGL_THREAD_PRIO, 10);
    if(err != RT_EOK)
    {
        LOG_E("Failed to create LVGL thread");
        return -1;
    }
    rt_thread_startup(&lvgl_thread);

    return 0;
}
INIT_ENV_EXPORT(lvgl_thread_init);
```

- The last step is to create a function to output the frame buffer to your LCD. The specifics of this function will depend on the features of your MCU. Here’s an example for a typical MCU interface: `my_flush_cb`.

```c
static void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#ifdef PKG_USING_ILI9341
    lcd_fill_array_spi(area->x1, area->y1, area->x2, area->y2, color_p);
#elif LV_USE_GPU_RA6M3_G2D
    lv_port_gpu_blit(area->x1, area->y1, color_p, area);
#else
    ......
#endif
    lv_disp_flush_ready(disp_drv);
}
```
