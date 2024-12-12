.. _coord:

============================
Positions, Sizes and Layouts
============================


Overview
********

Similar to many other parts of LVGL, the concept of setting the
coordinates was inspired by CSS. LVGL has by no means a complete
implementation of CSS but a comparable subset is implemented (sometimes
with minor adjustments).

In short this means:

- Explicitly set coordinates are stored in styles (position, size, layouts, etc.)
- support min-width, max-width, min-height, max-height
- have pixel, percentage, and "content" units
- x=0; y=0 coordinate means the top-left corner of the parent plus the left/top padding plus border width
- width/height means the full size, the "content area" is smaller with padding and border width
- a subset of flexbox and grid layouts are supported



.. _coord_units:

Length Units
************

When passing "length units" (a.k.a. "distance units" or "size units") as arguments to
functions that modify position, size, etc., to make layout of your UI convenient, you
have a choice of several different types of units you can use.

:pixels:             Specify size as pixels:  an integer value <
                     :c:macro:`LV_COORD_MAX` always means pixels.  E.g.
                     :cpp:expr:`lv_obj_set_x(btn, 10)`.

:percentage:         Specify size as a percentage of the size of the Widget's
                     parent or of itself, depending on the property.
                     :cpp:expr:`lv_pct(value)` converts ``value`` to a percentage.
                     E.g. :cpp:expr:`lv_obj_set_width(btn, lv_pct(50))`.  If you want
                     to avoid the overhead of the call to :cpp:func:`lv_pct`, you can
                     also use the macro :cpp:expr:`LV_PCT(x)` to mean the same thing.
                     Note that when you use this feature, your value is *stored as a
                     percent* so that if/when the size of the parent container (or
                     other positioning factor) changes, this style value dynamically
                     retains its meaning.

:contained content:  Specify size as a function of the Widget's children.  The macro
                     :c:macro:`LV_SIZE_CONTENT`: passed as a size value has special
                     meaning:  it means to set the width and/or height of a Widget
                     just large enough to include all of its children.  This is
                     similar to ``auto`` in CSS.  E.g.
                     :cpp:expr:`lv_obj_set_width(btn, LV_SIZE_CONTENT)`.

:inches:             Specify size as 1/160-th portion of an inch as if it were pixels
                     on a 160-DPI display, even though a display may have a different
                     DPI.  Use :cpp:expr:`lv_dpx(n)` or :cpp:expr:`LV_DPX(n)` to do
                     this.  Examples:

                     +----+-----+----------------------------+
                     | n  | DPI | Computed Pixels            |
                     +====+=====+============================+
                     | 40 | 320 | 80 pixels to make 1/4 inch |
                     +----+-----+----------------------------+
                     | 40 | 160 | 40 pixels to make 1/4 inch |
                     +----+-----+----------------------------+
                     | 40 | 130 | 33 pixels to make 1/4 inch |
                     +----+-----+----------------------------+
                     | 80 | 130 | 66 pixels to make 1/2 inch |
                     +----+-----+----------------------------+

                     See DPI under :ref:`display_features`.



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

.. image:: /misc/boxmodel.png
    :alt: The box models of LVGL: The content area is smaller than the bounding box with the padding and border width

The border is drawn inside the bounding box. Inside the border LVGL
keeps a "padding margin" when placing a Widget's children.

The outline is drawn outside the bounding box.



.. _coord_notes:

Important Notes
***************

This section describes special cases in which LVGL's behavior might be
unexpected.

.. _coord_postponed_coordinate_calculation:

Postponed coordinate calculation
--------------------------------

LVGL doesn't recalculate all the coordinate changes immediately. This is
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

Removing styles
---------------

As it's described in the :ref:`coord_using_styles` section,
coordinates can also be set via style properties. To be more precise,
under the hood every style coordinate related property is stored as a
style property. If you use :cpp:expr:`lv_obj_set_x(widget, 20)` LVGL saves ``x=20``
in the local style of the Widget.

This is an internal mechanism and doesn't matter much as you use LVGL.
However, there is one case in which you need to be aware of the
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



.. _positioning_widgets:

Positioning Widgets
*******************


Direct
------

To simply set the x and y coordinates of a Widget use:

.. code-block:: c

   lv_obj_set_x(widget, 10);          /* Separate... */
   lv_obj_set_y(widget, 20);
   lv_obj_set_pos(widget, 10, 20);    /* Or in one function */

By default, the x and y coordinates are measured from the top left
corner of the parent's content area. For example if the parent has five
pixels of padding on every side the above code will place ``obj`` at
(15, 25) because the content area starts after the padding.

