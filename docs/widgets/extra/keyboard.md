```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/keyboard.md
```


# Keyboard (lv_keyboard)

## Overview

The Keyboard object is a special [Button matrix](/widgets/core/btnmatrix) with predefined keymaps and other features to realize a virtual keyboard to write texts into a [Text area](/widgets/core/textarea).

## Parts and Styles
Similarly to Button matrices Keyboards consist of 2 part: 
- `LV_PART_MAIN` The main part and uses all the typical background properties
- `LV_PART_ITEMS` The buttons and it also uses all typical background properties and the *text* properties.

## Usage

### Modes
The Keyboards have the following modes:
- `LV_KEYBOARD_MODE_TEXT_LOWER`  Display lower case letters
- `LV_KEYBOARD_MODE_TEXT_UPPER`  Display upper case letters
- `LV_KEYBOARD_MODE_TEXT_SPECIAL` Display special characters
- `LV_KEYBOARD_MODE_NUM` Display numbers, +/- sign, and decimal dot.

The `TEXT` modes' layout contains buttons to change mode.

To set the mode manually, use `lv_keyboard_set_mode(kb, mode)`. The default mode is  `LV_KEYBOARD_MODE_TEXT_UPPER`.

### Assign Text area
You can assign a [Text area](/widgets/core/textarea) to the Keyboard to automatically put the clicked characters there.
To assign the text area, use `lv_keyboard_set_textarea(kb, ta)`.


### New Keymap
You can specify a new map (layout) for the keyboard with `lv_keyboard_set_map(kb, map)` and `lv_keyboard_set_ctrl_map(kb, ctrl_map)`.
Learn more about the [Button matrix](/widgets/core/btnmatrix) object.
Keep in mind that, using following keywords will have the same effect as with the original map:  
- `LV_SYMBOL_OK` Apply.
- `LV_SYMBOL_CLOSE` or `LV_SYMBOL_KEYBOARD` Close.
- `LV_SYMBOL_BACKSPACE` Delete on the left.
- `LV_SYMBOL_LEFT` Move the cursor left.
- `LV_SYMBOL_RIGHT` Move the cursor right.
- `LV_SYMBOL_NEW_LINE` New line.
- *"ABC"* Load the uppercase map.
- *"abc"* Load the lower case map.
- *"1#"* Load the lower case map.

## Events
- `LV_EVENT_VALUE_CHANGED` Sent when the button is pressed/released or repeated after long press. The event data is set to the ID of the pressed/released button.
- `LV_EVENT_READY` - The *Ok* button is clicked.
- `LV_EVENT_CANCEL` - The *Close* button is clicked.

The keyboard has a **default event handler** callback called `lv_keyboard_def_event_cb`.
It handles the button pressing, map changing, the assigned text area, etc.
You can remove it and replace it with a custom event handler if you wish.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_RIGHT/UP/LEFT/RIGHT` To navigate among the buttons and select one.
- `LV_KEY_ENTER` To press/release the selected button.

Learn more about [Keys](/overview/indev).


## Examples


```eval_rst

.. include:: ../../../examples/widgets/keyboard/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_keyboard.h
  :project: lvgl

```
