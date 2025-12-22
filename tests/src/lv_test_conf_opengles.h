#ifndef LV_TEST_CONF_OPENGLES_H
#define LV_TEST_CONF_OPENGLES_H

#ifdef LV_DRAW_BUF_STRIDE_ALIGN
    #undef LV_DRAW_BUF_STRIDE_ALIGN
#endif

#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_ALIGN 4
#define LV_USE_EGL 1
#if defined(LV_USE_GLFW) && LV_USE_GLFW
    #undef LV_USE_GLFW
    #define LV_USE_GLFW 0
#endif

#define LV_USE_OPENGLES 1
#define LV_USE_DRAW_OPENGLES 1

/* Snapshot is not supported with draw opengles */
#define LV_USE_SNAPSHOT 0

/* Draw opengl doesn't support vector graphics for now */
#define LV_USE_LOTTIE 0
#define LV_USE_SVG 0
#define LV_USE_VECTOR_GRAPHIC 0
#define LV_USE_THORVG 0
#define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 128

#endif /*LV_TEST_CONF_OPENGLES_H*/
