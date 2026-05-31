# LVGL Examples

The examples shown in the widget documentation live here. The fastest way to
add one is to **copy an existing example next to the one you're adding** and
adjust it — the surrounding files already follow every convention.

## Two kinds

- **XML (preferred)** — `lv_example_<module>_<feature>.xml` in its own
  `<module>/<feature>/` folder. Use for anything declarative: layout,
  styling, properties, data binding. The C examples are generated from it, see below.
- **C-only** — `lv_example_<module>_<feature>.c` directly in the `<module>/`
  folder. Use only when the point is imperative C that XML can't express
  (event callbacks, animations, draw events), or a feature XML doesn't
  support yet.

Most widgets need many XML examples and zero or one C-only example.

## Good examples to copy

- XML: [`widgets/arc/lv_example_arc_set_angles.xml`](widgets/arc/lv_example_arc_set_angles.xml)
- XML with data binding: [`widgets/slider/lv_example_slider_bind_value.xml`](widgets/slider/lv_example_slider_bind_value.xml)
- C-only: [`widgets/arc/lv_example_arc_event.c`](widgets/arc/lv_example_arc_event.c)

## Generating the C

You write `.xml`; a script produces the shipped `.c`. **Never hand-edit a
generated `.c`** — your changes will be overwritten.

```bash
python scripts/generate_examples.py [path ...]   # all examples if no path
```

Run it after every new or edited XML, then build to confirm it compiles.
The script auto-runs `scripts/code-format.py examples` at the end so every
generated `.c` ships astyle-formatted, and then wipes all `.c`/`.h` files
from `examples/xml_project/` (the CLI's project scaffolding) so they can't
collide with the host project's source globbing.

The generator drives the **LVGL Pro editor CLI** (`lved-cli.js`), which
ships with the LVGL Pro editor — see <https://lvgl.io/docs/pro>. The script
picks it up from your `PATH`, or point at it explicitly:

```bash
python scripts/generate_examples.py --cli /path/to/lved-cli.js [path ...]
```

## Hard rules

- **Fit 320×240** — every example runs in that target (see
  [`xml_project/project.xml`](xml_project/project.xml)). Size widgets to fit.
- **Reuse shared resources** — subjects, consts, styles and images come from
  [`xml_project/globals.xml`](xml_project/globals.xml). Don't invent
  per-example subjects; add to `globals.xml` only if nothing fits.
- **One example = one feature.** Show a meaningful variation, not defaults.
- Each example is surfaced in the docs with `<LvglExample>`.

## Comments

Every XML example opens with a comment block directly above `<screen>`,
plus one in-view hint. Keep the layers distinct — never repeat a sentence:

- **`@title`** — short, capitalized; mirrors the on-screen label. No "XML".
- **`@brief`** — one sentence, less than 80 chars, ending with a period.
- **Description** — 2–3 sentences after `@brief`: name the *specific*
  attributes/values that vary and *why* it's interesting. Self-contained
  (readable without the doc page).
- **💡 hint** — first child of `<view>`: imperative and specific about what
  to do and what visibly changes ("Drag each arc; lower `change_rate` lags
  behind"), not a vague "Adjust the arc".

## XML syntax

The XML format and the editor are documented at
<https://lvgl.io/docs/pro>. For the in-tree conventions, the best reference
is the existing examples — match the file you copied.
