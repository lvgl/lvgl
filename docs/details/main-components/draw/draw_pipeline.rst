.. _draw_pipeline:

================
Drawing Pipeline
================

Overview
--------

LVGL has a flexible and extendable drawing pipeline. You can use it to perform
custom rendering with a GPU or even completely replace the built-in software
renderer.

Using events, it's also possible to modify `draw_tasks` or insert new ones as
LVGL renders widgets.

The following sections describe the basic terminology and concepts of rendering.

Draw Task
---------

When functions like :cpp:expr:`lv_draw_rect`, :cpp:expr:`lv_draw_label`, or similar
are called, LVGL creates a "draw task."

Draw Unit
---------

The draw tasks are collected in a list and periodically dispatched to draw units.
A draw unit can represent a CPU core, GPU, custom rendering library for specific
tasks, or any entity capable of performing rendering.

For a reference implementation of a draw unit, see
`lv_draw_sw.c <https://github.com/lvgl/lvgl/blob/master/src/draw/sw/lv_draw_sw.c>`__.

Draw Task Evaluation
--------------------

Different draw units may render slightly different outputs (e.g., for image
transformations or gradients). If a draw task is assigned to different draw
units, the screen might jitter due to these differences.

To address this, each draw unit has an ``evaluate_cb`` function, called when a
draw task is created. Based on the task's type and parameters, each draw unit
decides whether to assign the task to itself. This ensures specific types of
draw tasks (e.g., rounded rectangles with horizontal gradients) are always
assigned to the same draw unit, avoiding jitter.

Dispatching
-----------

While collecting draw tasks, LVGL frequently attempts to dispatch the tasks to
available draw units. This is handled via the ``dispatch_cb`` function of each
draw unit.

If a draw unit is busy, it simply returns. If available, it accepts a draw task.

The helper function :cpp:expr:`lv_draw_get_next_available_task(layer, previous_task, draw_unit_id)`
returns an available draw task. An "available draw task" is one where all
dependencies (tasks that should be drawn beneath it) are completed and the task
is assigned to the given draw unit.

Hierarchy of Modules
--------------------

The structure of the drawing pipeline is as follows:

- **Draw Units**: Stored in a separate list.
- **Displays**: Each display has a main layer, and additional layers may be
  created during rendering.

  - **Draw Layers**: Each display maintains a list of layers. See
    :ref:`draw_layers` for more details.

    - **Draw Tasks**: Each layer has its own list of draw tasks.
