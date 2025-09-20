.. _animation:

===================
Animation (lv_anim)
===================

Animations allow you to define the way something should move or change over time, and
let LVGL do the heavy lifting of making it happen.  What makes it so powerful is that
the thing being changed can be virtually anything in your system.  It is very
convenient to apply this to LVGL Widgets in your user interface (UI), to change their
appearance, size or location over time.  But because it is --- at its core --- a
generic change-over-time manager, complete with a variety of optional event
callbacks, its application can be wider than just to UI components.

For each Animation you create, it accomplishes the above by providing a generic
method of varying a signed integer from a start value to an end value over a
specified time period.  It allows you to specify what object it applies to (the
"variable"), which is available in the callback functions that are called as the
Animation is playing through.

This variation over time can be linear (default), it can be on a path (curve) that
you specify, and there is even a variety of commonly-used non-linear effects that can
be specified.

The main callback called during an Animation (when it is playing) is called an
*animator* function, which has the following prototype:

.. code-block:: c

    void func(void *var , int32_t value);

This prototype makes it easy to use most of the LVGL *set* functions directly or via a trivial wrapper. It includes:

  - most of the widget properties
  - functions that set :ref:`local style properties <style_local>` directly on objects (needs a wrapper to set the *selector*)
  - set properties on :cpp:type:`lv_style_t` objects (e.g. :ref:`shared styles <style_initialize>`)  (``lv_obj_report_style_change`` needs to be called to notify the widgets having the style)

- ``lv_style_set_<property_name>(&style, <value>)``
- ``lv_obj_set_<property_name>(widget, <value>)``

Because of the former, an animation on a single :cpp:type:`lv_style_t` object shared
among several objects can simultaneously modify the appearance of all objects that
use it.  See :ref:`styles` for more details.

Examples of the latter are:  :cpp:expr:`lv_obj_set_x(widget, value)` or
:cpp:expr:`lv_obj_set_width(widget, value)`.

This makes it very convenient to apply to the appearance (and other attributes) of UI
components.  But you can provide your own "set" functions, and so the application of
Animations is really limited only by your imagination.

The number of Animations that can be playing at the same time for a given object with
a given *animator* callback is one (1).  However, the number of Animations that can
be playing at the same time is limited only by available RAM and CPU time for:

- a given object with different *animator* callbacks; and
- different objects.

Thus, you can have a Button's width being changed by one Animation while having its
height being changed by another Animation.



.. _animations_create:

Create an Animation
*******************

To create an Animation, start by creating an Animation *template* in an
:cpp:type:`lv_anim_t` variable.  It has to be initialized and configured with
``lv_anim_set_...()`` functions.

.. code-block:: c


   /* INITIALIZE AN ANIMATION
    *-----------------------*/

   static lv_anim_t   anim_template;
   static lv_anim_t * running_anim;

   lv_anim_init(&anim_template);

   /* MANDATORY SETTINGS
    *------------------*/

   /* Set the "animator" function */
   lv_anim_set_exec_cb(&anim_template, (lv_anim_exec_xcb_t) lv_obj_set_x);

   /* Set target of the Animation */
   lv_anim_set_var(&anim_template, widget);

   /* Length of the Animation [ms] */
   lv_anim_set_duration(&anim_template, duration_in_ms);

   /* Set start and end values. E.g. 0, 150 */
   lv_anim_set_values(&anim_template, start, end);

   /* OPTIONAL SETTINGS
    *------------------*/

   /* Time to wait before starting the Animation [ms] */
   lv_anim_set_delay(&anim_template, delay);

   /* Set path (curve). Default is linear */
   lv_anim_set_path_cb(&anim_template, lv_anim_path_ease_in);

   /* Set anim_template callback to indicate when the Animation is completed. */
   lv_anim_set_completed_cb(&anim_template, completed_cb);

   /* Set anim_template callback to indicate when the Animation is deleted (idle). */
   lv_anim_set_deleted_cb(&anim_template, deleted_cb);

   /* Set anim_template callback to indicate when the Animation is started (after delay). */
   lv_anim_set_start_cb(&anim_template, start_cb);

   /* When ready, play the Animation backward with this duration. Default is 0 (disabled) [ms] */
   lv_anim_set_reverse_duration(&anim_template, time);

   /* Delay before reverse play. Default is 0 (disabled) [ms] */
   lv_anim_set_reverse_delay(&anim_template, delay);

   /* Number of repetitions. Default is 1. LV_ANIM_REPEAT_INFINITE for infinite repetition */
   lv_anim_set_repeat_count(&anim_template, cnt);

   /* Delay before repeat. Default is 0 (disabled) [ms] */
   lv_anim_set_repeat_delay(&anim_template, delay);

   /* true (default): apply the start value immediately, false: apply start value after delay when the Anim. really starts. */
   lv_anim_set_early_apply(&anim_template, true/false);

   /* START THE ANIMATION
    *------------------*/
   running_anim = lv_anim_start(&anim_template);   /* Start the Animation */



