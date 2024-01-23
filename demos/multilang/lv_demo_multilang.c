/**
 * @file lv_demo_multilang.c
 * Recommended resolution 470x640
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_multilang.h"

#if LV_USE_DEMO_MULTILANG

#include "../../src/indev/lv_indev_scroll.h"

/*********************
 *      DEFINES
 *********************/
#define CARD_HEIGHT     180

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const void * image;
    const char * name;
    const char * description;
} card_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void inactive_timer_cb(lv_timer_t * t);
static void card_create(lv_obj_t * parent, card_info_t * info);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_style_t style_card_cont;
static lv_style_t style_placeholder;
static lv_style_t style_card;
static lv_style_t style_avatar;
static lv_style_t style_btn;
static lv_style_t style_hide;

extern lv_font_t font_multilang_small;
extern lv_font_t font_multilang_large;

/**********************
 *      MACROS
 **********************/
#define CARD_INFO_SET(_image, _name, _description) {.image = _image, .name = _name, .description = _description}

LV_IMAGE_DECLARE(img_multilang_avatar_1);
LV_IMAGE_DECLARE(img_multilang_avatar_2);
LV_IMAGE_DECLARE(img_multilang_avatar_3);
LV_IMAGE_DECLARE(img_multilang_avatar_4);
LV_IMAGE_DECLARE(img_multilang_avatar_5);
LV_IMAGE_DECLARE(img_multilang_avatar_6);
LV_IMAGE_DECLARE(img_multilang_avatar_7);
LV_IMAGE_DECLARE(img_multilang_avatar_8);
LV_IMAGE_DECLARE(img_multilang_avatar_9);
LV_IMAGE_DECLARE(img_multilang_avatar_10);
LV_IMAGE_DECLARE(img_multilang_avatar_11);
LV_IMAGE_DECLARE(img_multilang_avatar_12);
LV_IMAGE_DECLARE(img_multilang_avatar_13);
LV_IMAGE_DECLARE(img_multilang_avatar_14);
LV_IMAGE_DECLARE(img_multilang_avatar_15);
LV_IMAGE_DECLARE(img_multilang_avatar_16);
LV_IMAGE_DECLARE(img_multilang_avatar_17);
LV_IMAGE_DECLARE(img_multilang_avatar_18);
LV_IMAGE_DECLARE(img_multilang_avatar_22);
LV_IMAGE_DECLARE(img_multilang_avatar_25);

