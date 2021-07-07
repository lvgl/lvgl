```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/extra/spinner.md
```
# Spinner (lv_spinner)

## Overview
The Spinner object is a spinning arc over a ring. 

## Parts and Styles
The parts are identical to the parts of [lv_arc](/widgets/core/arc).

## Usage

### Create a spinner

To create a spinner use `lv_spinner_create(parent, spin_time, arc_length)`. `spin time` sets the spin time in milliseconds, `arc_length` sets the length of the spinning arc in degrees.

## Events
No special events are sent the the Spinner.

See the events of the [Arc](/widgets/core/arc) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).



## Example

```eval_rst

.. include:: ../../../examples/widgets/spinner/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_spinner.h
  :project: lvgl
        
```
