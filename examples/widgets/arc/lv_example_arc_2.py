# Create an arc which acts as a loader.
class loader_arc(lv.arc):

    def __init__(self, parent, color=lv.color_hex(0x000080), 
                       width=8, style=lv.style_plain, rate=20):
        super().__init__(parent)
        
        self.a = 0
        self.rate = rate

        # Create style for the Arcs
        self.style = lv.style_t()
        lv.style_copy(self.style, style)
        self.style.line.color = color
        self.style.line.width = width

        # Create an Arc
        self.set_angles(180, 180);
        self.set_style(self.STYLE.MAIN, self.style);

        # Spin the Arc
        self.spin()

    def spin(self):
        # Create an `lv_task` to update the arc.
        lv.task_create(self.task_cb, self.rate, lv.TASK_PRIO.LOWEST, {})

    
    # An `lv_task` to call periodically to set the angles of the arc
    def task_cb(self, task):
        self.a+=5;
        if self.a >= 359: self.a = 359

        if self.a < 180: self.set_angles(180-self.a, 180)
        else: self.set_angles(540-self.a, 180)

        if self.a == 359:
            self.a = 0
            lv.task_del(task)

# Create a loader arc
loader_arc = loader_arc(lv.scr_act())
loader_arc.align(None, lv.ALIGN.CENTER, 0, 0)