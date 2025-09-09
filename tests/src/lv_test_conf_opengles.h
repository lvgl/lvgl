#ifndef LV_TEST_CONF_OPENGLES_H
#define LV_TEST_CONF_OPENGLES_H

#if !defined(NON_AMD64_BUILD) && !defined(_MSC_VER) && !defined(_WIN32)
    #define LV_USE_OPENGLES               1
    #define LV_USE_OPENGLES_DEBUG         1
    #define LV_USE_GLFW                   1
#endif

#endif /*LV_TEST_CONF_OPENGLES_H*/
