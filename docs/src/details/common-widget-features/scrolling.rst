.. _scrolling:

=========
Scrolling
=========


Overview
********

In LVGL scrolling works very intuitively: if a Widget is outside its
parent content area (the size without padding), the parent becomes
scrollable and scrollbar(s) will appear. That's it.

Any Widget can be scrollable including :ref:`base_widget`, ``lv_image``,
``lv_button``, ``lv_meter``, etc

The Widget can either be scrolled horizontally or vertically in one
stroke; diagonal scrolling is not possible.


Scrollbar
---------

Mode
~~~~

Scrollbars are displayed according to the configured ``scrollbar-mode``.  The
following modes are available:

- :cpp:enumerator:`LV_SCROLLBAR_MODE_OFF`: Never show the scrollbars
- :cpp:enumerator:`LV_SCROLLBAR_MODE_ON`: Always show the scrollbars
- :cpp:enumerator:`LV_SCROLLBAR_MODE_ACTIVE`: Show scroll bars while a Widget is being scrolled
- :cpp:enumerator:`LV_SCROLLBAR_MODE_AUTO`: Show scroll bars when the content is large enough to be scrolled

:cpp:expr:`lv_obj_set_scrollbar_mode(widget, LV_SCROLLBAR_MODE_...)` sets the scrollbar mode on a Widget.

Styling
~~~~~~~

A Scrollbar is a dedicated part of a Widget, called
:cpp:enumerator:`LV_PART_SCROLLBAR`.  For example, a scrollbar can turn to red like
this:

.. code-block:: c

   static lv_style_t style_red;
   lv_style_init(&style_red);
   lv_style_set_bg_color(&style_red, lv_color_red());

   ...

   lv_obj_add_style(widget, &style_red, LV_PART_SCROLLBAR);

A Widget goes to the :cpp:enumerator:`LV_STATE_SCROLLED` state while it's being
scrolled.  This allows adding different styles to the Widget that will be effective
while it is being scrolled.  For example, this code makes the scrollbar blue while
the Widget is being scrolled:

.. code-block:: c

   static lv_style_t style_blue;
   lv_style_init(&style_blue);
   lv_style_set_bg_color(&style_blue, lv_color_blue());

   ...

   lv_obj_add_style(widget, &style_blue, LV_STATE_SCROLLED | LV_PART_SCROLLBAR);

If the base direction of the :cpp:enumerator:`LV_PART_SCROLLBAR` is RTL
(:c:macro:`LV_BASE_DIR_RTL`) the vertical scrollbar will be placed on the left.
Note that, the ``base_dir`` style property is inherited. Therefore, it
can be set directly on the :cpp:enumerator:`LV_PART_SCROLLBAR` part of a Widget, or
on the Widget's LV_PART_MAIN part, or that of any of its parents, to make a scrollbar
inherit the base direction.

``pad_left/right/top/bottom`` sets the spacing around the scrollbars,
``width`` sets the scrollbar's width and ``length`` sets the scrollbar's length:
If `length` is not set or left at `0` the scrollbar's length will be set automatically
according to the length of the content.

.. code-block:: c

   static lv_style_t style_scrollbar;
   lv_style_init(&style_scrollbar);
   lv_style_set_pad_left(&style_scrollbar, 2);
   lv_style_set_pad_right(&style_scrollbar, 2);
   lv_style_set_pad_top(&style_scrollbar, 2);
   lv_style_set_pad_bottom(&style_scrollbar, 2);
   lv_style_set_width(&style_scrollbar, 10);
   lv_style_set_length(&style_scrollbar, 50);

   ...

   lv_obj_add_style(widget, &style_scrollbar, LV_PART_SCROLLBAR);

The minimum length of the scrollbar is fixed to 10, while its maximum length is limited by the
Widget's height or width, depending on whether the scrollbar is vertical or horizontal. Any length value
set outside of these limits will automatically result in a length fixed to either limit.

.. _scroll_events:

Scrolling Events
----------------

The following events are emitted as part of scrolling:

- :cpp:enumerator:`LV_EVENT_SCROLL_BEGIN`: Signals that scrolling has begun. The
  event parameter is ``NULL`` or an ``lv_anim_t *`` with a scroll animation
  descriptor that can be modified if required.
- :cpp:enumerator:`LV_EVENT_SCROLL_END`: Signals that scrolling has ended.
- :cpp:enumerator:`LV_EVENT_SCROLL`: Signals that the scrolling position changed;
  triggered on every position change.



Features of Scrolling
*********************

