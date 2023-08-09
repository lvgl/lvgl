# Display interface

To register a display for LVGL, a `lv_disp_draw_buf_t` and a `lv_disp_drv_t` variable have to be initialized.
- `lv_disp_draw_buf_t` contains internal graphic buffer(s) called draw buffer(s).
- `lv_disp_drv_t` contains callback functions to interact with the display and manipulate low level drawing behavior.

## Draw buffer

Draw buffer(s) are simple array(s) that LVGL uses to render the screen content.
Once rendering is ready the content of the draw buffer is sent to the display using the `flush_cb` function set in the display driver (see below).

A draw buffer can be initialized via a `lv_disp_draw_buf_t` variable like this:
```c
/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[MY_DISP_HOR_RES * 10];
static lv_color_t buf_2[MY_DISP_HOR_RES * 10];

/*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, MY_DISP_HOR_RES*10);
```

Note that `lv_disp_draw_buf_t` must be a static, global or dynamically allocated variable. It cannot be a local variable as they are destroyed upon end of scope.

As you can see above, the draw buffer may be smaller than the screen. In this case, larger areas are redrawn in smaller segments that fit into the draw buffer(s).
If only a small area changes (e.g. a button is pressed) then only that area will be refreshed.

A larger buffer results in better performance but above 1/10 screen sized buffer(s) there is no significant performance improvement.
Therefore it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized.

## Buffering modes

There are several settings to adjust the number draw buffers and buffering/refreshing modes.

