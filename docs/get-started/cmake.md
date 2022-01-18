```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/cmake.md
```

# CMake
LVGL supports integrating with [CMake](https://cmake.org/). It comes with preconfigured targets for:
- [Espressif (ESP32)](https://docs.espressif.com/projects/esp-idf/en/v3.3/get-started-cmake/index.html)
- [MicroPython](https://docs.micropython.org/en/v1.15/develop/cmodules.html)
- [Zephyr](https://docs.zephyrproject.org/latest/guides/zephyr_cmake_package.html)

On top of the preconfigured targets you can also use "plain" CMake to integrate LVGL into any custom C/C++ project.

### Prerequisites
- CMake ( >= 3.12.4 )
- Compatible build tool e.g.
  - [Make](https://www.gnu.org/software/make/)
  - [Ninja](https://ninja-build.org/)

## Building LVGL with CMake
There are many ways to include external CMake projects into your own. A modern one also used in this example is the CMake [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module. This module conveniently allows us to download dependencies directly at configure time from e.g. [GitHub](https://github.com/). Here is an example how we might include LVGL into our own project.

```cmake
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(MyProject LANGUAGES C CXX)

# Build an executable called "MyFirmware"
add_executable(MyFirmware src/main.c)

# Specify path to own LVGL config header
set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
    CACHE STRING "" FORCE)

# Fetch LVGL from GitHub
FetchContent_Declare(lvgl URL https://github.com/lvgl/lvgl.git)
FetchContent_MakeAvailable(lvgl)

# The target "MyFirmware" depends on LVGL
target_link_libraries(MyFirmware PRIVATE lvgl::lvgl)
```

This configuration declares a dependency between the two targets **MyFirmware** and **lvgl**. Upon building the target **MyFirmware** this dependency will be resolved and **lvgl** will be built and linked with it. Since LVGL requires a config header called [lv_conf.h](https://github.com/lvgl/lvgl/blob/master/lv_conf_template.h) to be includable by its sources we also set the option `LV_CONF_PATH` to point to our own copy of it.

### Additional CMake options
Besides `LV_CONF_PATH` there are two additional CMake options to specify include paths.
  
`LV_LVGL_H_INCLUDE_SIMPLE` which specifies whether to `#include "lvgl.h"` absolut or relative 

| ON (default) | OFF            |
| ------------ | -------------- |
| "lvgl.h"     | "../../lvgl.h" |

`LV_CONF_INCLUDE_SIMPLE` which specifies whether to `#include "lv_conf.h"` and `"lv_drv_conf.h"` absolut or relative  

| ON (default)    | OFF                   |
| --------------- | --------------------- |
| "lv_conf.h"     | "../../lv_conf.h"     |
| "lv_drv_conf.h" | "../../lv_drv_conf.h" |

I do not recommend disabling those options unless your folder layout makes it absolutely necessary.

## Building LVGL examples with CMake
LVGL [examples](https://docs.lvgl.io/master/examples.html) have their own CMake target. If you want to build the examples simply add them to your dependencies.

```cmake
# The target "MyFirmware" depends on LVGL and examples
target_link_libraries(MyFirmware PRIVATE lvgl::lvgl lvgl::examples)
```

## Building LVGL drivers and demos with CMake
Exactly the same goes for the [drivers](https://github.com/lvgl/lv_drivers) and the [demos](https://github.com/lvgl/lv_demos).

```cmake
# Specify path to own LVGL demos config header
set(LV_DEMO_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_demo_conf.h
    CACHE STRING "" FORCE)

FetchContent_Declare(lv_drivers
                     GIT_REPOSITORY https://github.com/lvgl/lv_drivers)
FetchContent_MakeAvailable(lv_drivers)
FetchContent_Declare(lv_demos
                     GIT_REPOSITORY https://github.com/lvgl/lv_demos.git)
FetchContent_MakeAvailable(lv_demos)

# The target "MyFirmware" depends on LVGL, drivers and demos
target_link_libraries(MyFirmware PRIVATE lvgl::lvgl lvgl::drivers lvgl::examples)
```

Just like the [lv_conf.h](https://github.com/lvgl/lvgl/blob/master/lv_conf_template.h) header demos comes with its own config header called [lv_demo_conf.h](https://github.com/lvgl/lv_demos/blob/master/lv_demo_conf_template.h). Analogous to `LV_CONF_PATH` its path can be set by using the option `LV_DEMO_CONF_PATH`.
