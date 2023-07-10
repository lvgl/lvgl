# Drawing

With LVGL, you don't need to draw anything manually. Just create objects (like buttons, labels, arc, etc.), move and change them, and LVGL will refresh and redraw what is required.

However, it can be useful to have a basic understanding of how drawing happens in LVGL to add customization, make it easier to find bugs or just out of curiosity.

The basic concept is to not draw directly onto the display but rather to first draw on an internal draw buffer. When a drawing (rendering) is ready that buffer is copied to the display.

The draw buffer can be smaller than a display's size. LVGL will simply render in "tiles" that fit into the given draw buffer.

This approach has two main advantages compared to directly drawing to the display:
1. It avoids flickering while the layers of the UI are drawn. For example, if LVGL drew directly onto the display, when drawing a *background + button + text*, each "stage" would be visible for a short time.
2. It's faster to modify a buffer in internal RAM and finally write one pixel only once than reading/writing the display directly on each pixel access.
(e.g. via a display controller with SPI interface).

Note that this concept is different from "traditional" double buffering where there are two display sized frame buffers:
one holds the current image to show on the display, and rendering happens to the other (inactive) frame buffer, and they are swapped when the rendering is finished.
The main difference is that with LVGL you don't have to store two frame buffers (which usually requires external RAM) but only smaller draw buffer(s) that can easily fit into internal RAM.


## Mechanism of screen refreshing

Be sure to get familiar with the [Buffering modes of LVGL](/porting/display) first.

LVGL refreshes the screen in the following steps:
1. Something happens in the UI which requires redrawing. For example, a button is pressed, a chart is changed, an animation happened, etc.
2. LVGL saves the changed object's old and new area into a buffer, called an *Invalid area buffer*. For optimization, in some cases, objects are not added to the buffer:
    - Hidden objects are not added.
    - Objects completely out of their parent are not added.
    - Areas partially out of the parent are cropped to the parent's area.
    - Objects on other screens are not added.
3. In every `LV_DISP_DEF_REFR_PERIOD` (set in `lv_conf.h`) the following happens:
    - LVGL checks the invalid areas and joins those that are adjacent or intersecting.
    - Takes the first joined area, if it's smaller than the *draw buffer*, then simply renders the area's content into the *draw buffer*.
      If the area doesn't fit into the buffer, draw as many lines as possible to the *draw buffer*.
    - When the area is rendered, call `flush_cb` from the display driver to refresh the display.
    - If the area was larger than the buffer, render the remaining parts too.
    - Repeat the same with remaining joined areas.

When an area is redrawn the library searches the top-most object which covers that area and starts drawing from that object.
For example, if a button's label has changed, the library will see that it's enough to draw the button under the text and it's not necessary to redraw the display under the rest of the button too.

The difference between buffering modes regarding the drawing mechanism is the following:
1. **One buffer** - LVGL needs to wait for `lv_disp_flush_ready()` (called from `flush_cb`) before starting to redraw the next part.
2. **Two buffers** -  LVGL can immediately draw to the second buffer when the first is sent to `flush_cb` because the flushing should be done by DMA (or similar hardware) in the background.
3. **Double buffering** -  `flush_cb` should only swap the addresses of the frame buffers.

## Masking
*Masking* is the basic concept of LVGL's draw engine.
To use LVGL it's not required to know about the mechanisms described here but you might find interesting to know how drawing works under hood.
Knowing about masking comes in handy if you want to customize drawing.

To learn about masking let's see the steps of drawing first.
LVGL performs the following steps to render any shape, image or text. It can be considered as a drawing pipeline.

1. **Prepare the draw descriptors** Create a draw descriptor from an object's styles (e.g. `lv_draw_rect_dsc_t`).  This gives us the parameters for drawing, for example colors, widths, opacity, fonts, radius, etc.
2. **Call the draw function** Call the draw function with the draw descriptor and some other parameters (e.g. `lv_draw_rect()`).  It will render the primitive shape to the current draw buffer.
3. **Create masks** If the shape is very simple and doesn't require masks, go to #5. Otherwise, create the required masks in the draw function. (e.g. a rounded rectangle mask)
4. **Calculate all the added mask** It composites opacity values into a *mask buffer* with the "shape" of the created masks.
E.g. in case of a "line mask" according to the parameters of the mask, keep one side of the buffer as it is (255 by default) and set the rest to 0 to indicate that this side should be removed.
5. **Blend a color or image** During blending, masking (make some pixels transparent or opaque), blending modes (additive, subtractive, etc.) and color/image opacity are handled.

