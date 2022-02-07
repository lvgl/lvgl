```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/others/gridnav.md
```
# Grid navigation

Grid navigation (gridnav for short) is a feature that changes the currently focused child object as arrow keys are pressed.

If the children are arranged into a grid-like layout then the up, down, left and right arrows move focus to the nearest sibling
in the respective direction.

It doesn't matter how the children are positioned, as only the current x and y coordinates are considered.
This means that gridnav works with manually positioned children, as well as [Flex](/layouts/flex.html) and [Grid](/layouts/grid.html) layouts.

Gridnav also works if the children are arranged into a single row or column.
That makes it useful, for example, to simplify navigation on a [List widget](/widgets/extra/list.html).

Gridnav assumes that the object to which gridnav is added is part of a [group](/overview/indev.html#groups). 
This way, if the object with gridnav is focused, the arrow key presses are automatically forwarded to the object
so that gridnav can process the arrow keys.

To move the focus to the next widget of the group use `LV_KEY_NEXT/PREV` or `lv_group_focus_next/prev()` or the `TAB` key on keyboard as usual. 
 
If the container is scrollable and the focused child is out of the view, gridnav will automatically scroll the child into view.
 
## Usage

To add the gridnav feature to an object use `lv_gridnav_add(cont, flags)`.

`flags` control the behavior of gridnav:
- `LV_GRIDNAV_CTRL_NONE` Default settings 
- `LV_GRIDNAV_CTRL_ROLLOVER`  If there is no next/previous object in a direction, 
the focus goes to the object in the next/previous row (on left/right keys) or first/last row (on up/down keys 
- `LV_GRIDNAV_CTRL_SCROLL_FIRST` If an arrow is pressed and the focused object can be scrolled in that direction
then it will be scrolled instead of going to the next/previous object.  If there is no more room for scrolling the next/previous object will be focused normally 

`lv_gridnav_remove(cont)` Removes gridnav from an object.

## Focusable objects

An object needs to be clickable or click focusable (`LV_OBJ_FLAG_CLICKABLE` or `LV_OBJ_FLAG_CLICK_FOCUSABLE`) 
and not hidden (`LV_OBJ_FLAG_HIDDEN`) to be focusable by gridnav.


## Example

```eval_rst

.. include:: ../../examples/others/gridnav/index.rst

```
## API


```eval_rst

.. doxygenfile:: lv_gridnav.h
  :project: lvgl

```
