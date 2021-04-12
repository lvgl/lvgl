```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/checkbox.md
```
# Checkbox (lv_cb)


## Overview

The Checkbox objects are built from a [Button](/widgets/btn) background which contains an also Button *bullet* and a [Label](/widgets/label) to realize a classical checkbox.

## Parts and Styles
The Check box's main part is called `LV_CHECKBOX_PART_BG`. It's a container for a "bullet" and a text next to it. The background uses all the typical background style properties.

The bullet is real [lv_obj](/widgets/obj) object and can be referred with `LV_CHECKBOX_PART_BULLET`. 
The bullet automatically inherits the state of the background. So the background is pressed the bullet goes to pressed state as well.
The bullet also uses all the typical background style properties.

There is not dedicated part for the label. Its styles can be set in the background's styles because the *text* styles properties are always inherited.


## Usage


### Text
The text can be modified by the `lv_checkbox_set_text(cb, "New text")` function. It will dynamically allocate the text.

To set a static text, use `lv_checkbox_set_static_text(cb, txt)`. This way, only a pointer of `txt` will be stored and it shouldn't be deallocated while the checkbox exists.

### Check/Uncheck
You can manually check / un-check the Checkbox  via `lv_checkbox_set_checked(cb, true/false)`. Setting `true` will check the checkbox and `false` will un-check the checkbox.

### Disabled
To make the Checkbox disabled, use `lv_checkbox_set_disabled(cb, true)`.

### Get/Set Checkbox State
You can get the current state of the Checkbox with the `lv_checkbox_get_state(cb)` function which returns the current state.
You can set the current state of the Checkbox with the `lv_checkbox_set_state(cb, state)`.
The available states as defined by the enum `lv_btn_state_t` are:
- **LV_BTN_STATE_RELEASED**
- **LV_BTN_STATE_PRESSED**
- **LV_BTN_STATE_DISABLED**
- **LV_BTN_STATE_CHECKED_RELEASED**
- **LV_BTN_STATE_CHECKED_PRESSED**
- **LV_BTN_STATE_CHECKED_DISABLED**

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Checkboxes:
 - **LV_EVENT_VALUE_CHANGED** - sent when the checkbox is toggled.

Note that, the generic input device-related events (like `LV_EVENT_PRESSED`) are sent in the inactive state too. You need to check the state with `lv_cb_is_inactive(cb)` to ignore the events from inactive Checkboxes.

Learn more about [Events](/overview/event).


## Keys
The following *Keys* are processed by the 'Buttons':
- **LV_KEY_RIGHT/UP** - Go to toggled state if toggling is enabled
- **LV_KEY_LEFT/DOWN** - Go to non-toggled state if toggling is  enabled

Note that, as usual, the state of `LV_KEY_ENTER` is translated to `LV_EVENT_PRESSED/PRESSING/RELEASED` etc.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_checkbox/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_checkbox.h
  :project: lvgl

```
