#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>

#if LV_BUILD_TEST && !defined(LV_BUILD_TEST_NO_MAIN)
#include <sys/time.h>
#include "lv_test_init.h"

int main(void)
{
   lv_test_init();

    printf("Exit with success!\n");
    return 0;
}

#endif
