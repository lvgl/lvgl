#include "lv_test_malloc.h"

#ifndef LVGL_CI_USING_SYS_HEAP
    #include "../../src/misc/lv_malloc_builtin.h"
#endif

bool lv_control_lv_malloc;
void * lv_control_lv_malloc_stub;

void lv_malloc_test_init(void)
{
    lv_control_lv_malloc = false;
    lv_control_lv_malloc_stub = NULL;
}

void * lv_malloc_test_wrapper(size_t size)
{
    if(lv_control_lv_malloc) {
        (void) size;
        return lv_control_lv_malloc_stub;
    }
    else {
#ifdef LVGL_CI_USING_SYS_HEAP
        return malloc(size);
#else // LVGL_CI_USING_DEF_HEAP and others
        return lv_malloc_builtin(size);
#endif
    }
}