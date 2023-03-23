#ifndef LV_TEST_MALLOC
#define LV_TEST_MALLOC

#include <stdlib.h>

typedef void * (* lv_malloc_stub_cb)(size_t);

void lv_test_malloc_init(void);
void lv_test_malloc_set_cb(lv_malloc_stub_cb stub_malloc);
void * lv_test_malloc(size_t s);

#endif