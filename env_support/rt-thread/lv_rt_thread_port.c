/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-18     Meco Man     The first version
 */

#ifdef __RTTHREAD__

#include <rtthread.h>
#define DBG_TAG    "LVGL"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

#include <lvgl.h>
#include <lv_port_disp.h>
#include <lv_port_indev.h>

#if LV_USE_LOG
static void lv_rt_log(const char *buf)
{
    LOG_I(buf);
}
#endif

static int lv_port_init(void)
{
#if LV_USE_LOG
    lv_log_register_print_cb(lv_rt_log);
#endif

    lv_init();

    lv_port_disp_init();
    lv_port_indev_init();

    return 0;
}
INIT_COMPONENT_EXPORT(lv_port_init);

#endif /*__RTTHREAD__*/
