# Writing LVGL Examples

How to author the examples that appear in the widget documentation. Read this
end to end before adding or changing an example — following it produces
examples consistent with the ones already in this tree.

---

## Two kinds of example

| Kind | Source | Lives in | Surfaced by | Use it for |
|------|--------|----------|-------------|------------|
| **XML** (preferred) | `lv_example_<widget>_<feature>.xml` | `<widget>/<feature>/` (own folder) | `<LvglXMLExample>` | Anything declarative: layout, styling, properties, data binding |
| **C-only** | `lv_example_<widget>_<feature>.c` | `<widget>/` (directly, no subfolder) | `<LvglExample>` | Only what XML cannot express: event callbacks, animations, draw events, custom widgets |

Default to XML. Reach for a C-only example **only** when the point of the
example is imperative C the XML layer has no syntax for. Most widgets need
zero or one C-only example (an `_event` handler).

The XML layer is **declarative** — anything that needs imperative C or a
C-built descriptor has no XML form, so author it C-only. Recurring cases:

- C-built descriptors / arrays: point arrays (`lv_line`), gradient
  descriptors (`lv_grad_conical/radial/linear_init`), style transition
  descriptors, theme APIs (`lv_theme_*`).
- OR-ed flag-enum attribute values — only a single token is accepted
  (e.g. `border_side="full"`, not `"bottom|right"`). Selectors are the
  exception: they still take `|` (`selector="pressed|knob"`).
- Property groups the editor doesn't expose yet, and gradients beyond
  the simple `bg_grad_*` horizontal/vertical form.

When in doubt, generate and check the `.c`: if the feature didn't
survive the round-trip, it belongs in a C-only example.

### C-first

The docs target **LVGL Open**, the C library. `<LvglXMLExample>` renders the
**C** tab first, with XML as a secondary tab. So:

- Every prose sentence, API reference, and code snippet in the `.mdx` is
  written for the C reader.
- Mention XML only when it adds something C didn't already show (e.g. the
  `bind_text-fmt` attribute as a one-line aside).
- The XML *runtime* (`lv_xml`) is a private LVGL Pro component. Do **not**
  link to internal XML headers or in-repo XML syntax docs. The only public
  reference for XML syntax and the editor is <https://lvgl.io/docs/pro>.

---

## The pipeline

You write `.xml`; a script produces the shipped `.c`. Never hand-edit a
generated `.c`.

```
python scripts/generate_examples.py [path ...]      # all examples if no path
```

[`scripts/generate_examples.py`](../scripts/generate_examples.py) drives the
LVGL Pro editor CLI over [`examples/xml_project/`](xml_project/):

1. Copies your `lv_example_foo.xml` into `xml_project/screens/`.
2. Runs the editor to generate `lv_example_foo_gen.c`.
3. Copies it back beside the XML as `lv_example_foo.c`.
4. Runs the [`cleanup_examples.py`](../scripts/cleanup_examples.py)
   transformations in-process.
5. Rewrites the topic header `lv_example_<topic>.h` from the `.c` files on
   disk (XML- and C-only prototypes both, no hand-editing).

Run it after **every** new or edited XML, then build to confirm it compiles.
The script is idempotent — safe to re-run on already-processed output.

`cleanup_examples.py` is what makes generator output look hand-written: it
strips boilerplate, collapses width/height → `lv_obj_set_size`, roots the
tree at `lv_screen_active()`, renames the entry function, reduces includes to
a single `lvgl.h`, turns the top-of-file XML comment into a doxygen
`@title`/`@brief` block, maps per-element XML comments to `/* */` lines, and
initialises any `globals.xml` subjects/images the example references.

### `xml_project/`

[`xml_project/`](xml_project/) is the minimal project the generator runs
against. Two files matter when authoring:

- [`project.xml`](xml_project/project.xml) — declares a single **320×240**
  target. **Every example must fit 320×240.** Size widgets accordingly.
- [`globals.xml`](xml_project/globals.xml) — the shared subjects, consts,
  styles, and images every example may reference. **Reuse these; never invent
  per-example subjects.** Generic subjects available:
  - `subject_value` (int 0..100, =50), `subject_value2` (int 0..100, =20)
  - `subject_opa` (int 0..255, =128), `subject_index` (int, =0)
  - `subject_flag` (int, =0), `subject_text` (string, ="Hello")

  Add to `globals.xml` only if none fits, and explain why in its comment.

---

## File layout & naming

```
examples/<category>/<widget>/<feature>/lv_example_<widget>_<feature>.xml   ← XML
examples/<category>/<widget>/lv_example_<widget>_<feature>.c               ← C-only
```

