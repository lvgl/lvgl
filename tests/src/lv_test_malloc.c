#include "lv_test_malloc.h"

#ifndef LVGL_CI_USING_SYS_HEAP
    #include "../../src/misc/lv_malloc_builtin.h"
#endif

static lv_malloc_stub_cb malloc_stub_cb;
static lv_realloc_stub_cb realloc_stub_cb;
static lv_free_stub_cb free_stub_cb;

void lv_test_malloc_init(void)
{
    malloc_stub_cb = NULL;
    realloc_stub_cb = NULL;
    free_stub_cb = NULL;
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

void * lv_test_realloc_normal(void * p, size_t new_size)
{
#ifdef LVGL_CI_USING_SYS_HEAP
    return realloc(p, new_size);
#else // LVGL_CI_USING_DEF_HEAP and others
    return lv_realloc_builtin(p, new_size);
#endif
}

void * lv_test_malloc(size_t s)
{
    if(malloc_stub_cb) return malloc_stub_cb(s);
    else return lv_test_malloc_normal(s);
}

void lv_test_realloc_set_cb(lv_realloc_stub_cb stub_realloc)
{
    realloc_stub_cb = stub_realloc;
}

void * lv_test_realloc(void * p, size_t new_size)
{
    if(realloc_stub_cb) return realloc_stub_cb(p, new_size);
    else return lv_test_realloc_normal(p, new_size);
}

void lv_test_free_set_cb(lv_free_stub_cb stub_free)
{
    free_stub_cb = stub_free;
}

void lv_test_free_normal(void * p)
{
#ifdef LVGL_CI_USING_SYS_HEAP
    free(p);
#else // LVGL_CI_USING_DEF_HEAP and others
    lv_free_builtin(p);
#endif
}

void lv_test_free(void * p)
{
    if(free_stub_cb) free_stub_cb(p);
    else lv_test_free_normal(p);
}
