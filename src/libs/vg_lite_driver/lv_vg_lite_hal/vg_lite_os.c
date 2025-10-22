#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include "../../../lvgl.h"

void * vg_lite_os_malloc(uint32_t size)
{
    return lv_malloc(size);
}

void vg_lite_os_free(void * memory)
{
    lv_free(memory);
}

#endif
