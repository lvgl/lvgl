#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE && LV_USE_FONT_MANAGER

#include "rnd_unicodes/lv_rnd_unicodes.h"

#ifndef NON_AMD64_BUILD
#define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(INDEX) \
    lv_snprintf(buf, sizeof(buf), "libs/font_stress/snapshot_%0d.lp64.png", (INDEX));\
    TEST_ASSERT_EQUAL_SCREENSHOT(buf)
#else
#define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(INDEX) \
    lv_snprintf(buf, sizeof(buf), "libs/font_stress/snapshot_%0d.lp32.png", (INDEX));\
    TEST_ASSERT_EQUAL_SCREENSHOT(buf)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char ** font_name_arr;
    int font_cnt;
    int label_cnt;
    int loop_cnt;
} font_stress_config_t;

struct _font_stress_ctx_t;

typedef lv_font_t * (*font_create_cb_t)(struct _font_stress_ctx_t * ctx,
                                        const char * name,
                                        lv_freetype_font_render_mode_t render_mode,
                                        uint32_t size,
                                        lv_freetype_font_style_t style);

typedef void (*font_delete_cb_t)(struct _font_stress_ctx_t * ctx, lv_font_t * font);

typedef struct _font_stress_ctx_t {
    font_stress_config_t config;
    lv_obj_t * par;
    lv_obj_t ** label_arr;
    lv_font_manager_t * font_manager;
    font_create_cb_t font_create_cb;
    font_delete_cb_t font_delete_cb;
} font_stress_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static font_stress_ctx_t g_ctx = { 0 };

/**********************
 *      MACROS
 **********************/

#define RND_START_SEED 0x114514
#define CAPTURE_SKIP_FRAMES 17

