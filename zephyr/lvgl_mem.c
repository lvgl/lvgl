/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lvgl_mem.h"
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/sys_heap.h>


#define HEAP_BYTES (CONFIG_LV_Z_MEM_POOL_MAX_SIZE * \
		    CONFIG_LV_Z_MEM_POOL_NUMBER_BLOCKS)

static char lvgl_heap_mem[HEAP_BYTES] __aligned(8);
static struct sys_heap lvgl_heap;
static struct k_spinlock lvgl_heap_lock;

void *lvgl_malloc(size_t size)
{
	k_spinlock_key_t key;
	void *ret;

	key = k_spin_lock(&lvgl_heap_lock);
	ret = sys_heap_alloc(&lvgl_heap, size);
	k_spin_unlock(&lvgl_heap_lock, key);

	return ret;
}

void *lvgl_realloc(void *ptr, size_t size)
{
	k_spinlock_key_t key;
	void *ret;

	key = k_spin_lock(&lvgl_heap_lock);
	ret = sys_heap_realloc(&lvgl_heap, ptr, size);
	k_spin_unlock(&lvgl_heap_lock, key);

	return ret;
}

void lvgl_free(void *ptr)
{
	k_spinlock_key_t key;

	key = k_spin_lock(&lvgl_heap_lock);
	sys_heap_free(&lvgl_heap, ptr);
	k_spin_unlock(&lvgl_heap_lock, key);
}

void lvgl_print_heap_info(bool dump_chunks)
{
	k_spinlock_key_t key;

	key = k_spin_lock(&lvgl_heap_lock);
	sys_heap_print_info(&lvgl_heap, dump_chunks);
	k_spin_unlock(&lvgl_heap_lock, key);
}

static int lvgl_heap_init(void)
{
	sys_heap_init(&lvgl_heap, &lvgl_heap_mem[0], HEAP_BYTES);
	return 0;
}

SYS_INIT(lvgl_heap_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
