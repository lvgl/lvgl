```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/spinner.md
```
# Spinner (lv_spinner)

## Overview
The Spinner object is a spinning arc over a border. 

## Parts and Styles

The Spinner uses the the following parts:
- `LV_SPINNER_PART_BG`: main part
- `LV_SPINNER_PART_INDIC`: the spinning arc (virtual part)

The parts and style works the same as in case of [Arc](/widgets/arc). Read its documentation for a details description.

## Usage

### Arc length
The length of the arc can be adjusted by `lv_spinner_set_arc_length(spinner, deg)`.

### Spinning speed
The speed of the spinning can be adjusted by `lv_spinner_set_spin_time(preload, time_ms)`.

### Spin types
You can choose from more spin types:
- **LV_SPINNER_TYPE_SPINNING_ARC** spin the arc, slow down on the top
- **LV_SPINNER_TYPE_FILLSPIN_ARC** spin the arc, slow down on the top but also stretch the arc
- **LV_SPINNER_TYPE_CONSTANT_ARC** spin the arc at a constant speed

To apply one if them use `lv_spinner_set_type(preload, LV_SPINNER_TYPE_...)`

### Spin direction
The direction of spinning can be changed with `lv_spinner_set_dir(preload, LV_SPINNER_DIR_FORWARD/BACKWARD)`.


## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).



## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_spinner/index.rst

```

### MicroPython
No examples yet.

## API 

```eval_rst

.. doxygenfile:: lv_spinner.h
  :project: lvgl
        
```
