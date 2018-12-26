# Littlev Graphics Library

![LittlevGL cover](https://littlevgl.com/docs/themes/lv_theme_intro.png)

**LittlevGL provides everything you need to create a Graphical User Interface (GUI) on embedded systems with easy-to-use graphical elements, beautiful visual effects and low memory footprint.**  
https://littlevgl.com

Check out the [References](https://blog.littlevgl.com/2018-12-26/references) where LittlevGL was used. You can add your project too!

## Key features
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

## Porting
In the simplest case you need 5 things:
1. Call `lv_tick_inc(x)` every `x` milliseconds in a Timer or Task (`x` should be between 1 and 10)
2. Register a function which can **copy a pixel array** to an area of the screen.
3. Register a function which can **read an input device**. (E.g. touch pad)
4. Copy `lv_conf_templ.h` as `lv_conf.h` and set at least `LV_HOR_RES`, `LV_VER_RES` and `LV_COLOR_DEPTH`. 
5. Call `lv_task_handler()` periodically every few milliseconds.
For a detailed description visit https://github.com/littlevgl/lvgl/wiki/Porting
Or check the [Porting tutorial](https://github.com/littlevgl/lv_examples/blob/master/lv_tutorial/0_porting/lv_tutorial_porting.c)
 
## Project set-up
1. **Clone** or [Download](https://littlevgl.com/download) the lvgl repository: `git clone  https://github.com/littlevgl/lvgl.git`
2. **Create project** with your preferred IDE and add the *lvgl* folder
3. Copy **lvgl/lv_conf_templ.h** as **lv_conf.h** next to the *lvgl* folder
4. In the lv_conf.h delete the first `#if 0` and its `#endif`. Leave the default configuration for the first try.
5. In your *main.c*: #include "lvgl/lvgl.h"   
6. In your *main function*:
   * lvgl_init();
   * tick, display and input device initialization (see above)
7. To **test** create a label: `lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);`  
8. In the main *while(1)* call `lv_task_handler();` and make a few milliseconds delay (e.g. `my_delay_ms(5);`) 
9. Compile the code and load it to your embedded hardware

## PC Simulator
You can test the graphics library in a PC simulator, if you don't have an embedded hardware. The simulator uses [SDL2](https://www.libsdl.org/) library to emulate a display on your monitor and a touch pad with your mouse.

There is a pre-configured PC project for **Eclipse CDT** in this repository: https://github.com/littlevgl/pc_simulator

## Related repositories
* PC simulator: https://github.com/littlevgl/pc_simulator
* Projects: https://github.com/littlevgl/lv_projects
* Examples: https://github.com/littlevgl/lv_examples
* Drivers: https://github.com/littlevgl/lv_drivers

## Screenshots
![TFT material](http://www.gl.littlev.hu/github_res/tft_material.png)
![TFT zen](http://www.gl.littlev.hu/github_res/tft_zen.png)
![TFT alien](http://www.gl.littlev.hu/github_res/tft_alien.png)
![TFT night](http://www.gl.littlev.hu/github_res/tft_night.png)

## Contributing
See [CONTRIBUTING.md](https://github.com/littlevgl/lvgl/blob/master/docs/CONTRIBUTING.md)

## Donate
If you are pleased with this graphics library, found it useful, or are happy with the support you got, please help its further development:

[![Donate](https://littlevgl.com/donate_dir/donate_btn.png)](https://littlevgl.com/donate)
