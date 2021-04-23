```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/style.md
```
# Styles


*Styles* are used to set the appearance of the objects. Styles in lvgl are heavily inspired by CSS. The concept in nutshell is the following:
- A style is an `lv_style_t` variable which can hold properties, for example border width, text color and so on. It's similar to a `class` in CSS. 
- Styles can be assigned to objects to change their appearance. During the assignment the target part (*pseudo element* in CSS) and target state (*pseudo class*) can be specified.
For example add `style_blue` to the knob of a slider when it's in pressed state.
- The same style can be used by any number of objects.
- Styles can be cascaded which means multiple styles can be assigned to an object and each style can have different properties. 
Therefore not all properties have to be specified in style. LVLG will look for a property until a style defines it or use a default if it's not spefied by any of the styles.
For example `style_btn` can result in a default gray button and `style_btn_red` can add only a `background-color=red` to overwrite the background color.
- Later added styles have higher precedence. It means if a property is specified in two styles the later added will be used.
- Some properties (e.g. text color) can be inherited from the parent(s) if it's not specified in the object. 
- Objects can have local styles that have higher precedence than "normal" styles.
- Unlike CSS (where pseudo-classes describe different states, e.g. `:focus`), in LVGL a property is assigned to a given state. 
- Transitions can be applied when the object changes state.


## States
The objects can be in the combination of the following states:
- `LV_STATE_DEFAULT` (0x0000) Normal, released state
- `LV_STATE_CHECKED` (0x0001) Toggled or checked state
- `LV_STATE_FOCUSED` (0x0002) Focused via keypad or encoder or clicked via touchpad/mouse 
- `LV_STATE_FOCUS_KEY` (0x0004) Focused via keypad or encoder but not via touchpad/mouse 
- `LV_STATE_EDITED` (0x0008) Edit by an encoder
- `LV_STATE_HOVERED` (0x0010) Hovered by mouse (not supported now)
- `LV_STATE_PRESSED` (0x0020) Being pressed
- `LV_STATE_SCROLLED` (0x0040) Being scrolled
- `LV_STATE_DISABLED` (0x0080) Disabled state
- `LV_STATE_USER_1` (0x1000) Custom state
- `LV_STATE_USER_2` (0x2000) Custom state
- `LV_STATE_USER_3` (0x4000) Custom state
- `LV_STATE_USER_4` (0x8000) Custom state
  
The combination states the object can be focused and pressed at the same time. It represented as `LV_STATE_FOCUSED | LV_STATE_PRESSED`. 

The style can be added to any state and state combination. 
For example, setting a different background color for default and pressed state. 
If a property is not defined in a state the best matching state's property will be used. Typically it means the property with `LV_STATE_DEFAULT` state.˛
If the property is not set even for the default state the default value will be used. (See later)

But what does the "best matching state's property" really means? 
States have a precedence which is shown by their value (see in the above list). A higher value means higher precedence.
To determine which state's property to use let's use an example. Let's see the background color is defined like this:
- `LV_STATE_DEFAULT`: white
- `LV_STATE_PRESSED`: gray
- `LV_STATE_FOCUSED`: red

1. By the default the object is in default state, so it's a simple case: the property is perfectly defined in the object's current state as white.
2. When the object is pressed there are 2 related properties: default with white (default is related to every state) and pressed with gray. 
The pressed state has 0x0020 precedence which is higher than the default state's 0x0000 precedence, so gray color will be used.
3. When the object is focused the same thing happens as in pressed state and red color will be used. (Focused state has higher precedence than default state).
4. When the object is focused and pressed both gray and red would work, but the pressed state has higher precedence than focused so gray color will be used.
5. It's possible to set e.g rose color for `LV_STATE_PRESSED | LV_STATE_FOCUSED`. 
In this case, this combined state has 0x02 + 0x10 = 0x12 precedence, which higher than the pressed states precedence so rose color would be used.
6. When the object is in checked state there is no property to set the background color for this state. So in lack of a better option, the object remains white from the default state's property.

