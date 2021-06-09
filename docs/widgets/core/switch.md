```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/switch.md
```

# Switch (lv_switch)

## Overview

The Switch looks like a little slider and can be used to turn something on and off. 


## Parts and Styles
- `LV_PART_MAIN` The background of the switch uses all the typical background style properties. `padding` makes the indicator smaller in the respective direction.
- `LV_PART_INDICATOR` The indicator that shows the current state of the switch. Also uses all the typical background style properties.
- `LV_PART_KNOB` A rectangle (or circle) drawn at left or right side of the indicator. Also uses all the typical background properties to describe the knob(s). By default the knob is square (with a optional corner radius) with side length equal to the smaller side of the slider. The knob can be made larger with the `padding` values. Padding values can be asymmetric too. 

## Usage

### Change state
When the switch is turned on it goes to `LV_STATE_CHECKED`. To get the current satte of the switch use `lv_obj_has_state(switch, LV_STATE_CHECKED)`. 
To manually turn the switch on/off call `lvobj_add/clear_state(switch, LV_STATE_CHECKED)`.


## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the switch changes state.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_UP/RIGHT` Turns on the slider
- `LV_KEY_DOWN/LEFT` Turns off the slider
- `LV_KEY_ENTER` Toggles the switch 

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/switch/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_switch.h
  :project: lvgl
        
```
