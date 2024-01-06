.. _style_properties:

================
Style properties
================

Size and position
-----------------

Properties related to size, position, alignment and layout of the
objects.

width
~~~~~

Sets the width of object. Pixel, percentage and :c:macro:`LV_SIZE_CONTENT`
values can be used. Percentage values are relative to the width of the
parent's content area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default Widget dependent

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

min_width
~~~~~~~~~

Sets a minimal width. Pixel and percentage values can be used.
Percentage values are relative to the width of the parent's content
area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

max_width
~~~~~~~~~

Sets a maximal width. Pixel and percentage values can be used.
Percentage values are relative to the width of the parent's content
area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default LV_COORD_MAX

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

height
~~~~~~

Sets the height of object. Pixel, percentage and :c:macro:`LV_SIZE_CONTENT` can
be used. Percentage values are relative to the height of the parent's
content area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default Widget dependent

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

min_height
~~~~~~~~~~

Sets a minimal height. Pixel and percentage values can be used.
Percentage values are relative to the width of the parent's content
area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

max_height
~~~~~~~~~~

Sets a maximal height. Pixel and percentage values can be used.
Percentage values are relative to the height of the parent's content
area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default LV_COORD_MAX

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

x
~

Set the X coordinate of the object considering the set ``align``. Pixel
and percentage values can be used. Percentage values are relative to the
width of the parent's content area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

y
~

Set the Y coordinate of the object considering the set ``align``. Pixel
and percentage values can be used. Percentage values are relative to the
height of the parent's content area.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

align
~~~~~

Set the alignment which tells from which point of the parent the X and Y
coordinates should be interpreted. The possible values are:

- :cpp:enumerator:`LV_ALIGN_DEFAULT`,
- :cpp:enumerator:`LV_ALIGN_TOP_LEFT`
- :cpp:enumerator:`LV_ALIGN_TOP_MID`
- :cpp:enumerator:`LV_ALIGN_TOP_RIGHT`,
- :cpp:enumerator:`LV_ALIGN_BOTTOM_LEFT`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_MID`
- :cpp:enumerator:`LV_ALIGN_BOTTOM_RIGHT`,
- :cpp:enumerator:`LV_ALIGN_LEFT_MID`
- :cpp:enumerator:`LV_ALIGN_RIGHT_MID`,
- :cpp:enumerator:`LV_ALIGN_CENTER`.
- :cpp:enumerator:`LV_ALIGN_DEFAULT`

:cpp:enumerator:`LV_ALIGN_DEFAULT` means :cpp:enumerator:`LV_ALIGN_TOP_LEFT`
with LTR base direction and :cpp:enumerator:`LV_ALIGN_TOP_RIGHT` with RTL base
direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_ALIGN_DEFAULT`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_width
~~~~~~~~~~~~~~~

Make the object wider on both sides with this value. Pixel and
percentage (with :cpp:expr:`lv_pct(x)`) values can be used. Percentage values
are relative to the object's width.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_height
~~~~~~~~~~~~~~~~

Make the object higher on both sides with this value. Pixel and
percentage (with :cpp:expr:`lv_pct(x)`) values can be used. Percentage values
are relative to the object's height.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

translate_x
~~~~~~~~~~~

Move the object with this value in X direction. Applied after layouts,
aligns and other positioning. Pixel and percentage (with :cpp:expr:`lv_pct(x)`)
values can be used. Percentage values are relative to the object's
width.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

translate_y
~~~~~~~~~~~

Move the object with this value in Y direction. Applied after layouts,
aligns and other positioning. Pixel and percentage (with :cpp:expr:`lv_pct(x)`)
values can be used. Percentage values are relative to the object's
height.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_zoom
~~~~~~~~~~~~~~

Zoom an objects. The value 256 (or :cpp:enumerator:`LV_ZOOM_NONE`) means normal size,
128 half size, 512 double size, and so on

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_angle
~~~~~~~~~~~~~~~

Rotate an objects. The value is interpreted in 0.1 degree units. E.g.
450 means 45 deg.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_pivot_x
~~~~~~~~~~~~~~~~~

Set the pivot point's X coordinate for transformations. Relative to the
object's top left corner'

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

transform_pivot_y
~~~~~~~~~~~~~~~~~

Set the pivot point's Y coordinate for transformations. Relative to the
object's top left corner'

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Padding
-------

Properties to describe spacing between the parent's sides and the
children and among the children. Very similar to the padding properties
in HTML.

pad_top
~~~~~~~

