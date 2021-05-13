```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/drawing.md
```
# Drawing

With LVGL, you don't need to draw anything manually. Just create objects (like buttons, labels, arc, etc), move and change them and LVGL will refresh and redraw what is required.

However, it might be useful to have a basic understanding of how drawing happens in LVGL.

The basic concept is to not draw directly to the screen, but draw to an internal draw buffer first and then copy that buffer to screen when the rendering is ready. 
The draw buffer can be smaller than the screen's size. LVGL will simple draw "tiles" that fit into the given draw buffer.
This approach has two main advantages compared to directly drawing to the screen:
1. Avoids flickering while layers of the UI are drawn. For example, when drawing a *background + button + text*, each "stage" would be visible for a short time if LVGL drawn directly into the display.
2. It's faster to modify a buffer in internal RAM and finally write one pixel once than reading/writing the display directly on each pixel access. 
(e.g. via a display controller with SPI interface). 

Note that, this concept is different from "traditional" double buffering where there are 2 screen sized frame buffers: 
one holds the current image to display, rendering happens to the other (inactive) frame buffer, and they are swapped when the rendering is finished. 
The main difference is that with LVGL you don't have to store 2 frame buffers (which usually requires external RAM) but only smaller draw buffer(s) that can easily fit into the internal RAM too.

## Buffering modes

As you already might learn in the [Porting](/porting/display) section, there are 2 types of buffering:
1. **One buffer**  LVGL draws the content of the screen into a draw buffer and sends it to the display. 
The buffer can be smaller than the screen. In this case, the larger areas will be redrawn in multiple parts. 
If only small areas changes (e.g. a button is  pressed), then only those areas will be refreshed.
2. **Two buffers**  Having two buffers, LVGL can draw into one buffer while the content of the other buffer is sent to display in the background.
DMA or other hardware should be used to transfer the data to the display to let the CPU draw meanwhile.
This way, the rendering and refreshing of the display become parallel. If the buffer is smaller than the area to refresh, LVGL will draw the display's content in chunks similar to the *One buffer*.

In the display driver (`lv_disp_drv_t`) the `full_refresh` bit can be enabled to force LVGL always redraw the whole screen. It works with both *One buffer* and *Two buffer* modes.
With `full_refresh` enabled and providing 2 screen sized draw buffers LVGL work as in "traditional" double buffering.

## Mechanism of screen refreshing

1. Something happens on the GUI which requires redrawing. For example, a button has been pressed, a chart has been changed or an animation happened, etc.
2. LVGL saves the changed object's old and new area into a buffer, called an *Invalid area buffer*. For optimization, in some cases, objects are not added to the buffer:
    - Hidden objects are not added.
    - Objects completely out of their parent are not added.
    - Areas out of the parent are cropped to the parent's area.
    - The object on other screens are not added.
3. In every `LV_DISP_DEF_REFR_PERIOD` (set in *lv_conf.h*):
    - LVGL checks the invalid areas and joins the adjacent or intersecting areas.
    - Takes the first joined area, if it's smaller than the *draw buffer*, then simply draw the areas' content to the *draw buffer*. 
      If the area doesn't fit into the buffer, draw as many lines as possible to the *draw buffer*.
    - When the area is rendered, call `flush_cb` from the display driver to refresh the display.
    - If the area was larger than the buffer, render the remaining parts too.
    - Do the same with all the joined areas.

While an area is redrawn, the library searches the top most object which covers the area to redraw, and starts to draw from that object.
For example, if a button's label has changed, the library will see that it's enough to draw the button under the text, and it's not required to draw the screen too.

The difference between buffer types regarding the drawing mechanism is the following:
1. **One buffer** - LVGL needs to wait for `lv_disp_flush_ready()` (called at the end of `flush_cb`) before starting to redraw the next part.
2. **Two buffers** -  LVGL can immediately draw to the second buffer when the first is sent to `flush_cb` because the flushing should be done by DMA (or similar hardware) in the background.
3. **Double buffering** -  `flush_cb` should only swap the address of the frame buffer.

