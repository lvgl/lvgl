/**
 * @file lv_profiler_builtin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_profiler_builtin.h"
#include "../lvgl.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#define profiler_ctx LV_GLOBAL_DEFAULT()->profiler_context

#define LV_PROFILER_STR_MAX_LEN 128
#define LV_PROFILER_TICK_PER_SEC_MAX 1000000

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void default_flush_cb(const char * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_profiler_builtin_config_init(lv_profiler_builtin_config_t * config)
{
    LV_ASSERT_NULL(config);
    lv_memzero(config, sizeof(lv_profiler_builtin_config_t));
    config->buf_size = LV_PROFILER_BUILTIN_BUF_SIZE;
    config->tick_per_sec = 1000;
    config->tick_get_cb = lv_tick_get;
    config->flush_cb = default_flush_cb;
}

void lv_profiler_builtin_init(const lv_profiler_builtin_config_t * config)
{
    LV_ASSERT_NULL(config);
    LV_ASSERT_NULL(config->tick_get_cb);

    uint32_t num = config->buf_size / sizeof(lv_profiler_builtin_item_t);
    if(num == 0) {
        LV_LOG_WARN("buf_size must > %d", (int)sizeof(lv_profiler_builtin_item_t));
        return;
    }

    if(config->tick_per_sec == 0 || config->tick_per_sec > LV_PROFILER_TICK_PER_SEC_MAX) {
        LV_LOG_WARN("tick_per_sec range must be between 1~%d", LV_PROFILER_TICK_PER_SEC_MAX);
        return;
    }

    /*Free the old item_arr memory*/
    if(profiler_ctx.item_arr != NULL) {
        lv_profiler_builtin_uninit();
    }

    lv_memzero(&profiler_ctx, sizeof(profiler_ctx));
    profiler_ctx.item_arr = lv_malloc(num * sizeof(lv_profiler_builtin_item_t));
    LV_ASSERT_MALLOC(profiler_ctx.item_arr);

    if(profiler_ctx.item_arr == NULL) {
        LV_LOG_ERROR("malloc failed for item_arr");
        return;
    }

    profiler_ctx.item_num = num;
    profiler_ctx.config = *config;

    if(profiler_ctx.config.flush_cb) {
        /* add profiler header for perfetto */
        profiler_ctx.config.flush_cb("# tracer: nop\n");
        profiler_ctx.config.flush_cb("#\n");
    }

    lv_profiler_builtin_set_enable(true);

    LV_LOG_INFO("init OK, item_num = %d", (int)num);
}

void lv_profiler_builtin_uninit(void)
{
    LV_ASSERT_NULL(profiler_ctx.item_arr);
    lv_free(profiler_ctx.item_arr);
    lv_memzero(&profiler_ctx, sizeof(profiler_ctx));
}

void lv_profiler_builtin_set_enable(bool enable)
{
    profiler_ctx.enable = enable;
}

void lv_profiler_builtin_flush(void)
{
    LV_ASSERT_NULL(profiler_ctx.item_arr);
    if(!profiler_ctx.config.flush_cb) {
        LV_LOG_WARN("flush_cb is not registered");
        return;
    }

    uint32_t cur = 0;
    char buf[LV_PROFILER_STR_MAX_LEN];
    uint32_t tick_per_sec = profiler_ctx.config.tick_per_sec;
    while(cur < profiler_ctx.cur_index) {
        lv_profiler_builtin_item_t * item = &profiler_ctx.item_arr[cur++];
        uint32_t sec = item->tick / tick_per_sec;
        uint32_t usec = (item->tick % tick_per_sec) * (LV_PROFILER_TICK_PER_SEC_MAX / tick_per_sec);
        lv_snprintf(buf, sizeof(buf),
                    "   LVGL-1 [0] %" LV_PRIu32 ".%06" LV_PRIu32 ": tracing_mark_write: %c|1|%s\n",
                    sec,
                    usec,
                    item->tag,
                    item->func);
        profiler_ctx.config.flush_cb(buf);
    }
}

void lv_profiler_builtin_write(const char * func, char tag)
{
    LV_ASSERT_NULL(profiler_ctx.item_arr);
    LV_ASSERT_NULL(func);

    if(!profiler_ctx.enable) {
        return;
    }

    if(profiler_ctx.cur_index >= profiler_ctx.item_num) {
        lv_profiler_builtin_flush();
        profiler_ctx.cur_index = 0;
    }

    lv_profiler_builtin_item_t * item = &profiler_ctx.item_arr[profiler_ctx.cur_index];
    item->func = func;
    item->tag = tag;
    item->tick = profiler_ctx.config.tick_get_cb();
    profiler_ctx.cur_index++;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void default_flush_cb(const char * buf)
{
    LV_LOG("%s", buf);
}

#endif /*LV_USE_PROFILER_BUILTIN*/
