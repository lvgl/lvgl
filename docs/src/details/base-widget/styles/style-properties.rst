.. _style_properties:

================
Style Properties
================

Size and position
-----------------

Properties related to size, position, alignment and layout of Widgets.

width
~~~~~

Sets width of Widget. Pixel, percentage and `LV_SIZE_CONTENT` values can be used. Percentage values are relative to the width of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> Widget dependent</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

min_width
~~~~~~~~~

Sets a minimal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

max_width
~~~~~~~~~

Sets a maximal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> LV_COORD_MAX</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

height
~~~~~~

Sets height of Widget. Pixel, percentage and `LV_SIZE_CONTENT` can be used. Percentage values are relative to the height of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> Widget dependent</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

min_height
~~~~~~~~~~

Sets a minimal height. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

max_height
~~~~~~~~~~

Sets a maximal height. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> LV_COORD_MAX</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

length
~~~~~~

Its meaning depends on the type of Widget. For example in case of lv_scale it means the length of the ticks.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

x
~

Set X coordinate of Widget considering the ``align`` setting. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

y
~

Set Y coordinate of Widget considering the ``align`` setting. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

align
~~~~~

Set the alignment which tells from which point of the parent the X and Y coordinates should be interpreted. Possible values are: `LV_ALIGN_DEFAULT`, `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`, `LV_ALIGN_LEFT/RIGHT_MID`, `LV_ALIGN_CENTER`. `LV_ALIGN_DEFAULT` means `LV_ALIGN_TOP_LEFT` with LTR base direction and `LV_ALIGN_TOP_RIGHT` with RTL base direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_ALIGN_DEFAULT`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

transform_width
~~~~~~~~~~~~~~~

Make Widget wider on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to Widget's width.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

transform_height
~~~~~~~~~~~~~~~~

Make Widget higher on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to Widget's height.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

translate_x
~~~~~~~~~~~

Move Widget with this value in X direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to Widget's width.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

translate_y
~~~~~~~~~~~

Move Widget with this value in Y direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to Widget's height.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

translate_radial
~~~~~~~~~~~~~~~~

Move object around the centre of the parent object (e.g. around the circumference of a scale)

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

transform_scale_x
~~~~~~~~~~~~~~~~~

Zoom Widget horizontally. The value 256 (or `LV_SCALE_NONE`) means normal size, 128 half size, 512 double size, and so on

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

transform_scale_y
~~~~~~~~~~~~~~~~~

Zoom Widget vertically. The value 256 (or `LV_SCALE_NONE`) means normal size, 128 half size, 512 double size, and so on

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

transform_rotation
~~~~~~~~~~~~~~~~~~

Rotate Widget. The value is interpreted in 0.1 degree units. E.g. 450 means 45 deg.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

transform_pivot_x
~~~~~~~~~~~~~~~~~

Set pivot point's X coordinate for transformations. Relative to Widget's top left corner'

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

transform_pivot_y
~~~~~~~~~~~~~~~~~

Set pivot point's Y coordinate for transformations. Relative to Widget's top left corner'

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

transform_skew_x
~~~~~~~~~~~~~~~~

Skew Widget horizontally. The value is interpreted in 0.1 degree units. E.g. 450 means 45 deg.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

transform_skew_y
~~~~~~~~~~~~~~~~

Skew Widget vertically. The value is interpreted in 0.1 degree units. E.g. 450 means 45 deg.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

Padding
-------

Properties to describe spacing between the parent's sides and the children and among the children. Very similar to the padding properties in HTML.

pad_top
~~~~~~~

Sets the padding on the top. It makes the content area smaller in this direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_bottom
~~~~~~~~~~

Sets the padding on the bottom. It makes the content area smaller in this direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_left
~~~~~~~~

Sets the padding on the left. It makes the content area smaller in this direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_right
~~~~~~~~~

Sets the padding on the right. It makes the content area smaller in this direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_row
~~~~~~~

Sets the padding between the rows. Used by the layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_column
~~~~~~~~~~

Sets the padding between the columns. Used by the layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

pad_radial
~~~~~~~~~~

Pad text labels away from the scale ticks/remainder of the ``LV_PART_``

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Margin
------

Properties to describe spacing around a Widget. Very similar to the margin properties in HTML.

margin_top
~~~~~~~~~~

Sets margin on the top. Widget will keep this space from its siblings in layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

margin_bottom
~~~~~~~~~~~~~

Sets margin on the bottom. Widget will keep this space from its siblings in layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

margin_left
~~~~~~~~~~~

Sets margin on the left. Widget will keep this space from its siblings in layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

margin_right
~~~~~~~~~~~~

Sets margin on the right. Widget will keep this space from its siblings in layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Background
----------

Properties to describe the background color and image of Widget.

bg_color
~~~~~~~~

Set background color of Widget.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0xffffff`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_opa
~~~~~~

Set opacity of the background. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_grad_color
~~~~~~~~~~~~~

Set gradient color of the background. Used only if `grad_dir` is not `LV_GRAD_DIR_NONE`

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_grad_dir
~~~~~~~~~~~

