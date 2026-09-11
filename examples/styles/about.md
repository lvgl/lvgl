---
title: "Styles"
description: "The LVGL style system: properties, states, inheritance, and transitions."
order: 20
---

An `lv_style_t` holds a set of property values (color, padding, border, font, and so on) that you attach to a widget, a specific part of a widget, and a state such as `LV_STATE_PRESSED` or `LV_STATE_FOCUSED`. Styles cascade through the parent/child tree, and transitions let property changes animate between states. This section covers property groups, state handling, and common composition patterns.