Some practical notes:
- The precedence (value) of states is quite intuitve and it's somthing the user would expect naturally. E.g. if an object is focused, the user still want to see if it's pressed, therefore pressed state has a higher perecedence. 
If the foced state had higer precedence it would overwrite the pressed color.
- If you want to set a property for all state (e.g. red background color) just set it for the default state. If the object can't find a property for its current state it will fall back to the default state's property.
- Use ORed states to describe the properties for complex cases. (E.g. pressed + checked + focused)
- It might be a good idea to use different style elements for different states. 
For example, finding background colors for released, pressed, checked + pressed, focused, focused + pressed, focused + pressed + checked, etc states is quite difficult. 
Instead, for example, use the background color for pressed and checked states and indicate the focused state with a different border color. 

## Cascading styles
It's not required to set all the properties in one style. It's possible to add more styles to an object and let the later added style to modify or extend appearance.
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
Some properties (typically that are related to texts) can be inherited from the parent object's styles. 
Inheritance is applied only if the given property is not set in the object's styles (even in default state). 
In this case, if the property is inheritable, the property's value will be searched in the parents too until an object can tell a value for the property. The parents will use their own state to tell the value. 
So if a button is pressed, and the text color comes from here, the pressed text color will be used.


## Parts
Objects can have *parts* which can have their own styles. 

The following predefined parts exist in LVGL:
- `LV_PART_MAIN` A background like rectangle*/
- `LV_PART_SCROLLBAR`  The scrollbar(s)
- `LV_PART_INDICATOR` Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox
- `LV_PART_KNOB` Like a handle to grab to adjust the value*/
- `LV_PART_SELECTED` Indicate the currently selected option or section
- `LV_PART_ITEMS` Used if the widget has multiple similar elements (e.g. tabel cells)*/
- `LV_PART_TICKS` Ticks on scales e.g. for a chart or meter
- `LV_PART_CURSOR` Mark a specific place e.g. text area's or chart's cursor
- `LV_PART_CUSTOM_FIRST` Custom parts can be added from here.


For example a [Slider](/widgets/slider) has three parts:
- Background
- Indiactor
- Knob

It means the all three parts of teh slider can have their own styles. See later how to add style styles to objects and parts.

## Initialize styles and set/get properties

Styles are stored in `lv_style_t` variables. Style variables should be `static`, global or dynamically allocated. 
In other words they can not be local variables in functions which are destroyed when the function exists. 
Before using a style it should be initialized with `lv_style_init(&my_style)`. 
After initializing the style properties can be set or added to it.

Property set functions looks like this: `lv_style_set_<property_name>(&style, <value>);` For example: 
```c
static lv_style_t style_btn;
lv_style_init(&style_btn);
lv_style_set_bg_color(&style_btn, lv_color_grey());
lv_style_set_bg_opa(&style_btn, LV_OPA_50);
lv_style_set_border_width(&style_btn, 2);
lv_style_set_border_color(&style_btn, lv_color_black());

static lv_style_t style_btn_red;
lv_style_init(&style_btn_red);
lv_style_set_bg_color(&style_btn_red, lv_color_red());
lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
```

To remove a property use:

```c
lv_style_remove_prop(&style, LV_STYLE_BG_COLOR);
```

To get a properties value from style:
```c
lv_style_value_t v;
lv_res_t res = lv_style_rget_prop(&style, LV_STYLE_BG_COLOR, &v);
if(res == LV_RES_OK) {	/*Found*/
	do_something(v.color);
}
```

`lv_style_value_t` has 3 fields:
- `num` for integer, boolean and opacity prpperties
- `color` for color properies
- `ptr` for pointer properties

To reset a style (free all its data) use 
```c
lv_style_reset(&style);
```

## Add and remove styles to a widget
A style on its own not that useful. It should be assigned to an object to take its effect.

