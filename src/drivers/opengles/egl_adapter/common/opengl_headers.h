#ifndef LV_COMMON_OPENGL_HEADERS_H_
#define LV_COMMON_OPENGL_HEADERS_H_

#include "../../../../lv_conf_internal.h"

#if LV_EGL_ADAPTED_WITH_GL
    #include "../private/glad/include/glad/gl.h"
#elif LV_EGL_ADAPTED_WITH_GLESV2
    #include "../private/glad/include/glad/gles2.h"
#endif

#endif /* LV_COMMON_OPENGL_HEADERS_H_ */