static card_info_t card_info[] = {
    CARD_INFO_SET(&img_multilang_avatar_5, "Zhang Wei", "对编程和技术充满热情。 开源倡导者🚀"),
    CARD_INFO_SET(&img_multilang_avatar_18, "Ali Reza Karami", "عاشق تاریخ و عاشق همه چیز عتیقه. قسمت مورد علاقه من قرن 19 است."),
    CARD_INFO_SET(&img_multilang_avatar_25, "Sunita Kapoor", "🌍🌳 हरित कार्यकर्ता, एक स्थायी कल के लिए प्रयासरत। "),
    CARD_INFO_SET(&img_multilang_avatar_2, "Sofia Bianchi", "Aspirante romanziere con la passione per il caffè e i gatti 🐱"),
    CARD_INFO_SET(&img_multilang_avatar_3, "Matthew Parker", "Fitness enthusiast 💪 Always pushing to reach new goals"),
    CARD_INFO_SET(&img_multilang_avatar_12, "Shira Cohen", "קורא נלהב שצובר אוסף עצום של ספרים יקרים📚"),
    CARD_INFO_SET(&img_multilang_avatar_7, "Ahmad Al-Masri", "عاشق للأفلام وناقد سينمائي عرضي. معجب بستيفن سبيلبرغ (Steven Spielberg). "),
    CARD_INFO_SET(&img_multilang_avatar_4, "Ingrid Johansen", "Håpløs romantisk søker etter den spesielle personen ❤️"),
    CARD_INFO_SET(&img_multilang_avatar_14, "Anastasia Petrova", "Любитель приключений, опытный альпинист."),
    CARD_INFO_SET(&img_multilang_avatar_6, "Madison Reynolds", "Nature lover and avid hiker 🌳"),
    CARD_INFO_SET(&img_multilang_avatar_8, "Pierre Dubois", "Fanatique de sport ⚽ et fan de l'équipe à domicile."),
    CARD_INFO_SET(&img_multilang_avatar_9, "Benjamin Green", "Dog lover and proud owner of a furry friend 🐶"),
    CARD_INFO_SET(&img_multilang_avatar_10, "James Wilson", "Foodie with a passion for trying new restaurants and dishes"),
    CARD_INFO_SET(&img_multilang_avatar_11, "Olivia Marie Parker", "Gamer at heart, always up for a good round of Mario Kart "),
    CARD_INFO_SET(&img_multilang_avatar_1, "Ana Garcia Rodriguez", "Viajar por el mundo y probar nuevos alimentos. 🌍"),
    CARD_INFO_SET(&img_multilang_avatar_13, "Jamal Brown", "Photographer and amateur astronomer 📸"),
    CARD_INFO_SET(&img_multilang_avatar_15, "Pavel Svoboda", "Hudebník a návštěvník koncertů"),
    CARD_INFO_SET(&img_multilang_avatar_16, "Elin Lindqvist", "Språkinlärare och kulturentusiast "),
    CARD_INFO_SET(&img_multilang_avatar_17, "William Carter", "DIY enthusiast and home improvement guru"),
    CARD_INFO_SET(&img_multilang_avatar_22, "Ava Williams", "Artist and creative visionary 🎨"),
    CARD_INFO_SET(NULL, NULL, NULL),
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static const void * get_imgfont_path(const lv_font_t * font,
                                     uint32_t unicode, uint32_t unicode_next,
                                     int32_t * offset_y, void * user_data)
{
    LV_UNUSED(user_data);
    LV_UNUSED(unicode_next);
    LV_UNUSED(font);

    LV_IMAGE_DECLARE(img_emoji_artist_palette);
    LV_IMAGE_DECLARE(img_emoji_books);
    LV_IMAGE_DECLARE(img_emoji_camera_with_flash);
    LV_IMAGE_DECLARE(img_emoji_cat_face);
    LV_IMAGE_DECLARE(img_emoji_deciduous_tree);
    LV_IMAGE_DECLARE(img_emoji_dog_face);
    LV_IMAGE_DECLARE(img_emoji_earth_globe_europe_africa);
    LV_IMAGE_DECLARE(img_emoji_flexed_biceps);
    LV_IMAGE_DECLARE(img_emoji_movie_camera);
    LV_IMAGE_DECLARE(img_emoji_red_heart);
    LV_IMAGE_DECLARE(img_emoji_soccer_ball);
    LV_IMAGE_DECLARE(img_emoji_rocket);

    *offset_y = -1;
    switch(unicode) {
        case 0x1F30D:
            return &img_emoji_earth_globe_europe_africa;;
        case 0x1F431:
            return &img_emoji_cat_face;
        case 0x1F4AA:
            return &img_emoji_flexed_biceps;
        case 0x2764:
            return &img_emoji_red_heart;
        case 0x1F333:
            return &img_emoji_deciduous_tree;
        case 0x1F3A5:
            return &img_emoji_movie_camera;
        case 0x26BD:
            return &img_emoji_soccer_ball;
        case 0x1F436:
            return &img_emoji_dog_face;
        case 0x1F4DA:
            return &img_emoji_books;
        case 0x1F4F8:
            return &img_emoji_camera_with_flash;
        case 0x1F3A8:
            return &img_emoji_artist_palette;
        case 0x1F680:
            return &img_emoji_rocket;
        default:
            return NULL;
    }
}

lv_font_t * emoji_font;

void lv_demo_multilang(void)
{
    emoji_font = lv_imgfont_create(20, get_imgfont_path, NULL);
    font_multilang_small.fallback = emoji_font;

    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_screen_active(), 0, 0);
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xececec), 0);
    lv_obj_set_style_bg_grad_color(lv_screen_active(), lv_color_hex(0xf9f9f9), 0);
    lv_obj_set_style_bg_grad_dir(lv_screen_active(), LV_GRAD_DIR_HOR, 0);

    static const int32_t grid_cols[] = {LV_GRID_CONTENT, 4, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {LV_GRID_CONTENT, -10, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_style_init(&style_card_cont);
    lv_style_set_width(&style_card_cont, lv_pct(100));
    lv_style_set_height(&style_card_cont, CARD_HEIGHT);
    lv_style_set_layout(&style_card_cont, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&style_card_cont, LV_FLEX_FLOW_ROW);
    lv_style_set_bg_opa(&style_card_cont, 0);
    lv_style_set_border_opa(&style_card_cont, 0);
    lv_style_set_pad_top(&style_card_cont, 5);
    lv_style_set_pad_bottom(&style_card_cont, 10);
    lv_style_set_text_font(&style_card_cont, &font_multilang_small);

    lv_style_init(&style_card);
    lv_style_set_width(&style_card, lv_pct(100));
    lv_style_set_height(&style_card, lv_pct(100));
    lv_style_set_layout(&style_card, LV_LAYOUT_GRID);
    lv_style_set_grid_column_dsc_array(&style_card, grid_cols);
    lv_style_set_grid_row_dsc_array(&style_card, grid_rows);
    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_offset_y(&style_card, 5);
    lv_style_set_shadow_color(&style_card, lv_color_hex3(0xccc));
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_base_dir(&style_card, LV_BASE_DIR_AUTO);

    lv_style_init(&style_hide);
    lv_style_set_width(&style_hide, lv_pct(100));
    lv_style_set_height(&style_hide, lv_pct(100));
    lv_style_set_bg_color(&style_hide, lv_color_hex(0x759efe));
    lv_style_set_bg_grad_color(&style_hide, lv_color_hex(0x4173ff));
    lv_style_set_bg_grad_dir(&style_hide, LV_GRAD_DIR_HOR);
    lv_style_set_radius(&style_hide, 12);
    lv_style_set_text_font(&style_hide, &font_multilang_large);
    lv_style_set_text_color(&style_hide, lv_color_hex(0xffffff));

    lv_style_init(&style_placeholder);
    lv_style_set_width(&style_placeholder, lv_pct(100));
    lv_style_set_height(&style_placeholder, lv_pct(100));
    lv_style_set_bg_opa(&style_placeholder, 0);
    lv_style_set_border_opa(&style_placeholder, 0);

    lv_style_init(&style_avatar);
    lv_style_set_shadow_width(&style_avatar, 20);
    lv_style_set_shadow_offset_y(&style_avatar, 5);
    lv_style_set_shadow_color(&style_avatar, lv_color_hex3(0xbbb));
    lv_style_set_radius(&style_avatar, LV_RADIUS_CIRCLE);

    lv_style_init(&style_btn);
    lv_style_set_width(&style_btn, 160);
    lv_style_set_height(&style_btn, 37);
    lv_style_set_bg_color(&style_btn, lv_color_hex(0x759efe));
    lv_style_set_bg_grad_color(&style_btn, lv_color_hex(0x4173ff));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_HOR);
    lv_style_set_radius(&style_btn, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&style_btn, 0);

    uint32_t i;
    for(i = 0; card_info[i].image; i++) {
        card_create(lv_screen_active(), &card_info[i]);
    }

    lv_timer_create(inactive_timer_cb, 1000, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void inactive_timer_cb(lv_timer_t * t)
{
    LV_UNUSED(t);

    static bool scrolled = false;
    lv_obj_t * cont = lv_obj_get_child(lv_screen_active(), 0);
    if(cont == NULL) return;

    if(scrolled) {
        lv_obj_scroll_by(cont, -100, 0, LV_ANIM_ON);
        lv_obj_remove_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        scrolled = false;
        return;
    }

    if(lv_display_get_inactive_time(NULL) > 8000) {
        lv_display_trigger_activity(NULL);
        lv_obj_scroll_by(cont, 100, 0, LV_ANIM_ON);
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        scrolled = true;
    }
}

static void shrink_anim_cb(void * var, int32_t v)
{
    lv_obj_t * cont = var;
    lv_obj_set_height(cont, (v * CARD_HEIGHT) >> 8);
    lv_obj_set_style_opa(cont, v, 0);
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_indev_active();
    lv_obj_t * cont = lv_event_get_target(e);
    if(lv_indev_get_scroll_obj(indev) != cont) return;

    int32_t w = lv_obj_get_width(cont);
    int32_t scroll_x = lv_obj_get_scroll_x(cont) - lv_indev_scroll_throw_predict(indev, LV_DIR_HOR);

    if(scroll_x < w / 2) {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE);
        lv_obj_remove_flag(cont, LV_OBJ_FLAG_CLICKABLE);
        lv_indev_wait_release(indev);
        lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_ON);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, shrink_anim_cb);
        lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
        lv_anim_set_values(&a, 255, 0);
        lv_anim_set_duration(&a, 400);
        lv_anim_set_var(&a, cont);
        lv_anim_start(&a);
    }
}

