/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_LIB_GUI_LVGL_MEM_H_
#define ZEPHYR_LIB_GUI_LVGL_MEM_H_

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void *lvgl_malloc(size_t size);

void *lvgl_realloc(void *ptr, size_t size);

void lvgl_free(void *ptr);

void lvgl_print_heap_info(bool dump_chunks);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_GUI_LVGL_MEM_H_i */