Sets the padding on the top. It makes the content area smaller in this
direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

pad_bottom
~~~~~~~~~~

Sets the padding on the bottom. It makes the content area smaller in
this direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

pad_left
~~~~~~~~

Sets the padding on the left. It makes the content area smaller in this
direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

pad_right
~~~~~~~~~

Sets the padding on the right. It makes the content area smaller in this
direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

pad_row
~~~~~~~

Sets the padding between the rows. Used by the layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

pad_column
~~~~~~~~~~

Sets the padding between the columns. Used by the layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Margin
------

Properties to describe spacing around an object. Very similar to the
margin properties in HTML.

margin_top
~~~~~~~~~~

Sets the margin on the top. The object will keep this space from its
siblings in layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

margin_bottom
~~~~~~~~~~~~~

Sets the margin on the bottom. The object will keep this space from its
siblings in layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

margin_left
~~~~~~~~~~~

Sets the margin on the left. The object will keep this space from its
siblings in layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

margin_right
~~~~~~~~~~~~

Sets the margin on the right. The object will keep this space from its
siblings in layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Background
----------

Properties to describe the background color and image of the objects.

bg_color
~~~~~~~~

Set the background color of the object.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0xffffff``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_opa
~~~~~~

Set the opacity of the background. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or :cpp:enumerator:`LV_OPA_10`,
:cpp:enumerator:`LV_OPA_20`, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_TRANSP`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_grad_color
~~~~~~~~~~~~~

Set the gradient color of the background. Used only if ``grad_dir`` is
not :cpp:enumerator:`LV_GRAD_DIR_NONE`

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_grad_dir
~~~~~~~~~~~

Set the direction of the gradient of the background. The possible values
are:

- :cpp:enumerator:`LV_GRAD_DIR_NONE`
- :cpp:enumerator:`LV_GRAD_DIR_HOR`
- :cpp:enumerator:`LV_GRAD_DIR_VER`

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_GRAD_DIR_NONE`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_main_stop
~~~~~~~~~~~~

Set the point from which the background color should start for
gradients. 0 means to top/left side, 255 the bottom/right side, 128 the
center, and so on

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_grad_stop
~~~~~~~~~~~~

Set the point from which the background's gradient color should start. 0
means to top/left side, 255 the bottom/right side, 128 the center, and
so on

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 255

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_grad
~~~~~~~

Set the gradient definition. The pointed instance must exist while the
object is alive. ``NULL`` to disable. It wraps :cpp:enumerator:`BG_GRAD_COLOR`,
:cpp:enumerator:`BG_GRAD_DIR`, :cpp:enumerator:`BG_MAIN_STOP` and :cpp:enumerator:`BG_GRAD_STOP` into one
descriptor and allows creating gradients with more colors too.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_dither_mode
~~~~~~~~~~~~~~

Set the dithering mode of the gradient of the background. The possible
values are:

- :cpp:enumerator:`LV_DITHER_NONE`
- :cpp:enumerator:`LV_DITHER_ORDERED`
- :cpp:enumerator:`LV_DITHER_ERR_DIFF`

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_DITHER_NONE`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_img_src
~~~~~~~~~~

Set a background image. Can be a pointer to :cpp:struct:`lv_image_dsc_t`, a path to
a file or an ``LV_SYMBOL_...``

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_img_opa
~~~~~~~~~~

Set the opacity of the background image. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or LV_OPA_10,
LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_img_recolor
~~~~~~~~~~~~~~

Set a color to mix to the background image.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_img_recolor_opa
~~~~~~~~~~~~~~~~~~

Set the intensity of background image recoloring. Value 0, :cpp:enumerator:`LV_OPA_0`
or :cpp:enumerator:`LV_OPA_TRANSP` means no mixing, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means full recoloring, other values or LV_OPA_10,
LV_OPA_20, etc are interpreted proportionally.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_TRANSP`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

bg_img_tiled
~~~~~~~~~~~~

If enabled the background image will be tiled. The possible values are
``true`` or ``false``.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Border
------

Properties to describe the borders

border_color
~~~~~~~~~~~~

Set the color of the border

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

border_opa
~~~~~~~~~~

Set the opacity of the border. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or LV_OPA_10,
LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

border_width
~~~~~~~~~~~~

Set the width of the border. Only pixel values can be used.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

border_side
~~~~~~~~~~~

Set only which side(s) the border should be drawn. The possible values
are:

