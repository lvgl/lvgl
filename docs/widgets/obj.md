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
- contains the styles
- attributes like *Clickable*, *Scrollable*, etc.

In object-oriented thinking, it is the base class from which all other objects in LVGL are inherited. 

The functions and functionalities of the Base object can be used with other widgets too. For example `lv_obj_set_width(slider, 100)`

The Base object can be directly used as a simple widget: it's nothing more than a rectangle. In HTML terms, think of it as a `<div>`.

### Coordinates

Only a small subset of coordinate settings is described here. To see all the features of LVGL (padding, coordinates in styles, layouts, etc) visit the [Coordinates](/overview/coords) page.

#### Size
The object size can be modified on individual axes with `lv_obj_set_width(obj, new_width)` and `lv_obj_set_height(obj, new_height)`, or both axes can be modified at the same time with `lv_obj_set_size(obj, new_width, new_height)`.

#### Position
You can set the position relative to the parent with `lv_obj_set_x(obj, new_x)` and `lv_obj_set_y(obj, new_y)`, or both axes at the same time with `lv_obj_set_pos(obj, new_x, new_y)`.

#### Alignment
You can align the object on its parent with `lv_obj_set_align(obj, LV_ALIGN_...)`. After this every x and y setting will be relative to the set alignment mode.
For example, this will shift the object by 10;20 px from the center of its parent:
```c
lv_obj_set_align(obj, LV_ALIGN_CENTER);
lv_obj_set_pos(obj, 10, 20);

//Or in one function
lv_obj_align(obj, LV_ALIGN_CENTER, 10, 20);
```

To align one object to another use: `lv_obj_align_to(obj_to_align, obj_referece, LV_ALIGN_..., x, y)`

For example, to align a text below an image: `lv_obj_align_to(text, image, LV_ALIGN_OUT_BOTTOM_MID, 0, 10)`.

The following align types exist:
![](/misc/align.png "Alignment types in LVGL")


### Parents and children
You can set a new parent for an object with `lv_obj_set_parent(obj, new_parent)`. To get the current parent, use `lv_obj_get_parent(obj)`.

To get a specific child of a parent use `lv_obj_get_child(parent, idx)`. Some examples for `idx`:
- `0` get the child created first
- `1` get the child created second
- `-1` get the child created last

The children can be iterated lke this:
```c
uint32_t i;
for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
  lv_obj_t * child = lv_obj_get_child(parent, i);
  /*Do something with child*/
}
```

`lv_obj_get_index(obj)` returns the index of the object in its parent. It is equivalent to the number of younger children in the parent.

You can bring an object to the foreground or send it to the background with `lv_obj_move_foreground(obj)` and `lv_obj_move_background(obj)`.

You can change the index of an object in its parent using  `lv_obj_move_to_index(obj, index)`. 

You can swap the position of two objects with `lv_obj_swap(obj1, obj2)`.

### Display and Screens

At the highest level of the LVGL object hierarchy is the *display* which represents the driver for a display device (physical display or simulator). A display can have one or more screens associated with it. Each screen contains a hierarchy of objects for graphical widgets representing a layout that covers the entire display.

When you have created a screen like `lv_obj_t * screen = lv_obj_create(NULL)`, you can make it active with `lv_scr_load(screen)`. The `lv_scr_act()` function gives you a pointer to the active screen.

If you have multiple displays, it's important to know that the screen functions operate on the most recently created display or the one explicitly selected with `lv_disp_set_default`.

To get an object's screen use the `lv_obj_get_screen(obj)` function.

### Events

To set an event callback for an object, use `lv_obj_add_event_cb(obj, event_cb, LV_EVENT_..., user_data)`,

To manually send an event to an object, use `lv_event_send(obj, LV_EVENT_..., param)`

Read the [Event overview](/overview/event) to learn more about events.


### Styles
Be sure to read the [Style overview](/overview/style). Here only the most essential functions are described.

A new style can be added to an object with the `lv_obj_add_style(obj, &new_style, selector)` function. 
`selector` is an ORed combination of part and state(s). E.g. `LV_PART_SCROLLBAR | LV_STATE_PRESSED`.

The base objects use `LV_PART_MAIN` style properties and `LV_PART_SCROLLBAR` with the typical background style properties. 


### Flags
There are some attributes which can be enabled/disabled by `lv_obj_add/clear_flag(obj, LV_OBJ_FLAG_...)`:

