# Will be called when the styles of the base theme are already added
# to add new styles


class NewTheme(lv.theme_t):
    def __init__(self):
        super().__init__()
        # Initialize the styles
        self.style_button = lv.style_t()
        self.style_button.init()
        self.style_button.set_bg_color(lv.palette_main(lv.PALETTE.GREEN))
        self.style_button.set_border_color(lv.palette_darken(lv.PALETTE.GREEN, 3))
        self.style_button.set_border_width(3)

        # This theme is based on active theme
        th_act = lv.theme_get_from_obj(lv.scr_act())
        # This theme will be applied only after base theme is applied
        self.set_parent(th_act)


class ExampleStyle_14:

    def __init__(self):
        #
        # Extending the current theme
        #

        button = lv.button(lv.scr_act())
        button.align(lv.ALIGN.TOP_MID, 0, 20)

        label = lv.label(button)
        label.set_text("Original theme")

        self.new_theme_init_and_set()

        button = lv.button(lv.scr_act())
        button.align(lv.ALIGN.BOTTOM_MID, 0, -20)

        label = lv.label(button)
        label.set_text("New theme")

    def new_theme_apply_cb(self, th, obj):
        print(th,obj)
        if obj.get_class() == lv.button_class:
            obj.add_style(self.th_new.style_button, 0)

    def new_theme_init_and_set(self):
        print("new_theme_init_and_set")
        # Initialize the new theme from the current theme
        self.th_new = NewTheme()
        self.th_new.set_apply_cb(self.new_theme_apply_cb)
        lv.display_get_default().set_theme(self.th_new)


exampleStyle_14 = ExampleStyle_14()
