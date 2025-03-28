.. _draw_api:

========
Draw API
========

Where to Use the Drawing API
****************************

In most cases you use LVGL's Drawing API through the API of Widgets:  by creating
buttons, labels, etc., and setting the their styles, positions, and other properties.
In these cases rendering (drawing) is handled internally and you doen't see the
:ref:`Drawing Pipeline <draw_pipeline>` at all.

However there are three places where you can use LVGL's Drawing API directly.

1. **In the draw events of the Widgets**:
   There are event codes which are sent when the Widget needs to render itself:

   - :cpp:enumerator:`LV_EVENT_DRAW_MAIN_BEGIN`, :cpp:enumerator:`LV_EVENT_DRAW_MAIN`,
     :cpp:enumerator:`LV_EVENT_DRAW_MAIN_END`:
     Triggered before, during, and after a Widget is drawn, respectively.  Widget
     rendering typically occurs in :cpp:enumerator:`LV_EVENT_DRAW_MAIN`.
   - :cpp:enumerator:`LV_EVENT_DRAW_POST_BEGIN`, :cpp:enumerator:`LV_EVENT_DRAW_POST`,
     :cpp:enumerator:`LV_EVENT_DRAW_POST_END`:
     Triggered before, during, and after all child Widgets are rendered, respectively.
     This can be useful for overlay-like drawings, such as scrollbars which should be
     rendered on top of any children.

   These are relevant if a new Widget is implemented and it uses custom drawing.

2. **Modifying the created draw tasks**:
   The when a draw task is created for a Widget :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`
   is sent.  In this event the created draw task can be modified or new draw tasks
   can be added.  Typical use cases for this are modifying each bar of a bar chart,
   or cells of a table.

   For performance reasons, this event is disabled by default.  Enable it by setting
   the :cpp:enumerator:`LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS` flag on the Widget(s) you
   wish to emit this event.

3. **Draw to the Canvas Widget**:
   The drawing functions can be used directly to draw to a Canvas Widget.  Doing so
   renders custom drawing to a buffer which can be used later as an image or a mask.

   For more information see :ref:`lv_canvas`.



Drawing API
***********

The main components of LVGL's Drawing API are the :cpp:func:`lv_draw_rect`,
:cpp:func:`lv_draw_label`, :cpp:func:`lv_draw_image`, and similar functions.
When they are called :cpp:type:`lv_draw_task_t` objects are created internally.

These functions have the following parameters:

- **Layer**:  This is the target of the drawing.  See details at :ref:`draw_layers`.
- **Draw Descriptor**:  This is a large ``struct`` containing all the information
  about the drawing.  See details of the draw descriptors at :ref:`draw_descriptors`.
- **Area** (in some cases):  Specifies where to draw.



Coordinate System
*****************

Some functions and draw descriptors require area or point parameters.  These are
always **absolute coordinates** on the display.  For example, if the target layer is
on a 800x480 display and the layer's area is (100,100) (200,200), to render a 10x10
object in the middle, the coordinates (145,145) (154,154) should be used
(not (40,40) (49,49)).

Exception:  for the Canvas Widget the layer is always assumed to be at the (0,0)
coordinate, regardless of the Canvas Widget's position.



API
***

.. API equals:
    LV_EVENT_DRAW_MAIN_BEGIN
    lv_draw_arc
    lv_draw_image
    lv_draw_label
    lv_draw_line
    lv_draw_mask_rect
    lv_draw_triangle
