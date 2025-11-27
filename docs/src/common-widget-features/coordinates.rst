.. _coord:

=================
Position and Size
=================

Similar to many other parts of LVGL, the concept of setting coordinates is inspired
by CSS. LVGL does not implement CSS completely, but a comparable subset is provided
(sometimes with minor adjustments).

In short, this means:

- If the position of a parent changes, its children will move along with it.
- If a child is partially or fully outside its parent, the parts outside will not be
  visible.
- Explicitly set coordinates are stored in styles (position, size, layout, etc.).
- Supports ``min-width``, ``max-width``, ``min-height``, and ``max-height``.
- Supports pixel, percentage, and ``content`` units.
- ``x = 0``, ``y = 0`` means the top-left corner of the parent, plus the parent's
  left/top padding and border width.
- ``width`` and ``height`` represent the full size. The content area is smaller due
  to padding and border width.
- A subset of Flexbox and Grid layouts is supported.

Under the hood, position, size, and alignment are all style properties.

For simplicity, there are dedicated API functions to set position and size. However,
to keep LVGL's API leaner, less commonly used coordinate-related functions are only
available via style properties.

Here’s how to set position and size directly:

.. code-block:: c

   lv_obj_set_x(widget, 10);
   lv_obj_set_width(widget, 150);

Or via styles:

.. code-block:: c

   /* Using a style sheet */
   static lv_style_t style_main;
   lv_style_init(&style_main);
   lv_style_set_width(&style_main, 100);

   lv_obj_t * btn = lv_button_create(lv_screen_active());
   lv_obj_add_style(btn, &style_main, LV_PART_MAIN);

   /* Using a local style to set the width in the PRESSED state */
   lv_obj_set_style_width(btn, 150, LV_PART_PRESSED);

Advantages of using styles:

- Easily apply width/height/etc. to multiple widgets. E.g., make all sliders 100×10 px.
- Centralized control over values (modify in one place).
- Values can be selectively overridden. E.g., one style sets a widget to 100×50,
  another overrides only the width.
- Widget position or size can change by state. E.g., 100 px wide in
  :cpp:enumerator:`LV_STATE_DEFAULT` but 120 px in
  :cpp:enumerator:`LV_STATE_PRESSED`.
- Enables style transitions to animate size/position changes between states.



Important Notes
***************

This section describes special cases where LVGL's behavior might be unexpected.


.. _coord_postponed_coordinate_calculation:

Postponed Coordinate Calculation
--------------------------------

LVGL does **not** recalculate coordinates immediately after a change. Instead,
widgets are marked as "dirty." LVGL recalculates dirty widgets’ positions, sizes,
and layouts just before redrawing the screen.

So, if you change a widget’s position or size and then immediately read it, call
:cpp:func:`lv_obj_update_layout` first to ensure the values are up to date.

Note: Since layout and size may depend on the parent,
:cpp:func:`lv_obj_update_layout` recalculates coordinates for **all** widgets on
the screen of the given object.


.. _coord_removing_styles:

Removing Styles Makes Coordinates Disappear
-------------------------------------------

Coordinates are stored as style properties under the hood. So if you use:

.. code-block:: c

   lv_obj_set_x(widget, 20);

LVGL saves ``x = 20`` in the widget’s local style.

Normally this is not important for the user, but if you remove a widget’s styles, its
coordinates may be removed too:

.. code-block:: c

   /* The size of widget1 will reset to default */
   lv_obj_set_size(widget1, 200, 100);
   lv_obj_remove_style_all(widget1);

   /* widget2 will have 200×100 size in the end */
   lv_obj_remove_style_all(widget2);
   lv_obj_set_size(widget2, 200, 100);



.. _boxing_model:

Boxing Model
************

LVGL follows the CSS ``border-box`` model:

https://developer.mozilla.org/en-US/docs/Web/CSS/box-sizing

A widget's "box" includes:

:bounding box:  Full width/height of the widget.
:border width:  The width of the border.
:padding:       Space between the border and the content/children.
:margin:        Space outside the widget (used only in some layouts).
:content:       The content area (bounding box minus border and padding).

.. image:: /_static/images/boxmodel.png
   :alt: Box model in LVGL

The border is drawn **inside** the bounding box, and padding is applied **inside**
the border. The outline (if any) is drawn **outside** the bounding box.

Use:

.. code-block:: c

   lv_obj_set_content_width(widget, 50);

This sets the width of the *content*, and the actual widget width will include
padding and border.

To retrieve size information:

.. code-block:: c

   int32_t w = lv_obj_get_width(widget);
   int32_t h = lv_obj_get_height(widget);
   int32_t content_w = lv_obj_get_content_width(widget);
   int32_t content_h = lv_obj_get_content_height(widget);



Static Positioning
******************

The simplest way to position a widget is by pixel values:

.. code-block:: c

   lv_obj_set_x(widget, 10);          /* From the left */
   lv_obj_set_y(widget, 20);          /* From the top */
   lv_obj_set_pos(widget, 10, 20);    /* Set both */

