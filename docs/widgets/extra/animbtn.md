```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/animbtn.md
```
# Image button (lv_animbtn)

## Overview

The Animation button is very similar to the simple 'Button' object. The only difference is that it displays selected part of an animation in each state instead of drawing a rectangle.

Typically used with a unique [lv_rlottie_t](libs/rlottie.md) animation containing all the states of the button.



## Parts and Styles
None. It's a wrapper over a [lv_rlottie_t](libs/rlottie.md) object

## Usage

### Animation source
You'll construct this button with `lv_animbtn_create(parent, lottie)`.
The `lottie` instance is constructed as usual (for example like this: `lv_obj_t * lottie = lv_rlottie_create_from_file(parent, 64, 64, "test.json")`).
It's orphaned and attached as a child of this button so the given parent is not important. 

Then, to set the button's states from the animation, you'll have to build a `lv_animbtn_state_desc_t` containing the first frame to play when in the state, the last frame to play and the `control` mode for this animation state (forward, backward, with or without looping), see `lv_rlottie_ctrl_t`).

Then attach your state descriptor to a state with `lv_animbtn_set_state_desc(btn, state, desc)`.
By default, state descriptors are invalid and not used.

The possible states are:
- `LV_ANIMBTN_STATE_RELEASED`
- `LV_ANIMBTN_STATE_PRESSED`
- `LV_ANIMBTN_STATE_DISABLED`
- `LV_ANIMBTN_STATE_CHECKED_RELEASED`
- `LV_ANIMBTN_STATE_CHECKED_PRESSED`
- `LV_ANIMBTN_STATE_CHECKED_DISABLED`

If you set a descriptor only in `LV_ANIMBTN_STATE_RELEASED`, this descriptor will be used in other states too. 
If you set e.g. `LV_ANIMBTN_STATE_PRESSED` they will be used in pressed state instead of the released images.


### States
Instead of the regular `lv_obj_add/clear_state()` functions the `lv_animbtn_set_state(imgbtn, LV_ANIMBTN_STATE_...)` functions should be used to manually set a state.


## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the button is toggled.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP`  Go to toggled state if `LV_OBJ_FLAG_CHECKABLE` is enabled.
- `LV_KEY_LEFT/DOWN`  Go to non-toggled state if `LV_OBJ_FLAG_CHECKABLE` is enabled.
- `LV_KEY_ENTER` Clicks the button


Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/animbtn/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_animbtn.h
  :project: lvgl

```