LVGL has the following built-in mask types which can be calculated and applied real-time:
- `LV_DRAW_MASK_TYPE_LINE` Removes a side from a line (top, bottom, left or right). `lv_draw_line` uses four instances of it.
Essentially, every (skew) line is bounded with four line masks forming a rectangle.
- `LV_DRAW_MASK_TYPE_RADIUS` Removes the inner or outer corners of a rectangle with a radiused transition. It's also used to create circles by setting the radius to large value (`LV_RADIUS_CIRCLE`)
- `LV_DRAW_MASK_TYPE_ANGLE` Removes a circular sector. It is used by `lv_draw_arc` to remove the "empty" sector.
- `LV_DRAW_MASK_TYPE_FADE` Create a vertical fade (change opacity)
- `LV_DRAW_MASK_TYPE_MAP` The mask is stored in a bitmap array and the necessary parts are applied

Masks are used to create almost every basic primitive:
- **letters** Create a mask from the letter and draw a rectangle with the letter's color using the mask.
- **line** Created from four "line masks" to mask out the left, right, top and bottom part of the line to get a perfectly perpendicular perimeter.
- **rounded rectangle** A mask is created real-time to add a radius to the corners.
- **clip corner** To clip overflowing content (usually children) on rounded corners, a rounded rectangle mask is also applied.
- **rectangle border** Same as a rounded rectangle but the inner part is masked out too.
- **arc drawing** A circular border is drawn but an arc mask is applied too.
- **ARGB images** The alpha channel is separated into a mask and the image is drawn as a normal RGB image.

### Using masks

Every mask type has a related parameter structure to describe the mask's data. The following parameter types exist:
- `lv_draw_mask_line_param_t`
- `lv_draw_mask_radius_param_t`
- `lv_draw_mask_angle_param_t`
- `lv_draw_mask_fade_param_t`
- `lv_draw_mask_map_param_t`

1. Initialize a mask parameter with `lv_draw_mask_<type>_init`. See `lv_draw_mask.h` for the whole API.
2. Add the mask parameter to the draw engine with `int16_t mask_id = lv_draw_mask_add(&param, ptr)`. `ptr` can be any pointer to identify the mask, (`NULL` if unused).
3. Call the draw functions
4. Remove the mask from the draw engine with `lv_draw_mask_remove_id(mask_id)` or `lv_draw_mask_remove_custom(ptr)`.
5. Free the parameter with `lv_draw_mask_free_param(&param)`.

A parameter can be added and removed any number of times, but it needs to be freed when not required anymore.

`lv_draw_mask_add` saves only the pointer of the mask so the parameter needs to be valid while in use.

## Hook drawing
Although widgets can be easily customized by styles there might be cases when something more custom is required.
To ensure a great level of flexibility LVGL sends a lot of events during drawing with parameters that tell what LVGL is about to draw.
Some fields of these parameters can be modified to draw something else or any custom drawing operations can be added manually.

A good use case for this is the [Button matrix](/widgets/core/btnmatrix) widget. By default, its buttons can be styled in different states, but you can't style the buttons one by one.
However, an event is sent for every button and you can, for example, tell LVGL to use different colors on a specific button or to manually draw an image on some buttons.

Each of these events is described in detail below.

### Main drawing

These events are related to the actual drawing of an object. E.g. the drawing of buttons, texts, etc. happens here.

`lv_event_get_clip_area(event)` can be used to get the current clip area. The clip area is required in draw functions to make them draw only on a limited area.

#### LV_EVENT_DRAW_MAIN_BEGIN

Sent before starting to draw an object. This is a good place to add masks manually. E.g. add a line mask that "removes" the right side of an object.

#### LV_EVENT_DRAW_MAIN

The actual drawing of an object happens in this event. E.g. a rectangle for a button is drawn here. First, the widgets' internal events are called to perform drawing and after that you can draw anything on top of them.
For example you can add a custom text or an image.

#### LV_EVENT_DRAW_MAIN_END

Called when the main drawing is finished. You can draw anything here as well and it's also a good place to remove any masks created in `LV_EVENT_DRAW_MAIN_BEGIN`.

### Post drawing

Post drawing events are called when all the children of an object are drawn. For example LVGL use the post drawing phase to draw scrollbars because they should be above all of the children.

`lv_event_get_clip_area(event)` can be used to get the current clip area.

#### LV_EVENT_DRAW_POST_BEGIN

Sent before starting the post draw phase. Masks can be added here too to mask out the post drawn content.