Set direction of the gradient of the background. Possible values are `LV_GRAD_DIR_NONE/HOR/VER`.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRAD_DIR_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_main_stop
~~~~~~~~~~~~

Set point from which background color should start for gradients. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_grad_stop
~~~~~~~~~~~~

Set point from which background's gradient color should start. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 255</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_main_opa
~~~~~~~~~~~

Set opacity of the first gradient color

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 255</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_grad_opa
~~~~~~~~~~~

Set opacity of the second gradient color

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 255</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_grad
~~~~~~~

Set gradient definition. The pointed instance must exist while Widget is alive. NULL to disable. It wraps `BG_GRAD_COLOR`, `BG_GRAD_DIR`, `BG_MAIN_STOP` and `BG_GRAD_STOP` into one descriptor and allows creating gradients with more colors as well. If it's set other gradient related properties will be ignored'

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_image_src
~~~~~~~~~~~~

Set a background image. Can be a pointer to `lv_image_dsc_t`, a path to a file or an `LV_SYMBOL_...`

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

bg_image_opa
~~~~~~~~~~~~

Set opacity of the background image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_image_recolor
~~~~~~~~~~~~~~~~

Set a color to mix to the background image.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_image_recolor_opa
~~~~~~~~~~~~~~~~~~~~

Set intensity of background image recoloring. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means no mixing, 255, `LV_OPA_100` or `LV_OPA_COVER` means full recoloring, other values or LV_OPA_10, LV_OPA_20, etc are interpreted proportionally.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bg_image_tiled
~~~~~~~~~~~~~~

If enabled the background image will be tiled. Possible values are `true` or `false`.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Border
------

Properties to describe the borders

border_color
~~~~~~~~~~~~

Set color of the border

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

border_opa
~~~~~~~~~~

Set opacity of the border. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

border_width
~~~~~~~~~~~~

Set width of the border. Only pixel values can be used.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

border_side
~~~~~~~~~~~

Set only which side(s) the border should be drawn. Possible values are `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL`. OR-ed values can be used as well, e.g. `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT`.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BORDER_SIDE_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

border_post
~~~~~~~~~~~

Sets whether the border should be drawn before or after the children are drawn. `true`: after children, `false`: before children

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Outline
-------

Properties to describe the outline. It's like a border but drawn outside of the rectangles.

outline_width
~~~~~~~~~~~~~

Set width of outline in pixels.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

outline_color
~~~~~~~~~~~~~

Set color of outline.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

outline_opa
~~~~~~~~~~~

Set opacity of outline. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

outline_pad
~~~~~~~~~~~

Set padding of outline, i.e. the gap between Widget and the outline.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

Shadow
------

Properties to describe the shadow drawn under the rectangles.

shadow_width
~~~~~~~~~~~~

Set width of the shadow in pixels. The value should be >= 0.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

shadow_offset_x
~~~~~~~~~~~~~~~

Set an offset on the shadow in pixels in X direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

shadow_offset_y
~~~~~~~~~~~~~~~

Set an offset on the shadow in pixels in Y direction.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

shadow_spread
~~~~~~~~~~~~~

Make shadow calculation to use a larger or smaller rectangle as base. The value can be in pixels to make the area larger/smaller

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

shadow_color
~~~~~~~~~~~~

Set color of shadow

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

shadow_opa
~~~~~~~~~~

Set opacity of shadow. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

Image
-----

Properties to describe the images

image_opa
~~~~~~~~~

Set opacity of an image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

image_recolor
~~~~~~~~~~~~~

Set color to mix with the image.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

image_recolor_opa
~~~~~~~~~~~~~~~~~

Set intensity of color mixing. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Line
----

Properties to describe line-like Widgets

line_width
~~~~~~~~~~

Set width of lines in pixels.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

line_dash_width
~~~~~~~~~~~~~~~

Set width of dashes in pixels. Note that dash works only on horizontal and vertical lines

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

line_dash_gap
~~~~~~~~~~~~~

Set gap between dashes in pixels. Note that dash works only on horizontal and vertical lines

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

line_rounded
~~~~~~~~~~~~

Make end points of the lines rounded. `true`: rounded, `false`: perpendicular line ending

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

line_color
~~~~~~~~~~

Set color of lines.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

line_opa
~~~~~~~~

Set opacity of lines.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Arc
---

TODO

arc_width
~~~~~~~~~

Set width (thickness) of arcs in pixels.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
  </ul>

arc_rounded
~~~~~~~~~~~

Make end points of arcs rounded. `true`: rounded, `false`: perpendicular line ending

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

arc_color
~~~~~~~~~

Set color of arc.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

arc_opa
~~~~~~~

Set opacity of arcs.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

arc_image_src
~~~~~~~~~~~~~

Set an image from which arc will be masked out. It's useful to display complex effects on the arcs. Can be a pointer to `lv_image_dsc_t` or a path to a file

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Text
----

Properties to describe the properties of text. All these properties are inherited.

text_color
~~~~~~~~~~

Sets color of text.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_opa
~~~~~~~~

Set opacity of text. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_font
~~~~~~~~~

