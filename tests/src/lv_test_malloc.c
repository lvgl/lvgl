#include "lv_test_malloc.h"

#ifndef LVGL_CI_USING_SYS_HEAP
    #include "../../src/misc/lv_malloc_builtin.h"
#endif

static lv_malloc_stub_cb malloc_stub_cb;

void lv_test_malloc_init(void)
{
    malloc_stub_cb = NULL;
}

void lv_test_malloc_set_cb(lv_malloc_stub_cb stub_malloc)
{
    malloc_stub_cb = stub_malloc;
}

void * lv_test_malloc_normal(size_t size)
{
#ifdef LVGL_CI_USING_SYS_HEAP
    return malloc(size);
#else // LVGL_CI_USING_DEF_HEAP and others
    return lv_malloc_builtin(size);
#endif
}

void * lv_test_malloc(size_t s)
{
    if(malloc_stub_cb) return malloc_stub_cb(s);
    else return lv_test_malloc_normal(s);
}