/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lvgl_mem.h"
#include <zephyr/kernel.h>
#include <zephyr/init.h>

K_HEAP_DEFINE(lvgl_mem_pool, CONFIG_LV_Z_MEM_POOL_MAX_SIZE *
	      CONFIG_LV_Z_MEM_POOL_NUMBER_BLOCKS);

void *lvgl_malloc(size_t size)
{
	return k_heap_alloc(&lvgl_mem_pool, size, K_NO_WAIT);
}

void lvgl_free(void *ptr)
{
	k_heap_free(&lvgl_mem_pool, ptr);
}
