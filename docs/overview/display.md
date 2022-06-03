# Displays

``` important:: The basic concept of a *display* in LVGL is explained in the [Porting](/porting/display) section. So before reading further, please read the [Porting](/porting/display) section first.
```

## Multiple display support

In LVGL you can have multiple displays, each with their own driver and objects. The only limitation is that every display needs to have the same color depth (as defined in `LV_COLOR_DEPTH`).
If the displays are different in this regard the rendered image can be converted to the correct format in the drivers `flush_cb`.

Creating more displays is easy: just initialize more display buffers and register another driver for every display.
When you create the UI, use `lv_disp_set_default(disp)` to tell the library on which display to create objects.

Why would you want multi-display support? Here are some examples:
- Have a "normal" TFT display with local UI and create "virtual" screens on VNC on demand. (You need to add your VNC driver).
- Have a large TFT display and a small monochrome display.
- Have some smaller and simple displays in a large instrument or technology.
- Have two large TFT displays: one for a customer and one for the shop assistant.

### Using only one display
Using more displays can be useful but in most cases it's not required. Therefore, the whole concept of multi-display handling is completely hidden if you register only one display.
By default, the last created (and only) display is used.

`lv_scr_act()`, `lv_scr_load(scr)`, `lv_layer_top()`, `lv_layer_sys()`, `LV_HOR_RES` and `LV_VER_RES` are always applied on the most recently created (default) display.
If you pass `NULL` as `disp` parameter to display related functions the default display will usually be used.
E.g. `lv_disp_trig_activity(NULL)` will trigger a user activity on the default display. (See below in [Inactivity](#Inactivity)).

### Mirror display

To mirror the image of a display to another display, you don't need to use multi-display support. Just transfer the buffer received in `drv.flush_cb` to the other display too.

### Split image
You can create a larger virtual display from an array of smaller ones. You can create it as below:
1. Set the resolution of the displays to the large display's resolution.
2. In `drv.flush_cb`, truncate and modify the `area` parameter for each display.
3. Send the buffer's content to each real display with the truncated area.

## Screens

Every display has its own set of [screens](overview/object#screen-the-most-basic-parent) and the objects on each screen.

Be sure not to confuse displays and screens:

* **Displays** are the physical hardware drawing the pixels.
* **Screens** are the high-level root objects associated with a particular display. One display can have multiple screens associated with it, but not vice versa.

Screens can be considered the highest level containers which have no parent.
A screen's size is always equal to its display and their origin is (0;0). Therefore, a screen's coordinates can't be changed, i.e. `lv_obj_set_pos()`, `lv_obj_set_size()` or similar functions can't be used on screens.

A screen can be created from any object type but the two most typical types are [Base object](/widgets/obj) and [Image](/widgets/core/img) (to create a wallpaper).

To create a screen, use `lv_obj_t * scr = lv_<type>_create(NULL, copy)`. `copy` can be an existing screen copied into the new screen.

To load a screen, use `lv_scr_load(scr)`. To get the active screen, use `lv_scr_act()`. These functions work on the default display. If you want to specify which display to work on, use `lv_disp_get_scr_act(disp)` and `lv_disp_load_scr(disp, scr)`. A screen can be loaded with animations too. Read more [here](object.html#load-screens).

Screens can be deleted with `lv_obj_del(scr)`, but ensure that you do not delete the currently loaded screen.

### Transparent screens

Usually, the opacity of the screen is `LV_OPA_COVER` to provide a solid background for its children. If this is not the case (opacity &lt; 100%) the display's background color or image will be visible.
See the [Display background](#display-background) section for more details. If the display's background opacity is also not `LV_OPA_COVER` LVGL has no solid background to draw.

This configuration (transparent screen and display) could be used to create for example OSD menus where a video is played on a lower layer, and a menu is overlayed on an upper layer.

To handle transparent displays, special (slower) color mixing algorithms need to be used by LVGL so this feature needs to enabled with `LV_COLOR_SCREEN_TRANSP` in `lv_conf.h`.
As this mode operates on the Alpha channel of the pixels `LV_COLOR_DEPTH = 32` is also required. The Alpha channel of 32-bit colors will be 0 where there are no objects and 255 where there are solid objects.

In summary, to enable transparent screens and displays for OSD menu-like UIs:
- Enable `LV_COLOR_SCREEN_TRANSP` in `lv_conf.h`
- Be sure to use `LV_COLOR_DEPTH 32`
- Set the screen's opacity to `LV_OPA_TRANSP` e.g. with `lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP)`
- Set the display opacity to `LV_OPA_TRANSP` with `lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);`

## Features of displays

### Inactivity

A user's inactivity time is measured on each display. Every use of an [Input device](/overview/indev) (if [associated with the display](/porting/indev#other-features)) counts as an activity.
To get time elapsed since the last activity, use `lv_disp_get_inactive_time(disp)`. If `NULL` is passed, the lowest inactivity time among all displays will be returned (**NULL isn't just the default display**).

You can manually trigger an activity using `lv_disp_trig_activity(disp)`. If `disp` is `NULL`, the default screen will be used (**and not all displays**).

### Background
Every display has a background color, background image and background opacity properties. They become visible when the current screen is transparent or not positioned to cover the whole display.

The background color is a simple color to fill the display. It can be adjusted with `lv_disp_set_bg_color(disp, color)`;

The display background image is a path to a file or a pointer to an `lv_img_dsc_t` variable (converted image data) to be used as wallpaper. It can be set with `lv_disp_set_bg_image(disp, &my_img)`;
If a background image is configured the background won't be filled with `bg_color`.

The opacity of the background color or image can be adjusted with `lv_disp_set_bg_opa(disp, opa)`.

The `disp` parameter of these functions can be `NULL` to select the default display.



## API

```eval_rst

.. doxygenfile:: lv_disp.h
  :project: lvgl

```
