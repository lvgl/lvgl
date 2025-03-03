.. _draw_api:

========
Draw API
========

Where to Use the Draw API?
--------------------------

In most of the case LVGL uses through the API of the widget by creating buttons, labels, etc,
 and set the their styles, positions, and other properties. In this case rendering (drawing)
 is handled internally and user doesn't see the :ref:`Drawing Pipeline <draw_pipeline>` at all.

Howere there are three places where LVGL's draw API is directly.

1. **In the draw events of the widgets**:
   There are event codes which are sent when the widget needs to render itself:

   - :cpp:enumerator:`LV_EVENT_DRAW_MAIN_BEGIN`, :cpp:enumerator:`LV_EVENT_DRAW_MAIN`, :cpp:enumerator:`LV_EVENT_DRAW_MAIN_END`:
     Triggered before, during, or after a widget is drawn. Widget
     rendering typically occurs in :cpp:enumerator:`LV_EVENT_DRAW_MAIN`.
   - :cpp:enumerator:`LV_EVENT_DRAW_POST_BEGIN`, :cpp:enumerator:`LV_EVENT_DRAW_POST`, :cpp:enumerator:`LV_EVENT_DRAW_POST_END`:
     Triggered before, during, or after all child widgets are rendered. Useful
     for overlay-like drawings, such as scrollbars which should be rendered on top of the children.

   These are relevant if a new widget is implemented and it has a custom drawing.

2. **Modifying the created draw tasks**:
   The when a draw task is created for a widget :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`
   is sent. In this event the created draw task can be modified or new draw tasks can
   be added. Typical use cases for this are modifying each bar of a bar chart, or cells of a table.

   For performance reasons, this event is disabled by default. Enable it
   by setting the :cpp:enumerator:`LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS` flag.

3. **Draw to the canvas widget**:
   The drawing functions can be used directly to draw to a canvas too.
   This way custom drawings can be rendered to a buffer which can be used later as
   an image or a mask.

   For more information refer to the documentation of the :ref:`lv_canvas`.


The Draw API
------------

The main components of LVGL's the draw API are the ``lv_draw_rect/label/image/etc``
functions. When they are called ``draw_task``s are created internally.

These functions have the following parameters:

- **An :cpp:expr:`lv_layer_t` layer**: This is the target of the drawing.
  Learn more at :ref:`draw_layers`.
- **A draw descriptor**: It is a large ``struct`` containing all the information
  about the drawing. See the details of all the draw descriptors at :ref:`draw_descriptors`.
- **An area** (in some cases): to specify where to draw.

Coordinate System
-----------------

Some functions and draw descriptors require area or point parameters. These are
always **absolute coordinates** on the display. For example, if the target layer is
on a 800x480 display and the layer's area is (100;100) (200;200), to render a 10x10
object in the middle, the coordinates (145;145) (154;154) should be used
(instead of (40;40) (49;49)).

In case of the Canvas widget the layer is always assumed to be at the 0;0
coordinate, regardless of the canvas widget's position.
