<h3>radius</h3>
Set the radius on every corner. The value is interpreted in pixel (>= 0) or `LV_RADIUS_CIRCLE` for max. radius
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>clip_corner</h3>
Enable to clip the overflowed content on the rounded corner. Can be `true` or `false`.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>transform_width</h3>
Make the object wider on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>transform_height</h3>
Make the object higher on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>translate_x</h3>
Move the object with this value in X direction. Applied after layouts, aligns and other positionings. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>translate_y</h3>
Move the object with this value in Y direction. Applied after layouts, aligns and other positionings. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>transform_zoom</h3>
Zoom image-like objects. Multiplied with the zoom set on the object. The value 256 (or `LV_IMG_ZOOM_NONE`) maens normal size, 128 half size, 512 double size, and so on
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>transform_angle</h3>
 Rotate image-like objects. Added to the rotation set on the object. The value is interpreted in 0.1 degree unit. E.g. 45 deg. = 450 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>opa</h3>
Scale down all opacity values of the object by this factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>color_filter_dsc</h3>
Mix a color to all colors of the object.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>color_filter_opa</h3>
The intensity of mixing of color filter.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>anim_time</h3>
The animation time in milliseconds. It's meaning is widget specific. E.g. blink time of the cursor on the text area or scroll time of a roller. See the widgets' documentation to learn more.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>anim_speed</h3>
The animation speed in pixel/sec. It's meaning is widget specific. E.g. scroll speed of label. See the widgets' documentation to learn more.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>transition</h3>
An initialized `lv_style_transition_dsc_t` to describe a transition.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>blend_mode</h3>
Describes how to blend the colors to the background. The possibel values are `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_top</h3>
Sets the padding on the top. It makes the content arae smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_bottom</h3>
Sets the padding on the bottom. It makes the content arae smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_left</h3>
Sets the padding on the left. It makes the content arae smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_right</h3>
Sets the padding on the right. It makes the content arae smaller in this direction.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_row</h3>
Sets the padding between the rows. Used by the layouts.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>pad_column</h3>
Sets the padding between the columns. Used by the layouts.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>width</h3>
Sets the width of object. Pixel, percentage and `LV_SIZE_CONTENT` values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>min_width</h3>
Sets a minimal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>max_width</h3>
Sets a maximal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>height</h3>
Sets the height of object. Pixel, percentage and `LV_SIZE_CONTENT` can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>min_height</h3>
Sets a minimal height. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>max_height</h3>
Sets a maximal height. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>x</h3>
Set the X coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>y</h3>
Set the Y coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>align</h3>
Set the alignment whcih tells from which point of teh aprent the X and Y coordinates should be interptreted. The possibel values are: `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`, `LV_ALIGN_LEFT/RIGHT_MID`, `LV_ALIGN_CENTER`
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>layout</h3>
Set the layout if the object. The children will be repositioned and resized according to the policies set for the layout. For the possible values see the documentation of the layouts.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_color</h3>
Set the background color of the object.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_opa</h3>
Set the opacity of the bacground. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_grad_color</h3>
Set the gradien color of the background. Used only if `grad_dir` is not `LV_GRAD_DIR_NONE`
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_grad_dir</h3>
Set the direction of the gradient of the background. The possible values are `LV_GRAD_DIR_NONE/HOR/VER`.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_main_stop</h3>
Set the point from which the backround color should start for gradients. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_grad_stop</h3>
Set the point from which the backround's gradient color should start. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_img_src</h3>
Set a background image. Can be a pointer to `lv_img_dsc_t`, a path to a file or an `LV_SYMBOL_...`
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_img_opa</h3>
Set the opacity of the background image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_img_recolor</h3>
Set a color to mix to the background image.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_img_recolor_opa</h3>
Set the intensity of background image recoloring. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means no mixing, 256, `LV_OPA_100` or `LV_OPA_COVER` means full recoloring, other values or LV_OPA_10, LV_OPA_20, etc are interpreted proportinally.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>bg_img_tiled</h3>
If enbaled the background image will be tiled. The possible values are `ture` or `false`.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>border_color</h3>
Set the color of the border
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>border_opa</h3>
Set the opcitiy of the border. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>border_width</h3>
Set hte width of the border. Only pixel values can be used.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>border_side</h3>
Set ony which side(s) the border should be drawn. The possible values are `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL`. OR-ed calues an be used as well, e.g. `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT`.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>border_post</h3>
Sets wheter the the border should be drawn before or after the children ar drawn. `true`: after children, `false`: before children
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_color</h3>
Sets the color of the text.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_opa</h3>
Set the opícity of the text. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_font</h3>
Set the font of the text (a pointer `lv_font_t *`). 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_letter_space</h3>
Set the letter space in pixels
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_line_space</h3>
Set the line space in pixels.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_decor</h3>
Set decoration for the text. The possible values are `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH`. OR-ed values can be used as well.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>text_align</h3>
Set how to align the lines of the text. Note that it doesn't align the object itself, only the lines inside the obejct. The possibel values are `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO`. `LV_TEXT_ALIGN_AUTO` detect the text base direction and uses left or right alignment accordingly
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>img_opa</h3>
Set the opacity of an image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>img_recolor</h3>
Set color to mixt to the image.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>img_recolor_opa</h3>
Set the intensity of the color mixing. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>outline_width</h3>
Set the width of the outline in pixels. 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>outline_color</h3>
Set the color of the outline.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>outline_opa</h3>
Set the opacity of the outline. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>outline_pad</h3>
Set the padding of the outline, i.e. the gap between object and the outline.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_width</h3>
Set the width of the shadow in pixels. The value should be >= 0.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_ofs_x</h3>
Set an offset on the shadow in pixels in X direction. 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_ofs_y</h3>
Set an offset on the shadow in pixels in Y direction. 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_spread</h3>
Make the shadow calcuation to use a larger or smaller rectangle as base. The value can be in pixel t make the area larger/smaller
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_color</h3>
Set the color of the shadow
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>shadow_opa</h3>
Set the opacity of the shadow. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 256, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_width</h3>
Set the width of the lines in pixel.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_dash_width</h3>
Set the width of dashes in pixel. Note that dash works only on horizontal and vertical lines
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_dash_gap</h3>
Set the gap between dashes in pixel. Note that dash works only on horizontal and vertical lines
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_rounded</h3>
Make the end points of the lines rounded. `true`: rounded, `false`: perpadicular line ending 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_color</h3>
Set the color fo the lines.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>line_opa</h3>
Set the opacity of the lines.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>arc_width</h3>
Set the width (ticjkness) of the arcs in pixel.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>arc_rounded</h3>
Make the end points of the arcs rounded. `true`: rounded, `false`: perpadicular line ending 
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>arc_color</h3>
Set the color of the arc.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>arc_opa</h3>
Set the opacity of the arcs.
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

<h3>arc_img_src</h3>
Set an image from which the arc will be masked out. It's useful to display complex exxects on the arcs. Can be a pointer to `lv_img_dsc_t` or a path to a file
<ul>
<li style='display:inline; margin-right: 20px'><strong>Default</strong> 0</li>
<li style='display:inline; margin-right: 20px'><strong>Inherited</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Layout</strong> No</li>
<li style='display:inline; margin-right: 20px'><strong>Ext. draw</strong> No</li>
</ul>

