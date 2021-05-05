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

The Drop-down list is added to the deafult group (if it is set). Besides the Drop-down list is an editable object to allow selecting an option with encoder navigation too.

## Parts and Styles
The Dropdown widgets is built from the elements: a "button" and a "list" (they are not realted to the butto and list widgets)

### Button
- `LV_PART_MAIN` The background of the button. It uses the typicaly background proeprties and text proeprties for the text on it.
- `LV_PART_INDICATOR` Typically an arrow symbol that can be an image or a text (`LV_SYMBOL`).

The button goes to `LV_STATE_CHECKED` when its opened.

### List
- `LV_PART_MAIN` The list itself and it uses the typical background proeprties. `max_height` can be used to limit the height of the list. 
- `LV_PART_SCROLLBAR` The scrollbar the background, border, shadow properties and width (for its width) and right padding for the spacing on the right. 
- `LV_PART_SELECTED` Refers to the currently pressed, checked or prssed+checked option. 
It also uses the typical background properties. 

As the list not exists when the drop-down list is closed it's not possible to simply add styles to it.
Instead the following should be done:
1. Ad an event handler to the button for `LV_EVENT_VALUE_CHANGED` (triggered when the list is opened/closed)
2. Use `lv_obj_t * list = lv_dropdown_get_list(dropdown)`
3. `if(list != NULL) {/*Add the styles to the list*/}`

Alternatively the the theme can be extended with the new styles. 
 
## Usage

## Overview

### Set options
The options are passed to the drop-down list as a string with `lv_dropdown_set_options(dropdown, options)`. The options should be separated by `\n`. For example: `"First\nSecond\nThird"`.
The string will be saved in the drop-down list, so it can in local variable too.

The `lv_dropdown_add_option(dropdown, "New option", pos)` function inserts a new option to `pos` index.

To save memory the options can set from a static(constant) string too with `lv_dropdown_set_static_options(dropdown, options)`. 
In this case the options string should be alive while the drop-down list exists and  `lv_dropdown_add_option` can't be used

You can select an option manually with `lv_dropdown_set_selected(dropdown, id)`, where `id` is the index of an option.

### Get selected option
The get the currently selected option, use `lv_dropdown_get_selected(dropdown)`. It will return the *index* of the selected option.

`lv_dropdown_get_selected_str(dropdown, buf, buf_size)` copies the name of the selected option to a `buf`.

### Direction
The list can be created on any side. The default `LV_DIR_BOTTOM` can be modified by `lv_dropdown_set_dir(dropdown, LV_DIR_LEFT/RIGHT/UP/BOTTOM)` function.

If the list would be vertically out of the screen, it will aligned to the edge.

### Symbol
A symbol (typically an arrow) can be added to the drop down list with `lv_dropdown_set_symbol(dropdown, LV_SYMBOL_...)`

If the direction of the drop-down list is  `LV_DIR_LEFT` the symbol will be shown on the left, else on the right.

### Show selected
The main part can either show the selected option or a static text. If a static is set with `lv_dropdown_set_text(dropdown, "Some text")` it will be shown regardless to th selected option.
Id the text text is `NULL` the selected option is displayed on the button.

### Manually open/close
To manually open or close the drop-down list the `lv_dropdown_open/close(dropdown)` function can be used.

## Events
Besides the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the drop-down list:
- `LV_EVENT_VALUE_CHANGED` Sent when the new option is selected or the list is opened/closed.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/DOWN` Select the next option.
- `LV_KEY_LEFT/UP` Select the previous option.
- `LY_KEY_ENTER` Apply the selected option (Send `LV_EVENT_VALUE_CHANGED` event and close the drop-down list).

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/dropdown/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_dropdown.h
  :project: lvgl

```