## Masking
*Masking* is the basic concept of LVGL's drawing engine. 
To use LVGL it's not required to know about the mechanisms described here, 
but you might find interesting to know how the drawing works under hood. 

To learn masking let's learn the steps of drawing first:
1. Create a draw descriptor from an object's styles (e.g. `lv_draw_rect_dsc_t`). 
It tells the parameters of drawing, for example the colors, widths, opacity, fonts, radius, etc. 
2. Call the draw function with the initialized descriptor and some other parameters. (E.g. `lv_draw_rect()`) 
It renders the primitive shape to the current draw buffer. 
3. If the shape is very simple and doesn't require masks go to #5. 
Else create the required masks (e.g. a rounded rectangle mask)
4. Apply all the created mask(s) for one or a few lines. 
It create 0..255 values into a *mask buffer* with the "shape" of the created masks. 
E.g. in case of a "line mask" according to the parameters of the mask, 
keep one side of the buffer as it is (255 by default) and set the rest to 0 to indicate that the latter side should be removed.
5. Blend the image or rectangle to the screen. 
During blending masks (make some pixels transparent or opaque), blending modes (additive, subtractive, etc), opacity are handled.
6. Repeat from  #4. 

LVGL has the following built-in mask types which can be calculated and applied real-time:
- `LV_DRAW_MASK_TYPE_LINE` Removes a side of a line (top, bottom, left or right). `lv_draw_line` uses 4 of it. 
Essentially, every (skew) line is bounded with 4 line masks by forming a rectangle.
- `LV_DRAW_MASK_TYPE_RADIUS` Removes the inner or outer parts of a rectangle which can have radius too. It's also used to create circles by setting the radius to large value (`LV_RADIUS_CIRCLE`) 
- `LV_DRAW_MASK_TYPE_ANGLE` Removes a circle sector. It is used by `lv_draw_arc` to remove the "empty" sector. 
- `LV_DRAW_MASK_TYPE_FADE` Create a vertical fade (change opacity) 
- `LV_DRAW_MASK_TYPE_MAP` The mask is stored in an array and the necessary parts are applied 

Masks are used the create almost every basic primitives:
- **letters** create a mask from the letter and draw a “letter-colored” rectangle using the mask.
- **line** created from 4 "line masks", to mask out the left, right, top and bottom part of the line to get perfectly perpendicular line ending
- **rounded rectangle** a mask is created real-time for each line of a rounded rectangle and a normal filled rectangle is drawn according to the mask.
- **clip corner** to clip to overflowing content on the rounded corners also a rounded rectangle mask is applied.
- **rectangle border** same as a rounded rectangle, but inner part is masked out too.
- **arc drawing** a circle border is drawn, but an arc mask is applied too.
- **ARGB images** the alpha channel is separated into a mask and the image is drawn as a normal RGB image.

As mentioned in #3 above in some cases no mask is required:
- rectangles without radius
- non ARGB images

## Hooking drawing
Although widget can be very well customized by styles ther might be cases when something really custom is required. 
To ensure a great level of flexibility LVGL sends a lot events during drawing with parameters that tells what LVGL is planning to draw. 
Some fields of these parameters can be modified to draw something else or any custom drawing added manually.

A good use case for it is the [Button amtrix](/widgets/core/btnmatric) widget. Its buttons can be styled in different states but you can't style the buttons one by one. 
However, an event is sent for ever button and you can tell LVGL for example to use different colors on a specific button or manually draw an image on an other button.

Below each related events are described in detail.

### Main drawing

These events are related to the actual drawing of the object. E.g. drawing of buttons, texts, etc happens here.

`lv_event_get_clip_area(event)` can be used to get the current clip area.

#### LV_EVENT_DRAW_MAIN_BEGIN

Sent before starting to draw the object. It's a good place to add masks manually. E.g. add a line mask the "hides" the right side of an object.

#### LV_EVENT_DRAW_MAIN

