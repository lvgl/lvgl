#ifndef LV_TEST_MALLOC
#define LV_TEST_MALLOC

#include <stdlib.h>
#include <stdbool.h>

extern bool lv_control_lv_malloc;
void * lv_control_lv_malloc_stub;

#define FAKE_MALLOC_RETURN(ret) \
    do {                                    \
        lv_control_lv_malloc = true;        \
        lv_control_lv_malloc_stub = ret;    \
    } while (0);

#define FAKE_STOP_RETURN    \
    do {                                    \
        lv_control_lv_malloc = false;       \
        lv_control_lv_malloc_stub = NULL;   \
    } while (0);

void lv_malloc_test_init(void);
void * lv_malloc_test_wrapper(size_t size);

#endif