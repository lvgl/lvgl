# PikaScript Binding for LVGL

[PikaScript](https://github.com/pikasTech/pikascript) is an ultralightweight python engine that can run with 4KB of RAM and 32KB of Flash (such as STM32G030C8 and STM32F103C8), and is very easy to deploy and expand.

More details to see: [PikaScript and lvgl: Make Python Lighter, Easier and Smarter](https://blog.lvgl.io/2022-08-24/pikascript-and-lvgl)

The available Python APIs are in the `pika_lvgl.pyi`, and you need copy the `pika_lvgl.pyi` to the root path of pikascript, then `import pika_lvgl` in `main.py`.

The available simulation project on windows:  https://github.com/pikasTech/lv_pikascript

More document about PikaScript: https://pikadoc.readthedocs.io/en/latest/index.html