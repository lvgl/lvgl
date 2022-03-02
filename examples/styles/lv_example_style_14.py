# Will be called when the styles of the base theme are already added
# to add new styles


class NewTheme(lv.theme_t):
    def __init__(self):
        super().__init__()
        # Initialize the styles
        self.style_btn = lv.style_t()
        self.style_btn.init()
        self.style_btn.set_bg_color(lv.palette_main(lv.PALETTE.GREEN))
        self.style_btn.set_border_color(lv.palette_darken(lv.PALETTE.GREEN, 3))
        self.style_btn.set_border_width(3)

        # This theme is based on active theme 
        th_act = lv.theme_get_from_obj(lv.scr_act())
        # This theme will be applied only after base theme is applied
        self.set_parent(th_act)        


class ExampleStyle_14:
    
    def __init__(self):
        # 
        # Extending the current theme
        #

        btn = lv.btn(lv.scr_act())
        btn.align(lv.ALIGN.TOP_MID, 0, 20)

        label = lv.label(btn)
        label.set_text("Original theme")
        
        self.new_theme_init_and_set()
        
        btn = lv.btn(lv.scr_act())
        btn.align(lv.ALIGN.BOTTOM_MID, 0, -20)
        
        label = lv.label(btn)
        label.set_text("New theme")
        
    def new_theme_apply_cb(self, th, obj):
        print(th,obj)
        if obj.get_class() == lv.btn_class:
            obj.add_style(self.th_new.style_btn, 0)
        
    def new_theme_init_and_set(self):
        print("new_theme_init_and_set")
        # Initialize the new theme from the current theme        
        self.th_new = NewTheme()
        self.th_new.set_apply_cb(self.new_theme_apply_cb)
        lv.disp_get_default().set_theme(self.th_new)


exampleStyle_14 = ExampleStyle_14()
