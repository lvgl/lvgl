#include <src/dev/display/lcd/lv_lcd.h>
#include <src/dev/display/lcd/lv_lcd_panel_commands.h>

#if 0
void lv_lcd_driver_remove(lv_lcd_driver_t *drv)
{
	lv_free(drv);
}

void lv_lcd_driver_init(lv_lcd_driver_t * drv, lv_display_t *disp, uint32_t hor_res, uint32_t ver_res)
{
	drv->disp = disp;
	drv->hor_res = hor_res;
	drv->ver_res = ver_res;
}

lv_display_t *lv_lcd_get_display(lv_lcd_driver_t *drv)
{
	return drv->disp;
}


#endif
