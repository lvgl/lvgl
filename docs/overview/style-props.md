# Style properties

## Size and position
Properties related to size, position, alignment and layout of the objects.

### width
Sets the width of object. Pixel, percentage and `LV_SIZE_CONTENT` values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> Widget dependent</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### min_width
Sets a minimal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### max_width
Sets a maximal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> LV_COORD_MAX</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### height
Sets the height of object. Pixel, percentage and `LV_SIZE_CONTENT` can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> Widget dependent</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### min_height
Sets a minimal height. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### max_height
Sets a maximal height. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> LV_COORD_MAX</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### x
Set the X coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### y
Set the Y coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### align
Set the alignment which tells from which point of the parent the X and Y coordinates should be interpreted. The possible values are: `LV_ALIGN_DEFAULT`, `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`, `LV_ALIGN_LEFT/RIGHT_MID`, `LV_ALIGN_CENTER`. `LV_ALIGN_DEFAULT` means `LV_ALIGN_TOP_LEFT` with LTR base direction and `LV_ALIGN_TOP_RIGHT` with RTL base direction.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_ALIGN_DEFAULT`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### transform_width
Make the object wider on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### transform_height
Make the object higher on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### translate_x
Move the object with this value in X direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### translate_y
Move the object with this value in Y direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### transform_zoom
Zoom an objects. The value 256 (or `LV_IMG_ZOOM_NONE`) means normal size, 128 half size, 512 double size, and so on
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### transform_angle
Rotate an objects. The value is interpreted in 0.1 degree units. E.g. 450 means 45 deg.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### transform_pivot_x
Set the pivot point's X coordinate for transformations. Relative to the object's top left corner'
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### transform_pivot_y
Set the pivot point's Y coordinate for transformations. Relative to the object's top left corner'
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Padding
Properties to describe spacing between the parent's sides and the children and among the children. Very similar to the padding properties in HTML.

### pad_top
Sets the padding on the top. It makes the content area smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### pad_bottom
Sets the padding on the bottom. It makes the content area smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### pad_left
Sets the padding on the left. It makes the content area smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### pad_right
Sets the padding on the right. It makes the content area smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### pad_row
Sets the padding between the rows. Used by the layouts.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### pad_column
Sets the padding between the columns. Used by the layouts.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Background
Properties to describe the background color and image of the objects.

### bg_color
Set the background color of the object.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0xffffff`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_opa
Set the opacity of the background. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_grad_color
Set the gradient color of the background. Used only if `grad_dir` is not `LV_GRAD_DIR_NONE`
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_grad_dir
Set the direction of the gradient of the background. The possible values are `LV_GRAD_DIR_NONE/HOR/VER`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_GRAD_DIR_NONE`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_main_stop
Set the point from which the background color should start for gradients. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_grad_stop
Set the point from which the background's gradient color should start. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 255</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_grad
Set the gradient definition. The pointed instance must exist while the object is alive. NULL to disable. It wraps `BG_GRAD_COLOR`, `BG_GRAD_DIR`, `BG_MAIN_STOP` and `BG_GRAD_STOP` into one descriptor and allows creating gradients with more colors too.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_dither_mode
Set the dithering mode of the gradient of the background. The possible values are `LV_DITHER_NONE/ORDERED/ERR_DIFF`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_DITHER_NONE`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_img_src
Set a background image. Can be a pointer to `lv_img_dsc_t`, a path to a file or an `LV_SYMBOL_...`
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### bg_img_opa
Set the opacity of the background image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_img_recolor
Set a color to mix to the background image.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_img_recolor_opa
Set the intensity of background image recoloring. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means no mixing, 255, `LV_OPA_100` or `LV_OPA_COVER` means full recoloring, other values or LV_OPA_10, LV_OPA_20, etc are interpreted proportionally.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### bg_img_tiled
If enabled the background image will be tiled. The possible values are `true` or `false`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Border
Properties to describe the borders

### border_color
Set the color of the border
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### border_opa
Set the opacity of the border. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### border_width
Set hte width of the border. Only pixel values can be used.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### border_side
Set only which side(s) the border should be drawn. The possible values are `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL`. OR-ed values can be used as well, e.g. `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BORDER_SIDE_NONE`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### border_post
Sets whether the border should be drawn before or after the children are drawn. `true`: after children, `false`: before children
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Outline
Properties to describe the outline. It's like a border but drawn outside of the rectangles.

