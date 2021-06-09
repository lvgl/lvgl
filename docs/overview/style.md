```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/style.md
```
# Styles

*Styles* are used to set the appearance of the objects. Styles in lvgl are heavily inspired by CSS. The concept in nutshell is as follows:
- A style is an `lv_style_t` variable which can hold properties like border width, text color and so on. It's similar to a `class` in CSS. 
- Styles can be assigned to objects to change their appearance. During the assignment the target part (*pseudo element* in CSS) and target state (*pseudo class*) can be specified.
For example one can add `style_blue` to the knob of a slider when it's in pressed state.
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
  
The combination states the object can be focused and pressed at the same time. This is represented as `LV_STATE_FOCUSED | LV_STATE_PRESSED`. 

The style can be added to any state and state combination. 
For example, setting a different background color for default and pressed state. 
If a property is not defined in a state the best matching state's property will be used. Typically this means the property with `LV_STATE_DEFAULT` is used.˛
If the property is not set even for the default state the default value will be used. (See later)

But what does the "best matching state's property" really mean? 
States have a precedence which is shown by their value (see in the above list). A higher value means higher precedence.
To determine which state's property to use let's take an example. Imagine the background color is defined like this:
- `LV_STATE_DEFAULT`: white
- `LV_STATE_PRESSED`: gray
- `LV_STATE_FOCUSED`: red

1. By the default the object is in default state, so it's a simple case: the property is perfectly defined in the object's current state as white.
2. When the object is pressed there are 2 related properties: default with white (default is related to every state) and pressed with gray. 
The pressed state has 0x0020 precedence which is higher than the default state's 0x0000 precedence, so gray color will be used.
3. When the object is focused the same thing happens as in pressed state and red color will be used. (Focused state has higher precedence than default state).
4. When the object is focused and pressed both gray and red would work, but the pressed state has higher precedence than focused so gray color will be used.
5. It's possible to set e.g rose color for `LV_STATE_PRESSED | LV_STATE_FOCUSED`. 
In this case, this combined state has 0x0020 + 0x0002 = 0x0022 precedence, which is higher than the pressed state's precedence so rose color would be used.
6. When the object is in checked state there is no property to set the background color for this state. So for lack of a better option, the object remains white from the default state's property.

Some practical notes:
- The precedence (value) of states is quite intuitive and it's something the user would expect naturally. E.g. if an object is focused the user will still want to see if it's pressed, therefore pressed state has a higher precedence. 
If the focused state had a higher precedence it would overwrite the pressed color.
- If you want to set a property for all states (e.g. red background color) just set it for the default state. If the object can't find a property for its current state it will fall back to the default state's property.
- Use ORed states to describe the properties for complex cases. (E.g. pressed + checked + focused)
- It might be a good idea to use different style elements for different states. 
For example, finding background colors for released, pressed, checked + pressed, focused, focused + pressed, focused + pressed + checked, etc states is quite difficult. 
Instead, for example, use the background color for pressed and checked states and indicate the focused state with a different border color. 

## Cascading styles
It's not required to set all the properties in one style. It's possible to add more styles to an object and let the later added style to modify or extend appearance.
For example, create a general gray button style and create a new for red buttons where only the new background color is set. 

This is much like in CSS when used classes are listed like `<div class=".btn .btn-red">`.

Styles added later have precedence over ones set earlier. So in the gray/red button example above, the normal button style should be added first and the red style second. 
However, the precedence coming from states are still taken into account. 
So let's examine the following case:
- the basic button style defines dark-gray color for default state and light-gray color pressed state
- the red button style defines the background color as red only in the default state

