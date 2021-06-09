```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/micropython.md
```
# Micropython

## What is Micropython?

[Micropython](http://micropython.org/) is Python for microcontrollers.  
Using Micropython, you can write Python3 code and run it even on a bare metal architecture with limited resources.

### Highlights of Micropython

- **Compact** - Fits and runs within just 256k of code space and 16k of RAM. No OS is needed, although you can also run it with an OS, if you want.
- **Compatible** - Strives to be as compatible as possible with normal Python (known as CPython).
- **Versatile** - Supports many architectures (x86, x86-64, ARM, ARM Thumb, Xtensa).
- **Interactive** - No need for the compile-flash-boot cycle. With the REPL (interactive prompt) you can type commands and execute them immediately, run scripts etc.
- **Popular** - Many platforms are supported. The user base is growing bigger. Notable forks: [MicroPython](https://github.com/micropython/micropython), [CircuitPython](https://github.com/adafruit/circuitpython), [MicroPython_ESP32_psRAM_LoBo](https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo)
- **Embedded Oriented** - Comes with modules specifically for embedded systems, such as the [machine module](https://docs.micropython.org/en/latest/library/machine.html#classes) for accessing low-level hardware (I/O pins, ADC, UART, SPI, I2C, RTC, Timers etc.)

---

## Why Micropython + LVGL?

Currently, Micropython [does not have a good high-level GUI library](https://forum.micropython.org/viewtopic.php?f=18&t=5543) by default. LVGL is an [Object Oriented Component Based](https://blog.lvgl.io/2018-12-13/extend-lvgl-objects) high-level GUI library, which seems to be a natural candidate to map into a higher level language, such as Python. LVGL is implemented in C and its APIs are in C.

### Here are some advantages of using LVGL in Micropython:

- Develop GUI in Python, a very popular high level language. Use paradigms such as Object Oriented Programming.
- Usually, GUI development requires multiple iterations to get things right. With C, each iteration consists of **`Change code` > `Build` > `Flash` > `Run`**.  
In Micropython it's just **`Change code` > `Run`** ! You can even run commands interactively using the [REPL](https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop) (the interactive prompt)

### Micropython + LVGL could be used for:

- Fast prototyping GUI.  
- Shortening the cycle of changing and fine-tuning the GUI.
- Modelling the GUI in a more abstract way by defining reusable composite objects, taking advantage of Python's language features such as Inheritance, Closures, List Comprehension, Generators, Exception Handling, Arbitrary Precision Integers and others.
- Make LVGL accessible to a larger audience. No need to know C in order to create a nice GUI on an embedded system.  
This goes well with [CircuitPython vision](https://learn.adafruit.com/welcome-to-circuitpython/what-is-circuitpython). CircuitPython was designed with education in mind, to make it easier for new or unexperienced users to get started with embedded development.
- Creating tools to work with LVGL at a higher level (e.g. drag-and-drop designer).

---

## So what does it look like?

> TL;DR:
> It's very much like the C API, but Object Oriented for LVGL components.

Let's dive right into an example!  

### A simple example

```python
import lvgl as lv
lv.init()
scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")
lv.scr_load(scr)
```

## How can I use it?

### Online Simulator

If you want to experiment with LVGL + Micropython without downloading anything - you can use our online simulator!  
It's a fully functional LVGL + Micropython that runs entirely in the browser and allows you to edit a python script and run it.

[Click here to experiment on the online simulator](https://sim.lvgl.io/)

[Hello World](https://sim.lvgl.io/v7/micropython/ports/javascript/bundle_out/index.html?script=https://gist.githubusercontent.com/amirgon/51299ce9b6448328a855826149482ae6/raw/0f235c6d40462fd2f0e55364b874f14fe3fd613c/lvgl_hello_world.py&script_startup=https://gist.githubusercontent.com/amirgon/7bf15a66ba6d959bbf90d10f3da571be/raw/8684b5fa55318c184b1310663b187aaab5c65be6/init_lv_mp_js.py)

Note: the online simulator is available for lvgl v6 and v7.

### PC Simulator

Micropython is ported to many platforms. One notable port is "unix", which allows you to build and run Micropython (+LVGL) on a Linux machine. (On a Windows machine you might need Virtual Box or WSL or MinGW or Cygwin etc.)  

[Click here to know more information about building and running the unix port](https://github.com/lvgl/lv_micropython)

### Embedded platform

In the end, the goal is to run it all on an embedded platform.  
Both Micropython and LVGL can be used on many embedded architectures, such as stm32, ESP32 etc.  
You would also need display and input drivers. We have some sample drivers (ESP32+ILI9341, as well as some other examples), but chances are you would want to create your own input/display drivers for your specific hardware.
Drivers can be implemented either in C as a Micropython module, or in pure Micropython!

## Where can I find more information?

- In this [Blog Post](https://blog.lvgl.io/2019-02-20/micropython-bindings)
- `lv_micropython` [README](https://github.com/lvgl/lv_micropython)
- `lv_binding_micropython` [README](https://github.com/lvgl/lv_binding_micropython)
- The [LVGL micropython forum](https://forum.lvgl.io/c/micropython) (Feel free to ask anything!)
- At Micropython: [docs](http://docs.micropython.org/en/latest/) and [forum](https://forum.micropython.org/)
