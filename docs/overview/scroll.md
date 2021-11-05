```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/scroll.md
```
# Scroll

## Overview
In LVGL scrolling works very intuitively: if an object is outside of its parent content area (the size without padding), the parent becomes scrollable and scrollbar(s) will appear. That's it.

Any object can be scrollable including `lv_obj_t`, `lv_img`, `lv_btn`, `lv_meter`, etc

The object can either be scrolled horizontally or vertically in one stroke; diagonal scrolling is not possible.

### Scrollbar
 
#### Mode
Scrollbars are displayed according to a configured `mode`. The following `mode`s exist:
- `LV_SCROLLBAR_MODE_OFF`  Never show the scrollbars
- `LV_SCROLLBAR_MODE_ON`  Always show the scrollbars
- `LV_SCROLLBAR_MODE_ACTIVE` Show scroll bars while a object is being scrolled
- `LV_SCROLLBAR_MODE_AUTO`  Show scroll bars when the content is large enough to be scrolled

`lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_...)` sets the scrollbar mode on an object.


#### Styling
The scrollbars have their own dedicated part, called `LV_PART_SCROLLBAR`. For example a scrollbar can turn to red like this:
```c
static lv_style_t style_red;
lv_style_init(&style_red);
lv_style_set_bg_color(&style_red, lv_color_red());

...

lv_obj_add_style(obj, &style_red, LV_PART_SCROLLBAR);
```

An object goes to the `LV_STATE_SCROLLED` state while it's being scrolled. This allows adding different styles to the scrollbar or the object itself when scrolled.
This code makes the scrollbar blue when the object is scrolled:
```c
static lv_style_t style_blue;
lv_style_init(&style_blue);
lv_style_set_bg_color(&style_blue, lv_color_blue());

...

lv_obj_add_style(obj, &style_blue, LV_STATE_SCROLLED | LV_PART_SCROLLBAR);
```

If the base direction of the `LV_PART_SCROLLBAR` is RTL (`LV_BASE_DIR_RTL`) the vertical scrollbar will be placed on the left. 
Note that, the `base_dir` style property is inherited. Therefore, it can be set directly on the `LV_PART_SCROLLBAR` part of an object
or on the object's or any parent's main part to make a scrollbar inherit the base direction. 

### Events
The following events are related to scrolling:
- `LV_EVENT_SCROLL_BEGIN` Scrolling begins
- `LV_EVENT_SCROLL_END` Scrolling ends
- `LV_EVENT_SCROLL` Scroll happened. Triggered on every position change.
Scroll events

## Basic example
TODO

## Features of scrolling

Besides, managing "normal" scrolling there are many interesting and useful additional features.


### Scrollable

It's possible to make an object non-scrollable with `lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE)`.

Non-scrollable objects can still propagate the scrolling (chain) to their parents.

The direction in which scrolling happens can be controlled by `lv_obj_set_scroll_dir(obj, LV_DIR_...)`.
The following values are possible for the direction:
- `LV_DIR_TOP` only scroll up
- `LV_DIR_LEFT` only scroll left
- `LV_DIR_BOTTOM` only scroll down
- `LV_DIR_RIGHT` only scroll right
- `LV_DIR_HOR` only scroll horizontally
- `LV_DIR_TOP` only scroll vertically
- `LV_DIR_ALL` scroll any directions

OR-ed values are also possible. E.g. `LV_DIR_TOP | LV_DIR_LEFT`.


### Scroll chain
If an object can't be scrolled further (e.g. its content has reached the bottom-most position) additional scrolling is propagated to its parent. If the parent can be scrolled in that direction than it will be scrolled instead.
It continues propagating to the grandparent and grand-grandparents as well.

The propagation on scrolling is called "scroll chaining" and it can be enabled/disabled with the `LV_OBJ_FLAG_SCROLL_CHAIN` flag. 
If chaining is disabled the propagation stops on the object and the parent(s) won't be scrolled.

