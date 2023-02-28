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
- `buf_size_byte` size of the buffer(s) in bytes
- `render_mode`
  - `LV_DISP_RENDER_MODE_PARTIAL` Use the buffer(s) to render the screen is smaller parts. This way the buffers can be smaller then the display to save RAM. At least 1/10 sceen size buffer(s) are recommended. In `flush_cb` the rendered images needs to be copied to the given area of the display.
   - `LV_DISP_RENDER_MODE_DIRECT` The buffer(s) has to be screen sized and LVGL will render into the correct location of the buffer. This way the buffer always contain the whole image. If two buffer are used the rendered ares are automatically copied to the other buffer after flushing. Due to this in `flush_cb` typically only a frame buffer address needs to be changed and always the changed areas will be redrawn.
   - `LV_DISP_RENDER_MODE_FULL` The buffer can smaller or screen sized but LVGL will always redraw the whole screen even is only 1 pixel has been changed. If two screen sized draw buffers are provided, LVGL's display handling works like "traditional" double buffering. This means the `flush_cb` callback only has to update the address of the framebuffer (`color_p` parameter).

Example:
```c
static lv_color_t buf[LCD_HOR_RES * LCD_VER_RES / 10];
lv_disp_set_draw_buffers(disp, buf, NULL, sizeof(buf), LV_DISP_RENDER_MODE_PARTIAL);
```

#### One buffer
If only one buffer is used LVGL draws the content of the screen into that draw buffer and sends it to the display via the `flush_cb`.
LVGL then needs to wait until the content of the buffer is sent to the display before drawing something new into it.

#### Two buffers
If two buffers  are used LVGL can draw into one buffer while the content of the other buffer is sent to the display in the background.
DMA or other hardware should be used to transfer data to the display so the MCU can continue drawing.
This way, the rendering and refreshing of the display become parallel operations.


## Advanced options

### Resolution
To set the resolution of the display after creation use `lv_disp_set_res(disp, hor_res, ver_res);`

It's not mandatory to use the whole display for LVGL, however in some cases the physical resolution is important. For example the touchpad still sees the whole resolution and the values needs to be converted
to the active LVGL display area. So the physical resoltution and the offset of the active area can be set with `lv_disp_set_physical_res(disp, hor_res, ver_res);`and `lv_disp_set_offset(disp, x, y);`

### Rotation
LVGL supports rotation of the display in 90 degree increments. You can select whether you'd like software rotation or hardware rotation.

The orientation of the display can be changed with
`lv_disp_set_rotation(disp, LV_DISP_ROTATION_0/90/180/270, true/false)`.
LVGL will swap the horizontal and vertical resolutions internally according to the set degree. IF the last paramter is `true` LVGL will rotate the rendered image. If it's `false` the display driver should rotate the rendered image.

### Color format

Set the color format of the display. The default is `LV_COLOR_FORMAT_NATIVE` which means LVGL render with the follow formats dpeneding on `LV_COLOR_DEPTH`:
- `LV_COLOR_DEPTH 32` XRGB8888 (4 bytes/pixel)
- `LV_COLOR_DEPTH 24` RGB888 (3 bytes/pixel)
- `LV_COLOR_DEPTH 16` RGB565 (2 bytes/pixel)
- `LV_COLOR_DEPTH 8` L8 (1 bytes/pixel)

The `color_format` can be changed with `lv_disp_set_color_depth(disp, LV_COLOR_FORMAT_...)` to the following values:
- `LV_COLOR_FORMAT_NATIVE_ALPHA` Append an alpha byte to the native format resulting in A8L8, ARGB8565, ARGB8888 formats.
- `LV_COLOR_FORMAT_NATIVE_REVERSE` Reverse the byte order of the native format. Useful if the rendered image is sent to the disply via SPI and the display needs the bytes in the opposite order.
- `LV_COLOR_FORMAT_L8` Lightness only on 8 bit
- `LV_COLOR_FORMAT_A8` Alpha only on 8 bit
- `LV_COLOR_FORMAT_I8` Indexed (palette) 8 bit
- `LV_COLOR_FORMAT_A8L8` Lightness on 8 bit with 8 bit alpha
- `LV_COLOR_FORMAT_ARGB2222` ARGB with 2 bit for each channel
- `LV_COLOR_FORMAT_RGB565` 16 bit RGB565 format without alpha channel
- `LV_COLOR_FORMAT_ARGB8565` 16 bit RGB565 format and 8 bit alpha channel
- `LV_COLOR_FORMAT_ARGB1555` 5 bit for each color channel and 1 bit for alpha
- `LV_COLOR_FORMAT_ARGB4444` 4 bit for each channel
- `LV_COLOR_FORMAT_RGB888` 8 bit for each color channel with out alpha channel
- `LV_COLOR_FORMAT_ARGB8888`  8 bit for each channel
- `LV_COLOR_FORMAT_XRGB8888` 8 bit for each color channel and 8 bit placholder for the alpha cannel

If the color fotmat is set to non-native `draw_ctx->buffer_convert` function will be called before calling `flush_cb` to convert the native color format to the desired, therfore rendering in non-native formats has a negative effect on peroformance. Learn more about `draw_ctx` [here](/porting/gpu).

It's very important that draw buffer(s) should be large enough for both the native format and the target color format. For example if `LV_COLOR_DEPTH == 16` and `LV_COLOR_FORMAT_XRGB8888` is selected LVGL will choosoe the larger to figure out how many pixel can be rendered at once. Therefore with `LV_DISP_RENDER_MODE_FULL` and the larger pixel size needs to choosen.

`LV_DISP_RENDER_MODE_DIRECT` supports only the `LV_COLOR_FORMAT_NATIVE` format.

### Antialiasing
`lv_disp_set_antialiasing(disp, true/false)` enables/disables the antialiasing (edge smoothing) on the given display.

### User data
With `lv_disp_set_user_data(disp, p)` a pointer to a custom data can be stored in display object.

## Events
`lv_disp_add_event(disp, event_cb, LV_DISP_EVENT_..., user_data)` adds an event handler to a display.
The following events are sent:
- `LV_DISP_EVENT_INVALIDATE_AREA` An area is invalidated (marked for redraw). `lv_event_get_param(e)` returns a pointer to an `lv_area_t` varaible with the coordinates of the area to be invalidated. The ara can be freely modified is needed to adopt it the specialrequirement of the display. Usually needed with monoschrome displays to invalidate Nx8 lines at once.
- `LV_DISP_EVENT_RENDER_START` Called when rendering starts.
- `LV_DISP_EVENT_RENDER_READY` Called when rendering is ready
- `LV_DISP_EVENT_RESOLUTION_CHANGED` CAlled when the resolution changes due to `lv_disp_set_resolution()` or `lv_disp_set_rotation()`.



## Other options

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

.. doxygenfile:: lv_disp.h
  :project: lvgl

```
