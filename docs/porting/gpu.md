# Add custom GPU
LVGL has a flexible and extendable draw pipeline. You can hook it to do some rendering with a GPU or even completely replace the built-in software renderer.

## Draw context
The core structure of drawing is `lv_draw_ctx_t`.
It contains a pointer to a buffer where drawing should happen and a couple of callbacks to draw rectangles, texts, and other primitives.

### Fields
`lv_draw_ctx_t` has the following fields:
- `void * buf` Pointer to a buffer to draw into
- `lv_area_t * buf_area` The position and size of `buf` (absolute coordinates)
- `const lv_area_t * clip_area` The current clip area with absolute coordinates, always the same or smaller than `buf_area`. All drawings should be clipped to this area.
- `void (*draw_rect)()` Draw a rectangle with shadow, gradient, border, etc.
- `void (*draw_arc)()` Draw an arc
- `void (*draw_img_decoded)()` Draw an (A)RGB image that is already decoded by LVGL.
- `lv_res_t (*draw_img)()` Draw an image before decoding it (it bypasses LVGL's internal image decoders)
- `void (*draw_letter)()` Draw a letter
- `void (*draw_line)()` Draw a line
- `void (*draw_polygon)()` Draw a polygon
- `void (*draw_bg)()` Replace the buffer with a rect without decoration like radius or borders.
- `void (*wait_for_finish)()` Wait until all background operation are finished. (E.g. GPU operations)
- `void * user_data` Custom user data for arbitrary purpose

(For the sake of simplicity the parameters of the callbacks are not shown here.)

All `draw_*` callbacks receive a pointer to the current `draw_ctx` as their first parameter. Among the other parameters there is a descriptor that tells what to draw,
e.g. for `draw_rect` it's called [lv_draw_rect_dsc_t](https://github.com/lvgl/lvgl/blob/master/src/draw/lv_draw_rect.h),
for `lv_draw_line` it's called [lv_draw_line_dsc_t](https://github.com/lvgl/lvgl/blob/master/src/draw/lv_draw_line.h), etc.

To correctly render according to a `draw_dsc` you need to be familiar with the [Boxing model](https://docs.lvgl.io/master/overview/coords.html#boxing-model) of LVGL and the meanings of the fields. The name and meaning of the fields are identical to name and meaning of the [Style properties](https://docs.lvgl.io/master/overview/style-props.html).

### Initialization
The `lv_disp_drv_t` has 4 fields related to the draw context:
- `lv_draw_ctx_t * draw_ctx` Pointer to the `draw_ctx` of this display
- `void (*draw_ctx_init)(struct _lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)` Callback to initialize a `draw_ctx`
- `void (*draw_ctx_deinit)(struct _lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)` Callback to de-initialize a `draw_ctx`
- `size_t draw_ctx_size` Size of the draw context structure. E.g. `sizeof(lv_draw_sw_ctx_t)`

When you ignore these fields, LVGL will set default values for callbacks and size in `lv_disp_drv_init()` based on the configuration in `lv_conf.h`.
`lv_disp_drv_register()` will allocate a `draw_ctx` based on `draw_ctx_size` and call `draw_ctx_init()` on it.

However, you can overwrite the callbacks and the size values before calling `lv_disp_drv_register()`.
It makes it possible to use your own `draw_ctx` with your own callbacks.


## Software renderer
LVGL's built in software renderer extends the basic `lv_draw_ctx_t` structure and sets the draw callbacks. It looks like this:
```c
typedef struct {
   /** Include the basic draw_ctx type*/
    lv_draw_ctx_t base_draw;

    /** Blend a color or image to an area*/
    void (*blend)(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);
} lv_draw_sw_ctx_t;
```

Set the draw callbacks in `draw_ctx_init()` like:
```c
draw_sw_ctx->base_draw.draw_rect = lv_draw_sw_rect;
draw_sw_ctx->base_draw.draw_letter = lv_draw_sw_letter;
...
```

### Blend callback
As you saw above the software renderer adds the `blend` callback field. It's a special callback related to how the software renderer works.
All draw operations end up in the `blend` callback which can either fill an area or copy an image to an area by considering an optional mask.

The `lv_draw_sw_blend_dsc_t` parameter describes what and how to blend. It has the following fields:
- `const lv_area_t * blend_area` The area with absolute coordinates to draw on `draw_ctx->buf`. If `src_buf` is set, it's the coordinates of the image to blend.
- `const lv_color_t * src_buf` Pointer to an image to blend. If set, `color` is ignored. If not set fill `blend_area` with `color`
- `lv_color_t color` Fill color. Used only if `src_buf == NULL`
- `lv_opa_t * mask_buf` NULL if ignored, or an alpha mask to apply on `blend_area`
- `lv_draw_mask_res_t mask_res` The result of the previous mask operation. (`LV_DRAW_MASK_RES_...`)
- `const lv_area_t * mask_area` The area of `mask_buf` with absolute coordinates
- `lv_opa_t opa` The overall opacity
- `lv_blend_mode_t blend_mode` E.g. `LV_BLEND_MODE_ADDITIVE`


## Extend the software renderer

### New blend callback

Let's take a practical example: you would like to use your MCUs GPU for color fill operations only.

As all draw callbacks call `blend` callback to fill an area in the end only the `blend` callback needs to be overwritten.

First extend `lv_draw_sw_ctx_t`:
```c

/*We don't add new fields, so just for clarity add new type*/
typedef lv_draw_sw_ctx_t my_draw_ctx_t;

void my_draw_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    /*Initialize the parent type first */
    lv_draw_sw_init_ctx(drv, draw_ctx);

    /*Change some callbacks*/
    my_draw_ctx_t * my_draw_ctx = (my_draw_ctx_t *)draw_ctx;

    my_draw_ctx->blend = my_draw_blend;
    my_draw_ctx->base_draw.wait_for_finish = my_gpu_wait;
}
```

After calling `lv_disp_draw_init(&drv)` you can assign the new `draw_ctx_init` callback and set `draw_ctx_size` to overwrite the defaults:
```c
static lv_disp_drv_t drv;
lv_disp_draw_init(&drv);
drv->hor_res = my_hor_res;
drv->ver_res = my_ver_res;
drv->flush_cb = my_flush_cb;

/*New draw ctx settings*/
drv->draw_ctx_init = my_draw_ctx_init;
drv->draw_ctx_size = sizeof(my_draw_ctx_t);

lv_disp_drv_register(&drv);
```

This way when LVGL calls `blend` it will call `my_draw_blend` and we can do custom GPU operations. Here is a complete example:
```c
void my_draw_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;  /*Fully clipped, nothing to do*/

    /*Fill only non masked, fully opaque, normal blended and not too small areas*/
    if(dsc->src_buf == NULL && dsc->mask == NULL && dsc->opa >= LV_OPA_MAX &&
       dsc->blend_mode == LV_BLEND_MODE_NORMAL && lv_area_get_size(&blend_area) > 100) {

        /*Got the first pixel on the buffer*/
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area); /*Width of the destination buffer*/
        lv_color_t * dest_buf = draw_ctx->buf;
        dest_buf += dest_stride * (blend_area.y1 - draw_ctx->buf_area->y1) + (blend_area.x1 - draw_ctx->buf_area->x1);

        /*Make the blend area relative to the buffer*/      
        lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
       
        /*Call your custom gou fill function to fill blend_area, on dest_buf with dsc->color*/  
        my_gpu_fill(dest_buf, dest_stride, &blend_area, dsc->color);
    }
    /*Fallback: the GPU doesn't support these settings. Call the SW renderer.*/
    else {
      lv_draw_sw_blend_basic(draw_ctx, dsc);
    }
}
```

The implementation of wait callback is much simpler:
```c
void my_gpu_wait(lv_draw_ctx_t * draw_ctx)
{
    while(my_gpu_is_working());
   
    /*Call SW renderer's wait callback too*/
    lv_draw_sw_wait_for_finish(draw_ctx);
}
```

### New rectangle drawer
If your MCU has a more powerful GPU that can draw e.g. rounded rectangles you can replace the original software drawer too.
A custom `draw_rect` callback might look like this:
```c
void my_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
  if(lv_draw_mask_is_any(coords) == false && dsc->grad == NULL && dsc->bg_img_src == NULL &&
     dsc->shadow_width == 0 && dsc->blend_mode = LV_BLEND_MODE_NORMAL)
  {
    /*Draw the background*/
    my_bg_drawer(draw_ctx, coords, dsc->bg_color, dsc->radius);
   
    /*Draw the border if any*/
    if(dsc->border_width) {
      my_border_drawer(draw_ctx, coords, dsc->border_width, dsc->border_color, dsc->border_opa)
    }
   
    /*Draw the outline if any*/
    if(dsc->outline_width) {
      my_outline_drawer(draw_ctx, coords, dsc->outline_width, dsc->outline_color, dsc->outline_opa, dsc->outline_pad)
    }
  }
  /*Fallback*/
  else {
    lv_draw_sw_rect(draw_ctx, dsc, coords);
  }
}
```

`my_draw_rect` can fully bypass the use of `blend` callback if needed.

## Fully custom draw engine

For example if your MCU/MPU supports a powerful vector graphics engine you might use only that instead of LVGL's SW renderer.
In this case, you need to base the renderer on the basic `lv_draw_ctx_t` (instead of `lv_draw_sw_ctx_t`) and extend/initialize it as you wish.

