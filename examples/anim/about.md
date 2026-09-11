---
title: "Animations"
description: "Animate widget properties over time with lv_anim."
order: 40
---

An `lv_anim_t` describes a value interpolated between a start and end over a duration, with an optional easing path and completion callback. Animations tick on display refresh, so their smoothness tracks the refresh rate. You can chain animations, run them in parallel, and reuse a single `lv_anim_t` descriptor to drive any numeric property through a setter.
