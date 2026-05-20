# Documentation

LVGL documentation is authored in **MDX**. The rendering site that serves these pages is maintained separately by the LVGL team — contributors here author and review `.mdx` content under `./src/`.

> **Local preview Docker image — *coming soon*.** We will publish an image on GHCR (`ghcr.io/lvgl/lvgl-docs` — exact tag TBD) that bundles the docs site and serves this `./src/` content at `http://localhost:3000`.


## Everything Must Be Documented

Our firm policy: ***EVERYTHING MUST BE DOCUMENTED***. Public APIs, widgets, features, and configuration options all need an MDX page or section. When in doubt, add it.


## Where Content Lives

All MDX pages live under `./src/`. The directory structure drives the URL, and sidebar ordering is controlled by `meta.json` files inside each directory.

A page is a `.mdx` file with YAML frontmatter:

```mdx
---
title: Animations
description: Animate widget properties over time with the LVGL animation engine.
---

# Animations

Animations change a property's value over a period of time...

<LvglExample name="lv_example_anim_1" path="anim/lv_example_anim_1" />
```


## Common Components

A small set of components covers most authoring needs:

- `<Callout type="info|warn|error" title="...">` — notes, warnings, tips.
- `<LvglExample name="..." path="..." />` — embed a runnable code example.
- `<ApiLink name="lv_label_create" />` — inline link to an API symbol.
- `<ApiLinkList items={["lv_label"]} />` — link out to related API pages at the end of a page.
- `<Figure src="/_static/images/..." alt="..." caption="..." />` — images with captions.
- `<DirectoryIndex />` — auto-generated list of child pages on index pages.

The **full component catalog**, formatting rules, `meta.json` examples, and icon list live in [Writing Docs](./src/contributing/writing_docs.mdx). Start there for anything beyond the components above.


## More on Contributing

For coding style, DCO, pull requests, and other contribution guidelines, see the pages under [`./src/contributing/`](./src/contributing).
