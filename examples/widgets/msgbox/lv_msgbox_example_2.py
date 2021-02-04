welcome_info = "Welcome to the modal message box demo!\nPress the button to display a message box."
in_msg_info = "Notice that you cannot touch the button again while the message box is open."

class Modal(lv.mbox):
    """mbox with semi-transparent background"""
    def __init__(self, parent, *args, **kwargs):
        # Create a full-screen background
        modal_style = lv.style_t()
        lv.style_copy(modal_style, lv.style_plain_color)
        # Set the background's style
        modal_style.body.main_color = modal_style.body.grad_color = lv.color_make(0,0,0)
        modal_style.body.opa = lv.OPA._50
        
        # Create a base object for the modal background
        self.bg = lv.obj(parent)
        self.bg.set_style(modal_style)
        self.bg.set_pos(0, 0)
        self.bg.set_size(parent.get_width(), parent.get_height())
        self.bg.set_opa_scale_enable(True)  # Enable opacity scaling for the animation

        super().__init__(self.bg, *args, **kwargs)
        self.align(None, lv.ALIGN.CENTER, 0, 0)

        # Fade the message box in with an animation
        a = lv.anim_t()
        lv.anim_init(a)
        lv.anim_set_time(a, 500, 0)
        lv.anim_set_values(a, lv.OPA.TRANSP, lv.OPA.COVER)
        lv.anim_set_exec_cb(a, self.bg, lv.obj.set_opa_scale)
        lv.anim_create(a)
        super().set_event_cb(self.default_callback)

    def set_event_cb(self, callback):
        self.callback = callback

    def get_event_cb(self):
        return self.callback

    def default_callback(self, obj, evt):
        if evt == lv.EVENT.DELETE:# and obj == self:
            # Delete the parent modal background
            self.get_parent().del_async()
        elif evt == lv.EVENT.VALUE_CHANGED:
            # A button was clicked
            self.start_auto_close(0)
        # Call user-defined callback
        if self.callback is not None:
            self.callback(obj, evt)

def mbox_event_cb(obj, evt):
    if evt == lv.EVENT.DELETE:
        info.set_text(welcome_info)

def btn_event_cb(btn, evt):
    if evt == lv.EVENT.CLICKED:

        btns2 = ["Ok", "Cancel", ""]

        # Create the message box as a child of the modal background
        mbox = Modal(lv.scr_act())
        mbox.add_btns(btns2)
        mbox.set_text("Hello world!")
        mbox.set_event_cb(mbox_event_cb)

        info.set_text(in_msg_info)
        info.align(None, lv.ALIGN.IN_BOTTOM_LEFT, 5, -5)

# Get active screen
scr = lv.scr_act()

# Create a button, then set its position and event callback
btn = lv.btn(scr)
btn.set_size(200, 60)
btn.set_event_cb(btn_event_cb)
btn.align(None, lv.ALIGN.IN_TOP_LEFT, 20, 20)

# Create a label on the button
label = lv.label(btn)
label.set_text("Display a message box!")

# Create an informative label on the screen
info = lv.label(scr)
info.set_text(welcome_info)
info.set_long_mode(lv.label.LONG.BREAK) # Make sure text will wrap
info.set_width(scr.get_width() - 10)
info.align(None, lv.ALIGN.IN_BOTTOM_LEFT, 5, -5)