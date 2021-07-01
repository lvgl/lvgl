#
# Create a pie chart
#

meter = lv.meter(lv.scr_act())

# Remove the background and the circle from the middle
meter.remove_style(None, lv.PART.MAIN)
meter.remove_style(None, lv.PART.INDICATOR)

meter.set_size(200, 200)
meter.center()

# Add a scale first with no ticks.
scale = meter.add_scale()
meter.set_scale_ticks(scale, 0, 0, 0, lv.color_black())
meter.set_scale_range(scale, 0, 100, 360, 0)

# Add a three arc indicator*
indic_w = 100
indic1 = meter.add_arc(scale, indic_w,lv.palette_main(lv.PALETTE.ORANGE), 0)
meter.set_indicator_start_value(indic1, 0)
meter.set_indicator_end_value(indic1, 40)

indic2 = meter.add_arc(scale, indic_w, lv.palette_main(lv.PALETTE.YELLOW), 0)
meter.set_indicator_start_value(indic2, 40)  # Start from the previous
meter.set_indicator_end_value(indic2, 80)

indic3 = meter.add_arc(scale, indic_w, lv.palette_main(lv.PALETTE.DEEP_ORANGE), 0)
meter.set_indicator_start_value(indic3, 80)  # Start from the previous
meter.set_indicator_end_value(indic3, 100)