static void card_create(lv_obj_t * parent, card_info_t * info)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_add_style(cont, &style_card_cont, 0);
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t * remove = lv_obj_create(cont);
    lv_obj_add_style(remove, &style_hide, 0);
    lv_obj_remove_flag(remove, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_add_flag(remove, LV_OBJ_FLAG_FLOATING);
    lv_obj_remove_flag(remove, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * hide_label = lv_label_create(remove);
    lv_label_set_text(hide_label, "Hide");
    lv_obj_align(hide_label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t * placeholder = lv_obj_create(cont);
    lv_obj_add_style(placeholder, &style_placeholder, 0);

    lv_obj_t * card = lv_obj_create(cont);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN_HOR);

    lv_obj_t * avatar = lv_image_create(card);
    lv_image_set_src(avatar, info->image);
    lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 4);
    lv_obj_add_style(avatar, &style_avatar, 0);

    lv_obj_t * name = lv_label_create(card);
    lv_label_set_text(name, info->name);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(name, &font_multilang_large, 0);

    lv_obj_t * description = lv_label_create(card);
    lv_label_set_text(description, info->description);
    lv_obj_set_grid_cell(description, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_style_text_color(description, lv_color_hex(0x5b5b5b), 0);
    lv_obj_set_style_text_line_space(description, -3, 0);

    lv_obj_t * btn = lv_button_create(card);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_add_style(btn, &style_btn, 0);

    LV_IMAGE_DECLARE(img_multilang_like);
    lv_obj_t * btn_img = lv_image_create(btn);
    lv_image_set_src(btn_img, &img_multilang_like);
    lv_obj_align(btn_img, LV_ALIGN_LEFT_MID, 30, 0);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Like");
    lv_obj_align(btn_label, LV_ALIGN_LEFT_MID, 60, 1);

    lv_obj_scroll_to_view(card, LV_ANIM_OFF);
}

#endif