Besides, managing "normal" scrolling there are many interesting and
useful additional features.

Scrollable
----------

It is possible to make a Widget non-scrollable with
:cpp:expr:`lv_obj_remove_flag(widget, LV_OBJ_FLAG_SCROLLABLE)`.

Non-scrollable Widgets can still propagate the scrolling (chain) to
their parents.

The direction in which scrolling happens can be controlled by
:cpp:expr:`lv_obj_set_scroll_dir(widget, LV_DIR_...)`.

The following values can be used for the direction:

- :cpp:enumerator:`LV_DIR_TOP`: only scroll up
- :cpp:enumerator:`LV_DIR_LEFT`: only scroll left
- :cpp:enumerator:`LV_DIR_BOTTOM`: only scroll down
- :cpp:enumerator:`LV_DIR_RIGHT`: only scroll right
- :cpp:enumerator:`LV_DIR_HOR`: only scroll horizontally
- :cpp:enumerator:`LV_DIR_VER`: only scroll vertically
- :cpp:enumerator:`LV_DIR_ALL`: scroll any directions

OR-ed values are also possible. E.g. :cpp:expr:`LV_DIR_TOP | LV_DIR_LEFT`.

Scroll chaining
---------------

If a Widget can't be scrolled further (e.g. its content has reached the
bottom-most position), additional scrolling is propagated to its parent.
If the parent can be scrolled in that direction than it will be scrolled
instead. It continues propagating up the Widget's parent hierarchy up to
the :ref:`Screen <screens>`.

The propagation on scrolling is called "scroll chaining" and it can be
enabled/disabled with ``LV_OBJ_FLAG_SCROLL_CHAIN_HOR/VER`` flag. If
chaining is disabled the propagation stops on the Widget and the
parent(s) won't be scrolled.

Scroll momentum
---------------

When the user scrolls a Widget and releases it, LVGL can emulate
inertial momentum for the scrolling. It's like the Widget was "thrown"
and scrolling slows down smoothly.

Scroll momentum can be enabled/disabled with the
:cpp:enumerator:`LV_OBJ_FLAG_SCROLL_MOMENTUM` flag.

Elastic scroll
--------------

Normally a Widget can't be scrolled past the extremities of its
content.  That is, the top side of the content can't be below the top side
of the Widget, and vice versa for the bottom side.

