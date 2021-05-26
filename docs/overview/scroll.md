```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/scroll.md
```
# Scroll

## Overview
In LVGL scrolling works very intuitively: if an object is out of its parent content area (the size without paddings), the parent becomes scrollable and scrollbar(s) will appear. That's it.

Any object can be scrollable including `lv_obj_t`, `lv_img`, `lv_btn`, `lv_meter`, etc

The obejct can be scrolled either horizontally or vertically at a time, that is diagonal scrolling is not possible.

### Scrollbar
 
#### Mode
The scrollbars are displayed according to the set `mode`. The following `mode`s exist:
- `LV_SCROLLBAR_MODE_OFF`  Never show the scrollbars
- `LV_SCROLLBAR_MODE_ON`  Always show the scrollbars
- `LV_SCROLLBAR_MODE_ACTIVE` Show scroll bars while object is being scrolled
- `LV_SCROLLBAR_MODE_AUTO`  Show scroll bars when the content is large enough to be scrolled

`lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_...)` set the scrollbar mode on an object.


#### Styling
The scrollbars have its own dedicated part, called `LV_PART_SCROLLBAR`. For example a scrollbar can turned to red like this:
```c
static lv_style_t style_red;
lv_style_init(&style_red);
lv_style_set_bg_color(&style_red, lv_color_red());

...

lv_obj_add_style(obj, &style_red, LV_PART_SCROLLBAR);
```

The object goes to `LV_STATE_SCROLLED` state while it's being scrolled. It allows adding different style to the scrollbar or the object itself when scrolled.
This code makes the scrollbar blue when the object is scrolled:
```c
static lv_style_t style_blue;
lv_style_init(&style_blue);
lv_style_set_bg_color(&style_red, lv_color_blue());

...

lv_obj_add_style(obj, &style_blue, LV_STATE_SCROLLED | LV_PART_SCROLLBAR);
```

### Events
The following events are related to scrolling:
- `LV_EVENT_SCROLL_BEGIN` Scrolling begins
- `LV_EVENT_SCROLL_END` Scrolling ends
- `LV_EVENT_SCROLL` Scroll happened. Triggered on every position change.
Scroll events

## Basic example
TODO

## Features of scrolling

Besides managing "normal" scrolling there are many interesting and useful additional features too.


### Scrollable

It's possible to make an object non-scrollable with `lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE)`.

Non-scrollable object can still propagate the scrolling (chain) to the parents.

The direction in which scrolling can happen can be controlled by `lv_obj_set_scroll_dir(obj, LV_DIR_...)`.
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
If an object can't be scrolled further (e.g. it's content has reached the bottom most position) the scrolling is propagated to it's parent. If the parent an be scrolled in that direction than it will be scrolled instead.
It goes to the grad parent and grand grandparents too.

The propagation on scrolling in called "scroll chaining" and it can be enabled/disabled with the `LV_OBJ_FLAG_SCROLL_CHAIN` flag. 
If chaining is disabled the propagation stops on the object and the parent(s) won't be scrolled.

### Scroll momentum
When the user scrolls an object and releases it LVGL can emulate a momentum for the scrolling. It's like to object were thrown and the scrolling slows down smoothly. 

The scroll momentum can be enabled/disabled with the `LV_OBJ_FLAG_SCROLL_MOMENTUM` flag. 

### Elastic scroll
Normally the content can't be scrolled inside the object. That is the top side of the content can't be below the top side of the object. 

However, with `LV_OBJ_FLAG_SCROLL_ELASTIC` a fancy effect can be added when the user "over-scrolls" the content. The scrolling slows down, and the content can be scrolled inside the object. 
When the object is releases the content is scrolled in it will be animated back to the valid position. 

### Snaping
The children of an object can be snapped according to specific rules when scrolling ends. Children can be made snapable individually with the `LV_OBJ_FLAG_SNAPABLE` flag.
The object can align the snapped children in 4 ways: 
- `LV_SCROLL_SNAP_NONE` Snapping is disabled. (default)
- `LV_SCROLL_SNAP_START` Align the children to the left/top side of the scrolled object
- `LV_SCROLL_SNAP_END` Align the children to the right/bottom side of the scrolled object
- `LV_SCROLL_SNAP_CENTER` Align the children to the center of the scrolled object
  
The alignment can be set with `lv_obj_set_scroll_snap_x/y(obj, LV_SCROLL_SNAP_...)`:
 
Under the hood the followings happen
1. User scrolls an object and releases the screen
2. LVGL calculates where would the scroll end considering scroll momentum
3. LVGL finds the nearest scroll point
4. LVGL scrolls the snap point with an animation
 
### Scroll one
The "scroll one" feature tells LVGL to allow scrolling only one snapable children at a time. 
So it requires to make the children snapable and and set a scroll snap alignment different from `LV_SCROLL_SNAP_NONE`.

This feature can be enabled by the `LV_OBJ_FLAG_SCROLL_ONE` flag.

### Scroll on focus
Imagine that there a lot of objects in a group that are on scrollable object. Pressing the "Tab" button focuses the next object but it might be out of the visible area of the scrollable object. 
If the "scroll on focus" features is enabled LVGL will automatically scroll to the objects to bring the children into the view. 
The scrolling happens recursively therefore even nested scrollable object are handled properly. 
The object will be scrolled to the view even if it's on a different page of a tabview. 

## Scroll manually
The following API functions allow to manually scroll objects:
- `lv_obj_scroll_by(obj, x, y, LV_ANIM_ON/OFF)` scroll by `x` and `y` values
- `lv_obj_scroll_to(obj, x, y, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the top left corner
- `lv_obj_scroll_to_x(obj, x, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the left side
- `lv_obj_scroll_to_y(obj, y, LV_ANIM_ON/OFF)` scroll to bring the given coordinate to the left side

## Self size

Self size is a property of an object. Normally, the user shouldn't use this parameter but if a custom widget is created it might be useful.

In short, self size tell the size of the content. To understand it better take the example of a table. 
Let's say it has 10 rows each with 50 px height. So the total height of the content is 500 px. In other words the "self height" is 500 px. 
If the user sets only 200 px height for the table LVGL will see that the self size is larger and make the table scrollable.

It means not only the children can make an object scrollable but a larger self size too.

LVGL uses the `LV_EVENT_GET_SELF_SIZE` event to get the self size of an object. Here is an example to see how to handle the event
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
