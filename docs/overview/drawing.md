```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/drawing.md
```
# Drawing

With LVGL, you don't need to draw anything manually. Just create objects (like buttons, labels, arc, etc), move and change them, and LVGL will refresh and redraw what is required.

However, it might be useful to have a basic understanding of how drawing happens in LVGL to add customization, make it easier to find bugs or just out of curiosity.

The basic concept is to not draw directly to the screen, but draw to an internal draw buffer first. When drawing (rendering) is ready copy that buffer to the screen. 

The draw buffer can be smaller than the screen's size. LVGL will simply render in "tiles" that fit into the given draw buffer.

This approach has two main advantages compared to directly drawing to the screen:
1. It avoids flickering while the layers of the UI are drawn. For example, if LVGL drawn directly into the display, when drawing a *background + button + text*, each "stage" would be visible for a short time .
2. It's faster to modify a buffer in internal RAM and finally write one pixel only once than reading/writing the display directly on each pixel access. 
(e.g. via a display controller with SPI interface). 

Note that, this concept is different from "traditional" double buffering where there are 2 screen sized frame buffers: 
one holds the current image to show on the display, and rendering happens to the other (inactive) frame buffer, and they are swapped when the rendering is finished. 
The main difference is that with LVGL you don't have to store 2 frame buffers (which usually requires external RAM) but only smaller draw buffer(s) that can easily fit into the internal RAM too.


## Mechanism of screen refreshing

Be sure to get familiar with the [Buffering modes of LVGL](/porting/display) first.

LVGL refreshes the screen in the following steps:
1. Something happens on the UI which requires redrawing. For example, a button is pressed, a chart is changed or an animation happened, etc.
2. LVGL saves the changed object's old and new area into a buffer, called an *Invalid area buffer*. For optimization, in some cases, objects are not added to the buffer:
    - Hidden objects are not added.
    - Objects completely out of their parent are not added.
    - Areas partially out of the parent are cropped to the parent's area.
    - The objects on other screens are not added.
3. In every `LV_DISP_DEF_REFR_PERIOD` (set in `lv_conf.h`) the followings happen:
    - LVGL checks the invalid areas and joins the adjacent or intersecting areas.
    - Takes the first joined area, if it's smaller than the *draw buffer*, then simply render the area's content into the *draw buffer*. 
      If the area doesn't fit into the buffer, draw as many lines as possible to the *draw buffer*.
    - When the area is rendered, call `flush_cb` from the display driver to refresh the display.
    - If the area was larger than the buffer, render the remaining parts too.
    - Do the same with all the joined areas.

When an area is redrawn, the library searches the top most object which covers that area, and starts drawing from that object.
For example, if a button's label has changed, the library will see that it's enough to draw the button under the text, and it's not required to draw the screen under the button too.

The difference between buffering modes regarding the drawing mechanism is the following:
1. **One buffer** - LVGL needs to wait for `lv_disp_flush_ready()` (called from `flush_cb`) before starting to redraw the next part.
2. **Two buffers** -  LVGL can immediately draw to the second buffer when the first is sent to `flush_cb` because the flushing should be done by DMA (or similar hardware) in the background.
3. **Double buffering** -  `flush_cb` should only swap the address of the frame buffer.

## Masking
*Masking* is the basic concept of LVGL's draw engine. 
To use LVGL it's not required to know about the mechanisms described here,  you might find interesting to know how drawing works under hood. 
Knowing about mask comes in handy if you want to customize drawing.

To learn masking let's learn the steps of drawing first. 
LVGL performs the following steps to render any shape, image or text. It can be considered as a drawing pipeline.

1. **Prepare the draw descriptors** Create a draw descriptor from an object's styles (e.g. `lv_draw_rect_dsc_t`).  It tells the parameters of drawing, for example the colors, widths, opacity, fonts, radius, etc. 
2. **Call the draw function** Call the draw function with the draw descriptor and some other parameters (e.g. `lv_draw_rect()`).  It renders the primitive shape to the current draw buffer. 
3. **Create masks** If the shape is very simple and doesn't require masks go to #5.  Else create the required masks (e.g. a rounded rectangle mask)
4. **Calculate all the added mask**. It creates 0..255 values into a *mask buffer* with the "shape" of the created masks. 
E.g. in case of a "line mask" according to the parameters of the mask, keep one side of the buffer as it is (255 by default) and set the rest to 0 to indicate that this side should be removed.
5. **Blend a color or image** During blending masks (make some pixels transparent or opaque), blending modes (additive, subtractive, etc), opacity are handled.

LVGL has the following built-in mask types which can be calculated and applied real-time:
- `LV_DRAW_MASK_TYPE_LINE` Removes a side from a line (top, bottom, left or right). `lv_draw_line` uses 4 of it. 
Essentially, every (skew) line is bounded with 4 line masks by forming a rectangle.
- `LV_DRAW_MASK_TYPE_RADIUS` Removes the inner or outer parts of a rectangle which can have radius. It's also used to create circles by setting the radius to large value (`LV_RADIUS_CIRCLE`) 
- `LV_DRAW_MASK_TYPE_ANGLE` Removes a circle sector. It is used by `lv_draw_arc` to remove the "empty" sector. 
- `LV_DRAW_MASK_TYPE_FADE` Create a vertical fade (change opacity) 
- `LV_DRAW_MASK_TYPE_MAP` The mask is stored in an array and the necessary parts are applied 

