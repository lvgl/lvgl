# Canvas (lv_canvas)


## Overview

A Canvas inherits from [Image](/widgets/core/img) where the user can draw anything.
Rectangles, texts, images, lines, arcs can be drawn here using lvgl's drawing engine.
Additionally "effects" can be applied, such as rotation, zoom and blur.


## Parts and Styles
`LV_PART_MAIN` Uses the typical rectangle style properties and image style properties.

## Usage

### Buffer
The Canvas needs a buffer in which stores the drawn image.
To assign a buffer to a Canvas, use `lv_canvas_set_buffer(canvas, buffer, width, height, LV_IMG_CF_...)`.
Where  `buffer` is a static buffer (not just a local variable) to hold the image of the canvas.
For example,
`static lv_color_t buffer[LV_CANVAS_BUF_SIZE_TRUE_COLOR(width, height)]`.
`LV_CANVAS_BUF_SIZE_...` macros help to determine the size of the buffer with different color formats.

The canvas supports all the built-in color formats like `LV_IMG_CF_TRUE_COLOR` or `LV_IMG_CF_INDEXED_2BIT`.
See the full list in the [Color formats](/overview/image.html#color-formats) section.

### Indexed colors
For `LV_IMG_CF_INDEXED_1/2/4/8` color formats a palette needs to be
initialized with  `lv_canvas_set_palette(canvas, 3, LV_COLOR_RED)`. It sets pixels with *index=3* to red.

### Drawing
To set a pixel's color on the canvas, use `lv_canvas_set_px_color(canvas, x, y, LV_COLOR_RED)`.
With `LV_IMG_CF_INDEXED_...`  the index of the color needs to be passed as color.
E.g. `lv_color_t c; c.full = 3;`

To set a pixel's opacity with `LV_IMG_CF_TRUE_COLOR_ALPHA` or `LV_IMG_CF_ALPHA_...` format on the canvas, use `lv_canvas_set_px_opa(canvas, x, y, opa)`.


`lv_canvas_fill_bg(canvas, LV_COLOR_BLUE, LV_OPA_50)` fills the whole canvas to blue with 50% opacity. Note that if the current color format doesn't support colors (e.g. `LV_IMG_CF_ALPHA_2BIT`) the color will be ignored.
Similarly, if opacity is not supported (e.g. `LV_IMG_CF_TRUE_COLOR`) it will be ignored.

An array of pixels can be copied to the canvas with `lv_canvas_copy_buf(canvas, buffer_to_copy, x, y, width, height)`.
The color format of the buffer and the canvas need to match.

To draw something to the canvas use
- `lv_canvas_draw_rect(canvas, x, y, width, heigth, &draw_dsc)`
- `lv_canvas_draw_text(canvas, x, y, max_width, &draw_dsc, txt)`
- `lv_canvas_draw_img(canvas, x, y, &img_src, &draw_dsc)`
- `lv_canvas_draw_line(canvas, point_array, point_cnt, &draw_dsc)`
- `lv_canvas_draw_polygon(canvas, points_array, point_cnt, &draw_dsc)`
- `lv_canvas_draw_arc(canvas, x, y, radius, start_angle, end_angle, &draw_dsc)`

`draw_dsc` is a `lv_draw_rect/label/img/line/arc_dsc_t` variable which should be first initialized with one of `lv_draw_rect/label/img/line/arc_dsc_init()` and then modified with the desired colors and other values.

The draw function can draw to any color format. For example, it's possible to draw a text to an `LV_IMG_VF_ALPHA_8BIT` canvas and use the result image as a [draw mask](/overview/drawing) later.

### Transformations
`lv_canvas_transform()` can be used to rotate and/or scale the image of an image and store the result on the canvas.
The function needs the following parameters:
- `canvas` pointer to a canvas object to store the result of the transformation.
- `img pointer` to an image descriptor to transform. Can be the image descriptor of another canvas too (`lv_canvas_get_img()`).
- `angle` the angle of rotation (0..3600), 0.1 deg resolution
- `zoom` zoom factor (256: no zoom, 512: double size, 128: half size);
- `offset_x` offset X to tell where to put the result data on destination canvas
- `offset_y` offset X to tell where to put the result data on destination canvas
- `pivot_x` pivot X of rotation. Relative to the source canvas. Set to `source width / 2` to rotate around the center
- `pivot_y` pivot Y of rotation. Relative to the source canvas. Set to `source height / 2` to rotate around the center
- `antialias` true: apply anti-aliasing during the transformation. Looks better but slower.

Note that a canvas can't be rotated on itself.  You need a source and destination canvas or image.

### Blur
A given area of the canvas can be blurred horizontally with `lv_canvas_blur_hor(canvas, &area, r)` or vertically with `lv_canvas_blur_ver(canvas, &area, r)`.
`r` is the radius of the blur (greater value means more intensive burring). `area` is the area where the blur should be applied (interpreted relative to the canvas).

## Events
No special events are sent by canvas objects.
The same events are sent as for the

See the events of the [Images](/widgets/core/img) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example
```eval_rst

.. include:: ../../../examples/widgets/canvas/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_canvas.h
  :project: lvgl

```
