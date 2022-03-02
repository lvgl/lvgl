```eval_rst
.. include:: /header.rst
:github_url: |github_link_base|/widgets/core/core/label.md
```
# Label (lv_label)

## Overview
A label is the basic object type that is used to display text.

## Parts and Styles
- `LV_PART_MAIN`  Uses all the typical background properties and the text properties.  The padding values can be used to add space between the text and the background.
- `LV_PART_SCROLLBAR` The scrollbar that is shown when the text is larger than the widget's size.
- `LV_PART_SELECTED` Tells the style of the [selected text](#text-selection). Only `text_color` and `bg_color` style properties can be used.

## Usage

### Set text
You can set the text on a label at runtime with `lv_label_set_text(label, "New text")`.
This will allocate a buffer dynamically, and the provided string will be copied into that buffer.
Therefore, you don't need to keep the text you pass to `lv_label_set_text` in scope after that function returns.

With `lv_label_set_text_fmt(label, "Value: %d", 15)` printf formatting can be used to set the text.

Labels are able to show text from a static character buffer.  To do so, use `lv_label_set_text_static(label, "Text")`.
In this case, the text is not stored in the dynamic memory and the given buffer is used directly instead.
This means that the array can't be a local variable which goes out of scope when the function exits.
Constant strings are safe to use with `lv_label_set_text_static` (except when used with `LV_LABEL_LONG_DOT`, as it modifies the buffer in-place), as they are stored in ROM memory, which is always accessible.

### Newline

Newline characters are handled automatically by the label object. You can use `\n` to make a line break. For example: `"line1\nline2\n\nline4"`

### Long modes
By default, the width and height of the label is set to `LV_SIZE_CONTENT`. Therefore, the size of the label is automatically expanded to the text size.
Otherwise, if the width or height are explicitly set (using  e.g.`lv_obj_set_width` or a layout), the lines wider than the label's width can be manipulated according to several long mode policies.
Similarly, the policies can be applied if the height of the text is greater than the height of the label.
- `LV_LABEL_LONG_WRAP` Wrap too long lines. If the height is `LV_SIZE_CONTENT` the label's height will be expanded, otherwise the text will be clipped. (Default)
- `LV_LABEL_LONG_DOT` Replaces the last 3 characters from bottom right corner of the label with dots (`.`)
- `LV_LABEL_LONG_SCROLL` If the text is wider than the label scroll it horizontally back and forth. If it's higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- `LV_LABEL_LONG_SCROLL_CIRCULAR` If the text is wider than the label scroll it horizontally continuously. If it's higher, scroll vertically. Only one direction is scrolled and horizontal scrolling has higher precedence.
- `LV_LABEL_LONG_CLIP` Simply clip the parts of the text outside the label.

You can specify the long mode with `lv_label_set_long_mode(label, LV_LABEL_LONG_...)`

Note that `LV_LABEL_LONG_DOT` manipulates the text buffer in-place in order to add/remove the dots.
When `lv_label_set_text` or `lv_label_set_array_text` are used, a separate buffer is allocated and this implementation detail is unnoticed.
This is not the case with `lv_label_set_text_static`. The buffer you pass to `lv_label_set_text_static` must be writable if you plan to use `LV_LABEL_LONG_DOT`.

### Text recolor
In the text, you can use commands to recolor parts of the text. For example: `"Write a #ff0000 red# word"`.
This feature can be enabled individually for each label by `lv_label_set_recolor()` function.

### Text selection
If enabled by `LV_LABEL_TEXT_SELECTION` part of the text can be selected. It's similar to when you use your mouse on a PC to select a text.
The whole mechanism (click and select the text as you drag your finger/mouse) is implemented in [Text area](/widgets/core/textarea) and the Label widget only allows manual text selection with
`lv_label_get_text_selection_start(label, start_char_index)` and `lv_label_get_text_selection_start(label, end_char_index)`.

### Very long texts
LVGL can efficiently handle very long (e.g. > 40k characters) labels by saving some extra data (~12 bytes) to speed up drawing. To enable this feature, set `LV_LABEL_LONG_TXT_HINT   1` in `lv_conf.h`.

### Custom scrolling animations
Some aspects of the scrolling animations in long modes `LV_LABEL_LONG_SCROLL` and `LV_LABEL_LONG_SCROLL_CIRCULAR` can be customized by setting the animation property of a style, using `lv_style_set_anim()`.
Currently, only the start and repeat delay of the circular scrolling animation can be customized. If you need to customize another aspect of the scrolling animation, feel free to open an [issue on Github](https://github.com/lvgl/lvgl/issues) to request the feature.

### Symbols
The labels can display symbols alongside letters (or on their own). Read the [Font](/overview/font) section to learn more about the symbols.

## Events
No special events are sent by the Label.

See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/label/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_label.h
  :project: lvgl

```