- `LV_OBJ_FLAG_HIDDEN`  Make the object hidden. (Like it wasn't there at all)
- `LV_OBJ_FLAG_CLICKABLE`  Make the object clickable by input devices
- `LV_OBJ_FLAG_CLICK_FOCUSABLE`  Add focused state to the object when clicked
- `LV_OBJ_FLAG_CHECKABLE`  Toggle checked state when the object is clicked
- `LV_OBJ_FLAG_SCROLLABLE`  Make the object scrollable
- `LV_OBJ_FLAG_SCROLL_ELASTIC`  Allow scrolling inside but with slower speed
- `LV_OBJ_FLAG_SCROLL_MOMENTUM`  Make the object scroll further when "thrown"
- `LV_OBJ_FLAG_SCROLL_ONE` Allow scrolling only one snappable children
- `LV_OBJ_FLAG_SCROLL_CHAIN_HOR`  Allow propagating the horizontal scroll to a parent
- `LV_OBJ_FLAG_SCROLL_CHAIN_VER`  Allow propagating the vertical scroll to a parent
- `LV_OBJ_FLAG_SCROLL_CHAIN`  Simple packaging for (`LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER`)
- `LV_OBJ_FLAG_SCROLL_ON_FOCUS`  Automatically scroll object to make it visible when focused
- `LV_OBJ_FLAG_SCROLL_WITH_ARROW`  Allow scrolling the focused object with arrow keys
- `LV_OBJ_FLAG_SNAPPABLE` If scroll snap is enabled on the parent it can snap to this object
- `LV_OBJ_FLAG_PRESS_LOCK` Keep the object pressed even if the press slid from the object
- `LV_OBJ_FLAG_EVENT_BUBBLE` Propagate the events to the parent too
- `LV_OBJ_FLAG_GESTURE_BUBBLE` Propagate the gestures to the parent
- `LV_OBJ_FLAG_ADV_HITTEST` Allow performing more accurate hit (click) test. E.g. accounting for rounded corners
- `LV_OBJ_FLAG_IGNORE_LAYOUT` Make the object positionable by the layouts
- `LV_OBJ_FLAG_FLOATING` Do not scroll the object when the parent scrolls and ignore layout
- `LV_OBJ_FLAG_OVERFLOW_VISIBLE` Do not clip the children's content to the parent's boundary

- `LV_OBJ_FLAG_LAYOUT_1`  Custom flag, free to use by layouts
- `LV_OBJ_FLAG_LAYOUT_2`  Custom flag, free to use by layouts

- `LV_OBJ_FLAG_WIDGET_1`  Custom flag, free to use by widget
- `LV_OBJ_FLAG_WIDGET_2`  Custom flag, free to use by widget

- `LV_OBJ_FLAG_USER_1`  Custom flag, free to use by user
- `LV_OBJ_FLAG_USER_2`  Custom flag, free to use by user
- `LV_OBJ_FLAG_USER_3`  Custom flag, free to use by user
- `LV_OBJ_FLAG_USER_4`  Custom flag, free to use by user

Some examples:
```c
/*Hide on object*/
lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);

/*Make an object non-clickable*/
lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
```

### Groups

Read the [Input devices overview](/overview/indev) to learn more about *Groups*.

Objects are added to a *group* with `lv_group_add_obj(group, obj)`, and you can use `lv_obj_get_group(obj)` to see which group an object belongs to.

`lv_obj_is_focused(obj)` returns if the object is currently focused on its group or not. If the object is not added to a group, `false` will be returned.


### Extended click area
By default, the objects can be clicked only within their bounding area. However, this can be extended with `lv_obj_set_ext_click_area(obj, size)`.

## Events
- `LV_EVENT_VALUE_CHANGED` when the `LV_OBJ_FLAG_CHECKABLE` flag is enabled and the object clicked (on transition to/from the checked state)
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` is sent for the following types:
    - `LV_OBJ_DRAW_PART_RECTANGLE` The main rectangle
       - `part`: `LV_PART_MAIN`
       - `rect_dsc`
       - `draw_area`: the area of the rectangle
    - `LV_OBJ_DRAW_PART_BORDER_POST`  The border if the `border_post` style property is `true`
       - `part`: `LV_PART_MAIN`
       - `rect_dsc`
       - `draw_area`: the area of the rectangle
    - `LV_OBJ_DRAW_PART_SCROLLBAR` the scrollbars
       - `part`: `LV_PART_SCROLLBAR`
       - `rect_dsc`
       - `draw_area`: the area of the rectangle

Learn more about [Events](/overview/event).

## Keys
If `LV_OBJ_FLAG_CHECKABLE` is enabled, `LV_KEY_RIGHT` and `LV_KEY_UP` make the object checked, and `LV_KEY_LEFT` and `LV_KEY_DOWN` make it unchecked.

If `LV_OBJ_FLAG_SCROLLABLE` is enabled, but the object is not editable (as declared by the widget class), the arrow keys (`LV_KEY_UP`, `LV_KEY_DOWN`, `LV_KEY_LEFT`, `LV_KEY_RIGHT`) scroll the object. If the object can only scroll vertically, `LV_KEY_LEFT` and `LV_KEY_RIGHT` will scroll up/down instead, making it compatible with an encoder input device. See [Input devices overview](/overview/indev) for more on encoder behaviors and the edit mode.


Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../examples/widgets/obj/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_obj.h
  :project: lvgl

```
