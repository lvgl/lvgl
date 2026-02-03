.. _draw_pipeline:

=============
Draw Pipeline
=============


What is Drawing?
****************

Drawing (also known as :dfn:`rendering`) is writing pixel colors into a buffer from
where they will be delivered to a display panel as pixels. It can mean filling areas
with a color, blending images, or using complex algorithms to, for example, calculate rounded
corners or rotate images.

The following sections cover how LVGL's drawing logic works and how to use it.


Draw-Pipeline Overview
**********************

Modern embedded devices come with a wide variety of solutions to speed up rendering:

- 2D GPUs fill areas and blend images
- 2.5D (Vector graphics) GPUs
- 3D GPUs (e.g. for OpenGL)
- SIMD assembly-level acceleration
- multi-core CPUs
- Software libraries
- and more

To make it possible to utilize such facilities in the most efficient fashion, LVGL
implements a :dfn:`Drawing Pipeline`, like an assembly line, where decisions are
made as to which drawing tasks (:ref:`Draw Tasks`) are given to which :ref:`Draw Units`
(rendering engines) in order to be carried out.

This Pipeline is designed so that it is both flexible and extensible. You can use it
to perform custom rendering with a GPU or replace parts of the built-in software
rendering logic to any extent desired.

Using events, it's also possible to modify :ref:`draw tasks` or insert new ones as
LVGL renders Widgets.

The following sections describe the basic terminology and concepts of rendering.


.. _draw tasks:

Draw Tasks
**********

A "Draw Task" (:cpp:type:`lv_draw_task_t`) is a package of information that is
created at the beginning of the Drawing Pipeline when a request to draw is made.
Functions such as :cpp:expr:`lv_draw_rect()` and :cpp:expr:`lv_draw_label()` create
one or more Draw Tasks and pass them down the Drawing Pipeline. Each Draw Task
carries all the information required to:

- compute which :ref:`Draw Unit <draw units>` should receive this task, and
- give the Draw Unit all the information required to accomplish the drawing task.

A Draw Task carries the following information:

:type:                    defines the drawing algorithm involved (e.g. line, fill,
                          border, image, label, arc, triangle, etc.)
:area:                    defines the rectangle in which drawing will occur
:transformation matrix:   if :c:macro:`LV_DRAW_TRANSFORM_USE_MATRIX` is configured to '1'
:state:                   waiting, queued, in progress, completed
:drawing descriptor:      carries details of the drawing to be performed
:preferred Draw Unit ID:  identifier of the Draw Unit that should carry out this task
:preference score:        value describing the speed of the specified Draw Unit relative
                          to software rendering (more on this below)
:next:                    a link to the next Draw Task in the list.

Draw Tasks are collected in a list and periodically dispatched to Draw Units.


.. _draw units:

Draw Units
**********

A "Draw Unit" (based on :cpp:type:`lv_draw_unit_t`) is any "logic entity" that can
generate the output required by a :ref:`Draw Task <draw tasks>`. This can be a CPU
core, a GPU, a custom rendering library for specific Draw Tasks, or any entity
capable of performing rendering.

For a reference implementation of a draw unit, see
`lv_draw_sw.c <https://github.com/lvgl/lvgl/blob/master/src/draw/sw/lv_draw_sw.c>`_.


Creating Draw Units
-------------------

During LVGL's initialization (in :cpp:func:`lv_init`), a list of Draw Units is created
from the enabled built-in draw units. For example, if :c:macro:`LV_USE_DRAW_SW` is
enabled, it will be automatically initialized and used for rendering. The same applies for
:c:macro:`LV_USE_DRAW_OPENGLES`, :c:macro:`LV_USE_PXP`, :c:macro:`LV_USE_DRAW_SDL`, or
:c:macro:`LV_USE_DRAW_VG_LITE`.

You can also add your own Draw Unit(s) after :cpp:func:`lv_init` by calling
:cpp:expr:`lv_draw_create_unit(sizeof(your_draw_unit_t))`. You also need to
add custom ``evaluate_cb`` and ``dispatch_cb`` callbacks (mentioned later)
to the new draw unit.

For an example of how draw-unit creation and initialization is done, see
:cpp:func:`lv_draw_sw_init` in lv_draw_sw.c_ or the other draw units whose ``init``
functions are in :cpp:func:`lv_init`.

Thread Priority
---------------

If :c:macro:`LV_USE_OS` is set to something other than :c:macro:`LV_OS_NONE`, draw units might use a thread to
allow waiting for the completion of rendering in a non-blocking way.

The thread priority can be set using the :c:macro:`LV_DRAW_THREAD_PRIO`
(:c:macro:`LV_THREAD_PRIO_HIGH` by default) configuration option in ``lv_conf.h``.
This allows you to fine-tune the priority level for rendering in general.


Clip Area
---------

LVGL clips the children widgets to the parent's boundary. To do that, it needs to know
the current clip area when creating a draw task. The current clip area is the smallest
intersection of all parent clip areas and the widget to be rendered. So, if a widget is
out of its parent at the bottom and only its top part is visible, the clip area will be
that small top part.

As the current clip area always changes as LVGL traverses the widget tree, the clip
area is saved in each draw task. This clip area should be considered by the draw units
too, for example, to mask out only a smaller part of an image to be blended.


.. _draw task evaluation:

Draw Task Evaluation
********************

When each :ref:`Draw Task <draw tasks>` is created, each existing Draw Unit is
"consulted" as to its "appropriateness" for the task. It does this through
an "evaluation callback" function pointer (a.k.a. ``evaluate_cb``), which each Draw
Unit sets (for itself) during its initialization. Normally, that evaluation:

- optionally examines the existing "preference score" for the task mentioned above,
- if it can accomplish that type of task (e.g. line drawing) faster than other
  Draw Units that have already reported, it writes its own "preference score" and
  "preferred Draw Unit ID" to the respective fields in the task.

In this way, by the time the evaluation sequence is complete, the task will contain
the score and the ID of the Draw Unit that will be used to perform that task when
it is :ref:`dispatched <draw task dispatching>`.

This ensures that the same Draw Unit will be selected
consistently, depending on the type (and nature) of the drawing task, avoiding any
possible screen jitter in case more than one Draw Unit is capable of performing a
given task type.

.. _draw task dispatching:

Dispatching
***********

While collecting Draw Tasks, LVGL frequently dispatches the collected Draw Tasks to
their assigned Draw Units. This is handled via the ``dispatch_cb`` of the Draw Units.

If a Draw Unit is busy with another Draw Task, it just returns. However, if it is
available, it can take a Draw Task.

:cpp:expr:`lv_draw_get_next_available_task(layer, previous_task, draw_unit_id)` is a
useful helper function which is used by the ``dispatch_cb`` to get the next Draw Task
it should act on. If it handled the task, it sets the Draw Task's ``state`` field to
:cpp:enumerator:`LV_DRAW_TASK_STATE_FINISHED`.


Hierarchy Summary
*****************

All of the above have this relationship:

- LVGL (global)

  - list of :ref:`Draw Units`
  - list of :ref:`Display(s) <display_overview>`

    - Layer(s): Each :ref:`Display object <display_overview>` has its own list of :ref:`draw_layers`

      - Draw Tasks: Each Layer has its own list of :ref:`Draw Tasks`



API
***

.. API equals:
    lv_draw_create_unit
    lv_draw_get_next_available_task
    lv_draw_label
    lv_draw_rect
    lv_draw_sw_init
    lv_draw_task_t
    LV_DRAW_TRANSFORM_USE_MATRIX
    lv_draw_unit_t
    LV_USE_DRAW_OPENGLES
