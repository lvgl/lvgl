.. _draw:

================
Drawing Pipeline
================


Overview
********

Drawing is writing pixel colors into a buffer where they will be delivered to a
display panel as pixels.  Sometimes it involves computing those colors before they
are written (e.g. combining them with other colors when an object has partial opacity).

On modern computing hardware meant to be used with larger display panels, there are
sometimes options for different ways drawing can be accomplished.  For example, some
MCUs come with hardware that is very good (and fast) at certain types of drawing
tasks.  Alternatively, you might have access to a drawing library that performs
certain types of drawing tasks with great efficiency.  To make it possible to utilize
such facilities in the most efficient fashion, LVGL v9 and onwards implements a
"Drawing Pipeline", like an assembly line, where decisions are made as to which
drawing tasks (:ref:`Draw Tasks`) are given to which "logic entities"
(:ref:`Draw Units`) in order to be carried out.

This Pipeline is designed so that it is both flexible and extendable.  As a
programmer, you can hook into it in order to provide LVGL with guidance as to what
Draw Units should receive what types of Draw Tasks, or replace LVGL's built-in
software rendering logic to any degree you choose.


.. _draw tasks:

Draw Tasks
----------

A "Draw Task" (:cpp:type:`lv_draw_task_t`) is a package of information that is
created at the beginning of the Drawing Pipeline when a request to draw is made.
Functions such as :cpp:expr:`lv_draw_rect()` and :cpp:expr:`lv_draw_label()` create
one or more Draw Tasks and pass them down the Drawing Pipeline.  Each Draw Task
carries all the information required to:

- compute which :ref:`Draw Unit <draw units>` should receive this task, and
- give the Draw Unit all the details required to accomplish the drawing task.

A Draw Task carries the following information:

:type:                    defines the drawing algorithm involved (e.g. line, fill, border, image,
                          label, arc, triangle, etc.)
:area:                    defines the rectangle involved where drawing will occur
:transformation matrix:   if :c:macro:`LV_DRAW_TRANSFORM_USE_MATRIX` is configured to '1'
:state:                   waiting, queued, in progress, completed
:drawing descriptor:      carries details of the drawing to be performed
:preferred Draw Unit ID:  the ID of the Draw Unit that should take this task
:preference score:        value describing the speed of the specified Draw Unit relative
                          to software rendering (more on this below)
:next:                    a link to the next drawing task in the list.

Draw Tasks are collected in a list and periodically dispatched to Draw Units.


.. _draw units:

Draw Units
----------

A "Draw Unit" (based on :cpp:type:`lv_draw_unit_t`) is any "logic entity" that can
generate the output required by a :ref:`Draw Task <draw tasks>`.  This can be a CPU
core, a GPU, a new rendering library for certain (or all) Draw Tasks, or anything
that can accomplish drawing.

During LVGL's initialization (:cpp:func:`lv_init`), a list of Draw Units is created.
If :c:macro:`LV_USE_DRAW_SW` is set to ``1`` in ``lv_conf.h`` (it is by default), the
Software Drawing Unit enters itself at the head of that list.  If your platform has
other drawing units available, if they are configured to be used in ``lv_conf.h``,
they are added to this list during LVGL's initialization.  If you are adding your own
Draw Unit(s), you add each available drawing unit to that list by calling
:cpp:expr:`lv_draw_create_unit(sizeof(your_draw_unit_t))`.  With each call to that
function, the newly-created draw unit is added to the head of that list, pushing
already-existing draw units further back in the list, making the earliest Draw Unit
created last in the list.  The order of this list (and thus the order in which
:ref:`Draw Task Evaluation` is performed) is governed by the order in which each Draw
Unit is created.

Building this list (and initializing the Draw Units) is normally handled automatically
by configuring the available Draw Units in ``lv_conf.h``, such as setting
:c:macro:`LV_USE_DRAW_OPENGLES` or
:c:macro:`LV_USE_PXP` or
:c:macro:`LV_USE_DRAW_SDL` or
:c:macro:`LV_USE_DRAW_VG_LITE`
to ``1``.  However, if you are introducing your own Draw Unit(s), you will need to
create and initialize it (after :cpp:func:`lv_init`) as above.  This will include
several things, but setting its ``evaluate_cb`` and ``dispatch_cb`` callbacks
(mentioned later) are two of them.

For an example of how draw-unit cration and initialization is done, see
:cpp:func:`lv_draw_sw_init` in lv_draw_sw.c_ or the other draw units whose ``init``
functions are optionally called in :cpp:func:`lv_init`.


.. _draw task evaluation:

Draw Task Evaluation
--------------------

When each :ref:`Draw Task <draw tasks>` is created, each existing Draw Unit is
"consulted" as to its "appropriateness" for the task.  It does this through
an "evaluation callback" function pointer (a.k.a. ``evaluate_cb``), which each Draw
Unit sets (for itself) during its initialization.  Normally, that evaluation
optionally examines the existing "preference score" for the task mentioned above,
and if it can accomplish that type of task (e.g. line drawing) faster than other
Draw Units that have already reported, it writes its own "preference score" and
"preferred Draw Unit ID" to the respective fields in the task.  In this way, by the
time the evaluation sequence is complete, the task will contain the score and the ID
of the Drawing Unit that will be used to perform that task when it is
:ref:`dispatched <draw task dispatching>`.

As a side effect, this also ensures that the same Draw Unit will be selected
consistently, depending on the type (and nature) of the drawing task, avoiding any
possible screen jitter in case more than one Draw Unit is capable of performing a
given task type.

The sequence of the Draw Unit list (with the Software Draw Unit at the end) also
ensures that the Software Draw Unit is the "buck-stops-here" Draw Unit:  if no other
Draw Unit reported it was better at a given drawing task, then the Software Draw Unit
will handle it.


.. _draw task dispatching:

Dispatching
-----------

While collecting Draw Tasks LVGL frequently dispatches the collected Draw Tasks to
their assigned Draw Units. This is handled via the ``dispatch_cb`` of the Draw Units.

If a Draw Unit is busy with another Draw Task, it just returns. However, if it is
available it can take a Draw Task.

:cpp:expr:`lv_draw_get_next_available_task(layer, previous_task, draw_unit_id)` is a
useful helper function which is used by the ``dispatch_cb`` to get the next Draw Task
it should act on.  If it handled the task, it sets the Draw Task's ``state`` field to
:cpp:enumerator:`LV_DRAW_TASK_STATE_READY` (meaning "completed").  "Available" in
this context means that has been queued and assigned to a given Draw Unit and is
ready to be carried out.  The ramifications of having multiple drawing threads are
taken into account for this.


Layers
------

A layer is a buffer with a given area on which the pixel rendering occurrs.  Each
display has a "main" layer, but during rendering additional layers might be created
internally to handle for example arbitrary Widget transformations.


Object Hierarchy
----------------

All of the above have this relationship at run time:

- LVGL

  - list of Draw Units
  - list of Display(s)

    - Layer(s): Each Display has its own list of Layers

      - Draw Tasks: Each Layer has its own list of Draw Tasks



.. _draw_events:

Events
******

- :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED` when each :ref:`Draw Task <draw tasks>`
  is created and before it is dispatched to the :ref:`Draw Unit <draw units>` that
  will handle it.



.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.

    lv_draw_sw.c_


.. _lv_draw_sw.c:  https://github.com/lvgl/lvgl/blob/master/src/draw/sw/lv_draw_sw.c



API
***