#### LV_EVENT_DRAW_POST

The actual drawing should happen here.

#### LV_EVENT_DRAW_POST_END

Called when post drawing has finished. If masks were not removed in `LV_EVENT_DRAW_MAIN_END` they should be removed here.

### Part drawing

When LVGL draws a part of an object (e.g. a slider's indicator, a table's cell or a button matrix's button) it sends events before and after drawing that part with some context of the drawing.
This allows changing the parts on a very low level with masks, extra drawing, or changing the parameters that LVGL is planning to use for drawing.

In these events an `lv_obj_draw_part_t` structure is used to describe the context of the drawing. Not all fields are set for every part and widget.
To see which fields are set for a widget refer to the widget's documentation.

`lv_obj_draw_part_t` has the following fields:

```c
// Always set
const lv_area_t * clip_area;        // The current clip area, required if you need to draw something in the event
uint32_t part;                      // The current part for which the event is sent
uint32_t id;                        // The index of the part. E.g. a button's index on button matrix or table cell index.

// Draw desciptors, set only if related
lv_draw_rect_dsc_t * rect_dsc;      // A draw descriptor that can be modified to changed what LVGL will draw. Set only for rectangle-like parts
lv_draw_label_dsc_t * label_dsc;    // A draw descriptor that can be modified to changed what LVGL will draw. Set only for text-like parts
lv_draw_line_dsc_t * line_dsc;      // A draw descriptor that can be modified to changed what LVGL will draw. Set only for line-like parts
lv_draw_img_dsc_t *  img_dsc;       // A draw descriptor that can be modified to changed what LVGL will draw. Set only for image-like parts
lv_draw_arc_dsc_t *  arc_dsc;       // A draw descriptor that can be modified to changed what LVGL will draw. Set only for arc-like parts

// Other parameters
lv_area_t * draw_area;              // The area of the part being drawn
const lv_point_t * p1;              // A point calculated during drawing. E.g. a point of a chart or the center of an arc.
const lv_point_t * p2;              // A point calculated during drawing. E.g. a point of a chart.
char text[16];                      // A text calculated during drawing. Can be modified. E.g. tick labels on a chart axis.
lv_coord_t radius;                  // E.g. the radius of an arc (not the corner radius).
int32_t value;                      // A value calculated during drawing. E.g. Chart's tick line value.
const void * sub_part_ptr;          // A pointer the identifies something in the part. E.g. chart series.
```

`lv_event_get_draw_part_dsc(event)` can be used to get a pointer to `lv_obj_draw_part_t`.

#### LV_EVENT_DRAW_PART_BEGIN

Start the drawing of a part. This is a good place to modify the draw descriptors (e.g. `rect_dsc`), or add masks.

#### LV_EVENT_DRAW_PART_END

Finish the drawing of a part. This is a good place to draw extra content on the part or remove masks added in `LV_EVENT_DRAW_PART_BEGIN`.

### Others

#### LV_EVENT_COVER_CHECK

This event is used to check whether an object fully covers an area or not.

`lv_event_get_cover_area(event)` returns a pointer to an area to check and `lv_event_set_cover_res(event, res)` can be used to set one of these results:
- `LV_COVER_RES_COVER` the area is fully covered by the object
- `LV_COVER_RES_NOT_COVER` the area is not covered by the object
- `LV_COVER_RES_MASKED` there is a mask on the object, so it does not fully cover the area

Here are some reasons why an object would be unable to fully cover an area:
- It's simply not fully in area
- It has a radius
- It doesn't have 100% background opacity
- It's an ARGB or chroma keyed image
- It does not have normal blending mode. In this case LVGL needs to know the colors under the object to apply blending properly
- It's a text, etc

In short if for any reason the area below an object is visible than the object doesn't cover that area.

Before sending this event LVGL checks if at least the widget's coordinates fully cover the area or not. If not the event is not called.

You need to check only the drawing you have added. The existing properties known by a widget are handled in its internal events.
E.g. if a widget has &gt; 0 radius it might not cover an area, but you need to handle `radius` only if you will modify it and the widget won't know about it.

#### LV_EVENT_REFR_EXT_DRAW_SIZE

If you need to draw outside a widget, LVGL needs to know about it to provide extra space for drawing.
Let's say you create an event which writes the current value of a slider above its knob. In this case LVGL needs to know that the slider's draw area should be larger with the size required for the text.

You can simply set the required draw area with `lv_event_set_ext_draw_size(e, size)`.

