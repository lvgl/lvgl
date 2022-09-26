import display_driver
import lvgl as lv

# A class is used to keep track of the series list because later we
#  need to draw to the series in the reverse order to which they were initialised.
class StackedAreaChart:
    def __init__(self):
        self.obj = None
        self.series_list = [None, None, None]

stacked_area_chart = StackedAreaChart()

#
# Callback which draws the blocks of colour under the lines
#
def draw_event_cb(e):

    obj = e.get_target()
    cont_a = lv.area_t()
    obj.get_coords(cont_a)

    #Add the faded area before the lines are drawn
    dsc = e.get_draw_part_dsc()
    if dsc.part == lv.PART.ITEMS:
        if not dsc.p1 or not dsc.p2:
            return

        # Add a line mask that keeps the area below the line
        line_mask_param = lv.draw_mask_line_param_t()
        line_mask_param.points_init(dsc.p1.x, dsc.p1.y, dsc.p2.x, dsc.p2.y, lv.DRAW_MASK_LINE_SIDE.BOTTOM)
        line_mask_id = lv.draw_mask_add(line_mask_param, None)

        #Draw a rectangle that will be affected by the mask
        draw_rect_dsc = lv.draw_rect_dsc_t()
        draw_rect_dsc.init()
        draw_rect_dsc.bg_opa = lv.OPA.COVER
        draw_rect_dsc.bg_color = dsc.line_dsc.color

        a = lv.area_t()
        a.x1 = dsc.p1.x
        a.x2 = dsc.p2.x
        a.y1 = min(dsc.p1.y, dsc.p2.y)
        a.y2 = cont_a.y2 - 13 # -13 cuts off where the rectangle draws over the chart margin. Without this an area of 0 doesn't look like 0
        dsc.draw_ctx.rect(draw_rect_dsc, a)

        # Remove the mask
        lv.draw_mask_free_param(line_mask_param)
        lv.draw_mask_remove_id(line_mask_id)


#
# Helper function to round a fixed point number
#
def round_fixed_point(n, shift):
    # Create a bitmask to isolates the decimal part of the fixed point number
    mask = 1
    for bit_pos in range(shift):
        mask = (mask << 1) + 1

    decimal_part = n & mask

    # Get 0.5 as fixed point
    rounding_boundary = 1 << (shift - 1)

    # Return either the integer part of n or the integer part + 1
    if decimal_part < rounding_boundary:
        return (n & ~mask)
    return ((n >> shift) + 1) << shift


#
# Stacked area chart
#
def lv_example_chart_8():

    #Create a stacked_area_chart.obj
    stacked_area_chart.obj = lv.chart(lv.scr_act())
    stacked_area_chart.obj.set_size(200, 150)
    stacked_area_chart.obj.center()
    stacked_area_chart.obj.set_type( lv.chart.TYPE.LINE)
    stacked_area_chart.obj.set_div_line_count(5, 7)
    stacked_area_chart.obj.add_event_cb( draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

    # Set range to 0 to 100 for percentages. Draw ticks
    stacked_area_chart.obj.set_range(lv.chart.AXIS.PRIMARY_Y,0,100)
    stacked_area_chart.obj.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 3, 0, 5, 1, True, 30)

    #Set point size to 0 so the lines are smooth
    stacked_area_chart.obj.set_style_size(0, 0, lv.PART.INDICATOR)

    # Add some data series
    stacked_area_chart.series_list[0] = stacked_area_chart.obj.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
    stacked_area_chart.series_list[1] = stacked_area_chart.obj.add_series(lv.palette_main(lv.PALETTE.BLUE), lv.chart.AXIS.PRIMARY_Y)
    stacked_area_chart.series_list[2] = stacked_area_chart.obj.add_series(lv.palette_main(lv.PALETTE.GREEN), lv.chart.AXIS.PRIMARY_Y)

    for point in range(10):
        # Make some random data
        vals = [lv.rand(10, 20), lv.rand(20, 30), lv.rand(20, 30)]

        fixed_point_shift = 5
        total = vals[0] + vals[1] + vals[2]
        draw_heights = [0, 0, 0]
        int_sum = 0
        decimal_sum = 0

        # Fixed point cascade rounding ensures percentages add to 100
        for series_index in range(3):
            decimal_sum += int(((vals[series_index] * 100) << fixed_point_shift) // total)
            int_sum += int((vals[series_index] * 100) / total)

            modifier = (round_fixed_point(decimal_sum, fixed_point_shift) >> fixed_point_shift) - int_sum

            #  The draw heights are equal to the percentage of the total each value is + the cumulative sum of the previous percentages.
            #   The accumulation is how the values get "stacked"
            draw_heights[series_index] = int(int_sum + modifier)

            #  Draw to the series in the reverse order to which they were initialised.
            #   Without this the higher values will draw on top of the lower ones.
            #   This is because the Z-height of a series matches the order it was initialised
            stacked_area_chart.obj.set_next_value( stacked_area_chart.series_list[3 - series_index - 1], draw_heights[series_index])

    stacked_area_chart.obj.refresh()

lv_example_chart_8()
