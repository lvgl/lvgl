```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/keyboard.md
```


# Keyboard (lv_keyboard)

## Overview

The Keyboard object is a special [Button matrix](/widgets/btnmatrix) with predefined keymaps and other features to realize a virtual keyboard to write text.

## Parts and Styles
Similarly to Button matices Keyboards consist of 2 part: 
- `LV_KEYBOARD_PART_BG` which is the main part and uses all the typical background properties
- `LV_KEYBOARD_PART_BTN` which is virtual part for the buttons. It also uses all typical backround proeprties and the *text* properties.

## Usage

### Modes
The Keyboards have thefollowing modes:
- **LV_KEYBOARD_MODE_TEXT_LOWER** - Display lower case letters
- **LV_KEYBOARD_MODE_TEXT_UPPER** - Display upper case letters
- **LV_KEYBOARD_MODE_TEXT_SPECIAL** - Display special characters
- **LV_KEYBOARD_MODE_NUM** - Display numbers, +/- sign, and decimal dot.

The `TEXT` modes' layout contains buttons to change mode.

To set the mode manually, use `lv_keyboard_set_mode(kb, mode)`. The default more is  `LV_KEYBOARD_MODE_TEXT_UPPER`.

### Assign Text area
You can assign a [Text area](/widgets/textarea) to the Keyboard to automatically put the clicked characters there.
To assign the text area, use `lv_keyboard_set_textarea(kb, ta)`.

The assigned text area's **cursor can be managed** by the keyboard: when the keyboard is assigned, the previous text area's cursor will be hidden and the new one will be shown.
When the keyboard is closed by the *Ok* or *Close* buttons, the cursor also will be hidden. The cursor manager feature is enabled by `lv_keyboard_set_cursor_manage(kb, true)`. The default is not managed.


### New Keymap
You can specify a new map (layout) for the keyboard with `lv_keyboard_set_map(kb, map)` and `lv_keyboard_set_ctrl_map(kb, ctrl_map)`.
Learn more about the [Button matrix](/widgets/btnmatrix) object.
Keep in mind that, using following keywords will have the same effect as with the original map:  
- *LV_SYMBOL_OK* - Apply.
- *LV_SYMBOL_CLOSE* - Close.
- *LV_SYMBOL_BACKSPACE* - Delete on the left.
- *LV_SYMBOL_LEFT* - Move the cursor left.
- *LV_SYMBOL_RIGHT* - Move the cursor right.
- *"ABC"* - Load the uppercase map.
- *"abc"* - Load the lower case map.
- *"Enter"* - New line.

## Events
Besides the [Generic events](../overview/event.html#generic-events), the following [Special events](../overview/event.html#special-events) are sent by the keyboards:
 - **LV_EVENT_VALUE_CHANGED** - Sent when the button is pressed/released or repeated after long press. The event data is set to the ID of the pressed/released button.
 - **LV_EVENT_APPLY** - The *Ok* button is clicked.
 - **LV_EVENT_CANCEL** - The *Close* button is clicked.

The keyboard has a **default event handler** callback called `lv_keyboard_def_event_cb`.
It handles the button pressing, map changing, the assigned text area, etc.
You can completely replace it with your custom event handler however, you can call `lv_keyboard_def_event_cb` at the beginning of your event handler to handle the same things as before.

Learn more about [Events](/overview/event).

## Keys

The following *Keys* are processed by the buttons:
- **LV_KEY_RIGHT/UP/LEFT/RIGHT** - To navigate among the buttons and select one.
- **LV_KEY_ENTER** - To press/release the selected button.

Learn more about [Keys](/overview/indev).


## Examples


```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_keyboard/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_keyboard.h
  :project: lvgl

```
