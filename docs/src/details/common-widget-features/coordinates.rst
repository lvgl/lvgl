.. _coord:

=================
Position and Size
=================

Similar to many other parts of LVGL, the concept of setting the
coordinates was inspired by CSS. LVGL has by no means a complete
implementation of CSS but a comparable subset is implemented (sometimes
with minor adjustments).

In short this means:

- If the position of a parent changes, the children will move along with it
- If a child is partially or fully outside its parent then the parts outside will not be visible.
- Explicitly set coordinates are stored in styles (position, size, layouts, etc.)
- support min-width, max-width, min-height, max-height
- have pixel, percentage, and "content" units
- x=0; y=0 coordinate means the top-left corner of the parent plus the left/top padding plus border width
- width/height means the full size, the "content area" is smaller with padding and border width
- a subset of flexbox and grid layouts are supported

Under the hood the position, size and alignment properties are style
properties.

For the sake of similcity there are dedicated API functions to set the position and size,
however to keep LVGL's aPI leaner the less common coordinate related functions are available as style
properties.


This is an example to set position and size directly:

.. code-block:: c

   lv_obj_set_x(widget, 10);
   lv_obj_set_width(widget, 150);


And via styles:

.. code-block:: c

   /*Using a style sheet*/
   static lv_style_t style_main;
   lv_style_init(&style_main);
   lv_style_set_width(&style_main, 100);

   lv_obj_t * btn = lv_button_create(lv_screen_active());
   lv_obj_add_style(btn, &style_main, LV_PART_MAIN);

   /*Using a local style to set the width in PRESSED state*/
   lv_obj_set_style_width(btn, 150, LV_PART_PRESSED);


Using styles has many advantages

- It makes it easy to set the width/height/etc. for several Widgets
  together. E.g. make all the sliders 100x10 pixels sized.
- It also makes possible to modify the values in one place.
- The values can be partially overwritten by other styles. For example
  ``style_btn`` makes the Widget ``100x50`` by default but adding
  ``style_full_width`` overwrites only the width of the Widget.
- The Widget can have different position or size depending on state.
  E.g. 100 px wide in :cpp:enumerator:`LV_STATE_DEFAULT` but 120 px
  in :cpp:enumerator:`LV_STATE_PRESSED`.
- Style transitions can be used to make the coordinate changes smooth when the state changes.


Important Notes
***************

This section describes special cases in which LVGL's behavior might be
unexpected.

.. _coord_postponed_coordinate_calculation:

Postponed Coordinate Calculation
--------------------------------

LVGL does **not** recalculate all the coordinate changes immediately. This is
done to improve performance. Instead, the Widgets are marked as "dirty"
and before redrawing the screen LVGL checks if there are any "dirty"
Widgets. If so it refreshes their position, size and layout.

In other words, if you need to get the coordinate of a Widget and the
coordinates were just changed, LVGL needs to be forced to recalculate
the coordinates. To do this call :cpp:func:`lv_obj_update_layout`.

The size and position might depend on the parent or layout. Therefore
:cpp:func:`lv_obj_update_layout` recalculates the coordinates of all Widgets on
the screen of ``obj``.


.. _coord_removing styles:

Removing Styles Makes Corrdinats Disappear
------------------------------------------

As it's described above, coordinates are style properties under the hood.
If you use :cpp:expr:`lv_obj_set_x(widget, 20)` LVGL saves ``x=20``
in the local style of the Widget.

In most of the cases it doesn't matter much as you use LVGL,
however, there is one case in which you need to be aware of the
implementation. If the style(s) of a Widget are removed by

.. code-block:: c

   lv_obj_remove_style_all(widget)

or

.. code-block:: c

   lv_obj_remove_style(widget, NULL, LV_PART_MAIN);

the earlier set coordinates will be removed as well.

For example:

.. code-block:: c

   /* The size of obj1 will be set back to the default in the end */
   lv_obj_set_size(widget1, 200, 100);  /* Now obj1 has 200;100 size */
   lv_obj_remove_style_all(widget1);    /* It removes the set sizes */


   /* widget2 will have 200;100 size in the end */
   lv_obj_remove_style_all(widget2);
   lv_obj_set_size(widget2, 200, 100);