However, with :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ELASTIC` a fancy effect is added
when the user "over-scrolls" the content. The scrolling slows down, and
the content can be scrolled inside the Widget. When the Widget is
released the content scrolled in it is animated back to the closest valid
position.

Snapping
--------

The children of a Widget can be snapped according to specific rules
when scrolling ends. Children can be made snappable individually with
the :cpp:enumerator:`LV_OBJ_FLAG_SNAPPABLE` flag.

A Widget can align snapped children in four ways:

- :cpp:enumerator:`LV_SCROLL_SNAP_NONE`: Snapping is disabled. (default)
- :cpp:enumerator:`LV_SCROLL_SNAP_START`: Align the children to the left/top side of a scrolled Widget
- :cpp:enumerator:`LV_SCROLL_SNAP_END`: Align the children to the right/bottom side of a scrolled Widget
- :cpp:enumerator:`LV_SCROLL_SNAP_CENTER`: Align the children to the center of a scrolled Widget

Snap alignment is set with
:cpp:expr:`lv_obj_set_scroll_snap_x(widget, LV_SCROLL_SNAP_...)` and
:cpp:expr:`lv_obj_set_scroll_snap_y(widget, LV_SCROLL_SNAP_...)`.

This is what happens under the hood:

1. user scrolls and releases a Widget;
2. LVGL calculates where the scroll would end considering scroll momentum;
3. LVGL finds the nearest scroll point;
4. LVGL scrolls to the snap point with an animation.

Scroll one
----------

The "scroll one" feature tells LVGL to allow scrolling only one
snappable child at a time. This requires making the children snappable
and setting scroll snap alignment to something other than
:cpp:enumerator:`LV_SCROLL_SNAP_NONE`.

This feature can be enabled by the :cpp:enumerator:`LV_OBJ_FLAG_SCROLL_ONE` flag.

Scroll on focus
---------------

Imagine that there are a lot of Widgets in a group that are on a scrollable
Widget. Pressing the "Tab" button moves focus to the next Widget but it might
be outside the visible area of the scrollable Widget. If the "scroll on
focus" feature is enabled LVGL will automatically scroll Widgets to
bring the child Widget with focus into view.  The scrolling happens recursively
therefore even nested scrollable Widgets are handled properly. The
Widget will be scrolled into view even if it is on a different page of a
tabview.



Scrolling Programmatically
**************************

The following API functions allow programmatic scrolling of Widgets:

- ``lv_obj_scroll_by(widget, x, y, LV_ANIM_ON/OFF)`` scroll by ``x`` and ``y`` values
- ``lv_obj_scroll_to(widget, x, y, LV_ANIM_ON/OFF)`` scroll to bring the given coordinate to the top left corner
- ``lv_obj_scroll_to_x(widget, x, LV_ANIM_ON/OFF)`` scroll to bring the given coordinate to the left side
- ``lv_obj_scroll_to_y(widget, y, LV_ANIM_ON/OFF)`` scroll to bring the given coordinate to the top side

From time to time you may need to retrieve the *scroll position* of a
scrollable Widget, either to restore it later, or to dynamically display some
elements according to its current scroll position. Here is an example to illustrate
how to combine scroll event and store the scroll-top position.

.. code-block:: c

   static int scroll_value = 0;

   static void store_scroll_top_value_event_cb(lv_event_t* e) {
     lv_obj_t * scr = lv_event_get_target(e);
     scroll_value = lv_obj_get_scroll_top(scr);
     printf("%d pixels are scrolled above top edge of display.\n", scroll_value);
   }

   lv_obj_t * scr = lv_obj_create(NULL);
   lv_obj_add_event_cb(scr, store_scroll_top_value_event_cb, LV_EVENT_SCROLL, NULL);

Scroll coordinates can be retrieved from different axes with these functions:

- :cpp:expr:`lv_obj_get_scroll_x(widget)`      Pixels scrolled past left edge of Widget's view window.
- :cpp:expr:`lv_obj_get_scroll_y(widget)`      Pixels scrolled past top of Widget's view window.
- :cpp:expr:`lv_obj_get_scroll_top(widget)`    Identical to :cpp:expr:`lv_obj_get_scroll_y(widget)`
- :cpp:expr:`lv_obj_get_scroll_bottom(widget)` Pixels scrolled past bottom of Widget's view window.
- :cpp:expr:`lv_obj_get_scroll_left(widget)`   Identical to :cpp:expr:`lv_obj_get_scroll_x(widget)`.
- :cpp:expr:`lv_obj_get_scroll_right(widget)`  Pixels scrolled past right edge of Widget's view window.

Setting scroll position can be done with these functions:

- :cpp:expr:`lv_obj_scroll_by(widget, dx, dy, anim_enable)`               Scroll by given amount of pixels.
- :cpp:expr:`lv_obj_scroll_by_bounded(widget, dx, dy, animation_enable)`  Scroll by given amount of pixels.
- :cpp:expr:`lv_obj_scroll_to(widget, x, y, animation_enable)`            Scroll to given coordinate on Widget.
- :cpp:expr:`lv_obj_scroll_to_x(widget, x, animation_enable)`             Scroll to X coordinate on Widget.
- :cpp:expr:`lv_obj_scroll_to_y(widget, y, animation_enable)`             Scroll to Y coordinate on Widget.
- :cpp:expr:`lv_obj_scroll_to_view(widget, animation_enable)`             Scroll ``obj``'s parent Widget until ``obj`` becomes visible.
- :cpp:expr:`lv_obj_scroll_to_view_recursive(widget, animation_enable)`   Scroll ``obj``'s parent Widgets recursively until ``obj`` becomes visible.



Self Size
*********

Self size is a property of a Widget. Normally, the user shouldn't use
this parameter but if a custom widget is created it might be useful.

In short, self size establishes the size of a Widget's content. To
understand it better take the example of a table. Let's say it has 10
rows each with 50 px height. So the total height of the content is 500
px. In other words the "self height" is 500 px. If the user sets only
200 px height for the table LVGL will see that the self size is larger
and make the table scrollable.

This means not only the children can make a Widget scrollable but a
larger self size will as well.

LVGL uses the :cpp:enumerator:`LV_EVENT_GET_SELF_SIZE` event to get the self size of
a Widget. Here is an example to see how to handle the event:

.. code-block:: c

    if(event_code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);

        /* If x or y < 0 then it doesn't need to be calculated now. */
        if(p->x >= 0) {
            p->x = 200; /* Set or calculate self width. */
        }

        if(p->y >= 0) {
            p->y = 50;  /* Set or calculate self height. */
        }
    }



.. _scroll_example:

Examples
********

.. include:: /examples/scroll/index.rst

.. _scroll_api:



API
***