.. _animation_path:

Animation Path
**************

You can control the Path (curve) of an Animation.  The simplest case is linear,
meaning the current value between *start* and *end* is changed at the same rate (i.e.
with fixed steps) over the duration of the Animation.  A *Path* is a function which
calculates the next value to set based on the current state of the Animation.
There are a number of built-in *Paths* that can be used:

-  :cpp:func:`lv_anim_path_linear`: linear Animation (default)
-  :cpp:func:`lv_anim_path_step`: change in one step at the end
-  :cpp:func:`lv_anim_path_ease_in`: slow at the beginning
-  :cpp:func:`lv_anim_path_ease_out`: slow at the end
-  :cpp:func:`lv_anim_path_ease_in_out`: slow at the beginning and end
-  :cpp:func:`lv_anim_path_overshoot`: overshoot the end value
-  :cpp:func:`lv_anim_path_bounce`: bounce back a little from the end value (like
   hitting a wall)

Alternately, you can provide your own Path function.

:cpp:expr:`lv_anim_init(&my_anim)` sets the Path to :cpp:func:`lv_anim_path_linear`
by default.  If you want to use a different Path (including a custom Path function
you provide), you set it using :cpp:expr:`lv_anim_set_path_cb(&anim_template, path_cb)`.

If you provide your own custom Path function, its prototype is:

.. code-block:: c

    int32_t   calculate_value(lv_anim_t * anim);



.. _animation_speed_vs_time:

Speed vs Time
*************

Normally, you set the Animation duration directly using
:cpp:expr:`lv_anim_set_duration(&anim_template, duration_in_ms)`.  But in some cases
the *rate* is known but the duration is not known.  Given an Animation's ``start``
and ``end`` values, *rate* here means the number of units of change per second, i.e.
how quickly (units per second) the Animation's value needs to change between the
``start`` and ``end`` value.  For such cases there is a utility function
:cpp:func:`lv_anim_speed_to_time` you can use to compute the Animation's duration, so
you can set it like this:

.. code-block:: c

    uint32_t  change_per_sec = 20;
    uint32_t  duration_in_ms = lv_anim_speed_to_time(change_per_sec, 0, 100);
    /* `duration_in_ms` will be 5000 */
    lv_anim_set_duration(&anim_template, duration_in_ms);



.. _animation_direction:

Animating in Both Directions
****************************

Sometimes an Animation needs to play forward, and then play backwards, effectively
reversing course, animating from the ``end`` value back to the ``start`` value again.
To do this, pass a non-zero value to this function to set the duration for the
reverse portion of the Animation:
:cpp:expr:`lv_anim_set_reverse_duration(&anim_template, duration_in_ms)`.

Optionally, you can also introduce a delay between the forward and backward
directions using :cpp:expr:`lv_anim_set_reverse_delay(&anim_template, delay_in_ms)`



.. _animation_start:

Starting an Animation
*********************

After you have set up your :cpp:type:`lv_anim_t` object, it is important to realize
that what you have set up is a "template" for a live, running Animation that has
not been created yet.  When you call :cpp:expr:`lv_anim_start(&anim_template)`
passing the *template* you have set up, it uses your template to dynamically allocate
an internal object that is a *live, running* Animation.  This function returns a
pointer to that object.

.. code-block:: c

    static lv_anim_t   anim_template;
    static lv_anim_t * running_anim;

    /* Set up template... */
    lv_anim_init(&anim_template);
    /* ...and other set-up functions above. */

    /* Later... */
    running_anim = lv_anim_start(&anim_template);

.. note::

    :cpp:expr:`lv_anim_start(&anim_template)` makes its own copy of the Animation
    template, so if you do not need it later, its contents do not need to be
    preserved after this call.

Once a *live running* Animation has been started, it runs until it has completed,
or until it is deleted (see below), whichever comes first.  An Animation has
completed when:

- its "value" has reached the designated ``end`` value;
- if the Animation has a non-zero *reverse* duration value, then its value
  has run from the ``end`` value back to the ``start`` value again;
- if a non-zero repeat count has been set, it has repeated the Animation
  that number of times.

Once the *live, running* Animation reaches completion, it is automatically deleted
from the list of running Animations.  This does not impact your Animation template.

.. note::

    If :cpp:expr:`lv_anim_set_repeat_count(&anim_template, cnt)` has been called
    passing :c:macro:`LV_ANIM_REPEAT_INFINITE`, the animation never reaches a state
    of being "completed".  In this case, it must be deleted to terminate the
    Animation.



