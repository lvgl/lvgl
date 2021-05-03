```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/label.md
```
# Label (lv_label)

## Overview
A label is the basic object type that is used to display text. 

## Parts and Styles
The label has only a main part, called `LV_LABEL_PART_MAIN`. It uses all the typical background properties and the *text* properties. 
The padding values can be used to make the area for the text small in the related direction.

## Usage

### Set text
You can set the text on a label at runtime with `lv_label_set_text(label, "New text")`. 
It will allocate a buffer dynamically, and the provided string will be copied into that buffer. 
Therefore, you don't need to keep the text you pass to `lv_label_set_text` in scope after that function returns.

With `lv_label_set_text_fmt(label, "Value: %d", 15)` **printf formatting** can be used to set the text.

Labels are able to show text from a **static character buffer** which is `\0`-terminated. 
To do so, use `lv_label_set_text_static(label, "Text")`. 
In this case, the text is not stored in the dynamic memory and the given buffer is used directly instead. 
This means that the array can't be a local variable which goes out of scope when the function exits. 
Constant strings are safe to use with `lv_label_set_text_static` (except when used with `LV_LABEL_LONG_DOT`, as it modifies the buffer in-place), as they are stored in ROM memory, which is always accessible.


### Line break

Line breaks are handled automatically by the label object. You can use `\n` to make a line break. For example: `"line1\nline2\n\nline4"`

### Long modes
By default, the width of the label object automatically expands to the text size. Otherwise, the text can be manipulated according to several long mode policies:

* **LV_LABEL_LONG_EXPAND** - Expand the object size to the text size (Default)
* **LV_LABEL_LONG_BREAK** - Keep the object width, break (wrap) the too long lines and expand the object height
* **LV_LABEL_LONG_DOT** - Keep the object size, break the text and write dots in the last line (**not supported when using `lv_label_set_text_static`**)
* **LV_LABEL_LONG_SROLL** - Keep the size and scroll the label back and forth
* **LV_LABEL_LONG_SROLL_CIRC** - Keep the size and scroll the label circularly
* **LV_LABEL_LONG_CROP** - Keep the size and crop the text out of it

You can specify the long mode with `lv_label_set_long_mode(label, LV_LABEL_LONG_...)`

It's important to note that, when a label is created and its text is set, the label's size already expanded to the text size. 
In addition with the default `LV_LABEL_LONG_EXPAND`, *long mode* `lv_obj_set_width/height/size()` has no effect.

So you need to change the *long mode* first set the new *long mode* and then set the size with  `lv_obj_set_width/height/size()`.

Another important note is that **`LV_LABEL_LONG_DOT` manipulates the text buffer in-place** in order to add/remove the dots. 
When `lv_label_set_text` or `lv_label_set_array_text` are used, a separate buffer is allocated and this implementation detail is unnoticed. 
This is not the case with `lv_label_set_text_static`! **The buffer you pass to `lv_label_set_text_static` must be writable if you plan to use `LV_LABEL_LONG_DOT`.**

### Text align

The lines of the text can be aligned to the left, right or center with `lv_label_set_align(label, LV_LABEL_ALIGN_LEFT/RIGHT/CENTER)`. Note that, it will align only the lines, not the label object itself.

Vertical alignment is not supported by the label itself; you should place the label inside a larger container and align the whole label object instead.

### Text recolor
In the text, you can use commands to recolor parts of the text. For example: `"Write a #ff0000 red# word"`. 
This feature can be enabled individually for each label by `lv_label_set_recolor()` function. 

Note that, recoloring work only in a single line. Therefore, `\n` should not use in a recolored text or it should be wrapped by `LV_LABEL_LONG_BREAK` else, the text in the new line won't be recolored.

### Very long texts

Lvgl can efficiently handle very long (> 40k characters) by saving some extra data (~12 bytes) to speed up drawing. To enable this feature, set `LV_LABEL_LONG_TXT_HINT   1` in *lv_conf.h*.

### Symbols
The labels can display symbols alongside letters (or on their own). Read the [Font](/overview/font) section to learn more about the symbols.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_label/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_label.h
  :project: lvgl
        
```