### Add styles
To add a style to an object use `lv_obj_add_style(obj, &style, <selector>)`. `<selector>` is an OR-ed value of parts and state to which the style should be added. Some examples:
- `LV_PART_MAIN | LV_STATE_DEFAULT`
- `LV_STATE_PRESSED`: The main part in pressed state. `LV_PART_MAIN` can be omited
- `LV_PART_SCROLLBAR`: The scrollbar part in the default state. `LV_STATE_DEFAULT` can be omited.
- `LV_PART_SCROLLBAR | LV_STATE_SCROLLED`: The scrollbar part when the obejct is being scrolled
- `0` Same as `LV_PART_MAIN | LV_STATE_DEFAULT`. 
- `LV_PART_INDICATOR | LV_STATE_PRESSED | LV_STATE_CHECKED` The indicator part when the obejct is pressed and checked at the same time.

Using `lv_obj_add_style`: 
```c
lv_obj_add_style(btn, &style_btn, 0);      				  /*Default button style*/
lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED);  /*Overwrite only a some colors to red when pressed*/
```

### Remove styles
To remove all styles from an object use `lv_obj_reove_style_all(obj)`.

To remove specific styles use `lv_obj_remoev_style(obj, style, selector)`. This function will remove `style` only if the `selector` matches with the `selector` used in `lv_obj_add_style`. 
`style` can be `NULL` to check only the `selector` and remove all matching styles. The `selector` can use the `LV_STATE_ANY` and `LV_PART_ANY` values to remove the style with any state or part.


