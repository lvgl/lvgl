#ifndef LV_TEST_CONF_OPENGLES_H
#define LV_TEST_CONF_OPENGLES_H

#ifndef LV_USE_OPENGLES
    #if !defined(NON_AMD64_BUILD) && !defined(_MSC_VER) && !defined(_WIN32)
        #define LV_USE_OPENGLES               1
        #define LV_USE_OPENGLES_DEBUG         1
        #define LV_USE_EGL                    1
        #define LV_DRAW_BUF_ALIGN             4

        #ifdef LV_DRAW_BUF_STRIDE_ALIGN
            #undef LV_DRAW_BUF_STRIDE_ALIGN
            #define LV_DRAW_BUF_STRIDE_ALIGN 1 /* Not supported stride alignment yet */
        #endif/* LV_DRAW_BUF_STRIDE_ALIGN */
    #endif
#endif

#endif /*LV_TEST_CONF_OPENGLES_H*/
