#
# Scrolling with Right To Left base direction
#
obj = lv.obj(lv.scr_act())
obj.set_style_base_dir(lv.BASE_DIR.RTL, 0)
obj.set_size(200, 100)
obj.center()

label = lv.label(obj)
label.set_text("میکروکُنترولر (به انگلیسی: Microcontroller) گونه‌ای ریزپردازنده است که دارای حافظهٔ دسترسی تصادفی (RAM) و حافظهٔ فقط‌خواندنی (ROM)، تایمر، پورت‌های ورودی و خروجی (I/O) و درگاه ترتیبی (Serial Port پورت سریال)، درون خود تراشه است، و می‌تواند به تنهایی ابزارهای دیگر را کنترل کند. به عبارت دیگر یک میکروکنترلر، مدار مجتمع کوچکی است که از یک CPU کوچک و اجزای دیگری مانند تایمر، درگاه‌های ورودی و خروجی آنالوگ و دیجیتال و حافظه تشکیل شده‌است.")
label.set_width(400)
label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)