### outline_width
Set the width of the outline in pixels. 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### outline_color
Set the color of the outline.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### outline_opa
Set the opacity of the outline. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### outline_pad
Set the padding of the outline, i.e. the gap between object and the outline.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

## Shadow
Properties to describe the shadow drawn under the rectangles.

### shadow_width
Set the width of the shadow in pixels. The value should be >= 0.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### shadow_ofs_x
Set an offset on the shadow in pixels in X direction. 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### shadow_ofs_y
Set an offset on the shadow in pixels in Y direction. 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### shadow_spread
Make the shadow calculation to use a larger or smaller rectangle as base. The value can be in pixel to make the area larger/smaller
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### shadow_color
Set the color of the shadow
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### shadow_opa
Set the opacity of the shadow. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

## Image
Properties to describe the images

### img_opa
Set the opacity of an image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### img_recolor
Set color to mixt to the image.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### img_recolor_opa
Set the intensity of the color mixing. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Line
Properties to describe line-like objects

### line_width
Set the width of the lines in pixel.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### line_dash_width
Set the width of dashes in pixel. Note that dash works only on horizontal and vertical lines
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### line_dash_gap
Set the gap between dashes in pixel. Note that dash works only on horizontal and vertical lines
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### line_rounded
Make the end points of the lines rounded. `true`: rounded, `false`: perpendicular line ending 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### line_color
Set the color fo the lines.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### line_opa
Set the opacity of the lines.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Arc
TODO

### arc_width
Set the width (thickness) of the arcs in pixel.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> Yes</li>
</ul>

### arc_rounded
Make the end points of the arcs rounded. `true`: rounded, `false`: perpendicular line ending 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### arc_color
Set the color of the arc.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### arc_opa
Set the opacity of the arcs.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### arc_img_src
Set an image from which the arc will be masked out. It's useful to display complex effects on the arcs. Can be a pointer to `lv_img_dsc_t` or a path to a file
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Text
Properties to describe the properties of text. All these properties are inherited.

### text_color
Sets the color of the text.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `0x000000`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_opa
Set the opacity of the text. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_font
Set the font of the text (a pointer `lv_font_t *`). 
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_FONT_DEFAULT`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_letter_space
Set the letter space in pixels
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_line_space
Set the line space in pixels.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_decor
Set decoration for the text. The possible values are `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH`. OR-ed values can be used as well.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_TEXT_DECOR_NONE`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### text_align
Set how to align the lines of the text. Note that it doesn't align the object itself, only the lines inside the object. The possible values are `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO`. `LV_TEXT_ALIGN_AUTO` detect the text base direction and uses left or right alignment accordingly
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_TEXT_ALIGN_AUTO`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

## Miscellaneous
Mixed properties for various purposes.

### radius
Set the radius on every corner. The value is interpreted in pixel (>= 0) or `LV_RADIUS_CIRCLE` for max. radius
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### clip_corner
Enable to clip the overflowed content on the rounded corner. Can be `true` or `false`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### opa
Scale down all opacity values of the object by this factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_COVER`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### color_filter_dsc
Mix a color to all colors of the object.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### color_filter_opa
The intensity of mixing of color filter.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_OPA_TRANSP`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### anim
The animation template for the object's animation. Should be a pointer to `lv_anim_t`. The animation parameters are widget specific, e.g. animation time could be the E.g. blink time of the cursor on the text area or scroll time of a roller. See the widgets' documentation to learn more.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### anim_time
The animation time in milliseconds. Its meaning is widget specific. E.g. blink time of the cursor on the text area or scroll time of a roller. See the widgets' documentation to learn more.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### anim_speed
The animation speed in pixel/sec. Its meaning is widget specific. E.g. scroll speed of label. See the widgets' documentation to learn more.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### transition
An initialized `lv_style_transition_dsc_t` to describe a transition.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `NULL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### blend_mode
Describes how to blend the colors to the background. The possible values are `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE/MULTIPLY`
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BLEND_MODE_NORMAL`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### layout
Set the layout if the object. The children will be repositioned and resized according to the policies set for the layout. For the possible values see the documentation of the layouts.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>

### base_dir
Set the base direction of the object. The possible values are `LV_BIDI_DIR_LTR/RTL/AUTO`.
<ul>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Default</strong> `LV_BASE_DIR_AUTO`</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Inherited</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Layout</strong> Yes</li>
<li style='display:inline; margin-right: 20px; margin-left: 0px'><strong>Ext. draw</strong> No</li>
</ul>
