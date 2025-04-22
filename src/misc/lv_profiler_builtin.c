/**
 * @file lv_profiler_builtin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_profiler_builtin_private.h"
#include "../lvgl.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#define profiler_ctx LV_GLOBAL_DEFAULT()->profiler_context

#define LV_PROFILER_STR_MAX_LEN 128
#define LV_PROFILER_TICK_PER_SEC_MAX 1000000000 /* Maximum accuracy: 1 nanosecond */

#if LV_USE_OS
    #define LV_PROFILER_MULTEX_INIT   lv_mutex_init(&profiler_ctx->mutex)
    #define LV_PROFILER_MULTEX_DEINIT lv_mutex_delete(&profiler_ctx->mutex)
    #define LV_PROFILER_MULTEX_LOCK   lv_mutex_lock(&profiler_ctx->mutex)
    #define LV_PROFILER_MULTEX_UNLOCK lv_mutex_unlock(&profiler_ctx->mutex)
#else
    #define LV_PROFILER_MULTEX_INIT
    #define LV_PROFILER_MULTEX_DEINIT
    #define LV_PROFILER_MULTEX_LOCK
    #define LV_PROFILER_MULTEX_UNLOCK
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Structure representing a built-in profiler item in LVGL
 */
typedef struct {
    uint64_t tick;     /**< The tick value of the profiler item */
    char tag;          /**< The tag of the profiler item */
    const char * func; /**< A pointer to the function associated with the profiler item */
#if LV_USE_OS
    int tid;           /**< The thread ID of the profiler item */
    int cpu;         /**< The CPU ID of the profiler item */
#endif
} lv_profiler_builtin_item_t;

/**
 * @brief Structure representing a context for the LVGL built-in profiler
 */
typedef struct _lv_profiler_builtin_ctx_t {
    lv_profiler_builtin_item_t * item_arr; /**< Pointer to an array of profiler items */
    uint32_t item_num;                     /**< Number of profiler items in the array */
    uint32_t cur_index;                    /**< Index of the current profiler item */
    lv_profiler_builtin_config_t config;   /**< Configuration for the built-in profiler */
    bool enable;                           /**< Whether the built-in profiler is enabled */
#if LV_USE_OS
    lv_mutex_t mutex;                      /**< Mutex to protect the built-in profiler */
#endif
} lv_profiler_builtin_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint64_t default_tick_get_cb(void);
static void default_flush_cb(const char * buf);
static int default_tid_get_cb(void);
static int default_cpu_get_cb(void);
static void flush_no_lock(void);

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
    config->tick_get_cb = default_tick_get_cb;
    config->flush_cb = default_flush_cb;
    config->tid_get_cb = default_tid_get_cb;
    config->cpu_get_cb = default_cpu_get_cb;
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
    if(profiler_ctx) {
        lv_profiler_builtin_uninit();
    }

    profiler_ctx = lv_malloc_zeroed(sizeof(lv_profiler_builtin_ctx_t));
    LV_ASSERT_MALLOC(profiler_ctx);

    profiler_ctx->item_arr = lv_malloc(num * sizeof(lv_profiler_builtin_item_t));
    LV_ASSERT_MALLOC(profiler_ctx->item_arr);
    if(profiler_ctx->item_arr == NULL) {
        lv_free(profiler_ctx);
        profiler_ctx = NULL;
        LV_LOG_ERROR("malloc failed for item_arr");
        return;
    }

    LV_PROFILER_MULTEX_INIT;
    profiler_ctx->item_num = num;
    profiler_ctx->config = *config;

    if(profiler_ctx->config.flush_cb) {
        /* add profiler header for perfetto */
        profiler_ctx->config.flush_cb("# tracer: nop\n");
        profiler_ctx->config.flush_cb("#\n");
    }

    lv_profiler_builtin_set_enable(LV_PROFILER_BUILTIN_DEFAULT_ENABLE);

    LV_LOG_INFO("init OK, item_num = %d", (int)num);
}

void lv_profiler_builtin_uninit(void)
{
    LV_ASSERT_NULL(profiler_ctx);
    LV_PROFILER_MULTEX_DEINIT;
    lv_free(profiler_ctx->item_arr);
    lv_free(profiler_ctx);
    profiler_ctx = NULL;
}

void lv_profiler_builtin_set_enable(bool enable)
{
    if(!profiler_ctx) {
        return;
    }

    profiler_ctx->enable = enable;
}

void lv_profiler_builtin_flush(void)
{
    LV_ASSERT_NULL(profiler_ctx);

    LV_PROFILER_MULTEX_LOCK;
    flush_no_lock();
    LV_PROFILER_MULTEX_UNLOCK;
}

void lv_profiler_builtin_write(const char * func, char tag)
{
    LV_ASSERT_NULL(profiler_ctx);
    LV_ASSERT_NULL(func);

    if(!profiler_ctx->enable) {
        return;
    }

    LV_PROFILER_MULTEX_LOCK;

    if(profiler_ctx->cur_index >= profiler_ctx->item_num) {
        flush_no_lock();
        profiler_ctx->cur_index = 0;
    }

    lv_profiler_builtin_item_t * item = &profiler_ctx->item_arr[profiler_ctx->cur_index];
    item->func = func;
    item->tag = tag;
    item->tick = profiler_ctx->config.tick_get_cb();

#if LV_USE_OS
    item->tid = profiler_ctx->config.tid_get_cb();
    item->cpu = profiler_ctx->config.cpu_get_cb();
#endif

    profiler_ctx->cur_index++;

    LV_PROFILER_MULTEX_UNLOCK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint64_t default_tick_get_cb(void)
{
    return lv_tick_get();
}

static void default_flush_cb(const char * buf)
{
    LV_LOG("%s", buf);
}

static int default_tid_get_cb(void)
{
    return 1;
}

static int default_cpu_get_cb(void)
{
    return 0;
}

static void flush_no_lock(void)
{
    if(!profiler_ctx->config.flush_cb) {
        LV_LOG_WARN("flush_cb is not registered");
        return;
    }

    uint32_t cur = 0;
    char buf[LV_PROFILER_STR_MAX_LEN];
    uint32_t tick_per_sec = profiler_ctx->config.tick_per_sec;
    while(cur < profiler_ctx->cur_index) {
        lv_profiler_builtin_item_t * item = &profiler_ctx->item_arr[cur++];
        uint64_t sec = item->tick / tick_per_sec;
        uint64_t nsec = (item->tick % tick_per_sec) * (LV_PROFILER_TICK_PER_SEC_MAX / tick_per_sec);

#if LV_USE_OS
        lv_snprintf(buf, sizeof(buf),
                    "   LVGL-%d [%d] %" LV_PRIu64 ".%09" LV_PRIu64 ": tracing_mark_write: %c|1|%s\n",
                    item->tid,
                    item->cpu,
                    sec,
                    nsec,
                    item->tag,
                    item->func);
#else
        lv_snprintf(buf, sizeof(buf),
                    "   LVGL-1 [0] %" LV_PRIu64 ".%09" LV_PRIu64 ": tracing_mark_write: %c|1|%s\n",
                    sec,
                    nsec,
                    item->tag,
                    item->func);
#endif
        profiler_ctx->config.flush_cb(buf);
    }
}

#endif /*LV_USE_PROFILER_BUILTIN*/