- One example = one feature = one file. Don't bundle unrelated aspects.
- XML feature folder name == file stem minus `lv_example_`.
- Generated XML entry point: `void lv_example_<stem>_create(void);`
- C-only entry point: `void lv_example_<stem>(void);` (no `_create`).

When a topic has no widget, the same rule still holds: XML examples go
in a per-feature subfolder (→ `_create`, `<LvglXMLExample>`), C-only
examples sit **flat** in the topic folder (plain name, `<LvglExample>`).
The flat placement is what makes the topic-header generator emit the
plain `(void)` name a C-only `<LvglExample>` expects.

---

## XML authoring

### Skeleton

```xml
<!--
 @title <Short capitalized title — mirrors the on-screen label, no "XML">
 @brief <One sentence, ≤80 chars, ending with a period.>

 <2–3 sentences: name the specific attributes/values that vary, and why
 that variation is interesting (the visible or behavioral difference).
 Self-contained — readable without the doc page. Hard-wrap ~90 cols.>
-->
<screen>
	<consts>
		<color name="text_color" value="0x013992" />
	</consts>
	<view flex_flow="column" style_flex_cross_place="center" style_pad_row="16">
		<!-- 💡 What to do and what changes as a result. Specific to this feature. -->
		<lv_label width="100%" style_text_align="center" text="Widget: feature" />

		<!-- What this block demonstrates -->
		<lv_widget ...>
			<lv_label align="center" text="..." />
		</lv_widget>
	</view>
</screen>
```

### Rules

- **Top-of-file comment** is mandatory, directly above `<screen>`. The body
  names the *specific* attributes/values that vary and *why* — intent, not a
  description of the markup.
- **💡 hint** is the first child of `<view>`. Imperative and specific:
  "Drag each arc; lower `change_rate` lags behind." Not "Adjust the arc."
  The hint replaces a redundant title comment — don't write both.
- **Comment layering** — each layer adds new information: top-of-file =
  intent, 💡 = the interactive observation, per-block `<!-- -->` = what that
  block shows. Never repeat the same sentence across layers.
- **Layout** — default to the column-flex `<view>` above; spacing and
  centering fall out of it. Use manual `x`/`y` only when *positioning itself*
  is the subject (e.g. `ignore_layout`, overlap demos), and keep the tree
  flat — no wrapper containers.
- **Labels go inside the widget** they describe (`align="center"`), not as
  external siblings.
- **Show variation, not defaults.** No "default vs. featured" pairs unless
  the default illustrates a limitation. Pick two meaningfully different
  values (`knob_offset="-20"` vs. `"24"`, not `0` vs. `24`). Don't set a
  property that already holds its default — check the widget source.
- **Styles**: every named style is `style_`-prefixed and declared in a
  `<styles>` block, even one-offs. Format inline tags as 3-line blocks:
  ```xml
  <lv_widget>
  	<style name="style_x" selector="indicator" />
  </lv_widget>
  ```
- **Shared colors** go in `<consts>`, referenced as `#name`. The
  `text_color` const (`0x013992`, LVGL blue) is **only** for labels sitting
  *inside* fill-style widgets (`lv_bar`, `lv_slider`) whose indicator paints
  over the label. Do not add it to button/checkbox/LED labels — their
  default text color is fine.
- **Escaping**: a raw `<`/`>` inside an attribute breaks the XML. Use
  `&lt;`/`&gt;`, or rephrase ("above 80" not "> 80"). Newlines in text:
  `&#10;`. Enum/bool values lowercase (`mode="normal"`, `ignore_layout="true"`).
- **Local style part/state suffix**: `style_<prop>-<selector>` works
  (`style_bg_color-pressed="0x.."`), and `style_*` / a `<style>` child
  may target `<view>` (the screen root). But never put the bare
  `style_<prop>` *and* a `-<selector>` variant on the **same** element —
  the generator collapses them into one wrong call. Split them across
  elements (put the base look in a shared `<style>`).

### Styling examples (`<widget>/<widget>_styling/`)

One per widget. Goal: teach the widget's **part/state model**, not every
property.

- Use named `<style>` + `selector="part"` / `selector="part|state"` — that
  grammar is the headline. Local `style_*` on the tag is the one-off
  shortcut, shown sparingly.
- Cover every part the widget actually has. Multi-part widgets
  (`lv_buttonmatrix`, `lv_slider`, `lv_checkbox`, scale section…) need a
  style per relevant part. Scale section needs all three of `main`,
  `items`, `indicator` or the band is partly invisible.
