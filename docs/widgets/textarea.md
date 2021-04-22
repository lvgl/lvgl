```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/textarea.md
```
# Text area (lv_textarea)

## Overview

The Text Area is a [Page](/widgets/page) with a [Label](/widgets/label) and a cursor on it. 
Texts or characters can be added to it. 
Long lines are wrapped and when the text becomes long enough the Text area can be scrolled.

## Parts and Styles

The Text area has the same parts as [Page](/widgets/page). 
Expect `LV_PAGE_PART_SCRL` because it can't be referenced and it's always transparent.
Refer the Page's documentation of details.

Besides the Page parts the virtual `LV_TEXTAREA_PART_CURSOR` part exists to draw the cursor. 
The cursor's area is always the bounding box of the current character. 
A block cursor can be created by adding a background color and background opa to `LV_TEXTAREA_PART_CURSOR`'s style.
The create line cursor let the cursor transparent and set the *border_side* property.

## Usage

### Add text

You can insert text or characters  to the current cursor's position with:

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

The cursor position can be modified directly with `lv_textarea_set_cursor_pos(textarea, 10)`. 
The `0` position means "before the first characters", 
`LV_TA_CURSOR_LAST` means "after the last character"

You can step the cursor with
- `lv_textarea_cursor_right(textarea)`
- `lv_textarea_cursor_left(textarea)`
- `lv_textarea_cursor_up(textarea)`
- `lv_textarea_cursor_down(textarea)`

If `lv_textarea_set_cursor_click_pos(textarea, true)` is called the cursor will jump to the position where the Text area was clicked.

### Hide the cursor
The cursor can be hidden with `lv_textarea_set_cursor_hidden(textarea, true)`. 


### Cursor blink time
The blink time of the cursor can be adjusted with `lv_textarea_set_cursor_blink_time(textarea, time_ms)`.

### One line mode
The Text area can be configures to be one lined with `lv_textarea_set_one_line(ta, true)`. 
In this mode the height is set automatically to show only one line, line break character are ignored, and word wrap is disabled. 

### Password mode
The text area supports password mode which can be enabled with `lv_textarea_set_pwd_mode(textarea, true)`. 

If the `•` ([Bullet, U+2022](http://www.fileformat.info/info/unicode/char/2022/index.htm)) character exists in the font, the entered characters are converted to it after some time or when a new character is entered. 
If `•` not exists, `*` will be used.

In password mode `lv_textarea_get_text(textarea)` gives the real text, not the bullet characters.

The visibility time can be adjusted with `lv_textarea_set_pwd_show_time(textarea, time_ms)`.

### Text align
The text can be aligned to the left, center or right with `lv_textarea_set_text_align(textarea, LV_LABEL_ALIGN_LET/CENTER/RIGHT)`.

In one line mode, the text can be scrolled horizontally only if the text is left aligned.

### Accepted characters
You can set a list of accepted characters with `lv_textarae_set_accepted_chars(ta, "0123456789.+-")`. 
Other characters will be ignored. 

### Max text length
The maximum number of characters can be limited with `lv_textarea_set_max_length(textarea, max_char_num)`

### Very long texts
If there is a  very long text in the Text area  (e. g. > 20k characters) its scrolling and drawing might be slow. 
However, by enabling `LV_LABEL_LONG_TXT_HINT   1` in *lv_conf.h* it can be hugely improved. 
It will save some info about the label to speed up its drawing. 
Using `LV_LABEL_LONG_TXT_HINT` the scrolling and drawing will as fast as with "normal" short texts.

### Select text
A part of text can be selected if enabled with `lv_textarea_set_text_sel(textarea, true)`. 
It works like when you select a text on your PC with your mouse. 

### Scrollbars
The scrollbars can shown according to different policies set by `lv_textarea_set_scrollbar_mode(textarea, LV_SCRLBAR_MODE_...)`. 
Learn more at the [Page](/widgets/page) object.

### Scroll propagation
When the Text area is scrolled on an other scrollable object (like a Page) and the scrolling has reached the edge of the Text area, the scrolling can be propagated to the parent. 
In other words, when the Text area can be scrolled further, the parent will be scrolled instead.

It can be enabled with `lv_ta_set_scroll_propagation(ta, true)`.

Learn more at the [Page](/widgets/page) object.

### Edge flash
When the Text area is scrolled to edge a circle like flash animation can be shown if it is enabled with `lv_ta_set_edge_flash(ta, true)`

## Events
Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Slider:
- **LV_EVENT_INSERT** Sent when before a character or text is inserted. 
The event data is the text planned to insert. `lv_ta_set_insert_replace(ta, "New text")` replaces the text to insert. 
The new text can't be in a local variable which is destroyed when the event callback exists. `""` means do not insert anything.
- **LV_EVENT_VALUE_CHANGED** When the content of the text area has been changed. 
- **LV_EVENT_APPLY** When LV_KEY_ENTER is sent to a text area which is in one line mode.

## Keys
- **LV_KEY_UP/DOWN/LEFT/RIGHT** Move the cursor
- **Any character** Add the character to the current cursor position

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_textarea/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_textarea.h
  :project: lvgl
        
```
