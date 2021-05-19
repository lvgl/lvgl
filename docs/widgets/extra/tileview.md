```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/tileview.md
```
# Tile view (lv_tileview)

## Overview

The Tile view is a container object where its elements (called *tiles*) can be arranged in a grid form. 
By swiping the user can navigate between the tiles. 
Any direction of swiping can be disabled on the tiles individually to not allow moving from tile to an other. 

If the Tile view is screen sized it gives a user interface you might have seen on the smartwatches.

## Parts and Styles
The Tile view is build from an [lv_obj](/widgets/obj) container and also [lv_obj](/widgets/obj) tiles.

The parts and styles work the same as for [lv_obj](/widgets/obj).

## Usage

### Add a tile

`lv_tileview_add_tile(tileview, row_id, col_id, dir)` creates a new tile on the `row_id`th row and `col_id`th column. 
`dir` can be `LV_DIR_LEFT/RIGHT/TOP/BOTTOM/HOR/VER/ALL` or OR-ed values to enable moving to the adjacent tiles into the given direction with swiping. 

The returned value is an `lv_obj_t *` on which the content of the tab can be created.

### Change tile
The Tile view can scroll to a tile with `lv_obj_set_tile(tileview, tile_obj, LV_ANIM_ON/OFF)` or `lv_obj_set_tile_id(tileviewv, col_id, row_id, LV_ANIM_ON/OFF);`


## Events
- `LV_EVENT_VALUE_CHANGED` Sent when a new tile loaded either with scrolling. `lv_tileview_get_tile_act(tabview)` can be used to get current tile.

## Keys
*Keys* are not handled by the Tile view.

Learn more about [Keys](/overview/indev).

## Example


```eval_rst

.. include:: ../../../examples/widgets/tileview/index.rst

```


## API 

```eval_rst

.. doxygenfile:: lv_tileview.h
  :project: lvgl
        
```