Percentage values are calculated from the parent's content area size.

.. code-block:: c

   lv_obj_set_x(btn, lv_pct(10)); //x = 10 % of parent content area width


Alignment
---------


Inside parent widget
~~~~~~~~~~~~~~~~~~~~
In many cases it is more convenient to tell LVGL to align your object relative to
an "anchor" in its parent *other* than its upper left corner.  To establish
that "anchor", call :cpp:expr:`lv_obj_set_align(widget, LV_ALIGN_...)`.  After
that call, that "anchor" will be remembered until another one is established.
In other words, every futire x and y setting for that Widget will be relative to the
that "anchor".

Example:  Position Widget (10,20) px relative to the center of its parent:

.. code-block:: c

   lv_obj_set_align(widget, LV_ALIGN_CENTER);
   lv_obj_set_pos(widget, 10, 20);

   /* Or combine the above in one function... */
   lv_obj_align(widget, LV_ALIGN_CENTER, 10, 20);

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

See illustration below to visualize what these mean.

It's quite common to align a child to the center of its parent,
therefore a dedicated function exists:

.. code-block:: c

   lv_obj_center(widget);

   //Has the same effect
   lv_obj_align(widget, LV_ALIGN_CENTER, 0, 0);

If the parent's size changes, the set alignment and position of the
children is updated automatically.


Relative to another Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~
Alternately, you can choose an "anchor" on another Widget.

.. code-block:: c

   lv_obj_align_to(widget, reference_widget, align, x, y);

where ``align`` can be done of the following:

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
does not remember the "anchor" used, and so will not automatically reposition
the aligned widget if the reference widget later moves.

The following illustration shows the meaning of each "anchor" mentioned above.

.. image:: /misc/align.png



.. _coord_size:

Size
****

Sizing the simple way
---------------------

The width and the height of a Widget can be set easily as well:

.. code-block:: c

   lv_obj_set_width(widget, 200);         /* Separate... */
   lv_obj_set_height(widget, 100);
   lv_obj_set_size(widget, 200, 100);     /* Or in one function */

Percentage values are calculated based on the parent's content area
size. For example to set the Widget's height to the screen height:

.. code-block:: c

   lv_obj_set_height(widget, lv_pct(100));

The size settings support a special value: :c:macro:`LV_SIZE_CONTENT`. It means
the Widget's size in the respective direction will be set to the size of
its children. Note that only children on the right and bottom sides will
be considered and children on the top and left remain cropped. This
limitation makes the behavior more predictable.

Widgets with :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN` or :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` will be
ignored by the :c:macro:`LV_SIZE_CONTENT` calculation.

The above functions set the size of a Widget's bounding box but the
size of the content area can be set as well. This means a Widget's
bounding box will be enlarged with the addition of padding.

.. code-block:: c

   lv_obj_set_content_width(widget, 50);  /* The actual width: padding left + 50 + padding right */
   lv_obj_set_content_height(widget, 30); /* The actual width: padding top + 30 + padding bottom */

The size of the bounding box and the content area can be retrieved with
the following functions:

.. code-block:: c

   int32_t w = lv_obj_get_width(widget);
   int32_t h = lv_obj_get_height(widget);
   int32_t content_w = lv_obj_get_content_width(widget);
   int32_t content_h = lv_obj_get_content_height(widget);


.. _extending_click_area:

Extending the click area
------------------------

By default, Widgets can be clicked only within their bounding area.  However,
especially with small Widgets, it can be helpful to make a Widget's "clickable" area
larger.  You can do this with :cpp:expr:`lv_obj_set_ext_click_area(widget, size)`.



.. _coord_using_styles:

Using styles
************

Under the hood the position, size and alignment properties are style
properties. The above described "simple functions" hide the style
related code for the sake of simplicity and set the position, size, and
alignment properties in the local styles of the Widget.

However, using styles to set the coordinates has some great advantages:

- It makes it easy to set the width/height/etc. for several Widgets
  together. E.g. make all the sliders 100x10 pixels sized.
- It also makes possible to modify the values in one place.
- The values can be partially overwritten by other styles. For example
  ``style_btn`` makes the Widget ``100x50`` by default but adding
  ``style_full_width`` overwrites only the width of the Widget.
- The Widget can have different position or size depending on state.
  E.g. 100 px wide in :cpp:enumerator:`LV_STATE_DEFAULT` but 120 px
  in :cpp:enumerator:`LV_STATE_PRESSED`.
- Style transitions can be used to make the coordinate changes smooth.

Here are some examples to set a Widget's size using a style:

