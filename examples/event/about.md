---
title: "Events"
description: "Widget events and user input handling."
order: 50
---

Widgets emit events such as `LV_EVENT_CLICKED`, `LV_EVENT_VALUE_CHANGED`, `LV_EVENT_DRAW_MAIN`, and `LV_EVENT_KEY`. You register a callback with `lv_obj_add_event_cb()` and receive an `lv_event_t` pointer from which you read the code, target, and user data. The examples cover basic wiring, bubbling, custom event codes, and reading input device state inside a handler.
