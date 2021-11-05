```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/spinbox.md
```
# Spinbox (lv_spinbox)

## Overview
The Spinbox contains a number as text which can be increased or decreased by *Keys* or API functions. 
Under the hood the Spinbox is a modified [Text area](/widgets/core/textarea).

## Parts and Styles
The parts of the Spinbox are identical to the [Text area](/widgets/core/textarea).

### Value, range and step
`lv_spinbox_set_value(spinbox, 1234)` sets a new value on the Spinbox.

`lv_spinbox_increment(spinbox)` and `lv_spinbox_decrement(spinbox)` increments/decrements the value of the Spinbox according to the currently selected digit. 

`lv_spinbox_set_range(spinbox, -1000, 2500)` sets a range. If the value is changed by `lv_spinbox_set_value`, by *Keys*,`lv_spinbox_increment/decrement` this range will be respected.

`lv_spinbox_set_step(spinbox, 100)` sets which digits to change on increment/decrement. Only multiples of ten can be set, and not for example 3. 

`lv_spinbox_set_pos(spinbox, 1)` sets the cursor to a specific digit to change on increment/decrement. For example position '0' sets the cursor to the least significant digit.

If an encoder is used as input device, the selected digit is shifted to the right by default whenever the encoder button is clicked. To change this behaviour to shifting to the left, the `lv_spinbox_set_digit_step_direction(spinbox, LV_DIR_LEFT)` can be used

### Format

`lv_spinbox_set_digit_format(spinbox, digit_count, separator_position)` sets the number format. `digit_count` is the number of digits excluding the decimal separator and the sign.
`separator_position` is the number of digits before the decimal point. If 0, no decimal point is displayed.

### Rollover
`lv_spinbox_set_rollover(spinbox, true/false)` enables/disabled rollover mode. If either the minimum or maximum value is reached with rollover enabled, the value will change to the other limit. If rollover is disabled the value will remain at the minimum or maximum value. 

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the value has changed.

See the events of the [Text area](/widgets/core/textarea) too.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_LEFT/RIGHT` With *Keypad* move the cursor left/right. With *Encoder* decrement/increment the selected digit. 
- `LV_KEY_UP/DOWN` With *Keypad* and *Encoder* increment/decrement the value.  
- `LV_KEY_ENTER` With *Encoder* got the net digit. Jump to the first after the last. 

## Example

```eval_rst

.. include:: ../../../examples/widgets/spinbox/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_spinbox.h
  :project: lvgl
        
```
## Example
