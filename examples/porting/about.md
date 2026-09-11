---
title: "Porting"
description: "Platform integration: OS abstraction, display drivers, input drivers."
order: 110
flatten: true
---

Bringing LVGL to a new platform means supplying a tick source, a display flush callback that pushes rendered pixels to the panel, and input read callbacks for touch, keypad, or encoder devices. If you run under an RTOS, you also wire the OS primitives (threads, mutexes) through the abstraction layer. The examples here show each hook in isolation so you can port them one at a time.