You can measure the performance of different configurations using the [benchmark example](https://github.com/lvgl/lvgl/tree/master/demos/benchmark).

### One buffer
If only one buffer is used LVGL draws the content of the screen into that draw buffer and sends it to the display.
LVGL then needs to wait until the content of the buffer is sent to the display before drawing something new in it.

### Two buffers
If two buffers  are used LVGL can draw into one buffer while the content of the other buffer is sent to the display in the background.
DMA or other hardware should be used to transfer data to the display so the MCU can continue drawing.
This way, the rendering and refreshing of the display become parallel operations.

### Full refresh
In the display driver (`lv_disp_drv_t`) enabling the `full_refresh` bit will force LVGL to always redraw the whole screen. This works in both *one buffer* and *two buffers* modes.
If `full_refresh` is enabled and two screen sized draw buffers are provided, LVGL's display handling works like "traditional" double buffering.
This means the `flush_cb` callback only has to update the address of the framebuffer (`color_p` parameter).
This configuration should be used if the MCU has an LCD controller peripheral and not with an external display controller (e.g. ILI9341 or SSD1963) accessed via serial link. The latter will generally be too slow to maintain high frame rates with full screen redraws.

### Direct mode
If the `direct_mode` flag is enabled in the display driver LVGL will draw directly into a **screen sized frame buffer**. That is the draw buffer(s) needs to be screen sized.
It this case `flush_cb` will be called only once when all dirty areas are redrawn.
With `direct_mode` the frame buffer always contains the current frame as it should be displayed on the screen.
If 2 frame buffers are provided as draw buffers LVGL will alter the buffers but always draw only the dirty areas.
Therefore the 2 buffers needs to synchronized in `flush_cb` like this:
1. Display the frame buffer pointed by `color_p`
2. Copy the redrawn areas from `color_p` to the other buffer.

The get the redrawn areas to copy use the following functions
`_lv_refr_get_disp_refreshing()` returns the display being refreshed
`disp->inv_areas[LV_INV_BUF_SIZE]` contains the invalidated areas
`disp->inv_area_joined[LV_INV_BUF_SIZE]` if 1 that area was joined into another one and should be ignored
`disp->inv_p` number of valid elements in `inv_areas`

## Display driver

Once the buffer initialization is ready a `lv_disp_drv_t` display driver needs to be:
1. initialized with `lv_disp_drv_init(&disp_drv)`
2. its fields need to be set
3. it needs to be registered in LVGL with `lv_disp_drv_register(&disp_drv)`

Note that `lv_disp_drv_t` also needs to be a static, global or dynamically allocated variable.

### Mandatory fields
In the most simple case only the following fields of `lv_disp_drv_t` need to be set:
- `draw_buf` pointer to an initialized `lv_disp_draw_buf_t` variable.
- `hor_res` horizontal resolution of the display in pixels.
- `ver_res` vertical resolution of the display in pixels.
- `flush_cb` a callback function to copy a buffer's content to a specific area of the display.
`lv_disp_flush_ready(&disp_drv)` needs to be called when flushing is ready.
LVGL might render the screen in multiple chunks and therefore call `flush_cb` multiple times. To see if the current one is the last chunk of rendering use `lv_disp_flush_is_last(&disp_drv)`.

### Optional fields
There are some optional display driver data fields:
- `physical_hor_res` horizontal resolution of the full / physical display in pixels. Only set this when _not_ using the full screen (defaults to -1 / same as `hor_res`).
- `physical_ver_res` vertical resolution of the full / physical display in pixels. Only set this when _not_ using the full screen (defaults to -1 / same as `ver_res`).
- `offset_x` horizontal offset from the full / physical display in pixels. Only set this when _not_ using the full screen (defaults to 0).
- `offset_y` vertical offset from the full / physical display in pixels. Only set this when _not_ using the full screen (defaults to 0).
- `color_chroma_key` A color which will be drawn as transparent on chrome keyed images. Set to `LV_COLOR_CHROMA_KEY` from `lv_conf.h` by default.
- `anti_aliasing` use anti-aliasing (edge smoothing). Enabled by default if `LV_COLOR_DEPTH` is set to at least 16 in `lv_conf.h`.
- `rotated` and `sw_rotate` See the [Rotation](#rotation) section below.
- `screen_transp` if `1` the screen itself can have transparency as well. `LV_COLOR_SCREEN_TRANSP` must be enabled in `lv_conf.h` and `LV_COLOR_DEPTH` must be 32.
- `user_data` A custom `void` user data for the driver.
- `full_refresh` always redrawn the whole screen (see above)
- `direct_mode` draw directly into the frame buffer (see above)

Some other optional callbacks to make it easier and more optimal to work with monochrome, grayscale or other non-standard RGB displays:
- `rounder_cb` Round the coordinates of areas to redraw. E.g. a 2x2 px can be converted to 2x8.
It can be used if the display controller can refresh only areas with specific height or width (usually 8 px height with monochrome displays).
- `set_px_cb` a custom function to write the draw buffer. It can be used to store the pixels more compactly in the draw buffer if the display has a special color format. (e.g. 1-bit monochrome, 2-bit grayscale etc.)
This way the buffers used in `lv_disp_draw_buf_t` can be smaller to hold only the required number of bits for the given area size. Note that rendering with `set_px_cb` is slower than normal rendering.
- `monitor_cb` A callback function that tells how many pixels were refreshed and in how much time. Called when the last chunk is rendered and sent to the display.
- `clean_dcache_cb` A callback for cleaning any caches related to the display.
- `render_start_cb` A callback function that notifies the display driver that rendering has started. It also could be used to wait for VSYNC to start rendering. It's useful if rendering is faster than a VSYNC period.

LVGL has built-in support to several GPUs (see `lv_conf.h`) but if something else is required these functions can be used to make LVGL use a GPU:
- `gpu_fill_cb` fill an area in the memory with a color.
- `gpu_wait_cb` if any GPU function returns while the GPU is still working, LVGL will use this function when required to make sure GPU rendering is ready.

### Examples
All together it looks like this:
```c
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
disp_drv.hor_res = 320;                 /*Set the horizontal resolution in pixels*/
disp_drv.ver_res = 240;                 /*Set the vertical resolution in pixels*/

lv_disp_t * disp;
disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
```

Here are some simple examples of the callbacks:
```c
void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to implemented by you.*/
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            put_px(x, y, *color_p);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

void my_gpu_fill_cb(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, const lv_area_t * dest_area, const lv_area_t * fill_area, lv_color_t color);
{
    /*It's an example code which should be done by your GPU*/
    uint32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y < fill_area->y2; y++) {
        for(x = fill_area->x1; x < fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}


void my_rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  /* Update the areas as needed.
   * For example it makes the area to start only on 8th rows and have Nx8 pixel height.*/
   area->y1 = area->y1 & 0x07;
   area->y2 = (area->y2 & 0x07) + 8;
}

void my_set_px_cb(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
   /* Write to the buffer as required for the display.
    * For example it writes only 1-bit for monochrome displays mapped vertically.*/
   buf += buf_w * (y >> 3) + x;
   if(lv_color_brightness(color) > 128) (*buf) |= (1 << (y % 8));
   else (*buf) &= ~(1 << (y % 8));
}

void my_monitor_cb(lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{
  printf("%d px refreshed in %d ms\n", time, ms);
}

void my_clean_dcache_cb(lv_disp_drv_t * disp_drv, uint32)
{
  /* Example for Cortex-M (CMSIS) */
  SCB_CleanInvalidateDCache();
}
```

## Other options

### Rotation

LVGL supports rotation of the display in 90 degree increments. You can select whether you'd like software rotation or hardware rotation.

If you select software rotation (`sw_rotate` flag set to 1), LVGL will perform the rotation for you. Your driver can and should assume that the screen width and height have not changed. Simply flush pixels to the display as normal. Software rotation requires no additional logic in your `flush_cb` callback.

There is a noticeable amount of overhead to performing rotation in software. Hardware rotation is available to avoid unwanted slowdowns. In this mode, LVGL draws into the buffer as if your screen width and height were swapped. You are responsible for rotating the provided pixels yourself.

The default rotation of your display when it is initialized can be set using the `rotated` flag. The available options are `LV_DISP_ROT_NONE`, `LV_DISP_ROT_90`, `LV_DISP_ROT_180`, or `LV_DISP_ROT_270`. The rotation values are relative to how you would rotate the physical display in the clockwise direction. Thus, `LV_DISP_ROT_90` means you rotate the hardware 90 degrees clockwise, and the display rotates 90 degrees counterclockwise to compensate.

(Note for users upgrading from 7.10.0 and older: these new rotation enum values match up with the old 0/1 system for rotating 90 degrees, so legacy code should continue to work as expected. Software rotation is also disabled by default for compatibility.)

Display rotation can also be changed at runtime using the `lv_disp_set_rotation(disp, rot)` API.

If you enable rotation the coordinates of the pointer input devices (e.g. touchpad) will be rotated too.

Note that when using software rotation, you cannot use neither `direct_mode` nor `full_refresh` in the driver. When using either of these, you will have to rotate the pixels yourself e.g. in the `flush_cb`.

Support for software rotation is a new feature, so there may be some glitches/bugs depending on your configuration. If you encounter a problem please open an issue on [GitHub](https://github.com/lvgl/lvgl/issues).

### Decoupling the display refresh timer
Normally the dirty (a.k.a invalid) areas are checked and redrawn in every `LV_DISP_DEF_REFR_PERIOD` milliseconds (set in `lv_conf.h`).
However, in some cases you might need more control on when the display refreshing happen, for example to synchronize rendering with VSYNC or the TE signal.

You can do this in the following way:
```c
/*Delete the original display refresh timer*/
lv_timer_del(disp->refr_timer);
disp->refr_timer = NULL;


/*Call this anywhere you want to refresh the dirty areas*/
_lv_disp_refr_timer(NULL);
```

If you have multiple displays call `lv_disp_set_deafult(disp1);` to select the display to refresh before `_lv_disp_refr_timer(NULL);`.

Note that `lv_timer_handler()` and `_lv_disp_refr_timer()` can not run at the same time.

If the performance monitor is enabled, the value of `LV_DISP_DEF_REFR_PERIOD` needs to be set to be consistent with the refresh period of the display to ensure that the statistical results are correct.

## Further reading

- [lv_port_disp_template.c](https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_disp_template.c) for a template for your own driver.
- [Drawing](/overview/drawing) to learn more about how rendering works in LVGL.
- [Display features](/overview/display) to learn more about higher level display features.

## API

```eval_rst

.. doxygenfile:: lv_hal_disp.h
  :project: lvgl

```
