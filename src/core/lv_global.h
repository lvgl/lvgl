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

#include <stdbool.h>
#include "../draw/lv_img_cache.h"
#include "../draw/sw/lv_draw_sw.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_area.h"
#include "../misc/lv_color_op.h"
#include "../misc/lv_log.h"
#include "../misc/lv_profiler_builtin.h"
#include "../misc/lv_style.h"
#include "../misc/lv_timer.h"
#include "../osal/lv_os.h"
#include "../stdlib/builtin/lv_tlsf.h"

#if LV_USE_FONT_COMPRESSED
#include "../font/lv_font_fmt_txt.h"
#endif

#if LV_USE_SYSMON
#include "../others/sysmon/lv_sysmon.h"
#endif
/*********************
 *      DEFINES
 *********************/
#define ZERO_MEM_SENTINEL  0xa1b2c3d4

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_disp_t;
struct _lv_group_t;
struct _my_theme_t;
struct _lv_indev_t;
struct _lv_event_t;
struct _lv_obj_t;

#if LV_USE_SPAN != 0
struct _snippet_stack;
#endif

#if LV_USE_FREETYPE
struct _lv_freetype_context_t;
#endif

typedef struct {
    bool inited;

    bool style_refresh;
    struct _lv_disp_t * disp_refresh;
    struct _lv_group_t * group_default;
    struct _lv_disp_t * disp_default;
    lv_img_cache_manager_t img_cache_mgr;

    struct _lv_indev_t * indev_active;
    struct _lv_obj_t * indev_obj_active;

    uint32_t layout_count;
    uint32_t memory_zero;

    uint32_t math_rand_seed;
    bool layout_update_mutex;

    lv_area_transform_cache_t area_trans_cache;

    uint32_t style_custom_table_size;
    uint16_t style_last_custom_prop_id;

    struct _lv_event_t * event_header;
    uint32_t event_last_register_id;

    lv_log_print_g_cb_t custom_log_print_cb;
#if LV_LOG_USE_TIMESTAMP
    uint32_t log_last_log_time;
#endif

    lv_timer_state_t timer_state;
    lv_anim_state_t anim_state;

#if !LV_TICK_CUSTOM
    uint32_t tick_sys_time;
    volatile uint8_t tick_irq_flag;
#endif

#if LV_IMG_CACHE_DEF_SIZE
    uint16_t img_cache_entry_cnt;
#endif

    uint32_t draw_layer_used_mem_kb;
#if LV_USE_OS
    lv_thread_sync_t draw_sync;
    lv_mutex_t draw_sw_circle_cache_mutex;
#else
    int draw_dispatch_req;
#endif

#if defined(LV_DRAW_SW_SHADOW_CACHE_SIZE) && LV_DRAW_SW_SHADOW_CACHE_SIZE > 0
    lv_draw_sw_shadow_cache_t sw_shadow_cache;
#endif

#if LV_USE_THEME_BASIC
    struct _my_theme_t * theme_basic;
#endif

#if LV_USE_THEME_DEFAULT
    struct _my_theme_t * theme_default;
#endif

#if LV_USE_THEME_MONO
    struct _my_theme_t * theme_mono;
#endif

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    lv_tlsf_state_t tlsf_state;
#endif

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
    lv_profiler_builtin_ctx_t profiler_context;
#endif

#if LV_USE_MSG
    bool msg_restart_notify;
    unsigned int msg_recursion_counter;
#endif

#if LV_USE_FILE_EXPLORER != 0
    lv_style_t fe_list_btn_style;
#endif

#if LV_USE_SYSMON
    perf_info_t sysmon_perf_info;
#endif

#if LV_USE_IME_PINYIN != 0
    uint16_t ime_cand_len;
#endif
} lv_global_t;


/**********************
 *      MACROS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the default global object for current thread
 * @return  pointer to the default global object
 */
lv_global_t * lv_global_default();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLOBAL_H*/
