#
# Create LED's with different brightness and color
#

# Create a LED and switch it OFF
led1  = lv.led(lv.scr_act())
led1.align(lv.ALIGN.CENTER, -80, 0)
led1.off()

# Copy the previous LED and set a brightness
led2  = lv.led(lv.scr_act())
led2.align(lv.ALIGN.CENTER, 0, 0)
led2.set_brightness(150)
led2.set_color(lv.palette_main(lv.PALETTE.RED))

# Copy the previous LED and switch it ON
led3  = lv.led(lv.scr_act())
led3.align(lv.ALIGN.CENTER, 80, 0)
led3.on()

