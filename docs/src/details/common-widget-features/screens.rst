.. _screens:

=======
Screens
=======

Screens are simply any Widgets created with ``NULL`` passed as parent in the create
function. As such, they form the "root" of a Widget Tree.

Typically the Base Widget is used for this purpose since it has all the features most
Screens need. But an :ref:`lv_image` Widget can also be used to create a image as a
wallpaper background for the Widget Tree.



Creating Screens
****************

Screens are created like this:

.. code-block:: c

   lv_obj_t * scr1 = lv_obj_create(NULL);

Screens are created on the :ref:`Default display <default_display>`. As typically
there is only one displays the screen are created on them automatically.

The resolution of the screen always matches the resolution of the display. Functions
such as :cpp:func:`lv_obj_set_pos()` and :cpp:func:`lv_obj_set_size()` cannot be used
on screens.

The created Screens can be deleted with :cpp:expr:`lv_obj_delete(scr)`, but be sure
you do not delete the :ref:`active_screen`.



.. _active_screen:

Active Screen
*************

While each :ref:`display` object can have any number of Screen Widgets associated with
it, only one of those Screens is considered "Active" at any given time. That Screen
is referred to as the Display's "Active Screen". For this reason, only one Screen and
its child Widgets will ever be shown on a display at one time.

When each :ref:`display` object was created, a default screen was created with it and
set as its "Active Screen".

To get a pointer to the "Active Screen", call :cpp:func:`lv_screen_active`.

To set a Screen to be the "Active Screen", call :cpp:func:`lv_screen_load` or
:cpp:func:`lv_screen_load_anim`.



.. _loading_screens:

Loading Screens
***************

To make a screen visible, it needs to be "loaded".

The simplest way to load a screen is using :cpp:expr:`lv_screen_load(scr1)`. This
sets ``scr1`` as the Active Screen.

By using :cpp:expr:`lv_screen_load_anim(scr, transition_type, time, delay, auto_del)`
screen loading can be performed

- with an optional transition
- if ``auto_del`` is ``true`` the previous screen is automatically deleted when any
  transition animation finishes.

The following transition types exist:

- :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_NONE`: Switch immediately after ``delay``
  milliseconds
- :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OVER_LEFT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OVER_RIGHT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OVER_TOP` and
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OVER_BOTTOM`: Move the new screen over the
  current towards the given direction
- :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OUT_LEFT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OUT_RIGHT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OUT_TOP` and
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_OUT_BOTTOM`: Move out the old screen over the
  current towards the given direction
- :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_MOVE_LEFT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_MOVE_RIGHT`,
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_MOVE_TOP` and
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM`: Move both the current and new
  screens towards the given direction
- :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_FADE_IN` and
  :cpp:enumerator:`LV_SCREEN_LOAD_ANIM_FADE_OUT`: Fade the new screen over the old
  screen, or vice versa

The new Screen will become active (returned by :cpp:func:`lv_screen_active`) when the
animation starts after ``delay`` time.

All input device events (e.g. touch, keys, etc) are disabled during the Screen's
animation.



API
***

.. API equals:  lv_screen_load
