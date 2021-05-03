```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/spinbox.md
```
# Spinbox (lv_spinbox)

## Overview
The Spinbox contains a number as text which can be increased or decreased by *Keys* or API functions. 
Under the hood the Spinbox is a modified [Text area](/widgets/textarea).

## Parts and Styles
The Spinbox's main part is called `LV_SPINBOX_PART_BG` which is a rectangle-like background using all the typical background style properties. It also describes the style of the label with its *text* style properties. 

`LV_SPINBOX_PART_CURSOR` is a virtual part describing the cursor. Read the [Text area](/widgets/textarea) documentation for a detailed description.

### Set format
`lv_spinbox_set_digit_format(spinbox, digit_count, separator_position)` set the format of the number. 
`digit_count` sets the number of digits. Leading zeros are added to fill the space on the left. 
`separator_position` sets the number of digit before the decimal point. `0` means no decimal point.

`lv_spinbox_set_padding_left(spinbox, cnt)` add `cnt` "space" characters between the sign an the most left digit.

### Value and ranges
`lv_spinbox_set_range(spinbox, min, max)` sets the range of the Spinbox.

`lv_spinbox_set_value(spinbox, num)` sets the Spinbox's value manually. 

`lv_spinbox_increment(spinbox)` and `lv_spinbox_decrement(spinbox)` increments/decrements the value of the Spinbox. 

`lv_spinbox_set_step(spinbox, step)` sets the amount to increment decrement. 

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Drop down lists:
- **LV_EVENT_VALUE_CHANGED** sent when the value has changed. (the value is set as event data as `int32_t`)
- **LV_EVENT_INSERT** sent by the ancestor Text area but shouldn't be used.

Learn more about [Events](/overview/event).

## Keys
The following *Keys* are processed by the Buttons:
- **LV_KEY_LEFT/RIGHT** With *Keypad* move the cursor left/right. With *Encoder* decrement/increment the selected digit. 
- **LY_KEY_ENTER** Apply the selected option (Send `LV_EVENT_VALUE_CHANGED` event and close the Drop down list) 
- **LV_KEY_ENTER** With *Encoder* got the net digit. Jump to the first after the last. 


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_spinbox/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_spinbox.h
  :project: lvgl
        
```
## Example
