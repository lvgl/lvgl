```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/style.md
```
# Styles


*Styles* are used to set the appearance of the objects. Styles in lvgl are heavily inspired by CSS. The concept in nutshell is the following:
- A style is an `lv_style_t` variable which can hold properties, for example border width, text color and so on. It's similar to `class` in CSS. 
- Not all properties have to be specified. Unspecified properties will use a default value.
- Styles can be assigned to objects to change their appearance.
- A style can be used by any number of objects.
- Styles can be cascaded which means multiple styles can be assigned to an object and each style can have different properties.  
For example `style_btn` can result in a default gray button and `style_btn_red` can add only a `background-color=red` to overwrite the background color.
- Later added styles have higher precedence. It means if a property is specified in two styles the later added will be used.
- Some properties (e.g. text color) can be inherited from the parent(s) if it's not specified in the object. 
- Objects can have local styles that have higher precedence than "normal" styles.
- Unlike CSS (where pseudo-classes describes different states, e.g. `:hover`), in lvgl a property is assigned to a given state. (I.e. not the "class" is related to state but every single property has a state)
- Transitions can be applied when the object changes state.


## States
The objects can be in the following states:
- **LV_STATE_DEFAULT** (0x00): Normal, released
- **LV_STATE_CHECKED** (0x01): Toggled or checked
- **LV_STATE_FOCUSED** (0x02): Focused via keypad or encoder or clicked via touchpad/mouse 
- **LV_STATE_EDITED**  (0x04): Edit by an encoder
- **LV_STATE_HOVERED** (0x08): Hovered by mouse (not supported now)
- **LV_STATE_PRESSED** (0x10): Pressed
- **LV_STATE_DISABLED** (0x20): Disabled or inactive
  
Combination of states is  also possible, for example `LV_STATE_FOCUSED | LV_STATE_PRESSED`. 

The style properties can be defined in every state and state combination. For example, setting a different background color for default and pressed state. 
If a property is not defined in a state the best matching state's property will be used. Typically it means the property with `LV_STATE_DEFAULT` state.˛
If the property is not set even for the default state the default value will be used. (See later)

But what does the "best matching state's property" really means? 
States have a precedence which is shown by their value (see in the above list). A higher value means higher precedence.
To determine which state's property to use let's use an example. Let's see the background color is defined like this:
- `LV_STATE_DEFAULT`: white
- `LV_STATE_PRESSED`: gray
- `LV_STATE_FOCUSED`: red

1. By the default the object is in default state, so it's a simple case: the property is perfectly defined in the object's current state as white
2. When the object is pressed there are 2 related properties: default with white (default is related to every state) and pressed with gray. 
The pressed state has 0x10 precedence which is higher than the default state's 0x00 precedence, so gray color will be used.
3. When the object is focused the same thing happens as in pressed state and red color will be used. (Focused state has higher precedence than default state).
4. When the object is focused and pressed both gray and red would work, but the pressed state has higher precedence than focused so gray color will be used.
5. It's possible to set e.g rose color for `LV_STATE_PRESSED | LV_STATE_FOCUSED`. 
In this case, this combined state has 0x02 + 0x10 = 0x12 precedence, which higher than the pressed states precedence so rose color would be used.
6. When the object is checked there is no property to set the background color for this state. So in lack of a better option, the object remains white from the default state's property.

Some practical notes:
- If you want to set a property for all state (e.g. red background color) just set it for the default state. If the object can't find a property for its current state it will fall back to the default state's property.
- Use ORed states to describe the properties for complex cases. (E.g. pressed + checked + focused)
- It might be a good idea to use different style elements for different states. 
For example, finding background colors for released, pressed, checked + pressed, focused, focused + pressed, focused + pressed + checked, etc states is quite difficult. 
Instead, for example, use the background color for pressed and checked states and indicate the focused state with a different border color. 

## Cascading styles
It's not required to set all the properties in one style. It's possible to add more styles to an object and let the later added style to modify or extend the properties in the other styles.
For example, create a general gray button style and create a new for red buttons where only the new background color is set. 

It's the same concept when in CSS all the used classes are listed like `<div class=".btn .btn-red">`.