Set font of text (a pointer `lv_font_t *`).

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FONT_DEFAULT`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_letter_space
~~~~~~~~~~~~~~~~~

Set letter space in pixels

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_line_space
~~~~~~~~~~~~~~~

Set line space in pixels.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_decor
~~~~~~~~~~

Set decoration for the text. Possible values are `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH`. OR-ed values can be used as well.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_TEXT_DECOR_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

text_align
~~~~~~~~~~

Set how to align the lines of the text. Note that it doesn't align the Widget itself, only the lines inside the Widget. Possible values are `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO`. `LV_TEXT_ALIGN_AUTO` detect the text base direction and uses left or right alignment accordingly

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_TEXT_ALIGN_AUTO`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Miscellaneous
-------------

Mixed properties for various purposes.

radius
~~~~~~

Set radius on every corner. The value is interpreted in pixels (>= 0) or `LV_RADIUS_CIRCLE` for max. radius

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

radial_offset
~~~~~~~~~~~~~

Move start point of object (e.g. scale tick) radially

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

clip_corner
~~~~~~~~~~~

Enable to clip the overflowed content on the rounded corner. Can be `true` or `false`.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

opa
~~~

Scale down all opacity values of the Widget by this factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

opa_layered
~~~~~~~~~~~

First draw Widget on the layer, then scale down layer opacity factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

color_filter_dsc
~~~~~~~~~~~~~~~~

Mix a color with all colors of the Widget.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

color_filter_opa
~~~~~~~~~~~~~~~~

The intensity of mixing of color filter.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

anim
~~~~

Animation template for Widget's animation. Should be a pointer to `lv_anim_t`. The animation parameters are widget specific, e.g. animation time could be the E.g. blink time of the cursor on the Text Area or scroll time of a roller. See Widgets' documentation to learn more.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

anim_duration
~~~~~~~~~~~~~

Animation duration in milliseconds. Its meaning is widget specific. E.g. blink time of the cursor on the Text Area or scroll time of a roller. See Widgets' documentation to learn more.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

transition
~~~~~~~~~~

An initialized ``lv_style_transition_dsc_t`` to describe a transition.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

blend_mode
~~~~~~~~~~

Describes how to blend the colors to the background. Possible values are `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE/MULTIPLY/DIFFERENCE`

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BLEND_MODE_NORMAL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

layout
~~~~~~

Set layout of Widget. Children will be repositioned and resized according to policies set for the layout. For possible values see documentation of the layouts.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

base_dir
~~~~~~~~

Set base direction of Widget. Possible values are `LV_BIDI_DIR_LTR/RTL/AUTO`.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BASE_DIR_AUTO`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

bitmap_mask_src
~~~~~~~~~~~~~~~

If set, a layer will be created for the widget and the layer will be masked with this A8 bitmap mask.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

rotary_sensitivity
~~~~~~~~~~~~~~~~~~

Adjust sensitivity for rotary encoders in 1/256 unit. It means, 128: slow down the rotary to half, 512: speeds up to double, 256: no change

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `256`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Flex
----

Flex layout properties.

flex_flow
~~~~~~~~~

Defines in which direct the flex layout should arrange the children

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FLEX_FLOW_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

flex_main_place
~~~~~~~~~~~~~~~

Defines how to align the children in the direction of flex flow

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FLEX_ALIGN_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

flex_cross_place
~~~~~~~~~~~~~~~~

Defines how to align the children perpendicular to the direction of flex flow

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FLEX_ALIGN_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

flex_track_place
~~~~~~~~~~~~~~~~

Defines how to align the tracks of the flow

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FLEX_ALIGN_NONE`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

flex_grow
~~~~~~~~~

Defines how much space to take proportionally from the free space of the Widget's track

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FLEX_ALIGN_ROW`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

Grid
----

Grid layout properties.

grid_column_dsc_array
~~~~~~~~~~~~~~~~~~~~~

An array to describe the columns of the grid. Should be LV_GRID_TEMPLATE_LAST terminated

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_column_align
~~~~~~~~~~~~~~~~~

Defines how to distribute the columns

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_row_dsc_array
~~~~~~~~~~~~~~~~~~

An array to describe the rows of the grid. Should be LV_GRID_TEMPLATE_LAST terminated

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_row_align
~~~~~~~~~~~~~~

Defines how to distribute the rows.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_column_pos
~~~~~~~~~~~~~~~~~~~~

Set column in which Widget should be placed.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_x_align
~~~~~~~~~~~~~~~~~

Set how to align Widget horizontally.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_column_span
~~~~~~~~~~~~~~~~~~~~~

Set how many columns Widget should span. Needs to be >= 1.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_row_pos
~~~~~~~~~~~~~~~~~

Set row in which Widget should be placed.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_y_align
~~~~~~~~~~~~~~~~~

Set how to align Widget vertically.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>

grid_cell_row_span
~~~~~~~~~~~~~~~~~~

Set how many rows Widget should span. Needs to be >= 1.

.. raw:: html

  <ul>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRID_ALIGN_START`</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
  <li style='display:inline-block; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
  </ul>
