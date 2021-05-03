```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/btn.md
```
# Button (lv_btn)

## Overview

Buttons has no new features compared to the [Base object](/widgets/obj). It usufule for semantic purposes and has slightly different default settings.

Buttons differ from Base object in the following points by default:
- Not scrollable
- Added to the default group
- Its default height and width is `LV_SIZE_CONTENT`

## Parts and Styles
- `LV_PART_MAIN` The background of the button. It uses the typical background style properties.

## Usage

There are no new features compared to [Base object](/widgets/obj).

## Events
- `LV_EVENT_VALUE_CHANGED` when the `LV_OBJ_FLAG_CHECKABLE` flag is enabled and the obejct clicked (on transition to/from the checked state)


Learn more about [Events](/overview/event).

## Keys
If `LV_OBJ_FLAG_CHECKABLE` is enabled `LV_KEY_RIGHT` and `LV_KEY_UP` makes the object checked, and `LV_KEY_LEFT` and `LV_KEY_DOWN` makes it unchecked.

Note that, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

Learn more about [Keys](/overview/indev).

## Example
```eval_rst

.. include:: ../../../examples/widgets/btn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_btn.h
  :project: lvgl

```
