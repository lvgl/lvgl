# Display interface

To create a display for LVGL call `lv_disp_t * disp = lv_disp_create(hor_res, ver_res)`. You can create a multiple displays and a different driver for each (see below),


## Basic setup

Draw buffer(s) are simple array(s) that LVGL uses to render the screen's content.
Once rendering is ready the content of the draw buffer is sent to the display using the `flush_cb` function.


### flush_cb
An example `flush_cb` looks like this:
```c
void my_flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * buf)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to be implemented by you.*/
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            put_px(x, y, *color_p);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
    lv_disp_flush_ready(disp);
}
```

Use `lv_disp_set_flush_cb(disp, my_flush_cb)` to set a new `flush_cb`.

`lv_disp_flush_ready(disp)` needs to be called when flushing is ready to inform LVGL the buffer is not used anymore by the driver and it can render new content into it.

LVGL might render the screen in multiple chunks and therefore call `flush_cb` multiple times. To see if the current one is the last chunk of rendering use `lv_disp_flush_is_last(disp)`.


### Draw buffers
The draw buffers can be set with
`lv_disp_set_draw_buffers(disp, buf1, buf2, buf_size_px, render_mode);`

- `buf1` a bufer where LVGL can render
- `buf2` a second optional buffer (see more details below)
- `buf_size_px` size of the buffers in pixels
- `render_mode`
  - `LV_DISP_RENDER_MODE_PARTIAL` Use the buffer(s) to render the screen is smaller parts. This way the buffers can be smaller then the display to save RAM. At least 1/10 sceen size buffer(s) are recommended. In `flush_cb` the rendered images needs to be copied to the given area of the display.
   - `LV_DISP_RENDER_MODE_DIRECT` The buffer(s) has to be screen sized and LVGL will render into the correct location of the buffer. This way the buffer always contain the whole image.
   - `LV_DISP_RENDER_MODE_FULL` The buffer can smaller or screen sized but LVGL will always redraw the whole screen even is only 1 pixel has been changed. If two screen sized draw buffers are provided, LVGL's display handling works like "traditional" double buffering. This means the `flush_cb` callback only has to update the address of the framebuffer (`color_p` parameter).

Example:
```c
static lv_color_t buf[LCD_HOR_RES * LCD_VER_RES / 10];
lv_disp_set_draw_buffers(disp, buf, NULL, LCD_HOR_RES * LCD_VER_RES / 10, LV_DISP_RENDER_MODE_PARTIAL);
```

#### One buffer
If only one buffer is used LVGL draws the content of the screen into that draw buffer and sends it to the display via the `flush_cb`.
LVGL then needs to wait until the content of the buffer is sent to the display before drawing something new into it.

#### Two buffers
If two buffers  are used LVGL can draw into one buffer while the content of the other buffer is sent to the display in the background.
DMA or other hardware should be used to transfer data to the display so the MCU can continue drawing.
This way, the rendering and refreshing of the display become parallel operations.


## Advnaced options

To set the resolution of the display after creation use `lv_disp_set_res(disp, hor_res, ver_res);`

It's not mandatory to use the whole display for LVGL, however in some cases the physical resolution is important. For example the touchpad still sees the whole resolution and the values needs to be converted
to the active LVGL display area. So the physical resoltution and the offset of the active area can be set with `lv_disp_set_physical_res(disp, hor_res, ver_res);`and `lv_disp_set_offset(disp, x, y);`

The orientation of the display can be changed with `lv_disp_set_rotation(disp, LV_DISP_ROTATION_0/90/180/270, true/false)`.  LVGL will swap the horizontal and vertical resolutions internally according to the set degree. IF the last paramter is `true` LVGL will rotate the rendered image. If it's `false` the display driver should rotate the rendered image.

 * Set the color format of the display.
 * If set to not `LV_COLOR_FORMAT_NATIVE` the draw_ctx's `buffer_convert` function will be used
 * to convert the rendered content to the desired color format

- `anti_aliasing` use anti-aliasing (edge smoothing). Enabled by default if `LV_COLOR_DEPTH` is set to at least 16 in `lv_conf.h`.

- `screen_transp` if `1` the screen itself can have transparency as well. `LV_COLOR_DEPTH` must be 32.

- `user_data` A custom `void` user data for the driver.

- color format

## Events
- `monitor_cb` A callback function that tells how many pixels were refreshed and in how much time. Called when the last chunk is rendered and sent to the display.
- `render_start_cb` A callback function that notifies the display driver that rendering has started. It also could be used to wait for VSYNC to start rendering. It's useful if rendering is faster than a VSYNC period.

Some other optional callbacks to make it easier and more optimal to work with monochrome, grayscale or other non-standard RGB displays:
- `rounder_cb` Round the coordinates of areas to redraw. E.g. a 2x2 px can be converted to 2x8.
It can be used if the display controller can refresh only areas with specific height or width (usually 8 px height with monochrome displays).

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
     *`put_px` is just an example, it needs to be implemented by you.*/
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


```note::  For users upgrading from 7.10.0 and older: these new rotation enum values match up with the old 0/1 system for rotating 90 degrees, so legacy code should continue to work as expected. Software rotation is also disabled by default for compatibility.
```

Display rotation can also be changed at runtime using the `lv_disp_set_rotation(disp, rot)` API.

Support for software rotation is a new feature, so there may be some glitches/bugs depending on your configuration. If you encounter a problem please open an issue on [GitHub](https://github.com/lvgl/lvgl/issues).

### Decoupling the display refresh timer
Normally the dirty (a.k.a invalid) areas are checked and redrawn in every `LV_DEF_REFR_PERIOD` milliseconds (set in `lv_hal_disp.h`).
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

If the performance monitor is enabled, the value of `LV_DEF_REFR_PERIOD` needs to be set to be consistent with the refresh period of the display to ensure that the statistical results are correct.

## Further reading

- [lv_port_disp_template.c](https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_disp_template.c) for a template for your own driver.
- [Drawing](/overview/drawing) to learn more about how rendering works in LVGL.
- [Display features](/overview/display) to learn more about higher level display features.

## API

```eval_rst

.. doxygenfile:: lv_hal_disp.h
  :project: lvgl

```
