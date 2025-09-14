
#ifndef MK_CONF_INTERNAL_H_
#define MK_CONF_INTERNAL_H_
//#define LV_EGL_BUFFER_MODE 0x0      // Best auto
//#define LV_EGL_BUFFER_MODE 0x13     // R5-G6-B5-A0-D0
//#define LV_EGL_BUFFER_MODE 0x16     // R5-G6-B5-A0-D24
//#define LV_EGL_BUFFER_MODE 0x4      // R8-G8-B8-A8-D24

#ifndef __LV_EGL_BUFFER_MODE
    #define __LV_EGL_BUFFER_MODE 0x0
#endif /* __LV_EGL_BUFFER_MODE */
#ifndef __LV_EGL_HOR_RES
    #define __LV_EGL_HOR_RES 0
#endif /* __LV_EGL_HOR_RES */
#ifndef __LV_EGL_VER_RES
    #define __LV_EGL_VER_RES 0
#endif /* __LV_EGL_VER_RES */
#ifndef __LV_EGL_REFR
    #define __LV_EGL_REFR 60.0
#endif /* __LV_EGL_REFR */
#ifndef __LV_EGL_SYNC
    #define __LV_EGL_SYNC 1
#endif /* __LV_EGL_SYNC */
#ifndef __LV_EGL_DEVICE
    #define __LV_EGL_DEVICE ""
#endif /* __LV_EGL_DEVICE */
#define LV_EGL_BUFFER_MODE (getenv("LV_EGL_BUFFER_MODE") ? strtol(getenv("LV_EGL_BUFFER_MODE"), NULL, 16) : __LV_EGL_BUFFER_MODE)
#define LV_EGL_HOR_RES (getenv("LV_EGL_HOR_RES") ? atoi(getenv("LV_EGL_HOR_RES")) : __LV_EGL_HOR_RES)
#define LV_EGL_VER_RES (getenv("LV_EGL_VER_RES") ? atoi(getenv("LV_EGL_VER_RES")) : __LV_EGL_VER_RES)
#define LV_EGL_REFR (getenv("LV_EGL_REFR") ? (float)atoi(getenv("LV_EGL_REFR")) : __LV_EGL_REFR)
#define LV_EGL_SYNC (getenv("LV_EGL_SYNC") ? atoi(getenv("LV_EGL_SYNC")) : __LV_EGL_SYNC)
#define LV_EGL_DEVICE (getenv("LV_EGL_DEVICE") ? getenv("LV_EGL_DEVICE") : __LV_EGL_DEVICE )
#endif /* MK_CONF_INTERNAL_H_ */