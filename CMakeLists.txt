cmake_minimum_required(VERSION 3.12.4)

project(lvgl HOMEPAGE_URL https://github.com/lvgl/lvgl)

if(ESP_PLATFORM)
  include(cmake/esp.cmake)
elseif(ZEPHYR_BASE)
  include(cmake/zephyr.cmake)
elseif(MICROPY_DIR)
  include(cmake/micropython.cmake)
else()
  include(cmake/baremetal.cmake)
endif()
