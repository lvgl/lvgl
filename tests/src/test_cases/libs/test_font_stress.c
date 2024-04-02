#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE

#include "rnd_unicodes/lv_rnd_unicodes.h"

#if __WORDSIZE == 64
#define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(INDEX) \
    lv_snprintf(buf, sizeof(buf), "libs/font_stress/snapshot_%0d.lp64.png", (INDEX));\
    TEST_ASSERT_EQUAL_SCREENSHOT(buf)
#elif __WORDSIZE == 32
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
} lvx_font_stress_config_t;

typedef struct {
    lv_obj_t * par;
    lv_obj_t ** label_arr;
    lvx_font_stress_config_t config;
} stress_test_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void update_cb(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static stress_test_ctx_t g_ctx = { 0 };
static const char * font_name_arr[] = {
    "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
    "../src/libs/freetype/arial.ttf",
    "../demos/multilang/assets/fonts/Montserrat-Bold.ttf",
    "UNKNOWN_FONT_NAME"
};
static const uint16_t font_style[] = {
    LV_FREETYPE_FONT_STYLE_NORMAL,
    LV_FREETYPE_FONT_STYLE_ITALIC,
    LV_FREETYPE_FONT_STYLE_BOLD,
};

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

static lv_obj_t * label_create(const char * font_name, lv_obj_t * par, int size, int x, int y)
{
    uint32_t index = lv_rand(0, sizeof(font_style) / sizeof(uint16_t) - 1);
    uint32_t r = lv_rand(0, 0xFF);
    uint32_t g = lv_rand(0, 0xFF);
    uint32_t b = lv_rand(0, 0xFF);
    lv_opa_t opa = lv_rand(0, LV_OPA_COVER);
    lv_color_t color = lv_color_make(r, g, b);

    lv_font_t * font = lv_freetype_font_create(font_name, LV_FREETYPE_FONT_RENDER_MODE_BITMAP, size, font_style[index]);
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
static void label_delete(lv_obj_t * label)
{
    const lv_font_t * font = lv_obj_get_style_text_font(label, 0);
    LV_ASSERT_NULL(font);
    lv_freetype_font_delete((lv_font_t *)font);
    lv_obj_del(label);
}
static void label_delete_all(stress_test_ctx_t * ctx)
{
    for(int i = 0; i < ctx->config.label_cnt; i++) {
        lv_obj_t * label = ctx->label_arr[i];
        if(label) {
            label_delete(label);
            ctx->label_arr[i] = NULL;
        }
    }
}
static void update_cb(void)
{
    stress_test_ctx_t * ctx = &g_ctx;
    uint32_t label_index = lv_rand(0, ctx->config.label_cnt - 1);
    uint32_t font_index = lv_rand(0, ctx->config.font_cnt - 1);
    uint32_t font_size = lv_rand(0, MAX_FONT_SIZE);
    uint32_t label_x = lv_rand(0, LV_HOR_RES) - LV_HOR_RES / 2;
    uint32_t label_y = lv_rand(0, LV_VER_RES);

    lv_obj_t * label = ctx->label_arr[label_index];
    if(label) {
        label_delete(label);
        ctx->label_arr[label_index] = NULL;
    }
    else {
        const char * pathname = ctx->config.font_name_arr[font_index];
        LV_ASSERT_NULL(pathname);
        label = label_create(
                    pathname,
                    ctx->par,
                    (int)font_size,
                    (int)label_x,
                    (int)label_y);
        ctx->label_arr[label_index] = label;
    }
}

void setUp(void)
{
    lv_freetype_init(LV_FREETYPE_CACHE_FT_GLYPH_CNT);

    g_ctx.par = lv_scr_act();

    g_ctx.config.loop_cnt = MAX_LOOP_CNT;
    g_ctx.config.label_cnt = MAX_LABEL_CNT;
    g_ctx.config.font_name_arr = font_name_arr;
    g_ctx.config.font_cnt = sizeof(font_name_arr) / sizeof(font_name_arr[0]);

    size_t arr_size = sizeof(lv_obj_t *) * g_ctx.config.label_cnt;
    g_ctx.label_arr = lv_malloc(arr_size);
    LV_ASSERT_MALLOC(g_ctx.label_arr);
    lv_memzero(g_ctx.label_arr, arr_size);

    lv_rand_set_seed(RND_START_SEED);
}

void tearDown(void)
{
    label_delete_all(&g_ctx);
    lv_freetype_uninit();

    lv_free(g_ctx.label_arr);
}

void test_font_stress(void)
{
    for(uint32_t i = 0; g_ctx.config.loop_cnt > 0; g_ctx.config.loop_cnt--) {
        update_cb();
        lv_refr_now(NULL);

        if(g_ctx.config.loop_cnt % CAPTURE_SKIP_FRAMES == 0) {
            char buf[64];
            TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(i);
            i++;
        }
    }
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

#endif

#endif
