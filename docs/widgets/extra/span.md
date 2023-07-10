# Span (lv_span)

## Overview

A spangroup is the object that is used to display rich text. Different from the label object, `spangroup` can render text styled with different fonts, colors, and sizes into the spangroup object.

## Parts and Styles
- `LV_PART_MAIN` The spangroup has only one part.

## Usage

### Set text and style

The spangroup object uses span to describe text and text style. so, first we need to create `span` descriptor using `lv_span_t * span = lv_spangroup_new_span(spangroup)`. Then use `lv_span_set_text(span, "text")` to set text. The style of the span is configured as with a normal style object by using its `style` member, eg:`lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED))`.

If spangroup object `mode != LV_SPAN_MODE_FIXED` you must call `lv_spangroup_refr_mode()` after you have modified `span` style(eg:set text, changed the font size, del span).

### Retrieving a span child
Spangroups store their children differently from normal objects, so normal functions for getting children won't work.

`lv_spangroup_get_child(spangroup, id)` will return a pointer to the child span at index `id`. In addition, `id` can be negative to index from the end of the spangroup where `-1` is the youngest child, `-2` is second youngest, etc.

e.g. `lv_span_t* span = lv_spangroup_get_child(spangroup, 0)` will return the first child of the spangroup. `lv_span_t* span = lv_spangroup_get_child(spangroup, -1)` will return the last (or most recent) child.

### Child Count
Use the function `lv_spangroup_get_child_cnt(spangroup)` to get back the number of spans the group is maintaining.

e.g. `uint32_t size = lv_spangroup_get_child_cnt(spangroup)`

### Text align
like label object, the spangroup can be set to one the following modes:
- `LV_TEXT_ALIGN_LEFT` Align text to left.
- `LV_TEXT_ALIGN_CENTER` Align text to center.
- `LV_TEXT_ALIGN_RIGHT` Align text to right.
- `LV_TEXT_ALIGN_AUTO` Align text auto.

use function `lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_CENTER)` to set text align.

### Modes
The spangroup can be set to one the following modes:
- `LV_SPAN_MODE_FIXED` fixes the object size.
- `LV_SPAN_MODE_EXPAND` Expand the object size to the text size but stay on a single line.
- `LV_SPAN_MODE_BREAK` Keep width, break the too long lines and auto expand height.

Use `lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK)` to set object mode.

### Overflow
The spangroup can be set to one the following modes:
- `LV_SPAN_OVERFLOW_CLIP` truncates the text at the limit of the area.
- `LV_SPAN_OVERFLOW_ELLIPSIS` will display an ellipsis(`...`) when text overflows the area.

Use `lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP)` to set object overflow mode.

### first line indent
Use `lv_spangroup_set_indent(spangroup, 20)` to set the indent of the first line. all modes support pixel units, in addition to LV_SPAN_MODE_FIXED and LV_SPAN_MODE_BREAK mode supports percentage units too.

### lines
Use `lv_spangroup_set_lines(spangroup, 10)` to set the maximum number of lines to be displayed in LV_SPAN_MODE_BREAK mode, negative values indicate no limit.

## Events
No special events are sent by this widget.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/span/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_span.h
  :project: lvgl

```
