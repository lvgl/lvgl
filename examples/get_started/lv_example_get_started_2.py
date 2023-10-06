class CounterBtn():
    def __init__(self):
        self.cnt = 0
        #
        # Create a button with a label and react on click event.
        #

        button = lv.button(lv.scr_act())                               # Add a button the current screen
        button.set_pos(10, 10)                                      # Set its position
        button.set_size(120, 50)                                    # Set its size
        button.align(lv.ALIGN.CENTER,0,0)
        button.add_event(self.button_event_cb, lv.EVENT.ALL, None)  # Assign a callback to the button
        label = lv.label(button)                                    # Add a label to the button
        label.set_text("Button")                                 # Set the labels text
        label.center()

    def button_event_cb(self,e):
        code = e.get_code()
        button = e.get_target_obj()
        if code == lv.EVENT.CLICKED:
            self.cnt += 1

            # Get the first child of the button which is the label and change its text
            label = button.get_child(0)
            label.set_text("Button: " + str(self.cnt))


counterBtn = CounterBtn()