- :cpp:enumerator:`LV_BORDER_SIDE_NONE`
- :cpp:enumerator:`LV_BORDER_SIDE_TOP`
- :cpp:enumerator:`LV_BORDER_SIDE_BOTTOM`
- :cpp:enumerator:`LV_BORDER_SIDE_LEFT`
- :cpp:enumerator:`LV_BORDER_SIDE_RIGHT`
- :cpp:enumerator:`LV_BORDER_SIDE_INTERNAL`

OR-ed values can be used as well, e.g. :cpp:expr:`LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT`.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_BORDER_SIDE_NONE`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

border_post
~~~~~~~~~~~

Sets whether the border should be drawn before or after the children are
drawn. ``true``: after children, ``false``: before children

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Outline
-------

Properties to describe the outline. It's like a border but drawn outside
of the rectangles.

outline_width
~~~~~~~~~~~~~

Set the width of the outline in pixels.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

outline_color
~~~~~~~~~~~~~

Set the color of the outline.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

outline_opa
~~~~~~~~~~~

Set the opacity of the outline. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or LV_OPA_10,
LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

outline_pad
~~~~~~~~~~~

Set the padding of the outline, i.e. the gap between object and the
outline.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

Shadow
------

Properties to describe the shadow drawn under the rectangles.

shadow_width
~~~~~~~~~~~~

Set the width of the shadow in pixels. The value should be ``>= 0``.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

shadow_offset_x
~~~~~~~~~~~~

Set an offset on the shadow in pixels in X direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

shadow_offset_y
~~~~~~~~~~~~

Set an offset on the shadow in pixels in Y direction.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

shadow_spread
~~~~~~~~~~~~~

Make the shadow calculation to use a larger or smaller rectangle as
base. The value can be in pixel to make the area larger/smaller

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

shadow_color
~~~~~~~~~~~~

Set the color of the shadow

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

shadow_opa
~~~~~~~~~~

Set the opacity of the shadow. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or LV_OPA_10,
LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

Image
-----

Properties to describe the images

img_opa
~~~~~~~

Set the opacity of an image. Value 0, :cpp:enumerator:`LV_OPA_0` or :cpp:enumerator:`LV_OPA_TRANSP`
means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or :cpp:enumerator:`LV_OPA_COVER` means
fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi
transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

img_recolor
~~~~~~~~~~~

Set color to mixt to the image.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

img_recolor_opa
~~~~~~~~~~~~~~~

Set the intensity of the color mixing. Value 0, :cpp:enumerator:`LV_OPA_0` or
:cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or
:cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or LV_OPA_10,
LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Line
----

Properties to describe line-like objects

line_width
~~~~~~~~~~

Set the width of the lines in pixel.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

line_dash_width
~~~~~~~~~~~~~~~

Set the width of dashes in pixel. Note that dash works only on
horizontal and vertical lines

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

line_dash_gap
~~~~~~~~~~~~~

Set the gap between dashes in pixel. Note that dash works only on
horizontal and vertical lines

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

line_rounded
~~~~~~~~~~~~

Make the end points of the lines rounded. ``true``: rounded, ``false``:
perpendicular line ending

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

line_color
~~~~~~~~~~

Set the color of the lines.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

line_opa
~~~~~~~~

Set the opacity of the lines.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Arc
---

TODO

arc_width
~~~~~~~~~

Set the width (thickness) of the arcs in pixel.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw Yes

.. raw:: html

   </li>

.. raw:: html

   </ul>

arc_rounded
~~~~~~~~~~~

Make the end points of the arcs rounded. ``true``: rounded, ``false``:
perpendicular line ending

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

arc_color
~~~~~~~~~

Set the color of the arc.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

arc_opa
~~~~~~~

Set the opacity of the arcs.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

arc_img_src
~~~~~~~~~~~

Set an image from which the arc will be masked out. It's useful to
display complex effects on the arcs. Can be a pointer to
:cpp:struct:`lv_image_dsc_t` or a path to a file

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Text
----

Properties to describe the properties of text. All these properties are
inherited.

text_color
~~~~~~~~~~

Sets the color of the text.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``0x000000``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_opa
~~~~~~~~

Set the opacity of the text. Value 0, :cpp:enumerator:`LV_OPA_0` or :cpp:enumerator:`LV_OPA_TRANSP`
means fully transparent, 255, :cpp:enumerator:`LV_OPA_100` or :cpp:enumerator:`LV_OPA_COVER` means
fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi
transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_font
~~~~~~~~~

Set the font of the text (a pointer :cpp:type:`lv_font_t` ``*``).

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_FONT_DEFAULT`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_letter_space
~~~~~~~~~~~~~~~~~

