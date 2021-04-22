```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/obj.md
```
# Base object (lv_obj)

## Overview

The 'Base Object' implements the basic properties of widgets on a screen, such as:
- coordinates
- parent object
- children
- main style
- attributes like *Click enable*, *Drag enable*, etc.

In object-oriented thinking, it is the base class from which all other objects in LVGL are inherited. This, among another things, helps reduce code duplication.

The functions and functionalities of Base object can be used with other widgets too. For example `lv_obj_set_width(slider, 100)`

The Base object can be directly used as a simple widgets. It nothing else then a rectangle.

### Coordinates

#### Size
The object size can be modified on individual axes with `lv_obj_set_width(obj, new_width)` and `lv_obj_set_height(obj, new_height)`, or both axes can be modified at the same time with `lv_obj_set_size(obj, new_width, new_height)`.

Styles can add [Margin](/overview/style/#properties) to the objects. Margin tells that "I want this space around me". 
To set width or height reduced by the margin `lv_obj_set_width_margin(obj, new_width)` or `lv_obj_set_height_margin(obj, new_height)`. 
In more exact way: `new_width = left_margin + object_width + right_margin`.

To get the width or height which includes the margins use `lv_obj_get_width/height_margin(obj)`.


Styles can add [Padding](/overview/style/#properties) to the object as well. Padding means "I don't want my children too close to my sides, so keep this space".
To set width or height reduced by the padding `lv_obj_set_width_fit(obj, new_width)` or `lv_obj_set_height_fit(obj, new_height)`.  
In a more exact way: `new_width = left_pad + object_width + right_pad`
To get the width or height which is REDUCED by padding use `lv_obj_get_width/height_fit(obj)`. It can be considered the "useful size of the object".

Margin and padding gets important when [Layout](/widget/cont#layout) or [Auto-fit](/wisgets/cont#fit) is used by other widgets.

#### Position
You can set the x and y coordinates relative to the parent with `lv_obj_set_x(obj, new_x)` and `lv_obj_set_y(obj, new_y)`, or both at the same time with `lv_obj_set_pos(obj, new_x, new_y)`.

#### Alignment
You can align the object to another with `lv_obj_align(obj, obj_ref, LV_ALIGN_..., x_ofs, y_ofs)`.
- `obj` is the object to align.
- `obj_ref` is a reference object. `obj` will be aligned to it. If `obj_ref = NULL`, then the parent of `obj` will be used.
- The third argument is the *type* of alignment. These are the possible options:
![](/misc/align.png "Alignment types in LVGL")

  The alignment types build like `LV_ALIGN_OUT_TOP_MID`.
- The last two arguments allow you to shift the object by a specified number of pixels after aligning it.

For example, to align a text below an image: `lv_obj_align(text, image, LV_ALIGN_OUT_BOTTOM_MID, 0, 10)`.
Or to align a text in the middle of its parent: `lv_obj_align(text, NULL, LV_ALIGN_CENTER, 0, 0)`.


`lv_obj_align_origo` works similarly to `lv_obj_align` but it aligns the center of the object.

For example, `lv_obj_align_origo(btn, image, LV_ALIGN_OUT_BOTTOM_MID, 0, 0)` will align the center of the button the bottom of the image.

The parameters of the alignment will be saved in the object if `LV_USE_OBJ_REALIGN` is enabled in *lv_conf.h*. You can then realign the objects simply by calling `lv_obj_realign(obj)`. 
It's equivalent to calling `lv_obj_align` again with the same parameters.

If the alignment happened with `lv_obj_align_origo`, then it will be used when the object is realigned.

The `lv_obj_align_x/y` and `lv_obj_align_origo_x/y` function can be used t align only on one axis.

If `lv_obj_set_auto_realign(obj, true)` is used the object will be realigned automatically, if its size changes in `lv_obj_set_width/height/size()` functions. 
It's very useful when size animations are applied to the object and the original position needs to be kept.


**Note that the coordinates of screens can't be changed. Attempting to use these functions on screens will result in undefined behavior.**

### Parents and children
You can set a new parent for an object with `lv_obj_set_parent(obj, new_parent)`. To get the current parent, use `lv_obj_get_parent(obj)`.

To get the children of an object, use `lv_obj_get_child(obj, child_prev)` (from last to first) or `lv_obj_get_child_back(obj, child_prev)` (from first to last).
To get the first child, pass `NULL` as the second parameter and use the return value to iterate through the children. The function will return `NULL` if there are no more children. For example:

```c
lv_obj_t * child = lv_obj_get_child(parent, NULL);
while(child) {
    /*Do something with "child" */
    child = lv_obj_get_child(parent, child);
}
```

`lv_obj_count_children(obj)` tells the number of children on an object. `lv_obj_count_children_recursive(obj)` also tells the number of children but counts children of children recursively.

### Screens
When you have created a screen like `lv_obj_t * screen = lv_obj_create(NULL, NULL)`, you can load it with `lv_scr_load(screen)`. The `lv_scr_act()` function gives you a pointer to the current screen.

If you have more display then it's important to know that these functions operate on the lastly created or the explicitly selected (with `lv_disp_set_default`) display.

To get an object's screen use the `lv_obj_get_screen(obj)` function.

### Layers
There are two  automatically generated layers:
- top layer
- system layer

They are independent of the screens and they will be shown on every screen. The *top layer* is above every object on the screen and the *system layer* is above the *top layer* too.
You can add any pop-up windows to the *top layer* freely. But, the *system layer* is restricted to system-level things (e.g. mouse cursor will be placed here in `lv_indev_set_cursor()`).

The `lv_layer_top()` and `lv_layer_sys()` functions gives a pointer to the top or system layer.

You can bring an object to the foreground or send it to the background with `lv_obj_move_foreground(obj)` and `lv_obj_move_background(obj)`.

Read the [Layer overview](/overview/layer) section to learn more about layers.

### Events

To set an event callback for an object, use `lv_obj_set_event_cb(obj, event_cb)`,

To manually send an event to an object, use `lv_event_send(obj, LV_EVENT_..., data)`

Read the [Event overview](/overview/event) to learn more about the events.


## Parts

The widgets can have multiple parts. For example a [Button](/widgets/btn) has only a main part but a [Slider](/widgets/slider) is built from a background, an indicator and a knob. 

The name of the parts is constructed like `LV_ + <TYPE> _PART_ <NAME>`. For example `LV_BTN_PART_MAIN` or `LV_SLIDER_PART_KNOB`. The parts are usually used when styles are add to the objects. 
Using parts different styles can be assigned to the different parts of the objects. 

To learn more about the parts read the related section of the [Style overview](/overview/style#parts).


### States
The object can be in a combinations of the following states:
- **LV_STATE_DEFAULT**  Normal, released
- **LV_STATE_CHECKED** Toggled or checked
- **LV_STATE_FOCUSED** Focused via keypad or encoder or clicked via touchpad/mouse 
- **LV_STATE_EDITED**  Edit by an encoder
- **LV_STATE_HOVERED** Hovered by mouse (not supported now)
- **LV_STATE_PRESSED** Pressed
- **LV_STATE_DISABLED** Disabled or inactive

The states are usually automatically changed by the library as the user presses, releases, focuses etc an object. 
However, the states can be changed manually too. To completely overwrite the current state use `lv_obj_set_state(obj, part, LV_STATE...)`. 
To set or clear given state (but leave to other states untouched) use `lv_obj_add/clear_state(obj, part, LV_STATE_...)`
In both cases ORed state values can be used as well. E.g. `lv_obj_set_state(obj, part, LV_STATE_PRESSED | LV_PRESSED_CHECKED)`.

To learn more about the states read the related section of the [Style overview](/overview/style#states).

### Style
Be sure to read the [Style overview](/overview/style) first.

To add a style to an object use `lv_obj_add_style(obj, part, &new_style)` function. The Base object use all the rectangle-like style properties. 

To remove all styles from an object use `lv_obj_reset_style_list(obj, part)`

If you modify a style, which is already used by objects, in order to refresh the affected objects you can use either `lv_obj_refresh_style(obj)` on each object using it or 
to notify all objects with a given style use `lv_obj_report_style_mod(&style)`. If the parameter of `lv_obj_report_style_mod` is `NULL`, all objects will be notified.

### Attributes
There are some attributes which can be enabled/disabled by `lv_obj_set_...(obj, true/false)`:

- **hidden** -  Hide the object. It will not be drawn and will be considered by input devices as if it doesn't exist., Its children will be hidden too.
- **click** -  Allows you to click the object via input devices. If disabled, then click events are passed to the object behind this one. (E.g. [Labels](/widgets/label) are not clickable by default)
- **top** -  If enabled then when this object or any of its children is clicked then this object comes to the foreground.
- **drag** - Enable dragging (moving by an input device)
- **drag_dir** - Enable dragging only in specific directions. Can be `LV_DRAG_DIR_HOR/VER/ALL`.
- **drag_throw** - Enable "throwing" with dragging as if the object would have momentum
- **drag_parent** - If enabled then the object's parent will be moved during dragging. It will look like as if the parent is dragged. Checked recursively, so can propagate to grandparents too.
- **parent_event** - Propagate the events to the parents too. Checked recursively, so can propagate to grandparents too.
- **opa_scale_enable** - Enable opacity scaling. See the [#opa-scale](Opa scale) section.

### Protect
There are some specific actions which happen automatically in the library.
To prevent one or more that kind of actions, you can protect the object against them. The following protections exists:
- **LV_PROTECT_NONE** No protection
- **LV_PROTECT_POS**  Prevent automatic positioning (e.g.  Layout in [Containers](/widgets/cont))
- **LV_PROTECT_FOLLOW** Prevent the object be followed (make a "line break") in automatic ordering (e.g. Layout in [Containers](/widgets/cont))
- **LV_PROTECT_PARENT** Prevent automatic parent change. (e.g. [Page](/widgets/page) moves the children created on the background to the scrollable)
- **LV_PROTECT_PRESS_LOST** Prevent losing press when the press is slid out of the objects. (E.g. a [Button](/widgets/btn) can be released out of it if it was being pressed)
- **LV_PROTECT_CLICK_FOCUS** Prevent automatically focusing the object if it's in a *Group* and click focus is enabled.
- **LV_PROTECT_CHILD_CHG** Disable the child change signal. Used internally by the library

The `lv_obj_add/clear_protect(obj, LV_PROTECT_...)` sets/clears the protection. You can use *'OR'ed* values of protection types too.

### Groups

Once, an object is added to *group* with `lv_group_add_obj(group, obj)` the object's current group can be get with `lv_obj_get_group(obj)`.

`lv_obj_is_focused(obj)` tells if the object is currently focused on its group or not. If the object is not added to a group, `false` will be returned.

Read the [Input devices overview](/overview/indev) to learn more about the *Groups*.

### Extended click area
By default, the objects can be clicked only on their coordinates, however, this area can be extended with `lv_obj_set_ext_click_area(obj, left, right, top, bottom)`.
`left/right/top/bottom` describes how far the clickable area should extend past the default in each direction.

This feature needs to enabled in *lv_conf.h* with `LV_USE_EXT_CLICK_AREA`. The possible values are:
- **LV_EXT_CLICK_AREA_FULL** store all 4 coordinates as `lv_coord_t`
- **LV_EXT_CLICK_AREA_TINY** store only horizontal and vertical coordinates (use the greater value of left/right and top/bottom) as `uint8_t`
- **LV_EXT_CLICK_AREA_OFF** Disable this feature

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_obj/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_obj.h
  :project: lvgl

```
