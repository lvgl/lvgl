#ifndef MK_CONF_H_
#define MK_CONF_H_
/* ****************************************************
 * These defines aren't required but can be used to
 * explictly specify display factors like vertical sync,
 * buffer mode, and resolution (in DRM output mode).
 *
 * If they are defined, it must be before the inclusion of
 * lv_egl_adapter.h. All of these can be over-ridden at
 * run-time by setting the respective environment variable
 * prior to the application command, for example
 * "LV_EGL_SYNC=0 my_app" would run my_app with vertical sync
 * disabled.  The env var's have the same name, minus the
 * double underscore prefix.  If you use these in your code,
 * use the version without the prefix, and it will be either
 * the value you define below, or the user specified override.
 */

#define __LV_EGL_HOR_RES 640
#define __LV_EGL_VER_RES 480
#define __LV_EGL_REFR 60.0
#define __LV_EGL_SYNC 1
#define __LV_EGL_DEVICE ""

/* Below are several common buffer mode options, there's many
 * more.  If not specified, the driver will select the best
 * option with an alpha channel and depth buffer.
 */

//#define __LV_EGL_BUFFER_MODE 0x0      // Best all-purpose auto
#define __LV_EGL_BUFFER_MODE 0x13     // R5-G6-B5-A0-D0
//#define __LV_EGL_BUFFER_MODE 0x16     // R5-G6-B5-A0-D24
//#define __LV_EGL_BUFFER_MODE 0x4      // R8-G8-B8-A8-D24
//#define __LV_EGL_BUFFER_MODE 0xd      // R8-G8-B8-A0-D24

#endif /* MK_CONF_H_ */