Masks are used the create almost every basic primitives:
- **letters** Create a mask from the letter and draw a rectangle with the letter's color considering the mask.
- **line** Created from 4 "line masks", to mask out the left, right, top and bottom part of the line to get perfectly perpendicular line ending.
- **rounded rectangle** A mask is created real-time to add radius to the corners.
- **clip corner** To clip to overflowing content (usually children) on the rounded corners also a rounded rectangle mask is applied.
- **rectangle border** Same as a rounded rectangle, but inner part is masked out too.
- **arc drawing** A circle border is drawn, but an arc mask is applied too.
- **ARGB images** The alpha channel is separated into a mask and the image is drawn as a normal RGB image.

## Hook drawing
Although widgets can be very well customized by styles there might be cases when something really custom is required. 
To ensure a great level of flexibility LVGL sends a lot events during drawing with parameters that tells what LVGL is about to draw. 
Some fields of these parameters can be modified to draw something else or any custom drawing can be added manually.

A good use case for it is the [Button matrix](/widgets/core/btnmatrix) widget. By default its buttons can be styled in different states but you can't style the buttons one by one. 
However, an event is sent for ever button and you can tell LVGL for example to use different colors on a specific buttons or manually draw an image on an some buttons.

Below each related events are described in detail.

### Main drawing

These events are related to the actual drawing of the object. E.g. drawing of buttons, texts, etc happens here.

`lv_event_get_clip_area(event)` can be used to get the current clip area. The clip area is required in draw functions to make them draw only on limited area. 

#### LV_EVENT_DRAW_MAIN_BEGIN

Sent before starting to draw an object. It's a good place to add masks manually. E.g. add a line mask that "removes" the right side of an object.

#### LV_EVENT_DRAW_MAIN

The actual drawing of the object happens in this event. E.g. a rectangle for a button is drawn here. First, the widgets' internal events are called to perform drawing and after that you can draw anything on top of them.
For example you can add a custom text or an image.

#### LV_EVENT_DRAW_MAIN_END

Called when the main drawing is finished. You can draw anything here as well and it's also good place to remove the masks created in `LV_EVENT_DRAW_MAIN_BEGIN`.

### Post drawing

Post drawing events are called when all the children of an object are drawn. For example LVGL use the post drawing phase to draw the scrollbars because they should be above all the children. 

`lv_event_get_clip_area(event)` can be used to get the current clip area.

#### LV_EVENT_DRAW_POST_BEGIN

Sent before starting the post draw phase. Masks can be added here too to mask out the post drawn content.

#### LV_EVENT_DRAW_POST

The actual drawing should happens here.

#### LV_EVENT_DRAW_POST_END

Called when post drawing has finished. If the masks were not removed in `LV_EVENT_DRAW_MAIN_END` they should be removed here.

### Part drawing

When LVGL draws a part of an object (e.g. a slider's indicator, a table's cell or a button matrix's button) it sends events before and after drawing that part with some context of the drawing.
It allows changing the parts on a very low level with masks, extra drawing, or changing the parameters the LVGL is planning to use for drawing.

In these events an `lv_obj_draw_part_t` structure is used to describe the context of the drawing. Not all fields are set for every part and widget. 
To see which fields are set for a widget see the widget's documentation.

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

// Other paramters 
lv_area_t * draw_area;              // The area of the part being drawn
const lv_point_t * p1;              // A point calculated during drawing. E.g. a point of chart or the center of an arc.
const lv_point_t * p2;              // A point calculated during drawing. E.g. a point of chart.
char text[16];                      // A text calculated during drawing. Can be modified. E.g. tick labels on a chart axis.
lv_coord_t radius;                  // E.g. the radius of an arc (not the corner radius).
int32_t value;                      // A value calculated during drawing. E.g. Chart's tick line value.
const void * sub_part_ptr;          // A pointer the identifies something in the part. E.g. chart series. 
```

`lv_event_get_draw_part_dsc(event)` can be used to get a pointer to `lv_obj_draw_part_t`.

#### LV_EVENT_DRAW_PART_BEGIN

Start the drawing of a part. It's good place to modify the draw descriptors (e.g. `rect_dsc`), or add masks.
 
#### LV_EVENT_DRAW_PART_END
 
Finish the drawing of a part. It's a good place to draw extra content on the part, or remove the masks added in `LV_EVENT_DRAW_PART_BEGIN`.

### Others

#### LV_EVENT_COVER_CHECK   

This event is used to check whether an object fully covers an area or not. 

`lv_event_get_cover_area(event)` returns an pointer to an area to check and `lv_event_set_cover_res(event, res)` can be used to set one of these results:
- `LV_COVER_RES_COVER` the areas is fully covered by the object
- `LV_COVER_RES_NOT_COVER` the areas is not covered by the object
- `LV_COVER_RES_MASKED` there is a mask on the object so it can not covert the area

Here are some cases why can't an object fully cover an area:
- It's simply not fully on the that area
- It has radius
- It has not 100% background opacity
- It's an ARGB or chroma keyed image
- It has not normal blending mode. In this case LVGL needs to know the colors under the object to make the blending properly
- It's a text, etc 

In short if for any reason the the area below the object is visible than it doesn't cover that area.

Before sending this event LVGL checks if at least the widget's coordinates fully cover the area or not. If not the event is not called.

You need to check only the drawing you have added. The existing properties known by widget are handled in the widget's internal events. 
E.g. if a widget has &gt; 0 radius it might not cover an area but you need to handle `radius` only if you will modify it and widget can't know about it. 

#### LV_EVENT_REFR_EXT_DRAW_SIZE

If you need to draw outside of a widget LVGL needs to know about it to provide the extra space for drawing. 
Let's say you create an event the writes the current value of a slider above its knob. In this case LVGL needs to know that the slider's draw area should be larger with the size required for the text.

You can simple set the required draw area with `lv_event_set_ext_draw_size(e, size)`. 

