# Littlev Graphics Library  &nbsp; [![Tweet](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/intent/tweet?text=LittlevGL%20is%20a%20free%20and%20open%20source%20embedded%20GUI%20library%20with%20easy-to-use%20graphical%20elements,%20beautiful%20visual%20effects%20and%20low%20memory%20footprint.&url=https://littlevgl.com/&via=kisvegabor&hashtags=littlevgl,embedded,gui,free,opensource)

![price](https://img.shields.io/badge/price-FREE-brightgreen.svg)
![status](https://img.shields.io/badge/status-ACTIVE-brightgreen.svg)
![licence](https://img.shields.io/badge/licence-MIT-blue.svg)
![version](https://img.shields.io/badge/version-5.2-blue.svg)
![language](https://img.shields.io/badge/topic-GUI-yellow.svg)
![language](https://img.shields.io/badge/language-C/C++-yellow.svg)
![LittlevGL cover](https://littlevgl.com/docs/themes/lv_theme_intro.png)

**LittlevGL provides everything you need to create a Graphical User Interface (GUI) on embedded systems with easy-to-use graphical elements, beautiful visual effects and low memory footprint.**  

:star: **Star the project if you like it!** :star: 

**[Website](https://littlevgl.com) &middot; [Live demo](https://littlevgl.com/live-demo) &middot; [Simulator](https://docs.littlevgl.com/#PC-simulator) &middot; [Docs](https://docs.littlevgl.com/) &middot; [Blog](https://blog.littlevgl.com/)**


### Features
* **Powerful building blocks** buttons, charts, lists, sliders, images, etc.
* **Advanced graphics** with animations, anti-aliasing, opacity, smooth scrolling
* **Various input devices** touch pad, mouse, keyboard, encoder, buttons, etc.
* **Multi-language support** with UTF-8 encoding
* **Fully customizable** graphical elements
* **Hardware independent** to use with any microcontroller or display
* **Scalable** to operate with little memory (50 kB Flash, 10 kB RAM)
* **OS, External memory and GPU** supported but not required
* **Single frame buffer** operation even with advances graphical effects
* **Written in C** for maximal compatibility
* **Simulator** to develop on PC without embedded hardware
* **Tutorials, examples, themes** for rapid development
* **Documentation** and API references online

### Supported devices
Basically, every device which is capable of driving a display is suitable to run LittlevGL. The minimal requirements:
- 16, 32 or 64-bit microcontroller or processor
- &lt; 16 MHz clock speed
- 8 kB RAM for static data and >2 KB RAM for dynamic data (graphical objects)
- 64 kB program memory (flash)
- Optionally ~1/10 screen sized memory for internal buffering (at 240 × 320, 16-bit colors it means 15 kB)

Just to mention **a few platforms**:
- STM32F1, STM32F3, [STM32F4](https://blog.littlevgl.com/2017-07-15/stm32f429_disco_port), [STM32F7](https://github.com/littlevgl/stm32f746_disco_no_os_sw4stm32)
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- NXP Kinetis, LPC, iMX
- [Linux frame buffer](https://blog.littlevgl.com/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry PI](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- Espressif [ESP32](https://github.com/littlevgl/esp32_ili9431)
- Nordic nrf52
- Quectell M66

### Quick start in a simulator
Try LittlevGL in a simulator on you PC without any embedded hardware. Choose a project with your favourite IDE:

|   Eclipse   | CodeBlock  | Visual Studio | PlatfomIO | Qt Creator |
|-------------|----------- |---------------|-----------|------------|
|  [![Eclipse](https://littlevgl.com/logo/ide/eclipse.jpg)](https://github.com/littlevgl/pc_simulator_sdl_eclipse) | [![CodeBlocks](https://littlevgl.com/logo/ide/codeblocks.jpg)](https://github.com/littlevgl/pc_simulator_win_codeblocks) | [![VisualStudio](https://littlevgl.com/logo/ide/visualstudio.jpg)](https://github.com/littlevgl/visual_studio_2017_sdl_x64)   |   [![PlatformIO](https://littlevgl.com/logo/ide/platformio.jpg)](https://github.com/littlevgl/pc_simulator_sdl_platformio) | [![QtCreator](https://littlevgl.com/logo/ide/qtcreator.jpg)](https://blog.littlevgl.com/2019-01-03/qt-creator) |
| Cross-platform<br>with SDL | Native Windows | Cross-platform<br>with SDL | Cross-platform<br>with SDL | Cross-platform<br>with SDL |

### Porting to an embedded hardware
In the most simple case you need to do 5 things:
1. Call `lv_tick_inc(x)` every `x` milliseconds in a Timer or Task (`x` should be between 1 and 10)
2. Register a function which can **copy a pixel array** to an area of the screen.
3. Register a function which can **read an input device**. (E.g. touch pad)
4. Copy `lv_conf_templ.h` as `lv_conf.h` next to `lvgl` and set at least `LV_HOR_RES`, `LV_VER_RES` and `LV_COLOR_DEPTH`. 
5. Call `lv_task_handler()` periodically every few milliseconds.
For a detailed description visit https://docs.littlevgl.com/#Porting
Or check the [Porting tutorial](https://github.com/littlevgl/lv_examples/blob/master/lv_tutorial/0_porting/lv_tutorial_porting.c)
 
### Contributing
To ask questions and discuss topics we use [GitHub's Issue tracker](https://github.com/littlevgl/lvgl/issues). 
You contribute in several ways:
- **Answer other's question** click the Watch button on the top to get notified about the issues
- **Report and/or fix bugs** using the issue tracker in Pull-request
- **Suggest and/or implement new features** using the issue tracker in Pull-request
- **Improve and/or translate the documentation** learn more [here](https://github.com/littlevgl/docs)
- **Write a blog post about your experiences** learn more [here](https://github.com/littlevgl/blog)
- **Upload your project or product as a reference** to [this site](https://blog.littlevgl.com/2018-12-26/references)

Before contributing, please read [CONTRIBUTING.md](https://github.com/littlevgl/lvgl/blob/master/docs/CONTRIBUTING.md).
For more info read this [blog post](https://blog.littlevgl.com/2018-12-06/contributing).

### Donate
If you are pleased with the library, found it useful, or you are happy with the support you got, please help its further development:

[![Donate](https://littlevgl.com/donate_dir/donate_btn.png)](https://littlevgl.com/donate)
