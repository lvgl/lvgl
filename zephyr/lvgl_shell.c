/*
 * Copyright (c) 2023 Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/shell/shell.h>
#include <lvgl.h>
#include <autoconf.h>

#ifdef CONFIG_LV_Z_MEM_POOL_SYS_HEAP
#include "lvgl_mem.h"
#endif

#ifdef CONFIG_LV_USE_MONKEY
static lv_monkey_t *lvgl_monkeys[CONFIG_LV_Z_MAX_MONKEY_COUNT];

static const char *lvgl_monkey_indev_as_string(lv_monkey_t *monkey)
{
	lv_indev_t *input_device;

	input_device = lv_monkey_get_indev(monkey);
	if (!input_device || !input_device->driver) {
		return "unknown";
	}

	switch (input_device->driver->type) {
	case LV_INDEV_TYPE_POINTER:
		return "pointer";
	case LV_INDEV_TYPE_KEYPAD:
		return "keypad";
	case LV_INDEV_TYPE_BUTTON:
		return "button";
	case LV_INDEV_TYPE_ENCODER:
		return "encoder";
	default:
		return "unknown";
	}
}

static int lvgl_monkey_indev_from_string(const char *str, lv_indev_type_t *input_device)
{
	if (strcmp(str, "pointer") == 0) {
		*input_device = LV_INDEV_TYPE_POINTER;
	} else if (strcmp(str, "keypad") == 0) {
		*input_device = LV_INDEV_TYPE_KEYPAD;
	} else if (strcmp(str, "button") == 0) {
		*input_device = LV_INDEV_TYPE_BUTTON;
	} else if (strcmp(str, "encoder") == 0) {
		*input_device = LV_INDEV_TYPE_ENCODER;
	} else {
		return -EINVAL;
	}
	return 0;
}

static void dump_monkey_info(const struct shell *sh)
{
	shell_print(sh, "id   device    active");
	for (size_t i = 0; i < CONFIG_LV_Z_MAX_MONKEY_COUNT; i++) {
		if (lvgl_monkeys[i] != NULL) {
			shell_print(sh, "%-4u %-9s %-3s", i,
				    lvgl_monkey_indev_as_string(lvgl_monkeys[i]),
				    lv_monkey_get_enable(lvgl_monkeys[i]) ? "yes" : "no");
		}
	}
}

static int cmd_lvgl_monkey(const struct shell *sh, size_t argc, char *argv[])
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	dump_monkey_info(sh);
	shell_print(sh, "");
	shell_help(sh);

	return SHELL_CMD_HELP_PRINTED;
}

static int cmd_lvgl_monkey_create(const struct shell *sh, size_t argc, char *argv[])
{
	bool created_monkey = false;
	lv_monkey_config_t default_config;

	lv_monkey_config_init(&default_config);

	if (argc == 2) {
		if (lvgl_monkey_indev_from_string(argv[1], &default_config.type) < 0) {
			shell_error(sh, "Invalid monkey input device %s", argv[1]);
			shell_help(sh);
			return SHELL_CMD_HELP_PRINTED;
		}
	}

	for (size_t i = 0; i < CONFIG_LV_Z_MAX_MONKEY_COUNT; i++) {
		if (lvgl_monkeys[i] == NULL) {
			lvgl_monkeys[i] = lv_monkey_create(&default_config);
			lv_monkey_set_enable(lvgl_monkeys[i], true);
			created_monkey = true;
			break;
		}
	}

	if (!created_monkey) {
		shell_error(sh, "Error creating monkey instance");
		return -ENOSPC;
	}

	dump_monkey_info(sh);

	return 0;
}

static int cmd_lvgl_monkey_set(const struct shell *sh, size_t argc, char *argv[])
{
	int index;

	index = atoi(argv[1]);
	if (index < 0 || index >= CONFIG_LV_Z_MAX_MONKEY_COUNT || lvgl_monkeys[index] == NULL) {
		shell_error(sh, "Invalid monkey index");
		return -ENOEXEC;
	}

	if (strcmp(argv[2], "active") == 0) {
		lv_monkey_set_enable(lvgl_monkeys[index], true);
	} else if (strcmp(argv[2], "inactive") == 0) {
		lv_monkey_set_enable(lvgl_monkeys[index], false);
	} else {
		shell_error(sh, "Invalid monkey state %s", argv[2]);
		shell_help(sh);
		return SHELL_CMD_HELP_PRINTED;
	}

	dump_monkey_info(sh);

	return 0;
}
#endif /* CONFIG_LV_USE_MONKEY */

static int cmd_lvgl_stats(const struct shell *sh, size_t argc, char *argv[])
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_help(sh);
	return SHELL_CMD_HELP_PRINTED;
}

static int cmd_lvgl_stats_memory(const struct shell *sh, size_t argc, char *argv[])
{
#ifdef CONFIG_LV_Z_MEM_POOL_SYS_HEAP
	bool dump_chunks = false;

	if (argc == 2) {
		if (strcmp(argv[1], "-c") == 0) {
			dump_chunks = true;
		} else {
			shell_error(sh, "unsupported option %s", argv[1]);
			shell_help(sh);
			return SHELL_CMD_HELP_PRINTED;
		}
	}

	lvgl_print_heap_info(dump_chunks);
	return 0;
#else
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_error(sh, "Set CONFIG_LV_Z_MEM_POOL_SYS_HEAP to enable memory statistics support.");
	return -ENOTSUP;
#endif
}

SHELL_STATIC_SUBCMD_SET_CREATE(lvgl_cmd_stats,
			       SHELL_CMD_ARG(memory, NULL,
					     "Show LVGL memory statistics\n"
					     "Usage: lvgl stats memory [-c]\n"
					     "-c  dump chunk information",
					     cmd_lvgl_stats_memory, 1, 1),
			       SHELL_SUBCMD_SET_END);

#ifdef CONFIG_LV_USE_MONKEY
SHELL_STATIC_SUBCMD_SET_CREATE(
	lvgl_cmd_monkey,
	SHELL_CMD_ARG(create, NULL,
		      "Create a new monkey instance (default: pointer)\n"
		      "Usage: lvgl monkey create [pointer|keypad|button|encoder]",
		      cmd_lvgl_monkey_create, 1, 1),
	SHELL_CMD_ARG(set, NULL,
		      "Activate/deactive a monkey instance\n"
		      "Usage: lvgl monkey set <index> <active|inactive>\n",
		      cmd_lvgl_monkey_set, 3, 0),
	SHELL_SUBCMD_SET_END);
#endif /* CONFIG_LV_USE_MONKEY */

SHELL_STATIC_SUBCMD_SET_CREATE(
	lvgl_cmds, SHELL_CMD(stats, &lvgl_cmd_stats, "Show LVGL statistics", cmd_lvgl_stats),
#ifdef CONFIG_LV_USE_MONKEY
	SHELL_CMD(monkey, &lvgl_cmd_monkey, "LVGL monkey testing", cmd_lvgl_monkey),
#endif /* CONFIG_LV_USE_MONKEY */
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(lvgl, &lvgl_cmds, "LVGL shell commands", NULL);