- Show one state interaction (`pressed|knob`, `items|checked`).
- One coherent palette (one accent + one neutral + one text). One styled
  instance — don't stack five near-identical tiles.

### Data-binding examples (`<widget>/<widget>_bind_*/`)

One concept per file (`bind_value`, `bind_checked`, `bind_style`,
`bind_flag_if_*`, `bind_state_if_*`, event helpers).

- Reuse `globals.xml` subjects (see above).
- **Make the wire visible**: pair the bound widget with
  `<lv_label bind_text="subject_x" bind_text-fmt="%d"/>` showing the live
  value.
- **Hold-to-repeat**: a +/- button gets *two* `<subject_increment_event>`
  children — one `trigger="clicked"`, one `trigger="long_pressed_repeat"`.
- A binding that gates another widget needs both a control that drives the
  subject *and* the target that visibly reacts — otherwise nothing is
  observable. Cross-widget binding lives on the **driving** widget's page
  (a slider disabling a button is a slider example).
- 💡 hint describes the interaction, not the wiring: "Drag the slider; the
  label tracks it because both share `subject_value`."

---

## C-only authoring

Use only for imperative behavior XML can't express. Match the existing
files (e.g. [`widgets/arc/lv_example_arc_event.c`](widgets/arc/lv_example_arc_event.c)):

- Single `#include "../../lv_examples.h"` and a feature guard:
  `#if LV_USE_<WIDGET> && LV_BUILD_EXAMPLES` … `#endif`.
- Same top-of-file `@title`/`@brief`/body doxygen block as XML examples.
- Same column-flex screen setup so it looks consistent on the page.
- The handler must *do something visible* — read widget state and update
  another widget — not just `LV_LOG_USER`.

---

## Documentation integration

Each widget's page is [`docs/src/widgets/<widget>.mdx`](../docs/src/widgets/).

### Page structure (flat — no `## Usage` wrapper)

```
## Overview          ≤3 sentences: what the widget is
## <Feature>         one configurable aspect per section
## …
## Styling           parts list + the named-style/selector recipe
## Data binding       only if the widget exposes bindings
## Events             LV_EVENT_* list + one minimal C example
## Keys              only if it reacts to keys
```

Subsections use `###`, not deeper. `## Parts and Styles` folds into
`## Styling`.

### Section content

- **Example first, prose second.** The `<LvglXMLExample>` /
  `<LvglExample>` opens the section; prose explains *why* and lists the
  API. No trailing `## Examples` collector — every example sits in the
  feature subsection it illustrates.
- A C-only example goes inside its feature section with a 1–2 sentence
  intro saying what it shows that XML couldn't.
- **API references as bullets**, one per line:
  `- <ApiLink name="lv_x_set_y" display="lv_x_set_y(obj, v)" /> — what it does.`
  Wrap every function/enum name in `<ApiLink>` (bare `name=` for
  constants; `display=` for calls, escaping literal `"` as `&quot;`). In
  fenced ```c blocks keep calls as plain code.
- **Tables for catalogues** (3+ modes/parts/specifiers).
- Trim hard: drop implementation-detail asides, "Note that…" hedges,
  restatements, capital-W "Widget", marketing prose.

Reference URLs: concepts link to the public docs site
(`/common-widget-features/styles/overview`,
`/main-modules/observer/observer`). XML syntax / the editor → only
<https://lvgl.io/docs/pro>.

If a docs page is **generated** by a script, wire the example through
that script's data and re-run it — never hand-edit the generated
`.mdx`. Concept examples that don't belong to a single widget still
follow the rules above: example-first in the relevant page, no
`## Examples` collector.

---

## Validation checklist

- [ ] Top-of-file `@title` / `@brief` / 2–3-sentence body above `<screen>`,
      body names the specific varying attributes/values
- [ ] Specific 💡 hint as first `<view>` child; no redundant title comment
- [ ] No default-valued properties set; variation shown, not defaults
- [ ] Fits the 320×240 target
- [ ] Named styles `style_`-prefixed and in a `<styles>` block
- [ ] Labels inside their widget; shared colors via `<consts>` `#name`
- [ ] Data-binding examples reuse `globals.xml` subjects and show the value live
- [ ] `<` / `>` escaped in attributes
- [ ] `scripts/generate_examples.py` run; generated `.c` and topic header
      updated; project builds
- [ ] `<LvglXMLExample>` (or `<LvglExample>` for C-only) placed in the
      correct feature subsection; prose is C-first
- [ ] Imperative / XML-inexpressible features routed to a C-only
      example, not a broken XML round-trip
