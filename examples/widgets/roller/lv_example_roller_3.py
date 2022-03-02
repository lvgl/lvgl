import fs_driver
import sys

class Lv_Roller_3():

    def __init__(self):
        self.mask_top_id = -1
        self.mask_bottom_id = -1
        
        # 
        # Add a fade mask to roller.
        #
        style = lv.style_t()
        style.init()
        style.set_bg_color(lv.color_black())
        style.set_text_color(lv.color_white())
        
        lv.scr_act().add_style(style, 0)
        
        roller1 = lv.roller(lv.scr_act())
        roller1.add_style(style, 0)
        roller1.set_style_border_width(0, 0)
        roller1.set_style_pad_all(0, 0)
        roller1.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.SELECTED)
        
        #if LV_FONT_MONTSERRAT_22
        #    lv_obj_set_style_text_font(roller1, &lv_font_montserrat_22, LV_PART_SELECTED);
        #endif
        try:
            roller1.set_style_text_font(lv.font_montserrat_22,lv.PART.SELECTED)
        except:
            fs_drv = lv.fs_drv_t()
            fs_driver.fs_register(fs_drv, 'S')
            print("montserrat-22 not enabled in lv_conf.h, dynamically loading the font")
            font_montserrat_22 = lv.font_load("S:" + "../../assets/font/montserrat-22.fnt")
            roller1.set_style_text_font(font_montserrat_22,lv.PART.SELECTED)
           
        roller1.set_options("\n".join([
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December"]),lv.roller.MODE.NORMAL)

        roller1.center()
        roller1.set_visible_row_count(3)
        roller1.add_event_cb(self.mask_event_cb, lv.EVENT.ALL, None)

    def mask_event_cb(self,e):

        code = e.get_code()
        obj = e.get_target()

        if code == lv.EVENT.COVER_CHECK:
            e.set_cover_res(lv.COVER_RES.MASKED)

        elif code == lv.EVENT.DRAW_MAIN_BEGIN:
            # add mask 
            font = obj.get_style_text_font(lv.PART.MAIN)
            line_space = obj.get_style_text_line_space(lv.PART.MAIN)
            font_h = font.get_line_height()
            
            roller_coords = lv.area_t()
            obj.get_coords(roller_coords)
            
            rect_area = lv.area_t()
            rect_area.x1 = roller_coords.x1
            rect_area.x2 = roller_coords.x2
            rect_area.y1 = roller_coords.y1
            rect_area.y2 = roller_coords.y1 + (obj.get_height() - font_h - line_space) // 2
            
            fade_mask_top = lv.draw_mask_fade_param_t()
            fade_mask_top.init(rect_area, lv.OPA.TRANSP, rect_area.y1, lv.OPA.COVER, rect_area.y2)
            self.mask_top_id = lv.draw_mask_add(fade_mask_top,None)
            
            rect_area.y1 = rect_area.y2 + font_h + line_space - 1
            rect_area.y2 = roller_coords.y2
            
            fade_mask_bottom = lv.draw_mask_fade_param_t()
            fade_mask_bottom.init(rect_area, lv.OPA.COVER, rect_area.y1, lv.OPA.TRANSP, rect_area.y2)
            self.mask_bottom_id = lv.draw_mask_add(fade_mask_bottom, None)

        elif code == lv.EVENT.DRAW_POST_END:
            fade_mask_top = lv.draw_mask_remove_id(self.mask_top_id)
            fade_mask_bottom = lv.draw_mask_remove_id(self.mask_bottom_id)
            # Remove the masks
            lv.draw_mask_remove_id(self.mask_top_id)
            lv.draw_mask_remove_id(self.mask_bottom_id)
            self.mask_top_id = -1
            self.mask_bottom_id = -1
            
roller3 = Lv_Roller_3()
