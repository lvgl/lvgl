#if LV_BUILD_TEST
#include "../../lvgl.h"

int efi_main(void * image_handle, void * system_table)
{
    lv_init();
    return 0;
}

// memcpy is required as symbol for the clang compiler
void * memcpy(void * s, const void * ct, size_t n)
{
    const uint8_t * ct_8 = (const uint8_t *) ct;
    uint8_t * s_8 = (uint8_t *) s;

    while(n-- > 0) {
        *s_8++ = *ct_8++;
    }

    return s;
}

// memset is required as symbol for the clang compiler
void * memset(void * s, int c, size_t n)
{
    uint8_t * s_8 = (uint8_t *)s;

    while(n-- > 0) {
        *s_8++ = (uint8_t)c;
    }

    return s;
}

#endif
