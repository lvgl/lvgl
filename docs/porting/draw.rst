.. _porting_draw:

==========
Custom GPU
==========

LVGL has a flexible and extendable draw pipeline. You can hook it to do
some rendering with a GPU or even completely replace the built-in
software renderer.


Overview
********

Draw task
---------


When :cpp:expr:`lv_draw_rect`, :cpp:expr:`lv_draw_label` or similar functions are called
LVGL creates a so called draw task.

Draw unit
---------

The draw tasks are collected in a list and periodically dispatched to draw units. A
draw unit can a CPU core, a GPU, just a new rendering library for certain or all draw tasks,
or basically anything that can draw somehow.

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

If a draw unit is busy with an other draw task, it just returns. However, it is available it can take a draw task.

:cpp:expr:`lv_draw_get_next_available_task(layer, previous_task, draw_unit_id)` is a useful helper function which
returns an available draw task. "Available draw task" means that, all the draw tasks which should be drawn under a draw task
are ready and it is assigned to the given draw unit.


Layers
------

A layer is a buffer with a given area on which rendering happens. Each display has a "main" layer, but
during rendering additional layers might be created internally to handle for example arbitrary widget transformations.


Hierarchy of modules
--------------------

All these together looks like this
- list of draw units
- display(s)
   - layer(s): Each display has its own list of layers
      - draw tasks: Each layer has its own list of draw tasks

References
**********

As a reference take a look at `lv_draw_sw.c <https://github.com/lvgl/lvgl/blob/master/src/draw/sw/lv_draw_sw.c>`__

API
***

