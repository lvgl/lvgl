# LVGL XML Example Generation Guide

This guide documents all rules and best practices learned through systematic creation of XML examples for Flex, Grid, Label, Arc, and Slider widgets.

---

## Core Principles

### Feature-Based Coverage
- Create one XML example file per feature, not broader topics
- Examples should isolate and demonstrate a single configurable aspect
- Group examples only when direct visual comparison is essential (e.g., arc modes, slider modes)

### Avoid Repetition
- Do not show default values or "baseline then varied" comparisons unless the default illustrates a limitation
- Remove defaults from attributes (e.g., `bg_start_angle`, `bg_end_angle` for arcs when they match defaults)
- Skip redundant demo items; focus on demonstrating the feature difference only

---

## Documentation Structure

### Example File Organization
- **Path**: `examples/widgets/<widget_name>/<feature_name>/lv_example_<widget_name>_<feature_name>.xml`
- **Format**: Always wrapped in `<screen><view>...</view></screen>`
- **Layout**: Use simple, direct placement; avoid container-flow stacking for every demo item unless layout is the subject

### Doc Integration
- Add **inline feature section references** using `<LvglXMLExample name="..." path="..." />`
  - Place these immediately after the section heading or introduction text
  - Example: "### Value and range" followed by the XML example component
- Add **example list entries** using `<LvglExample name="..." path="..." />`
  - Append all new examples to the Examples section at end of widget doc
  - Title format: "### Widget: feature name" (no "XML" wording)

---

## XML Markup Conventions

### Styling
- **Shared styles**: Define `<styles>` block at document root when multiple items reuse the same properties
  - Avoids repetition across similar UI elements
  - Example: grid cells with identical background, padding, radius
- **Style naming**: All style names must be prefixed with `style_` (e.g., `style_cell`, `style_label_bg`)
- **Inline style tags**: Format consistently as 3-line blocks:
  ```xml
  <lv_widget>
    <style name="style_name" />
  </lv_widget>
  ```

### Labels and Text
- **Place labels inside widgets** (not external siblings) when demonstrating widget features
  - Example: `<lv_arc><lv_label align="center" text="normal" /></lv_arc>`
  - Keep labels centered within the widget using `align="center"`
- **Multi-line labels**: Use `&#10;` for newlines within text attributes
- **Label text**: Keep concise; abbreviate when space-constrained (e.g., "rev." for "reverse")

### Comments and Hints
- **Meaningful block comments**: Add `<!-- ... -->` above each logical demo section
  - Describe what the code demonstrates
  - Example: `<!-- Arc with negative knob offset -->`
- **Always include a 💡 hint** at the document's top-level comment, immediately after the opening `<view>`
  - Format: `<!-- 💡 Action/setting to observe and what changes. -->`
  - **Must be specific** to the feature, not generic
  - Example: ✅ "Drag each arc quickly: lower change_rate responds more slowly…"
  - Example: ❌ "Adjust the slider" (too vague)
- **Omit `<!-- Example title -->`** if a 💡 hint already exists; they are redundant

### Flag and Property Usage
- Use lowercase for boolean/enum attribute values (e.g., `mode="normal"`, `orientation="vertical"`, `ignore_layout="true"`)
- Set only non-default properties to keep examples concise
- Verify default values in widget source before including or excluding attributes

---

## Layout Simplification Rules

### Container Avoidance
- Do **not** wrap all demo items in a single `<view flex_flow="column" ...>` unless demonstrating layout itself
- Instead, position items directly with explicit `x`, `y` coordinates
- Reserve container-based stacking for layout feature examples (Flex, Grid)

### Simple Direct Placement
- Use `x` and `y` attributes for standalone positioning
- Group related items logically but keep structure minimal
- Title label can remain at top using full width; demo items follow with specific positions

---

## Example-Specific Rules

### Arc Examples
- **Avoid defaults**: Remove `bg_start_angle="135"` and `bg_end_angle="45"` unless the example explicitly deals with angles
- **Place labels inside**: Use `<lv_label align="center" text="..." />` as a child of each arc
- **Mode comparison**: Group normal, reverse, symmetrical in one file for visual comparison
- **Show variation, not repetition**: Don't show "default arc vs. arc with feature"; show two different feature values instead
  - Example: `knob_offset="-20"` vs. `knob_offset="24"` (not `0` vs. `24`)

### Slider Examples
- **Range modes**: Symmetrical requires negative+positive range; range mode uses both `left_value` and `value`
- **Orientation**: Use `orientation="vertical"` for explicit vertical; width/height can auto-detect
- **Direction reversal**: Swap min/max to reverse direction (e.g., `min_value="100" max_value="0"`)
- **Embed labels**: Place label inside slider using `align="center"` for consistency

### Flex/Grid Examples
- **Max item count**: Grid examples should have ≤ 6 items to keep visual simple
- **Grid property names**: Use `style_layout="grid"` (not `layout`)
- **Ignore layout**: Use `ignore_layout="true"` to exclude items from layout flow

### Label Examples
- **Long mode**: Consolidate all long-mode variants in one example file
- **Recolor syntax**: `#RRGGBB color# text# #RRGGBB ...` format
- **Text selection**: Use `text_selection_start` and `text_selection_end` attributes (integer indices)

---

## Documentation Best Practices

### Section Placement in Docs
- Inline examples (`LvglXMLExample`) go in **Usage** section under the relevant subsection
- Full example list entries (`LvglExample`) go in the **Examples** section at document end
- Both ensure users see quick reference in feature context and full reference in examples list

### Example Naming in Docs
- Titles: "Widget: feature name" format (no "XML" in title)
- Example: "### Slider: modes" or "### Arc: knob offset"

---

## Validation Checklist

Before finalizing examples:
- ✅ No default values are set unnecessarily
- ✅ All hints are specific to the feature, not generic
- ✅ Style names are prefixed with `style_`
- ✅ Labels are inside (not outside) widgets
- ✅ Comments exist above meaningful blocks
- ✅ No `<!-- Example title -->` when 💡 hint is present
- ✅ Simple layout: minimal use of containers
- ✅ Inline references added to docs feature sections
- ✅ Example list entries added to Examples section
- ✅ Files compile without errors (diagnostic check)

---

## Summary

The goal is to create **focused, pedagogical XML examples** that:
1. Demonstrate one feature per file
2. Avoid repetitive content and default values
3. Include specific, actionable hints
4. Use clean, readable XML with meaningful comments
5. Integrate seamlessly into widget documentation both inline and in example lists
6. Prioritize clarity and simplicity over comprehensiveness