.. code-block:: c

   static lv_style_t style;
   lv_style_init(&style);
   lv_style_set_width(&style, 100);

   lv_obj_t * btn = lv_button_create(lv_screen_active());
   lv_obj_add_style(btn, &style, LV_PART_MAIN);

As you will see below there are some other great features of size and
position setting. However, to keep the LVGL API lean, only the most
common coordinate setting features have a "simple" version and the more
complex features can be used via styles.



.. _coord_translation:

Translation
***********

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

Translation is applied from the current position of the Widget.

Percentage values can be used in translations as well. The percentage is
relative to the size of the Widget (and not to the size of the parent).
For example :cpp:expr:`lv_pct(50)` will move the Widget with half of its
width/height.

The translation is applied after the layouts are calculated. Therefore,
even laid out Widgets' position can be translated.

The translation actually moves the Widget. That means it makes the
scrollbars and :c:macro:`LV_SIZE_CONTENT` sized Widgets react to the position
change.



.. _coord_transformation:

Transformation
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

.. _coord_min_max_size:

Min and Max size
----------------

Similarly to CSS, LVGL also supports ``min-width``, ``max-width``,
``min-height`` and ``max-height``. These are limits preventing a
Widget's size from becoming smaller/larger than these values. They are
especially useful if the size is set by percentage or
:c:macro:`LV_SIZE_CONTENT`.

.. code-block:: c

   static lv_style_t style_max_height;
   lv_style_init(&style_max_height);
   lv_style_set_y(&style_max_height, 200);

   lv_obj_set_height(widget, lv_pct(100));
   lv_obj_add_style(widget, &style_max_height, LV_STATE_DEFAULT); //Limit the  height to 200 px

Percentage values can be used as well which are relative to the size of
the parent's content area.

.. code-block:: c

   static lv_style_t style_max_height;
   lv_style_init(&style_max_height);
   lv_style_set_y(&style_max_height, lv_pct(50));

   lv_obj_set_height(widget, lv_pct(100));
   lv_obj_add_style(widget, &style_max_height, LV_STATE_DEFAULT); //Limit the height to half parent height



.. _coord_layout:

Layout
******


Layout overview
---------------

Layouts can update the position and size of a Widget's children. They
can be used to automatically arrange the children into a line or column,
or in much more complicated forms.

The position and size set by the layout overwrites the "normal" x, y,
width, and height settings.

There is only one function that is the same for every layout:
:cpp:func:`lv_obj_set_layout` ``(widget, <LAYOUT_NAME>)`` sets the layout on a Widget.
For further settings of the parent and children see the documentation of
the given layout.


Built-in layouts
----------------

LVGL comes with two very powerful layouts:

* Flexbox: arrange Widgets into rows or columns, with support for wrapping and expanding items.
* Grid: arrange Widgets into fixed positions in 2D table.

Both are heavily inspired by the CSS layouts with the same name.
Layouts are described in detail in their own section of documentation.


Flags
-----

There are some flags that can be used on Widgets to affect how they
behave with layouts:

- :cpp:enumerator:`LV_OBJ_FLAG_HIDDEN` Hidden Widgets are ignored in layout calculations.
- :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT` The Widget is simply ignored by the layouts. Its coordinates can be set as usual.
- :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` Same as :cpp:enumerator:`LV_OBJ_FLAG_IGNORE_LAYOUT` but the Widget with :cpp:enumerator:`LV_OBJ_FLAG_FLOATING` will be ignored in :c:macro:`LV_SIZE_CONTENT` calculations.

These flags can be added/removed with :cpp:expr:`lv_obj_add_flag(widget, FLAG)` and :cpp:expr:`lv_obj_remove_flag(widget, FLAG)`


Adding new layouts
------------------

LVGL can be freely extended by a custom layout like this:

.. code-block:: c

   uint32_t MY_LAYOUT;

   ...

   MY_LAYOUT = lv_layout_register(my_layout_update, &user_data);

   ...

   void my_layout_update(lv_obj_t * widget, void * user_data)
   {
       /* Will be called automatically if it's required to reposition/resize the children of "obj" */
   }

Custom style properties can be added which can be retrieved and used in
the update callback. For example:

.. code-block:: c

   uint32_t MY_PROP;
   ...

   LV_STYLE_MY_PROP = lv_style_register_prop();

   ...
   static inline void lv_style_set_my_prop(lv_style_t * style, uint32_t value)
   {
       lv_style_value_t v = {
           .num = (int32_t)value
       };
       lv_style_set_prop(style, LV_STYLE_MY_PROP, v);
   }



.. _coord_example:

Examples
********



.. _coord_api:

API
***
