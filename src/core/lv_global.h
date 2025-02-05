/**
 * @file lv_global.h
 *
 */

#ifndef LV_GLOBAL_H
#define LV_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"
#include "../draw/lv_draw.h"
#if LV_USE_DRAW_SW
#include "../draw/sw/lv_draw_sw.h"
#endif
#include "../misc/lv_anim.h"
#include "../misc/lv_area.h"
#include "../misc/lv_color_op.h"
#include "../misc/lv_ll.h"
#include "../misc/lv_log.h"
#include "../misc/lv_style.h"
#include "../misc/lv_timer.h"
#include "../osal/lv_os.h"
#include "../others/sysmon/lv_sysmon.h"
#include "../stdlib/builtin/lv_tlsf.h"

#if LV_USE_FONT_COMPRESSED
#include "../font/lv_font_fmt_txt_private.h"
#endif

#include "../tick/lv_tick.h"
#include "../layouts/lv_layout.h"

#include "../misc/lv_types.h"

#include "../misc/lv_timer_private.h"
#include "../misc/lv_anim_private.h"
#include "../tick/lv_tick_private.h"
#include "../draw/lv_draw_buf_private.h"
#include "../draw/lv_draw_private.h"
#include "../draw/sw/lv_draw_sw_private.h"
#include "../draw/sw/lv_draw_sw_mask_private.h"
#include "../stdlib/builtin/lv_tlsf_private.h"
#include "../others/sysmon/lv_sysmon_private.h"
#include "../layouts/lv_layout_private.h"

/*********************
 *      DEFINES
 *********************/
#define ZERO_MEM_SENTINEL  0xa1b2c3d4

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_SPAN != 0
struct _snippet_stack;
#endif

#if LV_USE_FREETYPE
struct _lv_freetype_context_t;
#endif

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
struct _lv_profiler_builtin_ctx_t;
#endif

#if LV_USE_NUTTX
struct _lv_nuttx_ctx_t;
#endif

typedef struct _lv_global_t {
    bool inited;
    bool deinit_in_progress;     /**< Can be used e.g. in the LV_EVENT_DELETE to deinit the drivers too */

    lv_ll_t disp_ll;
    lv_display_t * disp_refresh;
    lv_display_t * disp_default;

    lv_ll_t style_trans_ll;
    bool style_refresh;
    uint32_t style_custom_table_size;
    uint32_t style_last_custom_prop_id;
    uint8_t * style_custom_prop_flag_lookup_table;

    lv_ll_t group_ll;
    lv_group_t * group_default;

    lv_ll_t indev_ll;
    lv_indev_t * indev_active;
    lv_obj_t * indev_obj_active;

    uint32_t layout_count;
    lv_layout_dsc_t * layout_list;
    bool layout_update_mutex;

    uint32_t memory_zero;
    uint32_t math_rand_seed;

    lv_event_t * event_header;
    uint32_t event_last_register_id;

    lv_timer_state_t timer_state;
    lv_anim_state_t anim_state;
    lv_tick_state_t tick_state;

    lv_draw_buf_handlers_t draw_buf_handlers;
    lv_draw_buf_handlers_t font_draw_buf_handlers;
    lv_draw_buf_handlers_t image_cache_draw_buf_handlers;  /**< Ensure that all assigned draw buffers
                                                            * can be managed by image cache. */

    lv_ll_t img_decoder_ll;

    lv_cache_t * img_cache;
    lv_cache_t * img_header_cache;

    lv_draw_global_info_t draw_info;
#if defined(LV_DRAW_SW_SHADOW_CACHE_SIZE) && LV_DRAW_SW_SHADOW_CACHE_SIZE > 0
    lv_draw_sw_shadow_cache_t sw_shadow_cache;
#endif
#if LV_DRAW_SW_COMPLEX
    lv_draw_sw_mask_radius_circle_dsc_arr_t sw_circle_cache;
#endif

#if LV_USE_LOG
    lv_log_print_g_cb_t custom_log_print_cb;
#endif

#if LV_USE_LOG && LV_LOG_USE_TIMESTAMP
    uint32_t log_last_log_time;
#endif

#if LV_USE_THEME_SIMPLE
    void * theme_simple;
#endif

#if LV_USE_THEME_DEFAULT
    void * theme_default;
#endif

#if LV_USE_THEME_MONO
    void * theme_mono;
#endif

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    lv_tlsf_state_t tlsf_state;
#endif

    lv_ll_t fsdrv_ll;
#if LV_USE_FS_STDIO != '\0'
    lv_fs_drv_t stdio_fs_drv;
#endif
#if LV_USE_FS_POSIX
    lv_fs_drv_t posix_fs_drv;
#endif

#if LV_USE_FS_FATFS
    lv_fs_drv_t fatfs_fs_drv;
#endif

#if LV_USE_FS_WIN32 != '\0'
    lv_fs_drv_t win32_fs_drv;
#endif

#if LV_USE_FS_UEFI
    lv_fs_drv_t uefi_fs_drv;
#endif

#if LV_USE_FS_LITTLEFS
    lv_fs_drv_t littlefs_fs_drv;
#endif

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    lv_fs_drv_t arduino_esp_littlefs_fs_drv;
#endif

#if LV_USE_FS_ARDUINO_SD
    lv_fs_drv_t arduino_sd_fs_drv;
#endif

#if LV_USE_FREETYPE
    struct _lv_freetype_context_t * ft_context;
#endif

#if LV_USE_FONT_COMPRESSED
    lv_font_fmt_rle_t font_fmt_rle;
#endif

#if LV_USE_SPAN != 0
    struct _snippet_stack * span_snippet_stack;
#endif

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    struct _lv_profiler_builtin_ctx_t * profiler_context;
#endif

#if LV_USE_FILE_EXPLORER != 0
    lv_style_t fe_list_button_style;
#endif

#if LV_USE_MEM_MONITOR
    lv_sysmon_backend_data_t sysmon_mem;
#endif

#if LV_USE_IME_PINYIN != 0
    size_t ime_cand_len;
#endif

#if LV_USE_OBJ_ID_BUILTIN
    void * objid_array;
    uint32_t objid_count;
#endif

#if LV_USE_NUTTX
    struct _lv_nuttx_ctx_t * nuttx_ctx;
#endif

#if LV_USE_OS != LV_OS_NONE
    lv_mutex_t lv_general_mutex;
#endif

#if LV_USE_OS == LV_OS_FREERTOS
    uint32_t freertos_idle_time_sum;
    uint32_t freertos_non_idle_time_sum;
    uint32_t freertos_task_switch_timestamp;
    bool freertos_idle_task_running;
#endif

#if LV_USE_EVDEV
    lv_evdev_discovery_t * evdev_discovery;
#endif

    void * user_data;
} lv_global_t;

/**********************
 *      MACROS
 **********************/

#if LV_ENABLE_GLOBAL_CUSTOM
#include LV_GLOBAL_CUSTOM_INCLUDE

#ifndef LV_GLOBAL_CUSTOM
#define LV_GLOBAL_CUSTOM() lv_global_default()
#endif
#define LV_GLOBAL_DEFAULT() LV_GLOBAL_CUSTOM()
#else
LV_ATTRIBUTE_EXTERN_DATA extern lv_global_t lv_global;
#define LV_GLOBAL_DEFAULT() (&lv_global)
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_ENABLE_GLOBAL_CUSTOM
/**
 * Get the default global object for current thread
 * @return  pointer to the default global object
 */
lv_global_t * lv_global_default(void);
#endif
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLOBAL_H*/
