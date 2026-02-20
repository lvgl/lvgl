#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FREETYPE && LV_USE_HARFBUZZ

#if LV_FREETYPE_USE_LVGL_PORT
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

static lv_obj_t * add_script_section(lv_obj_t * parent, const lv_font_t * font,
                                     const char * heading, const char * sample)
{
    /*Script name in the default font*/
    lv_obj_t * hdr = lv_label_create(parent);
    lv_obj_set_style_text_color(hdr, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_label_set_text(hdr, heading);

    /*Sample text in the target script*/
    lv_obj_t * lbl = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl, font, 0);
    lv_obj_set_width(lbl, lv_pct(100));
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
    lv_label_set_text(lbl, sample);
    return lbl;
}

/**
 * Render Bengali, Telugu, and Thai text with HarfBuzz shaping.
 *
 * Each script exercises a different shaping mechanism:
 *   - Bengali : left-matra reordering and stroke-fused conjuncts
 *   - Telugu  : below-base vertical consonant stacking
 *   - Thai    : above-base / below-base mark stacking (no conjuncts)
 *
 * Requires the following fonts in the working directory
 * (download from https://fonts.google.com/noto):
 *   - NotoSansBengali-Regular.ttf
 *   - NotoSansTelugu-Regular.ttf
 *   - NotoSansThai-Regular.ttf
 */
void lv_example_harfbuzz_2(void)
{
    /*Create fonts for each script*/
    lv_font_t * font_bengali = lv_freetype_font_create(
                                   PATH_PREFIX "NotoSansBengali-Regular.ttf",
                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 30,
                                   LV_FREETYPE_FONT_STYLE_NORMAL);

    lv_font_t * font_telugu = lv_freetype_font_create(
                                   PATH_PREFIX "NotoSansTelugu-Regular.ttf",
                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 30,
                                   LV_FREETYPE_FONT_STYLE_NORMAL);

    lv_font_t * font_thai = lv_freetype_font_create(
                                 PATH_PREFIX "NotoSansThai-Regular.ttf",
                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 30,
                                 LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font_bengali || !font_telugu || !font_thai) {
        LV_LOG_ERROR("failed to create one or more fonts");
        lv_obj_t * lbl = lv_label_create(lv_screen_active());
        lv_label_set_text(lbl, "Font file(s) not found.\n"
                          "Download NotoSansBengali, NotoSansTelugu,\n"
                          "and NotoSansThai from Google Fonts.");
        lv_obj_center(lbl);
        return;
    }

    /*Container with column layout*/
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 4, 0);

    /*--- Bengali ---*/
    /* "Nomoskar Bishwo" (Hello World) + conjuncts with left-matra reordering */
    add_script_section(cont, font_bengali, "Bengali", "নমস্কার বিশ্ব");

    /*--- Telugu ---*/
    /* "Namaskaaram Prapancham" (Hello World) + vertical consonant stacking */
    add_script_section(cont, font_telugu, "Telugu", "నమస్కారం ప్రపంచం");

    /*--- Thai ---*/
    /* "Sawasdee Chao Lok" (Hello World) + above-base mark stacking */
    add_script_section(cont, font_thai, "Thai", "สวัสดีชาวโลก");
}

#else

void lv_example_harfbuzz_2(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FreeType and HarfBuzz are not enabled");
    lv_obj_center(label);
}

#endif
#endif
