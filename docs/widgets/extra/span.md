```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/span.md
```
# Span (lv_span)

## Overview

A spangroup is the object that is used to display rich text. different from the label object, `spangroup` can automatically organize text of different fonts, colors, and sizes into the spangroup obj.

## Parts and Styles
- `LV_PART_MAIN` The spangroup has only the part.
 
## Usage

### Set text and style

spangroup object uses span to describe text and text style. so, first we need to create `span` descriptor use function `lv_span_t * span = lv_spangroup_new_span(spangroup)`.then use `lv_span_set_text(span, "text")` to set text.The style of the modified text is the same as the normal style used,eg:`lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED))`.

If spangroup object `mode != LV_SPAN_MODE_FIXED`.You must call `lv_spangroup_refr_mode()` after you have modified `span` style(eg:set text, changed the font size, del span).

### Text align
like label object, The spangroup can be one the following modes:
- `LV_TEXT_ALIGN_LEFT` Align text to left.
- `LV_TEXT_ALIGN_CENTER` Align text to center.
- `LV_TEXT_ALIGN_RIGHT` Align text to right.
- `LV_TEXT_ALIGN_AUTO` Align text auto.

use function `lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_CENTER)` to set text align.

### Modes
The spangroup can be one the following modes:
- `LV_SPAN_MODE_FIXED` fixed the obj size.
- `LV_SPAN_MODE_EXPAND` Expand the object size to the text size. only one line.
- `LV_SPAN_MODE_BREAK` Keep width, break the too long lines and auto expand height.

use function `lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK)` to set obj mode.

### Overflow
The spangroup can be one the following modes:
- `LV_SPAN_OVERFLOW_CLIP` truncate the text at the limit of the area.
- `LV_SPAN_OVERFLOW_ELLIPSIS` This mode value will display an ellipsis(`...`) when text overflow the area.

use function `lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_CLIP)` to set obj Overflow.

### first line indent
use function `lv_spangroup_set_indent(spangroup, 20)` to set text indent of first line.

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