The actual drawing of the object. E.g. a rectangle for a button is drawn here. First the widget's events are called to perform drawing and you can draw anything on top of them.
For example you can add a custom text or image.

#### LV_EVENT_DRAW_MAIN_END

Called when the main drawing is finished. You can draw anything here as well and it's also good place to remove the masks created in `LV_EVENT_DRAW_MAIN_BEGIN`.

### Post drawing

Post drawing events are called when all the children of an object are drawn. For example LVGL use the post drawing "phase" to draw the scrollbars because they should be above all the children. 

`lv_event_get_clip_area(event)` can be used to get the current clip area.

#### LV_EVENT_DRAW_POST_BEGIN

Sent before starting to starting the post draw phase. Masks can be added here too to mask out the post drawn content.

#### LV_EVENT_DRAW_POST

The actual drawing should happens here.

#### LV_EVENT_DRAW_POST_END

Called when post drawing has finished. If the mask were not removed in `LV_EVENT_DRAW_MAIN_END` they should be removed here.


### Part drawing

When LVGL draws a part of an object (e.g. a slider's indicator, a table's cell or a button matrix's button) it sends events before and after drawing that part with some context of the drawing.
It allows changind the parts on a very low level with masks, extra drawing, or changing the parameters the LVGL is planning to use for drawing.

In these events an `lv_obj_draw_part_t` structure is used to describe the context of the drawing. Not all fields are set. To see which fields are set for a widget see the widgets documentation.

It has the following fields:

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

Start drawing a part. It's good place to modify the draw descriptors (e.g. `rect_dsc`), or add masks.
 
#### LV_EVENT_DRAW_PART_END
 
Finish to drawing a part. It's a good place to draw extra content on the part, or remove the masks added in `LV_EVENT_DRAW_PART_BEGIN`.

### Others

#### LV_EVENT_COVER_CHECK   

Check if the object fully covers an area. `lv_event_get_cover_check_info(event)` returns an pointer to an `lv_cover_check_info_t` variable. Its `res` field should be set to the following values considering the `area` field: 
- `LV_DRAW_RES_COVER` the areas is fully covered
- `LV_DRAW_RES_NOT_COVER` the areas is not covered
- `LV_DRAW_RES_MASKED` the areas is masked out

Some guideline how to set the result:
- If already set to `LV_DRAW_RES_MASKED` do nothing. In this case an other event already set it and it's the "strongest" state that shouldn't be overwritten.
- If you added a draw mask on the object set `res` to `LV_DRAW_RES_MASKED`
- If there is no draw mask but the object simply not covers the area set `LV_DRAW_RES_NOT_COVER`
- If the area is fully covered by the object leave `res` unchanged.

In the practice probably you need to set `LV_DRAW_RES_MASKED` only if you added masks in a MAIN or POST draw events because "normal" cover checks are handles by the widgets.

However, if you really added masks in MAIN or POST draw events you need to `LV_EVENT_COVER_CHECK` event and tell LVGL there are masks on this object.

If masks are added and removed in  `LV_EVENT_DRAW_PART_BEGIN/END`, `LV_EVENT_COVER_CHECK` doesn't need to know about it except the masks affects `LV_PART_MAIN`. 
It's because if LVGL checks the main part to decide whether an object covers an area or not. So it doesn't matter e.g. if a tabel's cell is masked because the tables background already covered the area or not.

#### LV_EVENT_REFR_EXT_DRAW_SIZE

If you need to draw outside of a widget LVGL needs to know about it to provide this extra space. 
It good use case for it if you creat an event the writes the current value of a  slider above the knob. In this case LVGL need to know that the slider's draw area is larger with size required for the text.

`lv_event_get_ext_draw_size_info(event)` return a pointer to an `lv_coord_t` variable in which the extra draw size should be set. 
Note that, other events also might set values in this variable you should only values larger than the current value. For example:
```c
lv_coord_t * s = lv_event_get_ext_draw_size_info(event);
*s = LV_MAX(*s, 50);
``` 