### Scroll momentum
When the user scrolls an object and releases it, LVGL can emulate inertial momentum for the scrolling. It's like the object was thrown and scrolling slows down smoothly. 

The scroll momentum can be enabled/disabled with the `LV_OBJ_FLAG_SCROLL_MOMENTUM` flag. 

### Elastic scroll
Normally an object can't be scrolled past the extremeties of its content. That is the top side of the content can't be below the top side of the object. 

However, with `LV_OBJ_FLAG_SCROLL_ELASTIC` a fancy effect is added when the user "over-scrolls" the content. The scrolling slows down, and the content can be scrolled inside the object. 
When the object is released the content scrolled in it will be animated back to the valid position. 

### Snapping
The children of an object can be snapped according to specific rules when scrolling ends. Children can be made snappable individually with the `LV_OBJ_FLAG_SNAPPABLE` flag.

An object can align snapped children in four ways:
- `LV_SCROLL_SNAP_NONE` Snapping is disabled. (default)
- `LV_SCROLL_SNAP_START` Align the children to the left/top side of a scrolled object
- `LV_SCROLL_SNAP_END` Align the children to the right/bottom side of a scrolled object
- `LV_SCROLL_SNAP_CENTER` Align the children to the center of a scrolled object
  
Snap alignment is set with `lv_obj_set_scroll_snap_x/y(obj, LV_SCROLL_SNAP_...)`:
 
Under the hood the following happens:
1. User scrolls an object and releases the screen
2. LVGL calculates where the scroll would end considering scroll momentum
3. LVGL finds the nearest scroll point
4. LVGL scrolls to the snap point with an animation
 
### Scroll one
The "scroll one" feature tells LVGL to allow scrolling only one snappable child at a time. 
This requires making the children snappable and setting a scroll snap alignment different from `LV_SCROLL_SNAP_NONE`.

This feature can be enabled by the `LV_OBJ_FLAG_SCROLL_ONE` flag.

### Scroll on focus
Imagine that there a lot of objects in a group that are on a scrollable object. Pressing the "Tab" button focuses the next object but it might be outside the visible area of the scrollable object. 
If the "scroll on focus" feature is enabled LVGL will automatically scroll objects to bring their children into view. 
The scrolling happens recursively therefore even nested scrollable objects are handled properly. 
The object will be scrolled into view even if it's on a different page of a tabview. 

## Scroll manually
The following API functions allow manual scrolling of objects:
- `lv_obj_scroll_by(obj, x, y, LV_ANIM_ON/OFF)` scroll by `x` and `y` values
- `lv_obj_scroll_to(obj, x, y, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the top left corner
- `lv_obj_scroll_to_x(obj, x, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the left side
- `lv_obj_scroll_to_y(obj, y, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the top side

## Self size

Self size is a property of an object. Normally, the user shouldn't use this parameter but if a custom widget is created it might be useful.

In short, self size establishes the size of an object's content. To understand it better take the example of a table. 
Let's say it has 10 rows each with 50 px height. So the total height of the content is 500 px. In other words the "self height" is 500 px. 
If the user sets only 200 px height for the table LVGL will see that the self size is larger and make the table scrollable.

This means not only the children can make an object scrollable but a larger self size will too.

LVGL uses the `LV_EVENT_GET_SELF_SIZE` event to get the self size of an object. Here is an example to see how to handle the event:
```c
if(event_code == LV_EVENT_GET_SELF_SIZE) {
	lv_point_t * p = lv_event_get_param(e);
  
  //If x or y < 0 then it doesn't neesd to be calculated now
  if(p->x >= 0) {
    p->x = 200;	//Set or calculate the self width
  }
  
  if(p->y >= 0) {
    p->y = 50;	//Set or calculate the self height
  }
}
```

## Examples

```eval_rst

.. include:: ../../examples/scroll/index.rst

```
