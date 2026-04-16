#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_FONT_DEJAVU_16_PERSIAN_HEBREW

/**
 * @title RTL label scrolling
 * @brief Scroll a wide Persian label inside an RTL container.
 *
 * A 200x100 container has `LV_BASE_DIR_RTL` applied to its main part. A
 * child label 400 px wide, rendered with `lv_font_dejavu_16_persian_hebrew`,
 * carries a Persian paragraph; the scrollbar and scroll direction reflect
 * the right-to-left base direction as the label is dragged.
 */
void lv_example_scroll_5(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_base_dir(obj, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(obj, 200, 100);
    lv_obj_center(obj);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label,
                      "میکروکُنترولر (به انگلیسی: Microcontroller) گونه‌ای ریزپردازنده است که دارای حافظهٔ دسترسی تصادفی (RAM) و حافظهٔ فقط‌خواندنی (ROM)، تایمر، پورت‌های ورودی و خروجی (I/O) و درگاه ترتیبی (Serial Port پورت سریال)، درون خود تراشه است، و می‌تواند به تنهایی ابزارهای دیگر را کنترل کند. به عبارت دیگر یک میکروکنترلر، مدار مجتمع کوچکی است که از یک CPU کوچک و اجزای دیگری مانند تایمر، درگاه‌های ورودی و خروجی آنالوگ و دیجیتال و حافظه تشکیل شده‌است.");
    lv_obj_set_width(label, 400);
    lv_obj_set_style_text_font(label, &lv_font_dejavu_16_persian_hebrew, 0);

}

#endif