In this case, when the button is released (it's in default state) it will be red because a perfect match is found in the most recently added style (red). 
When the button is pressed the light-gray color is a better match because it describes the current state perfectly, so the button will be light-gray. 

## Inheritance 
Some properties (typically that are related to texts) can be inherited from the parent object's styles. 
Inheritance is applied only if the given property is not set in the object's styles (even in default state). 
In this case, if the property is inheritable, the property's value will be searched in the parents too until an object specifies a value for the property. The parents will use their own state to detemine the value. 
So if a button is pressed, and the text color comes from here, the pressed text color will be used.


## Parts
Objects can have *parts* which can have their own styles. 

The following predefined parts exist in LVGL:
- `LV_PART_MAIN` A background like rectangle*/
- `LV_PART_SCROLLBAR`  The scrollbar(s)
- `LV_PART_INDICATOR` Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox
- `LV_PART_KNOB` Like a handle to grab to adjust the value*/
- `LV_PART_SELECTED` Indicate the currently selected option or section
- `LV_PART_ITEMS` Used if the widget has multiple similar elements (e.g. table cells)*/
- `LV_PART_TICKS` Ticks on scales e.g. for a chart or meter
- `LV_PART_CURSOR` Mark a specific place e.g. text area's or chart's cursor
- `LV_PART_CUSTOM_FIRST` Custom parts can be added from here.


For example a [Slider](/widgets/core/slider) has three parts:
- Background
- Indiactor
- Knob

It means the all three parts of the slider can have their own styles. See later how to add style styles to objects and parts.

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

To get a property's value from a style:
```c
lv_style_value_t v;
lv_res_t res = lv_style_rget_prop(&style, LV_STYLE_BG_COLOR, &v);
if(res == LV_RES_OK) {	/*Found*/
	do_something(v.color);
}
```

`lv_style_value_t` has 3 fields:
- `num` for integer, boolean and opacity properties
- `color` for color properties
- `ptr` for pointer properties

To reset a style (free all its data) use 
```c
lv_style_reset(&style);
```

## Add and remove styles to a widget
A style on its own is not that useful, it needs to be assigned to an object to take effect.

### Add styles
To add a style to an object use `lv_obj_add_style(obj, &style, <selector>)`. `<selector>` is an OR-ed value of parts and state to which the style should be added. Some examples:
- `LV_PART_MAIN | LV_STATE_DEFAULT`
- `LV_STATE_PRESSED`: The main part in pressed state. `LV_PART_MAIN` can be omitted
- `LV_PART_SCROLLBAR`: The scrollbar part in the default state. `LV_STATE_DEFAULT` can be omitted.
- `LV_PART_SCROLLBAR | LV_STATE_SCROLLED`: The scrollbar part when the object is being scrolled
- `0` Same as `LV_PART_MAIN | LV_STATE_DEFAULT`. 
- `LV_PART_INDICATOR | LV_STATE_PRESSED | LV_STATE_CHECKED` The indicator part when the object is pressed and checked at the same time.

Using `lv_obj_add_style`: 
```c
lv_obj_add_style(btn, &style_btn, 0);      				  /*Default button style*/
lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED);  /*Overwrite only a some colors to red when pressed*/
```

### Remove styles
To remove all styles from an object use `lv_obj_remove_style_all(obj)`.

To remove specific styles use `lv_obj_remove_style(obj, style, selector)`. This function will remove `style` only if the `selector` matches with the `selector` used in `lv_obj_add_style`. 
`style` can be `NULL` to check only the `selector` and remove all matching styles. The `selector` can use the `LV_STATE_ANY` and `LV_PART_ANY` values to remove the style with any state or part.


### Report style changes
If a style which is already assigned to object changes (i.e. a property is added or changed) the objects using that style should be notified. There are 3 options to do this:
1. If you know that the changed properties can be applied by a simple redraw (e.g. color or opacity changes) just call `lv_obj_invalidate(obj)` or `lv_obj_invalideate(lv_scr_act())`. 
2. If more complex style properties were changed or added, and you know which object(s) are affected by that style call `lv_obj_refresh_style(obj, part, property)`. 
To refresh all parts and properties use `lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY)`.
3. To make LVGL check all objects to see whether they use the style and refresh them when needed call `lv_obj_report_style_change(&style)`. If `style` is `NULL` all objects will be notified about the style change.

### Get a property's value on an object
To get a final value of property - considering cascading, inheritance, local styles and transitions (see below) - get functions like this can be used: 
`lv_obj_get_style_<property_name>(obj, <part>)`. 
These functions uses the object's current state and if no better candidate returns a default value.  
For example:
```c
lv_color_t color = lv_obj_get_style_bg_color(btn, LV_PART_MAIN);
```

## Local styles
Besides "normal" styles, the objects can store local styles too. This concept is similar to inline styles in CSS (e.g. `<div style="color:red">`) with some modification. 

So local styles are like normal styles but they can't be shared among other objects. If used, local styles are allocated automatically, and freed when the object is deleted.
They are useful to add local customization to the object.

Unlike in CSS, in LVGL local styles can be assigned to states (*pseudo-classes*) and parts (*pseudo-elements*).

To set a local property use functions like `lv_obj_set_style_local_<property_name>(obj, <value>, <selector>);`  
For example:
```c
lv_obj_set_style_local_bg_color(slider, lv_color_red(), LV_PART_INDICATOR | LV_STATE_FOCUSED);
```
## Properties
For the full list of style properties click [here](/overview/style-props).

### Typical background properties
In the documentation of the widgets you will see sentences like "The widget use the typical background properties". The "typical background properties" are the ones related to:
- Background
- Border
- Outline
- Shadow
- Padding
- Width and height transformation
- X and Y translation


## Transitions
By default, when an object changes state (e.g. it's pressed) the new properties from the new state are set immediately. However, with transitions it's possible to play an animation on state change.
For example, on pressing a button its background color can be animated to the pressed color over 300 ms.

The parameters of the transitions are stored in the styles. It's possible to set 
- the time of the transition
- the delay before starting the transition 
- the animation path (also known as timing or easing function)
- the properties to animate 

The transition properties can be defined for each state. For example, setting 500 ms transition time in default state will mean that when the object goes to the default state a 500 ms transition time will be applied. 
Setting 100 ms transition time in the pressed state will mean a 100 ms transition time when going to pressed state.
So this example configuration will result in going to pressed state quickly and then going back to default slowly. 

To describe a transition an `lv_transition_dsc_t` variable needs to initialized and added to a style:
```c
/*Only its pointer is saved so must static, global or dynamically allocated */
static const lv_style_prop_t trans_props[] = {
											LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
											0, /*End marker*/
};

static lv_style_transition_dsc_t trans1;
lv_style_transition_dsc_init(&trans1, trans_props, lv_anim_path_ease_out, duration_ms, delay_ms);

lv_style_set_transition(&style1, &trans1);
```

## Color filter
TODO


## Themes
Themes are a collection of styles. If there is an active theme LVGL applies it on every created widget.
This will give a default appearance to the UI which can then be modified by adding further styles.

Every display can have a different theme. For example you could have a colorful theme on a TFT and monochrome theme on a secondary monochrome display.

To set a theme for a display, 2 steps are required:
1. Initialize a theme
2. Assign the initialized theme to a display.

Theme initialization functions can have different prototype. This example shows how to set the "default" theme:
```c
lv_theme_t * th = lv_theme_default_init(display,  /*Use the DPI, size, etc from this display*/ 
                                        LV_COLOR_PALETTE_BLUE, LV_COLOR_PALETTE_CYAN,   /*Primary and secondary palette*/
                                        false,    /*Light or dark mode*/ 
                                        &lv_font_montserrat_10, &lv_font_montserrat_14, &lv_font_montserrat_18); /*Small, normal, large fonts*/
                                        
lv_disp_set_theme(display, th); /*Assign the theme to the display*/
```


The themes can be enabled in `lv_conf.h`. If the default theme is enabled by `LV_USE_THEME_DEFAULT 1` LVGL automatically initializes and sets it when a display is created. 

### Extending themes

Built-in themes can be extended. 
If a custom theme is created a parent theme can be selected. The parent theme's styles will be added before the custom theme's styles. 
Any number of themes can be chained this way. E.g. default theme -> custom theme -> dark theme.

`lv_theme_set_parent(new_theme, base_theme)` extends the `base_theme` with the `new_theme`.

There is an example for it below.

## Examples

```eval_rst

.. include:: ../../examples/styles/index.rst

```

## API
```eval_rst

.. doxygenfile:: lv_style.h
  :project: lvgl

.. doxygenfile:: lv_theme.h
  :project: lvgl

.. doxygenfile:: lv_obj_style_gen.h
  :project: lvgl
  
.. doxygenfile:: lv_style_gen.h
  :project: lvgl
  

```