The later added styles have higher precedence over the earlier ones. So in the gray/red button example above, the normal button style should be added first and the red style second. 
However, the precedence coming from states are still taken into account. 
So let's examine the following case:
- the basic button style defines dark-gray color for default state and light-gray color pressed state
- the red button style defines the background color as red only in the default state

In this case, when the button is released (it's in default state) it will be red because a perfect match is found in the lastly added style (red style). 
When the button is pressed the light-gray color is a better match because it describes the current state perfectly, so the button will be light-gray. 

## Inheritance 
Some properties (typically that are related to texts) can be inherited from the parent object's styles. Inheritance is applied only if the given property is not set in the object's styles (even in default state). 
In this case, if the property is inheritable, the property's value will be searched in the parent too until a part can tell a value for the property. The parents will use their own state to tell the value. 
So is button is pressed, and text color comes from here, the pressed text color will be used.


## Parts
Objects can have *parts* which can have their own style. For example a [page](/widgets/page) has four parts:
- Background
- Scrollable
- Scrollbar
- Edge flash

There is three types of object parts **main**, **virtual** and **real**. 

The main part is usually the background and largest part of the object. Some object has only a main part. For example, a button has only a background.

The virtual parts are additional parts just drawn on the fly to the main part. There is no "real" object behind them. 
For example, the page's scrollbar is not a real object, it's just drawn when the page's background is drawn. 
The virtual parts always have the same state as the main part. 
If the property can be inherited, the main part will be also considered before going to the parent.

The real parts are real objects created and managed by the main object. For example, the page's scrollable part is real object. 
Real parts can be in different state than the main part.

To see which parts an object has visit their documentation page.

## Initialize styles and set/get properties

Styles are stored in `lv_style_t` variables. Style variables should be `static`, global or dynamically allocated. In other words they can not be local variables in functions which are destroyed when the function exists. 
Before using a style it should be initialized with `lv_style_init(&my_style)`. 
After initializing the style properties can be set or added to it.
Property set functions looks like this: `lv_style_set_<property_name>(&style, <state>, <value>);`
For example the [above mentioned](#states) example looks like this:
```c
static lv_style_t style1;
lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_WHITE);
lv_style_set_bg_color(&style1, LV_STATE_PRESSED, LV_COLOR_GRAY);
lv_style_set_bg_color(&style1, LV_STATE_FOCUSED, LV_COLOR_RED);
lv_style_set_bg_color(&style1, LV_STATE_FOCUSED | LV_STATE_PRESSED, lv_color_hex(0xf88));
```

It's possible to copy a style with `lv_style_copy(&style_destination, &style_source)`. After copy properties still can be added freely. 

To remove a property use:

```c
lv_style_remove_prop(&style, LV_STYLE_BG_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS));
```

To get the value from style in a given state functions with the following prototype are available: `_lv_style_get_color/int/opa/ptr(&style, <prop>, <result buf>);`.
The best matching property will be selected and it's precedence will be returned. `-1` will be returned if the property is not found. 

The form of the function (`...color/int/opa/ptr`) should be used according to the type of `<prop>`.

For example:

```c
lv_color_t color;
int16_t res;
res = _lv_style_get_color(&style1,  LV_STYLE_BG_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS), &color);
if(res >= 0) {
  //the bg_color is loaded into `color`
}
```

To reset a style (free all it's data) use 
```c
lv_style_reset(&style);
```

## Managing style list
A style on its own not that useful. It should be assigned to an object to take its effect.
Every part of the objects stores a *style list* which is the list of assigned styles.

To add a style to an object use `lv_obj_add_style(obj, <part>, &style)`
For example:
```c
lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn);      /*Default button style*/
lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_red);  /*Overwrite only a some colors to red*/
```

An objects style list can be reset with `lv_obj_reset_style_list(obj, <part>)` 

If a style which is already assigned to an object changes (i.e. one of it's property is set to a new value) the objects using that style should be notified with `lv_obj_refresh_style(obj, part, property)`. To refresh all parts and proeprties use `lv_obj_refresh_style(obj, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL)`.

To get a final value of property, including cascading, inheritance, local styles and transitions (see below), get functions like this can be used: `lv_obj_get_style_<property_name>(obj, <part>)`. 
These functions uses the object's current state and if no better candidate returns a default value.  
For example:
```c
lv_color_t color = lv_obj_get_style_bg_color(btn, LV_BTN_PART_MAIN);
```

## Local styles
In the object's style lists, so-called local properties can be stored as well. It's the same concept than CSS's `<div style="color:red">`.
The local style is the same as a normal style, but it belongs only to a given object and can not be shared with other objects.
To set a local property use functions like `lv_obj_set_style_local_<property_name>(obj, <part>, <state>, <value>);`  
For example:
```c
lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
```

## Transitions
By default, when an object changes state (e.g. it's pressed) the new properties from the new state are set immediately. However, with transitions it's possible to play an animation on state change.
For example, on pressing a button its background color can be animated to the pressed color over 300 ms.

The parameters of the transitions are stored in the styles. It's possible to set 
- the time of the transition
- the delay before starting the transition 
- the animation path (also known as timing function)
- the properties to animate 

The transition properties can be defined for each state. 
For example, setting 500 ms transition time in default state will mean that when the object goes to default state 500 ms transition time will be applied. 
Setting 100 ms transition time in the pressed state will mean a 100 ms transition time when going to presses state.
So this example configuration will result in fast going to presses state and slow going back to default. 

## Properties

The following properties can be used in the styles.

### Mixed properties
- **radius** (`lv_style_int_t`): Set the radius of the background. 0: no radius, `LV_RADIUS_CIRCLE`: maximal radius.  Default value: 0.
- **clip_corner** (`bool`): `true`: enable to clip the overflowed content on the rounded (radius > 0) corners. Default value: `false`.
- **size** (`lv_style_int_t`): Size of internal elements of the widgets. See the documentation of the widgets if this property is used or not. Default value: `LV_DPI / 20`.
- **transform_width**  (`lv_style_int_t`): Make the object wider on both sides with this value. Default value: 0.
- **transform_height**  (`lv_style_int_t`) Make the object higher on both sides with this value. Default value: 0.
- **transform_angle**  (`lv_style_int_t`): Rotate the image-like objects. It's uinit is 0.1 deg, for 45 deg use 450. Default value: 0.
- **transform_zoom**  (`lv_style_int_t`) Zoom image-like objects. 256 (or `LV_IMG_ZOOM_NONE`) for normal size, 128 half size, 512 double size, ans so on. Default value: `LV_IMG_ZOOM_NONE`.
- **opa_scale** (`lv_style_int_t`): Inherited. Scale down all opacity values of the object by this factor. As it's inherited the children objects will be affected too. Default value: `LV_OPA_COVER`.

### Padding and margin properties
*Padding* sets the space on the inner sides of the edges. It means "I don't want my children too close to my sides, so keep this space".  
*Padding inner* set the "gap" between the children.
*Margin* sets the space on the outer side of the edges. It means "I want this space around me". 

These properties are typically used by [Container](/widgets/cont) object if [layout](/widgets/cont#layout) or 
[auto fit](/widgets/cont#auto-fit) is enabled. 
However other widgets also use them to set spacing. See the documentation of the widgets for the details. 
- **pad_top** (`lv_style_int_t`): Set the padding on the top. Default value: 0.
- **pad_bottom** (`lv_style_int_t`): Set the padding on the bottom. Default value: 0.
- **pad_left** (`lv_style_int_t`): Set the padding on the left. Default value: 0.
- **pad_right** (`lv_style_int_t`): Set the padding on the right. Default value: 0.
- **pad_inner** (`lv_style_int_t`): Set the padding inside the object between children. Default value: 0.
- **margin_top** (`lv_style_int_t`): Set the margin on the top. Default value: 0.
- **margin_bottom** (`lv_style_int_t`): Set the margin on the bottom. Default value: 0.
- **margin_left** (`lv_style_int_t`): Set the margin on the left. Default value: 0.
- **margin_right** (`lv_style_int_t`): Set the margin on the right. Default value: 0.

### Background properties
The background is a simple rectangle which can have gradient and `radius` rounding.
- **bg_color** (`lv_color_t`) Specifies the color of the background. Default value: `LV_COLOR_WHITE`.
- **bg_opa** (`lv_opa_t`) Specifies opacity of the background. Default value: `LV_OPA_TRANSP`.
- **bg_grad_color** (`lv_color_t`) Specifies the color of the background's gradient. The color on the right or bottom is `bg_grad_dir != LV_GRAD_DIR_NONE`. Default value: `LV_COLOR_WHITE`.
- **bg_main_stop** (`uint8_t`): Specifies where should the gradient start. 0: at left/top most position, 255: at right/bottom most position. Default value: 0.
- **bg_grad_stop** (`uint8_t`): Specifies where should the gradient stop. 0: at left/top most position, 255: at right/bottom most position. Default value: 255.
- **bg_grad_dir** (`lv_grad_dir_t`) Specifies the direction of the gradient. Can be `LV_GRAD_DIR_NONE/HOR/VER`. Default value: `LV_GRAD_DIR_NONE`. 
- **bg_blend_mode** (`lv_blend_mode_t`): Set the blend mode the background. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_1.*
  :alt: Styling the background in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_1.c
  :language: c
```

### Border properties
The border is drawn on top of the *background*. It has `radius` rounding.
- **border_color** (`lv_color_t`) Specifies the color of the border. Default value: `LV_COLOR_BLACK`.
- **border_opa** (`lv_opa_t`) Specifies opacity of the border. Default value: `LV_OPA_COVER`.
- **border_width** (`lv_style_int_t`): Set the width of the border. Default value: 0.
- **border_side** (`lv_border_side_t`) Specifies which sides of the border to draw. Can be `LV_BORDER_SIDE_NONE/LEFT/RIGHT/TOP/BOTTOM/FULL`. ORed values are also possible. Default value: `LV_BORDER_SIDE_FULL`.
- **border_post** (`bool`): If `true` the border will be drawn after all children have been drawn. Default value: `false`.
- **border_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the border. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_2.*
  :alt: Styling the border in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_2.c
  :language: c
```

### Outline properties
The outline is similar to *border* but is drawn outside of the object.
- **outline_color** (`lv_color_t`) Specifies the color of the outline. Default value: `LV_COLOR_BLACK`.
- **outline_opa** (`lv_opa_t`) Specifies opacity of the outline. Default value: `LV_OPA_COVER`.
- **outline_width** (`lv_style_int_t`): Set the width of the outline. Default value: 0.
- **outline_pad** (`lv_style_int_t`) Set the space between the object and the outline. Default value: 0.
- **outline_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the outline. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_3.*
  :alt: Styling the outline in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_3.c
  :language: c
```

### Shadow properties
The shadow is a blurred area under the object.
- **shadow_color** (`lv_color_t`) Specifies the color of the shadow. Default value: `LV_COLOR_BLACK`.
- **shadow_opa** (`lv_opa_t`) Specifies opacity of the shadow. Default value: `LV_OPA_TRANSP`.
- **shadow_width** (`lv_style_int_t`): Set the width (blur size) of the outline. Default value: 0.
- **shadow_ofs_x** (`lv_style_int_t`): Set the an X offset for the shadow. Default value: 0.
- **shadow_ofs_y** (`lv_style_int_t`): Set the an Y offset for the shadow. Default value: 0.
- **shadow_spread** (`lv_style_int_t`): make the shadow larger than the background in every direction by this value. Default value: 0.
- **shadow_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the shadow. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_4.*
  :alt: Styling the shadow in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_4.c
  :language: c
```

### Pattern properties
The pattern is an image (or symbol) drawn in the middle of the background or repeated to fill the whole background.
- **pattern_image** (`const void *`): Pointer to an `lv_img_dsc_t` variable, a path to an image file or a symbol. Default value: `NULL`.
- **pattern_opa** (`lv_opa_t`): Specifies opacity of the pattern. Default value: `LV_OPA_COVER`.
- **pattern_recolor** (`lv_color_t`): Mix this color to the pattern image. In case of symbols (texts) it will be the text color. Default value: `LV_COLOR_BLACK`.
- **pattern_recolor_opa** (`lv_opa_t`): Intensity of recoloring. Default value: `LV_OPA_TRANSP` (no recoloring).
- **pattern_repeat** (`bool`): `true`: the pattern will be repeated as a mosaic. `false`: place the pattern in the middle of the background. Default value: `false`.
- **pattern_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the pattern. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_5.*
  :alt: Styling the shadow in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_5.c
  :language: c
```

### Value properties
Value is an arbitrary text drawn to the background. It can be a lightweighted replacement of creating label objects.

- **value_str** (`const char *`): Pointer to text to display. Only the pointer is saved! (Don't use local variable with lv_style_set_value_str, instead use static, global or dynamically allocated data). Default value: `NULL`.
- **value_color** (`lv_color_t`): Color of the text. Default value: `LV_COLOR_BLACK`.
- **value_opa** (`lv_opa_t`): Opacity of the text. Default value: `LV_OPA_COVER`.
- **value_font** (`const lv_font_t *`): Pointer to font of the text. Default value: `NULL`.
- **value_letter_space** (`lv_style_int_t`): Letter space of the text. Default value: 0.
- **value_line_space** (`lv_style_int_t`): Line space of the text. Default value: 0.
- **value_align** (`lv_align_t`): Alignment of the text. Can be `LV_ALIGN_...`. Default value: `LV_ALIGN_CENTER`.
- **value_ofs_x** (`lv_style_int_t`): X offset from the original position of the alignment. Default value: 0.
- **value_ofs_y** (`lv_style_int_t`): Y offset from the original position of the alignment. Default value: 0.
- **value_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the text. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_6.*
  :alt: Styling the value text in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_6.c
  :language: c
```

### Text properties
Properties for textual object.
- **text_color** (`lv_color_t`): Color of the text.  Default value: `LV_COLOR_BLACK`.
- **text_opa** (`lv_opa_t`): Opacity of the text. Default value: `LV_OPA_COVER`.
- **text_font** (`const lv_font_t *`): Pointer to font of the text. Default value: `NULL`.
- **text_letter_space** (`lv_style_int_t`): Letter space of the text. Default value: 0.
- **text_line_space** (`lv_style_int_t`): Line space of the text. Default value: 0.
- **text_decor** (`lv_text_decor_t`): Add text decoration. Can be `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH`. Default value: `LV_TEXT_DECOR_NONE`.
- **text_sel_color** (`lv_color_t`): Set  color of the text selection. Default value: `LV_COLOR_BLACK`
- **text_sel_bg_color** (`lv_color_t`): Set background color of text selection. Default value: `LV_COLOR_BLUE`
- **text_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the text. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_7.*
  :alt: Styling a text in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_7.c
  :language: c
```

### Line properties
Properties of lines.
- **line_color** (`lv_color_t`): Color of the line. Default value: `LV_COLOR_BLACK` 
- **line_opa** (`lv_opa_t`): Opacity of the line. Default value: `LV_OPA_COVER`
- **line_width** (`lv_style_int_t`): Width of the line. Default value: 0.
- **line_dash_width** (`lv_style_int_t`): Width of dash. Dashing is drawn only for horizontal or vertical lines. 0: disable dash. Default value: 0.
- **line_dash_gap** (`lv_style_int_t`): Gap between two dash line. Dashing is drawn only for horizontal or vertical lines. 0: disable dash. Default value: 0.
- **line_rounded** (`bool`): `true`: draw rounded line endings. Default value: `false`.
- **line_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the line. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_8.*
  :alt: Styling a line in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_8.c
  :language: c
```

### Image properties
Properties of image.
- **image_recolor** (`lv_color_t`):  Mix this color to the pattern image. In case of symbols (texts) it will be the text color. Default value: `LV_COLOR_BLACK`
- **image_recolor_opa** (`lv_opa_t`): Intensity of recoloring. Default value: `LV_OPA_TRANSP` (no recoloring). Default value: `LV_OPA_TRANSP`
- **image_opa** (`lv_opa_t`): Opacity of the image. Default value: `LV_OPA_COVER`
- **image_blend_mode** (`lv_blend_mode_t`): Set the blend mode of the image. Can be `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE`). Default value: `LV_BLEND_MODE_NORMAL`.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_9.*
  :alt: Styling an image in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_9.c
  :language: c
```

### Transition properties
Properties to describe state change animations.
- **transition_time** (`lv_style_int_t`): Time of the transition. Default value: 0.
- **transition_delay** (`lv_style_int_t`): Delay before the transition. Default value: 0.
- **transition_prop_1** (`property name`): A property on which transition should be applied. Use the property name with upper case with `LV_STYLE_` prefix, e.g. `LV_STYLE_BG_COLOR`. Default value: 0 (none).
- **transition_prop_2** (`property name`): Same as *transition_1* just for another property. Default value: 0 (none).
- **transition_prop_3** (`property name`): Same as *transition_1* just for another property. Default value: 0 (none).
- **transition_prop_4** (`property name`): Same as *transition_1* just for another property. Default value: 0 (none).
- **transition_prop_5** (`property name`): Same as *transition_1* just for another property. Default value: 0 (none).
- **transition_prop_6** (`property name`): Same as *transition_1* just for another property. Default value: 0 (none).
- **transition_path** (`lv_anim_path_t`): An animation path for the transition. (Needs to be static or global variable because only its pointer is saved).
Default value: `lv_anim_path_def` (linear path).
```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_10.*
  :alt: Styling an transitions in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_10.c
  :language: c
```

### Scale properties
Auxiliary properties for scale-like elements. Scales have a normal and end region. 
As the name implies the end region is the end of the scale where can be critical values or inactive values. The normal region is before the end region. 
Both regions could have different properties.
- **scale_grad_color** (`lv_color_t`):  In normal region make gradient to this color on the scale lines. Default value: `LV_COLOR_BLACK`.
- **scale_end_color** (`lv_color_t`):  Color of the scale lines in the end region. Default value: `LV_COLOR_BLACK`.
- **scale_width** (`lv_style_int_t`): Width of the scale. Default value: `LV_DPI / 8`. Default value: `LV_DPI / 8`.
- **scale_border_width** (`lv_style_int_t`): Width of a border drawn on the outer side of the scale in the normal region. Default value: 0.
- **scale_end_border_width** (`lv_style_int_t`): Width of a border drawn on the outer side of the scale in the end region. Default value: 0.
- **scale_end_line_width** (`lv_style_int_t`): Width of a scale lines in the end region. Default value: 0.

```eval_rst
.. image:: /lv_examples/src/lv_ex_style/lv_ex_style_11.*
  :alt: Styling a scale in lvgl

.. literalinclude:: /lv_examples/src/lv_ex_style/lv_ex_style_11.c
  :language: c
```

In the documentation of the widgets you will see sentences like "The widget use the typical background properties". The "typical background" properties are:
- Background
- Border
- Outline
- Shadow
- Pattern
- Value

## Themes
Themes are a collection of styles. There is always an active theme whose styles are automatically applied when an object is created. 
It gives a default appearance to UI which can be modified by adding further styles.

The default theme is set in `lv_conf.h` with `LV_THEME_...` defines. Every theme has the following properties
- primary color
- secondary color
- small font
- normal font
- subtitle font
- title font
- flags (specific to the given theme)

It up to the theme how to use these properties. 

There are 3 built-in themes:
- empty: no default styles are added
- material: an impressive, modern theme - mono: simple black and white theme for monochrome displays
- template: a very simple theme which can be copied to create a custom theme 

### Extending themes

Built-in themes can be extended by custom theme. If a custom theme is created a "base theme" can be selected. The base theme's styles will be added before the custom theme. Any number of themes can be chained this was. E.g. material theme -> custom theme -> dark theme.

Here is an example about how to create a custom theme based on the currently active built-in theme.
```c
 /*Get the current theme (e.g. material). It will be the base of the custom theme.*/   
lv_theme_t * base_theme = lv_theme_get_act();

/*Initialize a custom theme*/
static lv_theme_t custom_theme;                         /*Declare a theme*/
lv_theme_copy(&custom_theme, base_theme);               /*Initialize the custom theme from the base theme*/                           
lv_theme_set_apply_cb(&custom_theme, custom_apply_cb);  /*Set a custom theme apply callback*/
lv_theme_set_base(custom_theme, base_theme);            /*Set the base theme of the csutom theme*/

/*Initialize styles for the new theme*/
static lv_style_t style1;
lv_style_init(&style1);
lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, custom_theme.color_primary);

...

/*Add a custom apply callback*/
static void custom_apply_cb(lv_theme_t * th, lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_BTN:
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            _lv_style_list_add_style(list, &my_style);
            break;
    }
}
```



## Example

### Styling a button
```eval_rst
.. image:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_2.*
  :alt: Styling a button in LVGL

.. literalinclude:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_2.c
  :language: c
```

## API
```eval_rst

.. doxygenfile:: lv_style.h
  :project: lvgl

.. doxygenfile:: lv_theme.h
  :project: lvgl

```
