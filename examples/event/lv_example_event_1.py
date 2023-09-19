class Event_1():
    def __init__(self):
        self.cnt = 1
        #
        # Add click event to a button
        #

        button = lv.button(lv.scr_act())
        button.set_size(100, 50)
        button.center()
        button.add_event(self.event_cb, lv.EVENT.CLICKED, None)

        label = lv.label(button)
        label.set_text("Click me!")
        label.center()

    def event_cb(self,e):
        print("Clicked")

        button = e.get_target_obj()
        label = button.get_child(0)
        label.set_text(str(self.cnt))
        self.cnt += 1

evt1 = Event_1()
