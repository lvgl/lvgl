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
#include <lvgl.h>
#define DBG_TAG    "LVGL"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

#ifndef PKG_USING_LVGL_DISP_DEVICE
#  include <lv_port_disp.h>
#endif
#ifndef PKG_USING_LVGL_INDEV_DEVICE
#  include <lv_port_indev.h>
#endif

#if LV_USE_LOG && LV_LOG_PRINTF
static void lv_rt_log(const char *buf)
{
    LOG_I(buf);
}
#endif

static int lv_port_init(void)
{
#if LV_USE_LOG && LV_LOG_PRINTF
    lv_log_register_print_cb(lv_rt_log);
#endif

    lv_init();

#ifndef PKG_USING_LVGL_DISP_DEVICE
    lv_port_disp_init();
#endif
#ifndef PKG_USING_LVGL_INDEV_DEVICE
    lv_port_indev_init();
#endif

    return 0;
}
INIT_COMPONENT_EXPORT(lv_port_init);

#endif /*__RTTHREAD__*/
