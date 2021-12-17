# Demos for LVGL

## Add the examples to your projects
1. Clone this repository: `git clone https://github.com/lvgl/lv_demos.git`.
2. The `lv_demos` directory should be next to the `lvgl` directory in your project.

Similarly to `lv_conf.h` there is a configuration file for the examples too. It is called `lv_demo_conf.h`.
1. Copy `lv_demos/lv_demo_conf_template.h` next to `lv_demos` directory
2. Rename it to `lv_demo_conf.h`
3. Change the first `#if 0` to `#if 1` to enable the file's content
4. Enable or Disable demos

## Demos

### Widgets
Shows how the widgets look like out of the box using the built-in material theme.  
See in [lv_demo_widgets](https://github.com/lvgl/lv_examples/tree/master/src/lv_demo_widgets) folder.

<img src="https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_widgets/screenshot1.png?raw=true" width=600px alt="Basic demo to show the widgets of LVGL">

### Music player
The music player demo shows what kind of modern, smartphone-like user interfaces can be created on LVGL. It works the best with display with 480x272 or 272x480 resolution. 

See in [lv_demo_music](https://github.com/lvgl/lv_examples/tree/master/src/lv_demo_music) folder.

<img src="https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_music/screenshot1.gif?raw=true" width=600px alt="Music player demo with LVGL">

### Keypad and encoder
LVGL allows you to control the widgets with a keypad and/or encoder without a touchpad. This demo shows how to handle buttons, drop-down lists, rollers, sliders, switches, and text inputs without touchpad. 
Learn more about the touchpad-less usage of LVGL [here](https://docs.lvgl.io/v7/en/html/overview/indev.html#keypad-and-encoder).

See in [lv_demo_keypad_encoder](https://github.com/lvgl/lv_examples/tree/master/src/lv_demo_keypad_encoder) folder.

<img src="https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_keypad_encoder/screenshot1.png?raw=true" width=600px alt="Keypad and encoder navigation in LVGL embedded GUI library">

### Benchmark
A demo to measure the performance of LVGL or to compare different settings. 
See in [lv_demo_benchmark](https://github.com/lvgl/lv_examples/tree/master/src/lv_demo_benchmark) folder.
<img src="https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_benchmark/screenshot1.png?raw=true" width=600px alt="Benchmark demo with LVGL embedded GUI library">

### Stress
A stress test for LVGL. It contains a lot of object creation, deletion, animations, style usage, and so on. It can be used if there is any memory corruption during heavy usage or any memory leaks. 
See in [lv_demo_stress](https://github.com/lvgl/lv_examples/tree/master/src/lv_demo_stress) folder.
<img src="https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_stress/screenshot1.png?raw=true" width=600px alt="Stress tes tfor LVGL">

## Contributing
For contribution and coding style guidelines, please refer to the file docs/CONTRIBUTNG.md in the main LVGL repo:
  https://github.com/lvgl/lvgl
