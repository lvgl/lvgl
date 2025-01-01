.. _draw_pipeline:

================
Drawing Pipeline
================

Overview
--------

LVGL has a flexible and extendable drawing pipeline.  You can hook it to do
some rendering with a GPU or even completely replace the built-in
software renderer.

By using events, it's also possible to modify `draw_tasks` on insert new ones as LVGL renders the widgets.

The followings describe the basics terminology and concept of rendering.

Draw task
---------


When :cpp:expr:`lv_draw_rect`, :cpp:expr:`lv_draw_label` or similar functions are called
LVGL creates a so called draw task.

Draw unit
---------

The draw tasks are collected in a list and periodically dispatched to draw units. A
draw unit can a CPU core, a GPU, just a new rendering library for certain or all draw tasks,
or basically anything that can draw somehow.

As a reference draw unit take a look at `lv_draw_sw.c <https://github.com/lvgl/lvgl/blob/master/src/draw/sw/lv_draw_sw.c>`__

Draw task evaluation
--------------------

Different draw units might render slight different output for example for an image transformation or
a gradient. If such a draw task were assigned to a different draw units, the screen might jitter a
little bit. To resolve it each draw unit has an ``evaluate_cb`` which is called when a draw task is created.
Based on the type and parameters of the draw task each draw unit can decide if it want to assign the
draw task to itself. This way a certain type of draw task (e.g. rounded rectangle with horizontal
gradient) will be always assigned to the same draw unit. It avoid the above mentioned issue of
slight difference between draw units.


Dispatching
-----------

While collecting draw tasks LVGL frequently tries to dispatch the collected draw tasks to the draw units.
This handles via the ``dispatch_cb`` of the draw units.

If a draw unit is busy with another draw task, it just returns. However, it is available it can take a draw task.

:cpp:expr:`lv_draw_get_next_available_task(layer, previous_task, draw_unit_id)` is a useful helper function which
returns an available draw task. "Available draw task" means that, all the draw tasks which should be drawn under a draw task
are ready and it is assigned to the given draw unit.


Hierarchy of modules
--------------------

All these together looks like this

- the list of draw units is stpred in a separate list
- display(s): Each display has a main layer and other layer might be create during rendering

   - layer(s): Each display has its own list of layers. Learn more at :refr:`draw_layers`.

      - draw tasks: Each layer has its own list of draw tasks

Draw Events
-----------

LVGL provides two ways to customize the rendering of any widgets:
- Allow adding custom ``draw_task``s at various stages of rendering. The related event codes are:

  - ``LV_EVENT_DRAW_MAIN_BEGIN``, ``LV_EVENT_DRAW_MAIN``, ``LV_EVENT_DRAW_MAIN_END``:
    Called before, during, or after the widget is drawn. The built-in widget rendering usually happens in ``LV_EVENT_DRAW_MAIN``.
  - ``LV_EVENT_DRAW_POST_BEGIN``, ``LV_EVENT_DRAW_POST``, ``LV_EVENT_DRAW_POST_END``:
    Called before, during, or after all the children are rendered to add some overlay-like drawings. For example, scrollbars are rendered here.

- Modify a ``draw_task`` created by a widget. For this, ``LV_EVENT_DRAW_TASK_ADDED`` can be used.
  For performance reasons, this event is disabled by default and can be enabled by setting the
  ``LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS`` flag.
  A use case for this event is when you want to modify each bar of a bar chart.