.. _animation_delete:

Deleting Animations
*******************

You should delete an Animation using :cpp:expr:`lv_anim_delete(var, func)` if one of
these two conditions exists:

- the object (variable) being animated is deleted (and it is not a Widget) or
- a running animation needs to be stopped before it is completed.

.. note::

    If the object (variable) being deleted is a type of Widget, the housekeeping code
    involved in deleting it also deletes any running animations that are connected
    with it.  So :cpp:expr:`lv_anim_delete(var, func)` only needs to be called if the
    object being deleted is *not* one of the Widgets.

If you kept a copy of the pointer returned by :cpp:func:`lv_anim_start` as
``running_anim``, you can delete the running animation like this:

.. code-block:: c

    lv_anim_delete(running_anim->var, running_anim->exec_cb);

In the event that the Animation completes *after* you have determined it needs to be
deleted, and before the call to :cpp:func:`lv_anim_delete` is made, it does no harm
to call it a second time --- no damage will occur.

This function returns a Boolean value indicating whether any *live, running*
Animations were deleted.


.. _animation_pause:

Pausing Animations
******************

If you kept a copy of the pointer returned by :cpp:func:`lv_anim_start`,
you can pause the running animation using :cpp:expr:`lv_anim_pause(animation)` and then resume it
using :cpp:expr:`lv_anim_resume(animation)`.

:cpp:expr:`lv_anim_pause_for(animation, milliseconds)`
is also available if you wish for the animation to resume automatically after.


.. _animations_timeline:

Timeline
********

You can create a series of related animations that are linked together using an
Animation Timeline.  A Timeline is a collection of multiple Animations which makes it
easy to create complex composite Animations.  To create and use an Animation Timeline:

- Create an Animation template but do not call :cpp:func:`lv_anim_start` on it.

- Create an Animation Timeline object by calling :cpp:func:`lv_anim_timeline_create`.

- Add Animation templates to the Timeline by calling
  :cpp:expr:`lv_anim_timeline_add(timeline, start_time, &anim_template)`.
  ``start_time`` is the start time of the Animation on the Timeline.  Note that
  ``start_time`` will override any value given to
  :cpp:expr:`lv_anim_set_delay(&anim_template, delay)`.

- Call :cpp:expr:`lv_anim_timeline_start(timeline)` to start the Animation Timeline.

.. note::

    :cpp:expr:`lv_anim_timeline_add(timeline, start_time, &anim_template)` makes its
    own copy of the contents of the Animation template, so if you do not need it
    later, its contents do not need to be preserved after this call.

It supports forward and reverse play of the entire Animation group, using
:cpp:expr:`lv_anim_timeline_set_reverse(timeline, reverse)`. Note that if you want to
play in reverse from the end of the Timeline, you need to call
:cpp:expr:`lv_anim_timeline_set_progress(timeline, LV_ANIM_TIMELINE_PROGRESS_MAX)`
after adding all Animations and before telling it to start playing.

Call :cpp:expr:`lv_anim_timeline_pause(timeline)` to pause the Animation Timeline.
Note:  this does not preserve its state.  The only way to start it again is to call
:cpp:expr:`lv_anim_timeline_start(timeline)`, which starts the Timeline from the
beginning or at the point set by
:cpp:expr:`lv_anim_timeline_set_progress(timeline, progress)`.

Call :cpp:expr:`lv_anim_timeline_set_progress(timeline, progress)` function to set the
state of the Animation Timeline according to the ``progress`` value.  ``progress`` is
a value between ``0`` and ``32767`` (:c:macro:`LV_ANIM_TIMELINE_PROGRESS_MAX`) to indicate the
proportion of the Timeline that has "played".  Example:  a ``progress`` value of
:cpp:expr:`LV_ANIM_TIMELINE_PROGRESS_MAX / 2` would set the Timeline play to its
half-way point.

Call :cpp:expr:`lv_anim_timeline_get_playtime(timeline)` function to get the total
duration (in milliseconds) of the entire Animation Timeline.

Call :cpp:expr:`lv_anim_timeline_get_reverse(timeline)` function to get whether the
Animation Timeline is also played in reverse after its forward play completes.

Call :cpp:expr:`lv_anim_timeline_delete(timeline)` function to delete the Animation Timeline.
**Note**: If you need to delete a Widget during Animation, be sure to delete the
Animation Timeline before deleting the Widget. Otherwise, the program may crash or behave abnormally.

.. image:: /_static/images/anim-timeline.png

.. _animations_example:

Examples
********

.. include:: ../../examples/anim/index.rst

.. _animations_api:

API
***
