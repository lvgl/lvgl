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

#define __LV_EGL_HOR_RES 1024
#define __LV_EGL_VER_RES 768
#define __LV_EGL_REFR 60.0
#define __LV_EGL_SYNC 1
#define __LV_EGL_DEVICE ""

/* Below are several common buffer mode options, there's many
 * more.  If not specified, the driver will select the best
 * option with an alpha channel and depth buffer.
 */

//#define __LV_EGL_BUFFER_MODE 0x0      // Select best all-purpose auto

// This define is platform specific and the actual value may vary.
// To see a full list of available options, don't set this value
// and make sure LV_LOG_LEVEL is at least 2, to show USER level
// messages.   That message is not displayed if this value is
// defined below (and is a valid mode).
#define __LV_EGL_BUFFER_MODE 0xd        // R5-G6-B5-A0-D0 (on this device)


#endif /* MK_CONF_H_ */
