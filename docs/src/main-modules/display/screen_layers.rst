.. _display_screen_layers:

=============
Screen Layers
=============

When an ``lv_display_t`` object is created, 4 permanent :ref:`screens` that
facilitate layering are created and attached to it.

1.  Bottom Layer       (below Active Screen, transparent, not scroll-able, but click-able)
2.  :ref:`active_screen`
3.  Top Layer          (above Active Screen, transparent and neither scroll-able nor click-able)
4.  System Layer       (above Top Layer, transparent and neither scroll-able nor click-able)

1, 3 and 4 are independent of the :ref:`active_screen` and they will be shown (if
they contain anything that is visible) regardless of which screen is the
:ref:`active_screen`.

.. note::

    For the bottom layer to be visible, the Active Screen's background has to be
    at least partially, if not fully, transparent.

You can get pointers to each of these screens on the :ref:`default_display` by using
(respectively):

- :cpp:func:`lv_screen_active`,
- :cpp:func:`lv_layer_top`,
- :cpp:func:`lv_layer_sys`, and
- :cpp:func:`lv_layer_bottom`.

You can get pointers to each of these screens on a specified display by using
(respectively):

- :cpp:expr:`lv_display_get_screen_active(disp)`,
- :cpp:expr:`lv_display_get_layer_top(disp)`,
- :cpp:expr:`lv_display_get_layer_sys(disp)`, and
- :cpp:expr:`lv_display_get_layer_bottom(disp)`.

To set a Screen you create to be the :ref:`active_screen`, call
:cpp:func:`lv_screen_load` or :cpp:func:`lv_screen_load_anim`.

Calling :cpp:expr:`lv_display_get_screen_loading(disp)` will return the screen that is
being loaded. A Screen is considered "being loaded" until the loading animation finishes.
If no Screen is being loaded, this function will return ``NULL``.

.. _layers_top_and_sys:

Top and System Layers
*********************

LVGL uses the Top Layer and System Layer to empower you to ensure that certain
:ref:`widgets` are *always* on top of other layers.

You can add "pop-up windows" to the *Top Layer* freely.  The Top Layer was meant to
be used to create Widgets that are visible on all Screens shown on a Display.  But,
the *System Layer* is intended for system-level things (e.g. mouse cursor will be
placed there with :cpp:func:`lv_indev_set_cursor`).

These layers work like any other Widget, meaning they have styles, and any kind of
Widgets can be created in them.

.. note::
    While the Top Layer and System Layer are created by their owning :ref:`display`
    as not scroll-able and not click-able, these behaviors can be overridden the same
    as any other Widget by using :cpp:expr:`lv_obj_set_scrollbar_mode(scr1, LV_SCROLLBAR_MODE_xxx)`
    and :cpp:expr:`lv_obj_add_flag(scr1, LV_OBJ_FLAG_CLICKABLE)` respectively.

If the :cpp:enumerator:`LV_OBJ_FLAG_CLICKABLE` flag is set on the Top Layer, then it will
absorb all user clicks and acts as a modal Widget.

.. code-block:: c

   lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);



.. _layers_bottom:

Bottom Layer
************

Similar to the Top- and System Layers, the Bottom Layer is also the full size of the
Display, but it is located below the :ref:`active_screen`.  It's visible only if the
Active Screen's background opacity is < 255.


.. admonition::  Further Reading

    :ref:`transparent_screens`.



API
***

.. API equals:
    lv_screen_active,
    lv_layer_top,
    lv_layer_sys,
    lv_layer_bottom,
    lv_display_get_screen_active,
    lv_display_get_layer_top,
    lv_display_get_layer_sys,
    lv_display_get_layer_bottom
