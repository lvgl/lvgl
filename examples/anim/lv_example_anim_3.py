from micropython import const

# the example show the use of cubic-bezier3 in animation.
# the control point P1,P2 of cubic-bezier3 can be adjusted by slider.
# and the chart shows the cubic-bezier3 in real time. you can click
# run button see animation in current point of cubic-bezier3.
#

CHART_POINTS_NUM = const(256)

class LvExampleAnim_3():
    #
    # create an animation
    #
    def __init__(self):
        # Create a container with grid
        col_dsc = [lv.grid_fr(1), 200, lv.grid_fr(1), lv.GRID_TEMPLATE.LAST]
        row_dsc = [30, 10, 10, lv.grid_fr(1),lv.GRID_TEMPLATE.LAST]

        self.p1 = 0
        self.p2 = 0
        self.cont = lv.obj(lv.scr_act())
        self.cont.set_style_pad_all(2, lv.PART.MAIN)
        self.cont.set_style_pad_column(10, lv.PART.MAIN)
        self.cont.set_style_pad_row(10, lv.PART.MAIN)
        self.cont.set_grid_dsc_array(col_dsc, row_dsc)
        self.cont.set_size(320, 240)
        self.cont.center()
        self.page_obj_init(self.cont)

        self.a = lv.anim_t()
        self.a.init()
        self.a.set_var(self.anim_obj)
        end = self.cont.get_style_width(lv.PART.MAIN) - self.anim_obj.get_style_width(lv.PART.MAIN) - 10
        self.a.set_values(5, end)
        self.a.set_time(2000)
        self.a.set_custom_exec_cb(lambda a,val: self.anim_x_cb(self.anim_obj,val))
        self.a.set_path_cb(self.anim_path_bezier3_cb)
        self.refer_chart_cubic_bezier()

    def page_obj_init(self,par):
        self.anim_obj = lv.obj(par)
        self.anim_obj.set_size(30, 30)
        self.anim_obj.set_align(lv.ALIGN.TOP_LEFT)
        self.anim_obj.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.anim_obj.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), lv.PART.MAIN)
        self.anim_obj.set_grid_cell(lv.GRID_ALIGN.START, 0, 1,lv.GRID_ALIGN.START, 0, 1)

        self.p1_label = lv.label(par)
        self.p2_label = lv.label(par)
        self.p1_label.set_text("p1:0")
        self.p2_label.set_text("p2:0")
        self.p1_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1,lv.GRID_ALIGN.START, 1, 1)
        self.p2_label.set_grid_cell(lv.GRID_ALIGN.START, 0, 1,lv.GRID_ALIGN.START, 2, 1)

        self.p1_slider = lv.slider(par)
        self.p2_slider = lv.slider(par)
        self.p1_slider.set_range(0, 1024)
        self.p2_slider.set_range(0, 1024)
        self.p1_slider.set_style_pad_all(2, lv.PART.KNOB)
        self.p2_slider.set_style_pad_all(2, lv.PART.KNOB)
        self.p1_slider.add_event_cb(self.slider_event_cb, lv.EVENT.VALUE_CHANGED, None)
        self.p2_slider.add_event_cb(self.slider_event_cb, lv.EVENT.VALUE_CHANGED, None)
        self.p1_slider.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1,lv.GRID_ALIGN.START, 1, 1)
        self.p2_slider.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 1,lv.GRID_ALIGN.START, 2, 1)

        self.run_btn = lv.btn(par)
        self.run_btn.add_event_cb(self.run_btn_event_handler, lv.EVENT.CLICKED, None)
        btn_label = lv.label(self.run_btn)
        btn_label.set_text(lv.SYMBOL.PLAY)
        btn_label.center()
        self.run_btn.set_grid_cell(lv.GRID_ALIGN.STRETCH, 2, 1,lv.GRID_ALIGN.STRETCH, 1, 2)

        self.chart = lv.chart(par)
        self.chart.set_style_pad_all(0, lv.PART.MAIN)
        self.chart.set_style_size(0, 0, lv.PART.INDICATOR)
        self.chart.set_type(lv.chart.TYPE.SCATTER)
        self.ser1 = self.chart.add_series(lv.palette_main(lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
        self.chart.set_range(lv.chart.AXIS.PRIMARY_Y, 0, 1024)
        self.chart.set_range(lv.chart.AXIS.PRIMARY_X, 0, 1024)
        self.chart.set_point_count(CHART_POINTS_NUM)
        self.chart.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 3,lv.GRID_ALIGN.STRETCH, 3, 1)

    def refer_chart_cubic_bezier(self):
        for i in range(CHART_POINTS_NUM+1):
            t = i * (1024 // CHART_POINTS_NUM)
            step = lv.bezier3(t, 0, self.p1, self.p2, 1024)
            self.chart.set_value_by_id2(self.ser1, i, t, step)
            self.chart.refresh()

    def slider_event_cb(self,e):
        slider = e.get_target()

        if slider == self.p1_slider:
            label = self.p1_label
            self.p1 = slider.get_value()
            label.set_text("p1: {:d}".format(self.p1))
        else:
            label = self.p2_label
            self.p2 = slider.get_value()
            label.set_text("p1: {:d}".format(self.p2))

        self.refer_chart_cubic_bezier()


    def run_btn_event_handler(self,e):

        code = e.get_code()
        if code == lv.EVENT.CLICKED:
                lv.anim_t.start(self.a)

    def anim_x_cb(self, var, v):
        var.set_style_translate_x(v, lv.PART.MAIN)

    def anim_path_bezier3_cb(self,a):
        t = lv.map(a.act_time, 0, a.time, 0, 1024)
        step = lv.bezier3(t, 0, self.p1, self.p2, 1024)
        new_value = step * (a.end_value - a.start_value)
        new_value = new_value >> 10
        new_value += a.start_value
        return new_value

lv_example_anim_3 = LvExampleAnim_3()