.. _boxing_model:

Boxing Model
************

LVGL follows CSS's `border-box <https://developer.mozilla.org/en-US/docs/Web/CSS/box-sizing>`__
model. A Widget's "box" is built from the following parts:

:bounding box:  the width/height of the elements.
:border width:  the width of the border.
:padding:       space between the sides of the Widget and its children.
:margin:        space outside of the Widget (considered only by some layouts)
:content:       the content area which is the size of the bounding box reduced by the border width and padding.

.. image:: /_static/images/boxmodel.png
    :alt: The box models of LVGL: The content area is smaller than the bounding box with the padding and border width

The border is drawn inside the bounding box. Inside the border LVGL
keeps a "padding margin" when placing a Widget's children.

The outline is drawn outside the bounding box.


:cpp:expr:`lv_obj_set_width()` and :cpp:expr:`lv_obj_set_height()`
sets the size of a Widget's bounding box but the
size of the content area can be set as well. This means a Widget's
bounding box will be enlarged with the addition of padding and border width.

.. code-block:: c

   /* The actual width: border_width + padding left + 50 + padding right + border_width*/
   lv_obj_set_content_width(widget, 50);

The size of the bounding box and the content area can be retrieved with
the following functions:

.. code-block:: c

   int32_t w = lv_obj_get_width(widget);
   int32_t h = lv_obj_get_height(widget);
   int32_t content_w = lv_obj_get_content_width(widget);
   int32_t content_h = lv_obj_get_content_height(widget);



Static Positioning
*******************

The simplest way of setting positions is just using pixel values.
For example setting :cpp:expr:`lv_obj_set_x(btn, 10)` will set the button's
horizontal position from the left side of the parent. In accordence with the bxong model
the button will be offsetted by the parent's border width and left padding as well.

To simply set the x and y coordinates of a Widget use:

.. code-block:: c

   lv_obj_set_x(widget, 10);          /* From the Left */
   lv_obj_set_y(widget, 20);          /* From the Top*/
   lv_obj_set_pos(widget, 10, 20);    /* Or in one function */

Dynamic Positioning
*******************

In LVGL there are multiple ways to set the Widget's position dynamically
based on the parent's size, the siblings, or other settings.

Align to the Parent
-------------------

In many cases it convenient to position a widget relative to a side or corner of the parent.

The default "anchor" is the top-left corner, that's why X and Y coordinates are measured from
the top left corner of the parent.

By changing the anchor to an other corner or side the set X and Y coordinates will be interpreted
relative to that part of the parent. It's all dynamic, meaning as the parent's size changes
the child position will change too.

To set this anchor use :cpp:expr:`lv_obj_set_align(widget, LV_ALIGN_...)`
or it's shorthand :cpp:expr:`lv_obj_align(widget, LV_ALIGN_..., x_ofs, y_ofs)`

9 convenient "anchors" can be used with these functions:

