#ifndef LV_TEST_MALLOC
#define LV_TEST_MALLOC

#include <stdlib.h>

typedef void * (* lv_malloc_stub_cb)(size_t);
typedef void * (* lv_realloc_stub_cb)(void * p, size_t new_size);
typedef void (* lv_free_stub_cb)(void * p);

void lv_test_malloc_init(void);
void lv_test_malloc_set_cb(lv_malloc_stub_cb stub_malloc);
void * lv_test_malloc(size_t s);

void lv_test_realloc_set_cb(lv_realloc_stub_cb stub_realloc);
void * lv_test_realloc(void * p, size_t new_size);

void lv_test_free_set_cb(lv_free_stub_cb stub_free);
void lv_test_free(void * p);

#endif