### Report style changes
If a style - which is already assigned to objecty - changes (i.e. one of it's property is set to a new value) the objects using that style should be notified. There are 3 options to do this:
1. If you know that the changed proeprties can be applied by a simple (e.g. color or opacity changes) redraw just call `lv_obj_invalidate(obj)`, `lv_obj_invalideate(lv_scr_act())`. 
2. If something more complex change happened on a style and you know which object(s) are affacted by that style call `lv_obj_refresh_style(obj, part, property)`. 
To refresh all parts and properties use `lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY)`.
3. No make LVGL check all object wheter thy use the style and refresh them use `lv_obj_report_style_change(&style)`. If `style` is `NULL` all object's will be notified.

### Get a property's value on an object
To get a final value of property - cosidering cascading, inheritance, local styles and transitions (see below) - get functions like this can be used: 
`lv_obj_get_style_<property_name>(obj, <part>)`. 
These functions uses the object's current state and if no better candidate returns a default value.  
For example:
```c
lv_color_t color = lv_obj_get_style_bg_color(btn, LV_PART_MAIN);
```

## Local styles
Besides "normal" styles, the objects can store local styles too. This concept is similar to inline styles in CSS (e.g. `<div style="color:red">`) with some modification. 

So local styles are like normal styles but they can't be shared among other objects. If used, local styles are allocated automatically, and freed whe nthe object is deleted.
They are usuful to add local customizations to the object.

Unlike in CSS, in LVGL local styles can be assiged to states (*pseudo-classes*) and parts (pseudo-elements).

To set a local property use functions like `lv_obj_set_style_local_<property_name>(obj, <value>, <selector>);`  
For example:
```c
lv_obj_set_style_local_bg_color(slider, lv_color_red(), LV_PART_INDICATOR | LV_STATE_FOCUSED);
```

## Transitions
By default, when an object changes state (e.g. it's pressed) the new properties from the new state are set immediately. However, with transitions it's possible to play an animation on state change.
For example, on pressing a button its background color can be animated to the pressed color over 300 ms.

The parameters of the transitions are stored in the styles. It's possible to set 
- the time of the transition
- the delay before starting the transition 
- the animation path (also known as timing or easing function)
- the properties to animate 

The transition properties can be defined for each state. For example, setting 500 ms transition time in default state will mean that when the object goes to default state 500 ms transition time will be applied. 
Setting 100 ms transition time in the pressed state will mean a 100 ms transition time when going to presses state.
So this example configuration will result in fast going to presses state and slow going back to default. 

To describe a transition an `lv_transition_dsc_t` variable needs to initialized and added to a style:
```c
/*Only its pointer is saved so must static, global or dynamically allocated */
static const lv_style_prop_t trans_props[] = {
											LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
											0, /*End marker*/
};

static lv_style_transition_dsc_t trans1;
lv_style_transition_dsc_init(&trans1, trans_props, lv_anim_path_easeout, duration_ms, delay_ms);

lv_style_set_transition(&style1, &trans1);
```

## Properties

The following properties can be used in the styles.




### Mixed properties
| Name | Description | Values | Default |  Inherited | Layout | Ext. draw |
|------|--------|---------|-------------|------------|--------|-----------|
| **radius** | Set the radius on every corner | px (>= 0) or `LV_RADIUS_CIRCLE` for max. radius | 0 | No | No | No |
| **clip_corner** | Enable to clip the overflowed content on the rounded corners | `true`, `false` | `false` |  No | No | No |
| **transform_width** | Make the object wider on both sides with this value. Percentage values are relative to the object's width. | px or `lv_pct()` | 0 | No | Yes | No |
| **transform_height** | Make the object higher on both sides with this value. Percentage values are relative to the object's height. | `px or `lv_pct()` | 0 | No | Yes | No |
| **translate_x** | Move the object with this value in X direction. Applied after layouts positioning. Percentage values are relative to the object's width. | `px or `lv_pct()` | 0 | Yes | No | No |
| **translate_y** | Move the object with this value in Y direction. Applied after layouts positioning. Percentage values are relative to the object's height. | `px or `lv_pct()` | 0 | Yes | No | No |
| **transform_angle** | Rotate image-like objects. Multiplied with the zoom set on the object. Added to the rotation set on the object.| 0.1 degree, e.g. 45 deg. = 450 | 0 | Yes | No | No |
| **transform_zoom** | Zoom image-like objects. Multiplied with the zoom set on the object. | 256 (or `LV_IMG_ZOOM_NONE`): normal size, 128: half size, 512: double size, and so on | 256 | Yes | No | No |
| **opa** | Scale down all opacity values of the object by this factor. | 0..255 or `LV_OPA_...` | `LV_OPA_COVER` | Yes | No | Yes |

### radius 
Set the radius on every corner. It's value can be in px (>= 0) or `LV_RADIUS_CIRCLE` for maximal radius 

| Default |  Inherited | Layout | Ext. draw |
|---------|------------|--------|-----------|
| 0 | No | No | No |


| **clip_corner** | Enable to clip the overflowed content on the rounded corners | `true`, `false` | `false` |  No | No | No |
| **transform_width** | Make the object wider on both sides with this value. Percentage values are relative to the object's width. | px or `lv_pct()` | 0 | No | Yes | No |
| **transform_height** | Make the object higher on both sides with this value. Percentage values are relative to the object's height. | `px or `lv_pct()` | 0 | No | Yes | No |
| **translate_x** | Move the object with this value in X direction. Applied after layouts positioning. Percentage values are relative to the object's width. | `px or `lv_pct()` | 0 | Yes | No | No |
| **translate_y** | Move the object with this value in Y direction. Applied after layouts positioning. Percentage values are relative to the object's height. | `px or `lv_pct()` | 0 | Yes | No | No |
| **transform_angle** | Rotate image-like objects. Multiplied with the zoom set on the object. Added to the rotation set on the object.| 0.1 degree, e.g. 45 deg. = 450 | 0 | Yes | No | No |
| **transform_zoom** | Zoom image-like objects. Multiplied with the zoom set on the object. | 256 (or `LV_IMG_ZOOM_NONE`): normal size, 128: half size, 512: double size, and so on | 256 | Yes | No | No |
| **opa** | Scale down all opacity values of the object by this factor. | 0..255 or `LV_OPA_...` | `LV_OPA_COVER` | Yes | No | Yes |


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
