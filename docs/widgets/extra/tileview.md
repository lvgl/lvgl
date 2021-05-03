```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/tileview.md
```
# Tile view (lv_tileview)

## Overview

The Tileview is a container object where its elements (called *tiles*) can be arranged in a grid form. 
By swiping the user can navigate between the tiles. 

If the Tileview is screen sized it gives a user interface you might have seen on the smartwatches.

## Parts and Styles


The Tileview has the same parts as [Page](/widgets/page). 
Expect `LV_PAGE_PART_SCRL` because it can't be referenced and it's always transparent.
Refer the Page's documentation of details.

## Usage

### Valid positions

The tiles don't have to form a full grid where every element exists. 
There can be holes in the grid but it has to be continuous, i.e. there can't be an empty rows or columns. 

With `lv_tileview_set_valid_positions(tileview, valid_pos_array, array_len)` the valid positions can be set. 
Scrolling will be possible only to this positions. 
The `0,0` index means the top left tile. 
E.g. `lv_point_t valid_pos_array[] = {{0,0}, {0,1}, {1,1}, {{LV_COORD_MIN, LV_COORD_MIN}}` gives a Tile view with "L" shape. 
It indicates that there is no tile in `{1,1}` therefore the user can't scroll there.

In other words, the `valid_pos_array` tells where the tiles are. 
It can be changed on the fly to disable some positions on specific tiles. 
For example, there can be a 2x2 grid where all tiles are added but the first row (y = 0) as a  "main row" and the second row (y = 1) contains options for the tile above it. 
Let's say horizontal scrolling is possible only in the main row and not possible between the options in the second row. In this case the `valid_pos_array` needs to changed when a new main tile is selected:
- for the first main tile: `{0,0}, {0,1}, {1,0}` to disable the `{1,1}` option tile
- for the second main tile `{0,0}, {1,0}, {1,1}` to disable the `{0,1}` option tile

### Set tile

To set the currently visible tile use `lv_tileview_set_tile_act(tileview, x_id, y_id, LV_ANIM_ON/OFF)`. 


### Add element

To add elements just create an object on the Tileview and position it manually to the desired position.

`lv_tileview_add_element(tielview, element)` should be used to make possible to scroll (drag) the Tileview by one its element.
For example, if there is a button on a tile, the button needs to be explicitly added to the Tileview to enable the user to scroll the Tileview with the button too.


### Scroll propagation
The scroll propagation feature of page-like objects (like [List](/widgets/list)) can be used very well here. 
For example, there can be a full-sized List and when it reaches the top or bottom most position the user will scroll the tile view instead.


### Animation time

The animation time of the Tileview can be adjusted with `lv_tileview_set_anim_time(tileview, anim_time)`.

Animations are applied when
- a new tile is selected with `lv_tileview_set_tile_act`
- the current tile is scrolled a little and then released (revert the original title)
- the current tile is scrolled more than half size and then released (move to the next tile)

### Edge flash

An "edge flash" effect can be added when the tile view reached hits an invalid position or the end of tile view when scrolled.

Use `lv_tileview_set_edge_flash(tileview, true)` to enable this feature.

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Slider:
- **LV_EVENT_VALUE_CHANGED** Sent when a new tile loaded either with scrolling or `lv_tileview_set_act`. The event data is set ti the index of the new tile in `valid_pos_array` (It's type is `uint32_t *`)

## Keys
- **LV_KEY_UP**, **LV_KEY_RIGHT** Increment the slider's value by 1
- **LV_KEY_DOWN**, **LV_KEY_LEFT** Decrement the slider's value by 1

Learn more about [Keys](/overview/indev).

## Example


```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_tileview/index.rst

```


## API 

```eval_rst

.. doxygenfile:: lv_tileview.h
  :project: lvgl
        
```