These values are relative to the parent's top-left corner, including the parent’s
padding and border.



Dynamic Positioning
*******************

Align to the Parent
-------------------

You can position a widget relative to any side or corner of the parent:

.. code-block:: c

   lv_obj_align(widget, LV_ALIGN_TOP_RIGHT, -10, 5);

Or:

.. code-block:: c

   lv_obj_set_align(widget, LV_ALIGN_BOTTOM_MID);

Available anchor points:

- :cpp:enumerator:`LV_ALIGN_TOP_LEFT`
- :cpp:enumerator:`LV_ALIGN_TOP_MID`
- :cpp:enumerator:`LV_ALIGN_TOP_RIGHT`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_LEFT`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_MID`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_RIGHT`
- :cpp:enumerator:`LV_ALIGN_LEFT_MID`
- :cpp:enumerator:`LV_ALIGN_RIGHT_MID`
- :cpp:enumerator:`LV_ALIGN_CENTER`

Use :cpp:expr:`lv_obj_center(widget)` as a shortcut to center a widget.

Align to Another Widget
------------------------

Align to a different reference widget:

.. code-block:: c

   lv_obj_align_to(label, btn, LV_ALIGN_OUT_TOP_MID, 0, -10);

This places ``label`` 10 px above ``btn``.

Unlike ``lv_obj_align``, this alignment is not persistent—repositioning ``btn`` won’t
update ``label``.

.. image:: /_static/images/align.png

Layouts
-------

:ref:`Flex <flex>` and :ref:`Grid <grid>` layouts can automatically position widgets
in rows, columns, or 2D grids. They are fully responsive.

Percentage
----------

You can use percentages for dynamic positioning:

.. code-block:: c

   lv_obj_set_x(widget, lv_pct(50));

``100%`` means the parent's content area (the size excluding padding and border).



Static Sizing
***************

Set fixed size in pixels:

.. code-block:: c

   lv_obj_set_size(widget, 200, 100);



Dynamic Sizing
**************

Percentage
----------

Set width or height as a percentage of the parent’s content area:

.. code-block:: c

   lv_obj_set_height(widget, lv_pct(100));

It also works in a responsive way, meaning the Widget's size will be updated
automatically as the parent’s size changes.

Content
-------

Use ``LV_SIZE_CONTENT`` to size the widget based on its children:

.. code-block:: c

   lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
   lv_obj_t * label = lv_label_create(cont);
   lv_label_set_text(label, "Some text");

Ignored for hidden (``LV_OBJ_FLAG_HIDDEN``) or floating (``LV_OBJ_FLAG_FLOATING``)
widgets.

Layouts
-------

Flex and Grid layouts can also stretch widgets to control both position and size.



.. _coord_min_max_size:

Min and Max Size
****************

LVGL supports ``min-width``, ``max-width``, ``min-height``, and ``max-height``.

Use them to set limits when using ``LV_SIZE_CONTENT`` or percentage sizes:

.. code-block:: c

   lv_style_set_max_height(&style, 200);
   lv_obj_add_style(widget, &style, LV_STATE_DEFAULT);

You can also use percentages:

.. code-block:: c

   lv_style_set_max_height(&style, lv_pct(50));



.. _coord_translation:

Translate Position
******************

To visually move a widget from its current position without changing its base
coordinates:

.. code-block:: c

   lv_style_set_translate_y(&style_pressed, -20);

Useful for hover or press effects.

Percentage-based translation is relative to the widget’s own size (not the parent).

Coordinate translation applies after layout and affects scrollbars and
``LV_SIZE_CONTENT``.



.. _coord_transformation:

Transform Size
**************

You can also visually increase the widget size without affecting layout:

.. code-block:: c

   lv_style_set_transform_width(&style_pressed, 10);
   lv_style_set_transform_height(&style_pressed, 10);

Unlike ``translate``, this is a visual-only effect and does **not** affect layout,
scrollbar behavior, or container resizing.



DPI Independent Unit
********************

If just an integer is used as a size or position unit, it means the number of physical
pixels on the display. This also means that if the display is changed to a new one
with the same size but higher pixel density, everything will appear smaller.

By using :cpp:expr:`lv_dpx(n)`, the values are scaled based on the DPI of the display
to make the result independent of pixel density.

The reference DPI is 160, on which ``lv_dpx(100)`` returns ``100``.
If the DPI were only 80, ``lv_dpx(100)`` would return ``50``—only 50 pixels.

The default DPI of the displays is set by :c:macro:`LV_DPI_DEF` in ``lv_conf.h``,
but it can be modified with:

:cpp:expr:`lv_display_set_dpi(disp, new_dpi)`

``lv_dpx`` can be used anywhere a pixel value is needed (``x``, ``y``, ``width``,
``height``, ``radius``, ``padding``, etc.).

Note that ``lv_dpx`` is evaluated only once, so if the DPI changes afterward, the value
won’t be updated automatically.

With the help of ``lv_dpx``, only the pixel values can be scaled. Images and fonts
still need to be changed manually to adapt to a new DPI.
