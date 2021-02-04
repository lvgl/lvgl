# Create a style for the LED
style_led = lv.style_t()
lv.style_copy(style_led, lv.style_pretty_color)
style_led.body.radius = 800 # large enough to draw a circle
style_led.body.main_color = lv.color_make(0xb5, 0x0f, 0x04)
style_led.body.grad_color = lv.color_make(0x50, 0x07, 0x02)
style_led.body.border.color = lv.color_make(0xfa, 0x0f, 0x00)
style_led.body.border.width = 3
style_led.body.border.opa = lv.OPA._30
style_led.body.shadow.color = lv.color_make(0xb5, 0x0f, 0x04)
style_led.body.shadow.width = 5

# Create a LED and switch it OFF
led1  = lv.led(lv.scr_act())
led1.set_style(lv.led.STYLE.MAIN, style_led)
led1.align(None, lv.ALIGN.CENTER, -80, 0)
led1.off()

# Copy the previous LED and set a brightness
led2  = lv.led(lv.scr_act(), led1)
led2.align(None, lv.ALIGN.CENTER, 0, 0)
led2.set_bright(190)

# Copy the previous LED and switch it ON
led3  = lv.led(lv.scr_act(), led1)
led3.align(None, lv.ALIGN.CENTER, 80, 0)
led3.on()