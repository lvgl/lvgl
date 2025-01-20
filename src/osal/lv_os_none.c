#include "../lv_conf_internal.h"
#if LV_USE_OS == LV_OS_NONE
#include "lv_os.h"
#include "../misc/lv_timer.h"

uint32_t lv_os_get_idle_percent(void)
{
    return lv_timer_get_idle();
}

#endif
