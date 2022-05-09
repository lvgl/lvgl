# Text area (lv_textarea)

## Overview

The Text Area is a [Base object](widgets/obj) with a [Label](/widgets/core/label) and a cursor on it.
Texts or characters can be added to it.
Long lines are wrapped and when the text becomes long enough the Text area can be scrolled.

One line mode and password modes are supported.

## Parts and Styles
- `LV_PART_MAIN` The background of the text area. Uses all the typical background style properties and the text related style properties including `text_align` to align the text to the left, right or center.
- `LV_PART_SCROLLBAR` The scrollbar that is shown when the text is too long.
- `LV_PART_SELECTED` Determines the style of the [selected text](/widgets/core/label.html#text-selection). Only `text_color` and `bg_color` style properties can be used. `bg_color` should be set directly on the label of the text area.
- `LV_PART_CURSOR` Marks the position where the characters are inserted. The cursor's area is always the bounding box of the current character.
A block cursor can be created by adding a background color and background opacity to `LV_PART_CURSOR`'s style. The create line cursor leave the cursor transparent and set a left border.
The `anim_time` style property sets the cursor's blink time.
- `LV_PART_TEXTAREA_PLACEHOLDER` Unique to Text Area, allows styling the placeholder text.

## Usage

### Add text

You can insert text or characters to the current cursor's position with:

- `lv_textarea_add_char(textarea, 'c')`
- `lv_textarea_add_text(textarea, "insert this text")`

To add wide characters like `'á'`, `'ß'` or CJK characters use `lv_textarea_add_text(ta, "á")`.

`lv_textarea_set_text(ta, "New text")` changes the whole text.

### Placeholder

A placeholder text can be specified - which is displayed when the Text area is empty - with `lv_textarea_set_placeholder_text(ta, "Placeholder text")`

### Delete character

To delete a character from the left of the current cursor position use `lv_textarea_del_char(textarea)`.
To delete from the right use `lv_textarea_del_char_forward(textarea)`

### Move the cursor

The cursor position can be modified directly like `lv_textarea_set_cursor_pos(textarea, 10)`.
The `0` position means "before the first characters",
`LV_TA_CURSOR_LAST` means "after the last character"

You can step the cursor with
- `lv_textarea_cursor_right(textarea)`
- `lv_textarea_cursor_left(textarea)`
- `lv_textarea_cursor_up(textarea)`
- `lv_textarea_cursor_down(textarea)`

If `lv_textarea_set_cursor_click_pos(textarea, true)` is applied the cursor will jump to the position where the Text area was clicked.

### Hide the cursor
The cursor is always visible, however it can be a good idea to style it to be visible only in `LV_STATE_FOCUSED` state.

### One line mode
The Text area can be configured to be on a single line with `lv_textarea_set_one_line(textarea, true)`.
In this mode the height is set automatically to show only one line, line break characters are ignored, and word wrap is disabled.

### Password mode
The text area supports password mode which can be enabled with `lv_textarea_set_password_mode(textarea, true)`.

If the `•` ([Bullet, U+2022](http://www.fileformat.info/info/unicode/char/2022/index.htm)) character exists in the font, the entered characters are converted to it after some time or when a new character is entered.
If `•` not exists, `*` will be used.

In password mode `lv_textarea_get_text(textarea)` returns the actual text entered, not the bullet characters.

The visibility time can be adjusted with `LV_TEXTAREA_DEF_PWD_SHOW_TIME)` in `lv_conf.h`.

### Accepted characters
You can set a list of accepted characters with `lv_textarea_set_accepted_chars(textarea, "0123456789.+-")`.
Other characters will be ignored.

### Max text length
The maximum number of characters can be limited with `lv_textarea_set_max_length(textarea, max_char_num)`


### Very long texts
If there is a very long text in the Text area (e.g. > 20k characters), scrolling and drawing might be slow.
However, by enabling `LV_LABEL_LONG_TXT_HINT   1` in `lv_conf.h` the performance can be hugely improved.
This will save some additional information about the label to speed up its drawing.
Using `LV_LABEL_LONG_TXT_HINT` the scrolling and drawing will as fast as with "normal" short texts.

### Select text
Any part of the text can be selected if enabled with `lv_textarea_set_text_selection(textarea, true)`.
This works much like when you select text on your PC with your mouse.

## Events
- `LV_EVENT_INSERT` Sent right before a character or text is inserted.
The event parameter is the text about to be inserted. `lv_textarea_set_insert_replace(textarea, "New text")` replaces the text to insert.
The new text cannot be in a local variable which is destroyed when the event callback exists. `""` means do not insert anything.
- `LV_EVENT_VALUE_CHANGED` Sent when the content of the text area has been changed.
- `LV_EVENT_READY` Sent when `LV_KEY_ENTER` is pressed (or sent) to a one line text area.

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
- `LV_KEY_UP/DOWN/LEFT/RIGHT` Move the cursor
- `Any character` Add the character to the current cursor position

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/textarea/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_textarea.h
  :project: lvgl

```
