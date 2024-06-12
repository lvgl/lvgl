# Wayland display and input driver

Wayland display and input driver, with support for keyboard, pointer (i.e. mouse) and touchscreen.
Keyboard support is based on libxkbcommon.

Following shell are supported:

* xdg_shell

> xdg_shell requires an extra build step; see section _Generate protocols_ below.


Basic client-side window decorations (simple title bar, minimize and close buttons)
are supported, while integration with desktop environments is not.


## Install headers and libraries

### Ubuntu

```
sudo apt-get install libwayland-dev libxkbcommon-dev libwayland-bin wayland-protocols
```

### Fedora

```
sudo dnf install wayland-devel libxkbcommon-devel wayland-utils wayland-protocols-devel
```


## Generate protocols

Support for non-basic shells (i.e. other than _wl_shell_) requires additional
source files to be generated before the first build of the project. To do so,
navigate to the _wayland_ folder (the one which includes this file) and issue
the following commands:

```
cmake .
make
```


## Build configuration under Eclipse

In "Project properties > C/C++ Build > Settings" set the followings:

- "Cross GCC Compiler > Command line pattern"
  - Add ` ${wayland-cflags}` and ` ${xkbcommon-cflags}` to the end (add a space between the last command and this)


- "Cross GCC Linker > Command line pattern"
  - Add ` ${wayland-libs}` and ` ${xkbcommon-libs}`  to the end (add a space between the last command and this)


- In "C/C++ Build > Build variables"
  - Configuration: [All Configuration]

  - Add
    - Variable name: `wayland-cflags`
      - Type: `String`
      - Value: `pkg-config --cflags wayland-client`
    - Variable name: `wayland-libs`
      - Type: `String`
      - Value: `pkg-config --libs wayland-client`
    - Variable name: `xkbcommon-cflags`
      - Type: `String`
      - Value: `pkg-config --cflags xkbcommon`
    - Variable name: `xkbcommon-libs`
      - Type: `String`
      - Value: `pkg-config --libs xkbcommon`


## Init Wayland in LVGL

1. In `main.c` `#incude "drivers/wayland/wayland.h"`
2. Enable the Wayland driver in `lv_conf.h` with `LV_USE_WAYLAND 1` and
   configure its features below, enabling at least support for one shell.
3. `LV_COLOR_DEPTH` should be set either to `32` in `lv_conf.h`;(ensure LV_MEM_SIZE enough)
4. After `lv_init()` call `lw_wayland_init()`.
5. Add a display (or more than one) using `lw_create_window(...)`,
   possibly with a close callback to track the status of each display:
```c
  #define H_RES (800)
  #define V_RES (480)

  /* Create a display */
  lv_disp_t * disp = lw_create_window(H_RES, V_RES, "Window Title", close_cb);
```
  As part of the above call, the Wayland driver will register four input devices
  for each display:
  - a KEYPAD connected to Wayland keyboard events
  - a POINTER connected to Wayland touch events
  - a POINTER connected to Wayland pointer events
  - a ENCODER connected to Wayland pointer axis events
  Handles for input devices of each display can be get using respectively
  `lw_get_lv_indev_keyboard()`, `lw_get_lv_indev_touchscreen()`,
  `lw_get_lv_indev_pointer()` and `lw_get_lv_indev_pointeraxis()`, using
  `disp` as argument.
5. After `lv_deinit()` (if used), or in any case during de-initialization, call
  `lw_wayland_deinit()`.

### Fullscreen mode

In order to set one window as fullscreen or restore it as a normal one,
call the `lw_set_window_fullscreen()` function respectively with `true`
or `false` as `fullscreen` argument.

### Disable window client-side decoration at runtime

Even when client-side decorations are enabled at compile time, they can be
disabled at runtime setting the `LV_WAYLAND_DISABLE_WINDOWDECORATION`
environment variable to `1`.

### Event-driven timer handler

call `lw_timer_handler()`
in your timer loop (in place of `lv_timer_handler()`).

You can now sleep/wait until the next timer/event is ready, e.g.:
```
    /* [After initialization and display creation] */
    #include <limits.h>
    #include <errno.h>
    #include <poll.h>

    struct pollfd pfd;
    uint32_t time_till_next;
    int sleep;

    pfd.fd = lw_get_wayland_fd();
    pfd.events = POLLIN;

    while (true) {
        usleep(LV_DEF_REFR_PERIOD * 1000); /* 5 recommended for LV_DEF_REFR_PERIOD*/

        if ((poll(&pfd, 1, sleep_time) < 0) && (errno == EINTR)) continue;

        time_till_next = lw_timer_handler();
        /* Wait for something interesting to happen */
        if (time_till_next == LV_NO_TIMER_READY) {
            sleep_time = -1;
            break;
        }
        else if (time_till_next > INT_MAX) {
            sleep_time = INT_MAX;
        }
        else {
            sleep_time = time_till_next;
        }

    };
```
