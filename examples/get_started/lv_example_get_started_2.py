class CounterBtn():
    def __init__(self):
        self.cnt = 0
        #
        # Create a button with a label and react on click event.
        #

        btn = lv.btn(lv.scr_act())                               # Add a button the current screen
        btn.set_pos(10, 10)                                      # Set its position
        btn.set_size(120, 50)                                    # Set its size
        btn.align(lv.ALIGN.CENTER,0,0)
        btn.add_event_cb(self.btn_event_cb, lv.EVENT.ALL, None)  # Assign a callback to the button
        label = lv.label(btn)                                    # Add a label to the button
        label.set_text("Button")                                 # Set the labels text
        label.center()

    def btn_event_cb(self,evt):
        code = evt.get_code()
        btn = evt.get_target()
        if code == lv.EVENT.CLICKED:
            self.cnt += 1

        # Get the first child of the button which is the label and change its text
        label = btn.get_child(0)
        label.set_text("Button: " + str(self.cnt))


counterBtn = CounterBtn()

