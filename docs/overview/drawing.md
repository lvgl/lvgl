```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/drawing.md
```
# Drawing

With LVGL, you don't need to draw anything manually. Just create objects (like buttons and labels), move and change them and LVGL will refresh and redraw what is required.

However, it might be useful to have a basic understanding of how drawing happens in LVGL.

The basic concept is to not draw directly to the screen, but draw to an internal buffer first and then copy that buffer to screen when the rendering is ready. It has two main advantages:
1. **Avoids flickering** while layers of the UI are drawn. For example, when drawing a *background + button + text*, each "stage" would be visible for a short time.
2. **It's faster** to modify a buffer in RAM and finally write one pixel once
than read/write a display directly on each pixel access. (e.g. via a display controller with SPI interface). Hence, it's suitable for pixels that are redrawn multiple times (e.g. background + button + text).

## Buffering types

As you already might learn in the [Porting](/porting/display) section, there are 3 types of buffers:
1. **One buffer** -  LVGL draws the content of the screen into a buffer and sends it to the display. The buffer can be smaller than the screen. In this case, the larger areas will be redrawn in multiple parts. If only small areas changes (e.g. button press), then only those areas will be refreshed.
2. **Two non-screen-sized buffers** -  having two buffers, LVGL can draw into one buffer while the content of the other buffer is sent to display in the background.
DMA or other hardware should be used to transfer the data to the display to let the CPU draw meanwhile.
This way, the rendering and refreshing of the display become parallel. If the buffer is smaller than the area to refresh, LVGL will draw the display's content in chunks similar to the *One buffer*.
3. **Two screen-sized buffers** -
In contrast to *Two non-screen-sized buffers*, LVGL will always provide the whole screen's content, not only chunks. This way, the driver can simply change the address of the frame buffer to the buffer received from LVGL.
Therefore, this method works best when the MCU has an LCD/TFT interface and the frame buffer is just a location in the RAM.

## Mechanism of screen refreshing

1. Something happens on the GUI which requires redrawing. For example, a button has been pressed, a chart has been changed or an animation happened, etc.
2. LVGL saves the changed object's old and new area into a buffer, called an *Invalid area buffer*. For optimization, in some cases, objects are not added to the buffer:
    - Hidden objects are not added.
    - Objects completely out of their parent are not added.
    - Areas out of the parent are cropped to the parent's area.
    - The object on other screens are not added.
3. In every `LV_DISP_DEF_REFR_PERIOD` (set in *lv_conf.h*):
    - LVGL checks the invalid areas and joins the adjacent or intersecting areas.
    - Takes the first joined area, if it's smaller than the *display buffer*, then simply draw the areas' content to the *display buffer*. If the area doesn't fit into the buffer, draw as many lines as possible to the *display buffer*.
    - When the area is drawn, call `flush_cb` from the display driver to refresh the display.
    - If the area was larger than the buffer, redraw the remaining parts too.
    - Do the same with all the joined areas.

While an area is redrawn, the library searches the most top object which covers the area to redraw, and starts to draw from that object.
For example, if a button's label has changed, the library will see that it's enough to draw the button under the text, and it's not required to draw the background too.

The difference between buffer types regarding the drawing mechanism is the following:
1. **One buffer** - LVGL needs to wait for `lv_disp_flush_ready()` (called at the end of `flush_cb`) before starting to redraw the next part.
2. **Two non-screen-sized buffers** -  LVGL can immediately draw to the second buffer when the first is sent to `flush_cb` because the flushing should be done by DMA (or similar hardware) in the background.
3. **Two screen-sized buffers** -  After calling `flush_cb`, the first buffer, if being displayed as frame buffer. Its content is copied to the second buffer and all the changes are drawn on top of it.

## Masking
*Masking* is the basic concept of LVGL's drawing engine. 
To use LVGL it's not required to know about the mechanisms described here, 
but you might find interesting to know how the drawing works under hood. 

To learn masking let's learn the steps of drawing first:
1. Create a draw descriptor from an object's styles (e.g. `lv_draw_rect_dsc_t`). 
It tells the parameters of drawing, for example the colors, widths, opacity, fonts, radius, etc. 
2. Call the draw function with the initialized descriptor and some other parameters. 
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

Masks are used the create almost every basic primitives:
- **letters** create a mask from the letter and draw a “letter-colored” rectangle using the mask.
- **line** created from 4 "line masks", to mask out the left, right, top and bottom part of the line to get perfectly perpendicular line ending
- **rounded rectangle** a mask is created real-time for each line of a rounded rectangle and a normal filled rectangle is drawn according to the mask.
- **clip corner** to clip to overflowing content on the rounded corners also a rounded rectangle mask is applied.
- **rectangle border** same as a rounded rectangle, but inner part is masked out too
- **arc drawing** a circle border is drawn, but an arc mask is applied.
- **ARGB images** the alpha channel is separated into a mask and the image is drawn as a normal RGB image.

As mentioned in #3 above in some cases no mask is required:
- a mono colored, not rounded rectangles
- RGB images


LVGL has the following built-in mask types which can be calculated and applied real-time:
- **LV_DRAW_MASK_TYPE_LINE** Removes a side of a line (top, bottom, left or right). `lv_draw_line` uses 4 of it. 
Essentially, every (skew) line is bounded with 4 line masks by forming a rectangle.
- **LV_DRAW_MASK_TYPE_RADIUS** Removes the inner or outer parts of a rectangle which can have radius too. It's also used to create circles by setting the radius to large value (`LV_RADIUS_CIRCLE`) 
- **LV_DRAW_MASK_TYPE_ANGLE** Removes a circle sector. It is used by `lv_draw_arc` to remove the "empty" sector. 
- **LV_DRAW_MASK_TYPE_FADE** Create a vertical fade (change opacity) 
- **LV_DRAW_MASK_TYPE_MAP** The mask is stored in an array and the necessary parts are applied 


Masks are create and removed automatically during drawing but the [lv_objmask](/widgets/objmask) allows the user to add masks.
Here is an example:

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_objmask/index.rst

```
