```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/dropdown.md
```
# Drop-down list (lv_dropdown)


## Overview

The drop-down list allows the user to select one value from a list. 

The drop-down list is closed by default and displays a single value or a predefined text. 
When activated (by click on the drop-down list), a list is created from which the user may select one option. 
When the user selects a new value, the list is deleted.

## Parts and Styles
The drop-down list's main part is called `LV_DROPDOWN_PART_MAIN` which is a simple [lv_obj](/widgets/obj) object. 
It uses all the typical background properties. *Pressed*, *Focused*, *Edited* etc. stiles are also applied as usual.

The list, which is created when the main object is clicked, is an [Page](/widgets/page). 
Its background part can be referenced with `LV_DROPDOWN_PART_LIST` and uses all the typical background properties for the rectangle itself and text properties for the options. 
To adjust the space between the options use the *text_line_space* style property.
Padding values can be used to make some space on the edges.

The scrollable part of the page is hidden and its styles are always empty (so transparent with no padding).

The scrollbar can be referenced with `LV_DROPDOWN_PART_SCRLBAR` and uses all the typical background properties.

The selected option can be referenced with `LV_DROPDOWN_PART_SELECTED` and uses all the typical background properties. 
It will used in its default state to draw a rectangle on the selected option, and in pressed state to draw a rectangle on the being pressed option.

## Usage

## Overview

### Set options
The options are passed to the drop-down list as a string with `lv_dropdown_set_options(dropdown, options)`. The options should be separated by `\n`. For example: `"First\nSecond\nThird"`.
The string will be saved in the drop-down list, so it can in local variable too.

The `lv_dropdown_add_option(dropdown, "New option", pos)` function inserts a new option to `pos` index.

To save memory the options can set from a static(constant) string too with `lv_dropdown_set_static_options(dropdown, options)`. 
In this case the options string should be alive while the drop-down list exists and  `lv_dropdown_add_option` can't be used

You can select an option manually with `lv_dropdown_set_selected(dropdown, id)`, where _id_ is the index of an option.

### Get selected option
The get the currently selected option, use `lv_dropdown_get_selected(dropdown)`. It will return the *index* of the selected option.

`lv_dropdown_get_selected_str(dropdown, buf, buf_size)` copies the name of the selected option to a `buf`.

### Direction
The list can be created on any side. The default `LV_DROPDOWN_DOWN` can be modified by `lv_dropdown_set_dir(dropdown, LV_DROPDOWN_DIR_LEFT/RIGHT/UP/DOWN)` function.

If the list would be vertically out of the screen, it will aligned to the edge.

### Symbol
A symbol (typically an arrow) can be added to the drop down list with `lv_dropdown_set_symbol(dropdown, LV_SYMBOL_...)`

If the direction of the drop-down list is  `LV_DROPDOWN_DIR_LEFT` the symbol will be shown on the left, else on the right.


### Maximum height
The maximum height of drop-down list can be set via `lv_dropdown_set_max_height(dropdown, height)`. By default it's set to 3/4 vertical resolution.

### Show selected
The main part can either show the selected option or a static text. It can  controlled with `lv_dropdown_set_show_selected(sropdown, true/false)`.

The static text can be set with `lv_dropdown_set_text(dropdown, "Text")`. Only the pointer of the text is saved.

If you also don't want the selected option to be highlighted, a custom transparent style can be used for `LV_DROPDOWN_PART_SELECTED`.

### Animation time
The drop-down list's open/close animation time is adjusted by `lv_dropdown_set_anim_time(ddlist, anim_time)`. Zero animation time means no animation.

### Manually open/close
To manually open or close the drop-down list the `lv_dropdown_open/close(dropdown, LV_ANIM_ON/OFF)` function can be used.

## Events
Besides the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the drop-down list:
 - **LV_EVENT_VALUE_CHANGED** - Sent when the new option is selected.

Learn more about [Events](/overview/event).

## Keys
The following *Keys* are processed by the Buttons:
- **LV_KEY_RIGHT/DOWN** - Select the next option.
- **LV_KEY_LEFT/UP** - Select the previous option.
- **LY_KEY_ENTER** - Apply the selected option (Send `LV_EVENT_VALUE_CHANGED` event and close the drop-down list).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_dropdown/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_dropdown.h
  :project: lvgl

```
