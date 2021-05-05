```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/switch.md
```

# Switch (lv_switch)

## Overview

The Switch can be used to turn on/off something. It looks like a little slider. 


## Parts and Styles

The Switch uses the the following parts:
- `LV_SWITCH_PART_BG`: main part
- `LV_SWITCH_PART_INDIC`: the indicator (virtual part)
- `LV_SWITCH_PART_KNOB`: the knob (virtual part)

The parts and style works the same as in case of [Slider](/widgets/slider). Read its documentation for a details description.

##Usage

### Change state
The state of the Switch can be changed by clicking on it or by `lv_switch_on(switch, LV_ANIM_ON/OFF)`, `lv_switch_off(switch, LV_ANIM_ON/OFF)` or `lv_switch_toggle(switch, LV_ANOM_ON/OFF)` functions

### Animation time

The time of animations, when the switch changes state, can be adjusted with `lv_switch_set_anim_time(switch, anim_time)`.

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Switch:
- **LV_EVENT_VALUE_CHANGED** Sent when the switch changes state.

## Keys
- **LV_KEY_UP**, **LV_KEY_RIGHT** Turn on the slider
- **LV_KEY_DOWN**, **LV_KEY_LEFT** Turn off the slider

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_switch/index.rst

```


## API 

```eval_rst

.. doxygenfile:: lv_switch.h
  :project: lvgl
        
```