Set the letter space in pixels

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_line_space
~~~~~~~~~~~~~~~

Set the line space in pixels.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_decor
~~~~~~~~~~

Set decoration for the text. The possible values are:

- :cpp:enumerator:`LV_TEXT_DECOR_NONE`
- :cpp:enumerator:`LV_TEXT_DECOR_UNDERLINE`
- :cpp:enumerator:`LV_TEXT_DECOR_STRIKETHROUGH`

OR-ed values can be used as well.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_TEXT_DECOR_NONE`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

text_align
~~~~~~~~~~

Set how to align the lines of the text. Note that it doesn't align the
object itself, only the lines inside the object. The possible values are:

- :cpp:enumerator:`LV_TEXT_ALIGN_LEFT`
- :cpp:enumerator:`LV_TEXT_ALIGN_CENTER`
- :cpp:enumerator:`LV_TEXT_ALIGN_RIGHT`
- :cpp:enumerator:`LV_TEXT_ALIGN_AUTO` `

:cpp:enumerator:`LV_TEXT_ALIGN_AUTO` detect the text base direction and uses left or right alignment accordingly

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_TEXT_ALIGN_AUTO`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

Miscellaneous
-------------

Mixed properties for various purposes.

radius
~~~~~~

Set the radius on every corner. The value is interpreted in pixel (>= 0)
or :c:macro:`LV_RADIUS_CIRCLE` for max. radius

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

clip_corner
~~~~~~~~~~~

Enable to clip the overflowed content on the rounded corner. Can be
``true`` or ``false``.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

opa
~~~

Scale down all opacity values of the object by this factor. Value 0,
:cpp:enumerator:`LV_OPA_0` or :cpp:enumerator:`LV_OPA_TRANSP` means fully transparent, 255,
:cpp:enumerator:`LV_OPA_100` or :cpp:enumerator:`LV_OPA_COVER` means fully covering, other values or
LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_COVER`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

color_filter_dsc
~~~~~~~~~~~~~~~~

Mix a color to all colors of the object.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

color_filter_opa
~~~~~~~~~~~~~~~~

The intensity of mixing of color filter.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_OPA_TRANSP`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

anim
~~~~

The animation template for the object's animation. Should be a pointer
to :cpp:type:`lv_anim_t`. The animation parameters are widget specific,
e.g. animation time could be the E.g. blink time of the cursor on the
text area or scroll time of a roller. See the widgets' documentation to
learn more.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

anim_time
~~~~~~~~~

The animation time in milliseconds. Its meaning is widget specific. E.g.
blink time of the cursor on the text area or scroll time of a roller.
See the widgets' documentation to learn more.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

anim_speed
~~~~~~~~~~

The animation speed in pixel/sec. Its meaning is widget specific. E.g.
scroll speed of label. See the widgets' documentation to learn more.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

transition
~~~~~~~~~~

An initialized :cpp:struct:`lv_style_transition_dsc_t` to describe a transition.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default ``NULL``

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

blend_mode
~~~~~~~~~~

Describes how to blend the colors to the background. The possible values
are:

- :cpp:enumerator:`LV_BLEND_MODE_NORMAL`
- :cpp:enumerator:`LV_BLEND_MODE_ADDITIVE`
- :cpp:enumerator:`LV_BLEND_MODE_SUBTRACTIVE`
- :cpp:enumerator:`LV_BLEND_MODE_MULTIPLY`

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_BLEND_MODE_NORMAL`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

layout
~~~~~~

Set the layout if the object. The children will be repositioned and
resized according to the policies set for the layout. For the possible
values see the documentation of the layouts.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default 0

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited No

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>

base_dir
~~~~~~~~

Set the base direction of the object. The possible values are:

- :cpp:enumerator:`LV_BASE_DIR_LTR`
- :cpp:enumerator:`LV_BASE_DIR_RTL`
- :cpp:enumerator:`LV_BASE_DIR_AUTO`.

.. raw:: html

   <ul>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Default :cpp:enumerator:`LV_BASE_DIR_AUTO`

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Inherited Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Layout Yes

.. raw:: html

   </li>

.. raw:: html

   <li style="display:inline; margin-right: 20px; margin-left: 0px">

Ext. draw No

.. raw:: html

   </li>

.. raw:: html

   </ul>
