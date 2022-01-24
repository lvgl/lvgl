```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/espressif.md
```

# Espressif (ESP32)
Since v7.7.1 LVGL includes a Kconfig file, so LVGL can be used as an ESP-IDF v4 component.

## Get the LVGL demo project for ESP32

We've created [lv_port_esp32](https://github.com/lvgl/lv_port_esp32), a project using ESP-IDF and LVGL to show one of the demos from [lv_examples](https://github.com/lvgl/lv_examples).
You are able to configure the project to use one of the many supported display controllers, see [lvgl_esp32_drivers](https://github.com/lvgl/lvgl_esp32_drivers) for a complete list
of supported display and indev (touch) controllers.

## Use LVGL in your ESP32 project

### Prerequisites

ESP-IDF v4 framework is the suggested version to use.

### Get LVGL

It is suggested that you add LVGL as a "component" to your project. This component can be located inside a directory named "components" in the project root directory.

When your project is a git repository you can include LVGL as a git submodule:

```c
git submodule add https://github.com/lvgl/lvgl.git components/lvgl
```

The above command will clone LVGL's main repository into the `components/lvgl` directory. LVGL includes a `CMakeLists.txt` file that sets some configuration options so you can use LVGL right away.

When you are ready to configure LVGL, launch the configuration menu with `idf.py menuconfig` on your project root directory, go to `Component config` and then `LVGL configuration`.

## Use lvgl_esp32_drivers in your project

You can also add `lvgl_esp32_drivers` as a "component". This component can be located inside a directory named "components" on your project root directory.

When your project is a git repository you can include `lvgl_esp32_drivers` as a git submodule:

```c
git submodule add https://github.com/lvgl/lvgl_esp32_drivers.git components/lvgl_esp32_drivers
```

### Support for ESP32-S2

Basic support for ESP32-S2 has been added into the `lvgl_esp32_drivers` repository.
