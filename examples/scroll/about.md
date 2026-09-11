---
title: "Scrolling"
description: "Scroll-enabled containers and content."
order: 70
---

Any `lv_obj_t` whose content exceeds its bounds can scroll, with per-axis control and optional snap points that lock motion to child boundaries. Scrollbars are themeable through the `LV_PART_SCROLLBAR` style part, and event callbacks let you translate, fade, or scale children as they move through the viewport. The examples cover scrollbar mode and styling, RTL placement, the scrollable/chain/snap/scroll-one/floating behaviors, scroll events, programmatic scrolling, and the infinite and circular scroll patterns.