- :cpp:enumerator:`LV_ALIGN_TOP_LEFT`
- :cpp:enumerator:`LV_ALIGN_TOP_MID`
- :cpp:enumerator:`LV_ALIGN_TOP_RIGHT`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_LEFT`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_MID`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_RIGHT`
- :cpp:enumerator:`LV_ALIGN_LEFT_MID`
- :cpp:enumerator:`LV_ALIGN_RIGHT_MID`
- :cpp:enumerator:`LV_ALIGN_CENTER`

It's very common that a widget needs to be positioned to center. For this
:cpp:expr:`lv_obj_center(widget)` can be used. It sets X and Y to zero too.


Align to Another Widget
-----------------------

Alternately, you can choose an "anchor" on another Widget.

.. code-block:: c

   lv_obj_align_to(widget, reference_widget, align, x, y);

where ``align`` can be outside of the reference widget too:

-  :cpp:enumerator:`LV_ALIGN_OUT_TOP_LEFT`
-  :cpp:enumerator:`LV_ALIGN_OUT_TOP_MID`
-  :cpp:enumerator:`LV_ALIGN_OUT_TOP_RIGHT`
-  :cpp:enumerator:`LV_ALIGN_OUT_BOTTOM_LEFT`
-  :cpp:enumerator:`LV_ALIGN_OUT_BOTTOM_MID`
-  :cpp:enumerator:`LV_ALIGN_OUT_BOTTOM_RIGHT`
-  :cpp:enumerator:`LV_ALIGN_OUT_LEFT_TOP`
-  :cpp:enumerator:`LV_ALIGN_OUT_LEFT_MID`
-  :cpp:enumerator:`LV_ALIGN_OUT_LEFT_BOTTOM`
-  :cpp:enumerator:`LV_ALIGN_OUT_RIGHT_TOP`
-  :cpp:enumerator:`LV_ALIGN_OUT_RIGHT_MID`
-  :cpp:enumerator:`LV_ALIGN_OUT_RIGHT_BOTTOM`

Example:  to horizontally center a label 10 pixels above a button:

.. code-block:: c

   lv_obj_align_to(label, btn, LV_ALIGN_OUT_TOP_MID, 0, -10);

Note that, unlike with :cpp:func:`lv_obj_align`, :cpp:func:`lv_obj_align_to`
does not remember the "anchor" used, and so will **not** automatically reposition
the aligned widget if the reference widget later moves.

The following illustration shows the meaning of each "anchor" mentioned above.

.. image:: /_static/images/align.png

Layouts
-------

Layouts like :ref:`flex` and :ref:`grid` are powerful way to automatically set the position of the widgets
in row, column, or a 2D grid.

Layouts are also fully responsice so that they respond to the parent and shild size changes automatically.

Percentage
----------

Setting the x or y coordinate in percentage positions the widget relative to the parent's size.
100% means the parent's size minute the parent's border and padding in the respective direction.

To convert a value to percentage use ``lv_pct(50)`` or ``LV_PCT(50)``
(for compile time evaluation).

Static Sizing
*************

The width and the height of a Widget can be set easily in pixels:

.. code-block:: c

   lv_obj_set_width(widget, 200);
   lv_obj_set_height(widget, 100);
   lv_obj_set_size(widget, 200, 100);     /* Or in one function */

Dynamic Sizing
**************

Percentage
----------

Percentage values are calculated based on the parent's content area
size.

Setting a Widget's width or hiegt to the parent's width or height is
one of the most common use cases of percentage size:

.. code-block:: c

   lv_obj_set_height(widget, lv_pct(100));

Percentage size units are also responsive, meaning the widget's size will be updated
as the paren't size changes

Content
-------

Setting the size of widget to include all the children is one of the most powerful
ways to handle responsive and dynamic sizes.

:c:macro:`LV_SIZE_CONTENT` can be set as either width or height to set the size to the
last farthest widgets bottom or right edge plus the parent's padding and border width.


Below is a great example of how to create container that sets its size automatically
to the child label.

.. code-block:: c

   lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
   lv_obj_t * label = lv_label_create(cont);
   lv_label_set_text(label, "Some text");

``LV_SIZE_CONTENT`` works well with layouts too. For example in case of a :ref:`flex`
layout no matter how many children are placed nexr to eachother the parent size can be set t include all
automatically.

Widgets with :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN` or :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` will be
ignored by the :c:macro:`LV_SIZE_CONTENT` calculation.


Layouts
-------

Both :ref:`flex` and :ref:`grid` layouts can set not only the position but the size of the widgets
by stretch them according to some rules.


.. _coord_min_max_size:

Min and Max Size
****************

Similarly to CSS, LVGL also supports ``min-width``, ``max-width``,
``min-height`` and ``max-height``. These are limits preventing a
Widget's size from becoming smaller/larger than these values. They are
especially useful if the size is set by percentage or
:c:macro:`LV_SIZE_CONTENT`.

Min/max width/height doesn't have dedicated ``set`` functions but they can be set directly via
style properties:

.. code-block:: c

   static lv_style_t style_max_height;
   lv_style_init(&style_max_height);
   lv_style_set_max_height(&style_max_height, 200);

   lv_obj_set_height(widget, lv_pct(100));
   lv_obj_add_style(widget, &style_max_height, LV_STATE_DEFAULT); /*Limit the  height to 200 px*/

   /*Or as a local style*/
   lv_obj_set_style_max_height(widget, 200, LV_STATE_DEFAULT);

Percentage values can be used here as well which are relative to the size of
the parent's content area.

.. code-block:: c

   static lv_style_t style_max_height;
   lv_style_init(&style_max_height);
   lv_style_set_max_height(&style_max_height, lv_pct(50));

   lv_obj_set_height(widget, lv_pct(100));
   lv_obj_add_style(widget, &style_max_height, LV_STATE_DEFAULT); /*Limit the height to half parent height*/

.. _coord_translation:

Translate Position
******************

Let's say the there are 3 buttons next to each other. Their position is
set as described above. Now you want to move a button up a little when
it's pressed.

One way to achieve this is by setting a new Y coordinate for the pressed
state:

.. code-block:: c

   static lv_style_t style_normal;
   lv_style_init(&style_normal);
   lv_style_set_y(&style_normal, 100);

   static lv_style_t style_pressed;
   lv_style_init(&style_pressed);
   lv_style_set_y(&style_pressed, 80);

   lv_obj_add_style(btn1, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn1, &style_pressed, LV_STATE_PRESSED);

   lv_obj_add_style(btn2, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn2, &style_pressed, LV_STATE_PRESSED);

   lv_obj_add_style(btn3, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn3, &style_pressed, LV_STATE_PRESSED);

This works, but it's not really flexible because the pressed coordinate
is hard-coded. If the buttons are not at y=100, ``style_pressed`` won't
work as expected. Translations can be used to solve this:

.. code-block:: c

   static lv_style_t style_normal;
   lv_style_init(&style_normal);
   lv_style_set_y(&style_normal, 100);

   static lv_style_t style_pressed;
   lv_style_init(&style_pressed);
   lv_style_set_translate_y(&style_pressed, -20);

   lv_obj_add_style(btn1, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn1, &style_pressed, LV_STATE_PRESSED);

   lv_obj_add_style(btn2, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn2, &style_pressed, LV_STATE_PRESSED);

   lv_obj_add_style(btn3, &style_normal, LV_STATE_DEFAULT);
   lv_obj_add_style(btn3, &style_pressed, LV_STATE_PRESSED);

Coordinate translation is applied from the current position of the Widget.

Percentage values can be used in coordinate translations as well. The percentage is
relative to the size of the Widget (and not to the size of the parent).
For example :cpp:expr:`lv_pct(50)` will move the Widget with half of its
width/height.

The coordinate translation is applied after the layouts are calculated. Therefore,
even laid out Widgets' position can be translated.

The coordinate translation actually moves the Widget. That means it makes the
scrollbars and :c:macro:`LV_SIZE_CONTENT` sized Widgets react to the position
change.



.. _coord_transformation:

Transform Size
**************

Similarly to position, a Widget's size can be changed relative to the
current size as well. The transformed width and height are added on both
sides of the Widget. This means a 10 px transformed width makes the
Widget 2x10 pixels wider.

Unlike position translation, the size transformation doesn't make the
Widget "really" larger. In other words scrollbars, layouts, and
:c:macro:`LV_SIZE_CONTENT` will not react to the transformed size. Hence, size
transformation is "only" a visual effect.

This code enlarges a button when it's pressed:

.. code-block:: c

   static lv_style_t style_pressed;
   lv_style_init(&style_pressed);
   lv_style_set_transform_width(&style_pressed, 10);
   lv_style_set_transform_height(&style_pressed, 10);

   lv_obj_add_style(btn, &style_pressed, LV_STATE_PRESSED);



.. _coord_example:

Examples
********



.. _coord_api:

API
***
