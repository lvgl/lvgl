/**
 * @file lv_linux.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_os.h"

#if LV_USE_OS != LV_OS_NONE && defined(__linux__)

#include "../misc/lv_log.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

#define LV_UPTIME_MONITOR_FILE	       "/proc/stat"

#define LV_PROC_STAT_VAR_FORMAT	       " %" PRIu32
#define LV_PROC_STAT_IGNORE_VAR_FORMAT " %*" PRIu32

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t lv_proc_get_uptime(uint32_t *active, uint32_t *idle);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t last_active, last_idle;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_os_get_idle_percent(void)
{
	uint32_t delta_active, delta_idle;
	{
		uint32_t active, idle;

		lv_result_t err = lv_proc_get_uptime(&active, &idle);

		if (err == LV_RESULT_INVALID) {
			return UINT32_MAX;
		}

		delta_active = active - last_active;
		delta_idle = idle - last_idle;

		/* Update for next call */
		last_active = active;
		last_idle = idle;
	}

	/* From here onwards, there's no risk of overflowing as long as we call this function regularly */

	const uint32_t total = delta_active + delta_idle;

	if (total == 0) {
		return 0;
	}

	return (delta_idle * 100) / total;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t lv_proc_get_uptime(uint32_t *active, uint32_t *idle)
{
	FILE *fp = fopen(LV_UPTIME_MONITOR_FILE, "r");

	if (!fp) {
		LV_LOG_ERROR("Failed to open " LV_UPTIME_MONITOR_FILE);
		return LV_RESULT_INVALID;
	}

	int err = fscanf(
		fp,
		"cpu " LV_PROC_STAT_VAR_FORMAT LV_PROC_STAT_IGNORE_VAR_FORMAT
			LV_PROC_STAT_IGNORE_VAR_FORMAT LV_PROC_STAT_VAR_FORMAT,
		active, idle);

	fclose(fp);

	if (err != 2) {
		LV_LOG_ERROR("Failed to parse " LV_UPTIME_MONITOR_FILE);
		return LV_RESULT_INVALID;
	}
	return LV_RESULT_OK;
}

#endif