#define MAX_FONT_SIZE 128
#define MAX_LABEL_CNT 32
#define MAX_LOOP_CNT 200
#define MAX_LABEL_CONTENT_LEN 16

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * font_stress_label_create(font_stress_ctx_t * ctx, const char * font_name, lv_obj_t * par, int size,
                                           int x, int y)
{
    static const uint16_t font_style[] = {
        LV_FREETYPE_FONT_STYLE_NORMAL,
        LV_FREETYPE_FONT_STYLE_ITALIC,
        LV_FREETYPE_FONT_STYLE_BOLD,
    };

    uint32_t index = lv_rand(0, sizeof(font_style) / sizeof(uint16_t) - 1);
    uint32_t r = lv_rand(0, 0xFF);
    uint32_t g = lv_rand(0, 0xFF);
    uint32_t b = lv_rand(0, 0xFF);
    lv_opa_t opa = lv_rand(0, LV_OPA_COVER);
    lv_color_t color = lv_color_make(r, g, b);

    lv_font_t * font = ctx->font_create_cb(ctx, font_name, LV_FREETYPE_FONT_RENDER_MODE_BITMAP, size, font_style[index]);
    if(!font) {
        return NULL;
    }

    lv_obj_t * label = lv_label_create(par);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_opa(label, opa, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_outline_color(label, color, 0);
    lv_obj_set_style_outline_width(label, 1, 0);

    uint8_t str[128];
    lv_random_utf8_chars(str, sizeof(str), LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE, LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE_LEN,
                         MAX_LABEL_CONTENT_LEN);
    lv_label_set_text(label, (char *)str);
    return label;
}

static void font_stress_label_delete(font_stress_ctx_t * ctx, lv_obj_t * label)
{
    const lv_font_t * font = lv_obj_get_style_text_font(label, 0);
    TEST_ASSERT_NOT_NULL(font);
    lv_obj_del(label);
    ctx->font_delete_cb(ctx, (lv_font_t *)font);
}

static void font_stress_label_delete_all(font_stress_ctx_t * ctx)
{
    for(int i = 0; i < ctx->config.label_cnt; i++) {
        lv_obj_t * label = ctx->label_arr[i];
        if(label) {
            font_stress_label_delete(ctx, label);
            ctx->label_arr[i] = NULL;
        }
    }
}

static void font_stress_update(font_stress_ctx_t * ctx)
{
    uint32_t label_index = lv_rand(0, ctx->config.label_cnt - 1);
    uint32_t font_index = lv_rand(0, ctx->config.font_cnt - 1);
    uint32_t font_size = lv_rand(0, MAX_FONT_SIZE);
    uint32_t label_x = lv_rand(0, LV_HOR_RES) - LV_HOR_RES / 2;
    uint32_t label_y = lv_rand(0, LV_VER_RES);

    lv_obj_t * label = ctx->label_arr[label_index];
    if(label) {
        font_stress_label_delete(ctx, label);
        ctx->label_arr[label_index] = NULL;
    }
    else {
        const char * pathname = ctx->config.font_name_arr[font_index];
        TEST_ASSERT_NOT_NULL(pathname);
        label = font_stress_label_create(
                    ctx,
                    pathname,
                    ctx->par,
                    (int)font_size,
                    (int)label_x,
                    (int)label_y);
        ctx->label_arr[label_index] = label;
    }
}

static lv_font_t * freetype_font_create_cb(font_stress_ctx_t * ctx,
                                           const char * name,
                                           lv_freetype_font_render_mode_t render_mode,
                                           uint32_t size,
                                           lv_freetype_font_style_t style)
{
    LV_UNUSED(ctx);
    return lv_freetype_font_create(name, render_mode, size, style);
}

static void freetype_font_delete_cb(font_stress_ctx_t * ctx, lv_font_t * font)
{
    LV_UNUSED(ctx);
    lv_freetype_font_delete(font);
}

static lv_font_t * font_manager_font_create_cb(font_stress_ctx_t * ctx,
                                               const char * name,
                                               lv_freetype_font_render_mode_t render_mode,
                                               uint32_t size,
                                               lv_freetype_font_style_t style)
{
    TEST_ASSERT_NOT_NULL(ctx->font_manager);
    return lv_font_manager_create_font(ctx->font_manager, name, render_mode, size, style, LV_FONT_KERNING_NONE);
}

static void font_manager_font_delete_cb(font_stress_ctx_t * ctx, lv_font_t * font)
{
    TEST_ASSERT_NOT_NULL(ctx->font_manager);
    lv_font_manager_delete_font(ctx->font_manager, font);
}

void setUp(void)
{
    lv_freetype_init(LV_FREETYPE_CACHE_FT_GLYPH_CNT);

    g_ctx.par = lv_screen_active();
    g_ctx.config.label_cnt = MAX_LABEL_CNT;
    g_ctx.label_arr = lv_malloc_zeroed(sizeof(lv_obj_t *) * g_ctx.config.label_cnt);
    TEST_ASSERT_NOT_NULL(g_ctx.label_arr);
}

void tearDown(void)
{
    font_stress_label_delete_all(&g_ctx);

    lv_freetype_uninit();
    lv_free(g_ctx.label_arr);
    g_ctx.label_arr = NULL;
}

void test_font_stress(void)
{
    lv_rand_set_seed(RND_START_SEED);

    static const char * font_name_arr[] = {
        "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
        "../src/libs/freetype/arial.ttf",
        "./src/test_files/fonts/Montserrat-Bold.ttf",
        "UNKNOWN_FONT_NAME"
    };

    g_ctx.config.loop_cnt = MAX_LOOP_CNT;
    g_ctx.config.font_name_arr = font_name_arr;
    g_ctx.config.font_cnt = sizeof(font_name_arr) / sizeof(font_name_arr[0]);
    g_ctx.font_create_cb = freetype_font_create_cb;
    g_ctx.font_delete_cb = freetype_font_delete_cb;

    for(uint32_t i = 0; g_ctx.config.loop_cnt > 0; g_ctx.config.loop_cnt--) {
        font_stress_update(&g_ctx);
        lv_refr_now(NULL);

        if(g_ctx.config.loop_cnt % CAPTURE_SKIP_FRAMES == 0) {
            char buf[64];
            TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(i);
            i++;
        }
    }

    font_stress_label_delete_all(&g_ctx);
}

void test_font_manager_stress(void)
{
    lv_rand_set_seed(RND_START_SEED);

    g_ctx.font_manager = lv_font_manager_create(2);
    TEST_ASSERT_NOT_NULL(g_ctx.font_manager);
    lv_font_manager_add_src_static(g_ctx.font_manager, "NotoSansSC-Regular",
                                   "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                   &lv_freetype_font_class);
    lv_font_manager_add_src_static(g_ctx.font_manager, "Arial", "../src/libs/freetype/arial.ttf", &lv_freetype_font_class);
    lv_font_manager_add_src(g_ctx.font_manager, "Montserrat-Bold", "./src/test_files/fonts/Montserrat-Bold.ttf",
                            &lv_freetype_font_class);
    lv_font_manager_add_src(g_ctx.font_manager, "UNKNOWN", "UNKNOWN_FONT_PATH", &lv_freetype_font_class);

    static const char * font_name_arr[] = {
        "NotoSansSC-Regular,Arial",
        "Arial",
        "Montserrat-Bold",
        "UNKNOWN"
    };

    g_ctx.config.loop_cnt = MAX_LOOP_CNT;
    g_ctx.config.font_name_arr = font_name_arr;
    g_ctx.config.font_cnt = sizeof(font_name_arr) / sizeof(font_name_arr[0]);
    g_ctx.font_create_cb = font_manager_font_create_cb;
    g_ctx.font_delete_cb = font_manager_font_delete_cb;

    for(uint32_t i = 0; g_ctx.config.loop_cnt > 0; g_ctx.config.loop_cnt--) {
        font_stress_update(&g_ctx);
        lv_refr_now(NULL);

        if(g_ctx.config.loop_cnt % CAPTURE_SKIP_FRAMES == 0) {
            char buf[64];
            TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(i);
            i++;
        }
    }

    font_stress_label_delete_all(&g_ctx);

    bool remove_ok = lv_font_manager_remove_src(g_ctx.font_manager, "Arial");
    TEST_ASSERT_TRUE(remove_ok);

    remove_ok = lv_font_manager_remove_src(g_ctx.font_manager, "UNKNOWN");
    TEST_ASSERT_TRUE(remove_ok);

    bool success = lv_font_manager_delete(g_ctx.font_manager);
    TEST_ASSERT_TRUE(success);
    g_ctx.font_manager = NULL;
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_font_stress(void)
{
}

void test_font_manager_stress(void)
{
}

#endif

#endif
