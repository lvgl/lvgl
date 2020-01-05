#include "../lvgl.h"
#include <stdio.h>

#if LV_BUILD_TEST

int main(void)
{
    printf("Call lv_init...\n");
    lv_init();

    printf("Exit with success!\n");
    return 0;
}

#endif

