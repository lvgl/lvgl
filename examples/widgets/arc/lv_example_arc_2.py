#
# An `lv_timer` to call periodically to set the angles of the arc
#
class ArcLoader():
    def __init__(self):
        self.a = 270

    def arc_loader_cb(self,tim,arc):
        # print(tim,arc)
        self.a += 5

        arc.set_end_angle(self.a)

        if self.a >= 270 + 360:
            tim._del()


#
# Create an arc which acts as a loader.
#

# Create an Arc
arc = lv.arc(lv.scr_act())
arc.set_bg_angles(0, 360)
arc.set_angles(270, 270)
arc.center()

# create the loader
arc_loader = ArcLoader()

# Create an `lv_timer` to update the arc.

timer = lv.timer_create_basic()
timer.set_period(20)
timer.set_cb(lambda src: arc_loader.arc_loader_cb(timer,arc))


