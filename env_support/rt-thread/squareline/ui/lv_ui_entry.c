/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-05-13     Meco Man      First version
 */

#ifdef __RTTHREAD__

void lv_user_gui_init(void)
{
    extern void ui_init(void);
    ui_init();
}

#endif /* __RTTHREAD__ */
