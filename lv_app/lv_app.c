/**
 * @file lv_app.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app.h"
#include "lvgl/lv_misc/anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*Actions*/
static lv_action_res_t lv_app_menu_rel_action(lv_obj_t * app_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_menu_elem_rel_action(lv_obj_t * app_elem_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_area_rel_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_area_pr_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_pr_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_win_minim_action(lv_obj_t * close_btn, lv_dispi_t * dispi);

static void lv_app_win_close_anim_cb(lv_obj_t * app_win);
static void lv_app_win_minim_anim_cb(lv_obj_t * app_win);

static void lv_app_init_icons(void);
static void lv_app_init_style(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t app_dsc_ll; /*Store a pointer to the app descriptors*/
static ll_dsc_t app_inst_ll; /*Store the running apps*/

static lv_obj_t * menuh; 	/*Holder of timg_bubbleshe menu on the top*/
static lv_obj_t * app_btn;  /*The "Apps" button on the menu*/
static lv_obj_t * sys_apph; /*Holder of the system app. buttons*/
static lv_obj_t * app_list;
static lv_obj_t * sc_area;
static lv_obj_t * sc_page;

static lv_app_style_t app_style;
#include "lvgl/lv_objx/lv_img.h"
/*Declare icons*/
LV_IMG_DECLARE(img_add);
LV_IMG_DECLARE(img_battery_empty);
LV_IMG_DECLARE(img_battery_full);
LV_IMG_DECLARE(img_battery_half);
LV_IMG_DECLARE(img_bubble);
LV_IMG_DECLARE(img_calendar);
LV_IMG_DECLARE(img_clock);
LV_IMG_DECLARE(img_close);
LV_IMG_DECLARE(img_down);
LV_IMG_DECLARE(img_driver);
LV_IMG_DECLARE(img_eject);
LV_IMG_DECLARE(img_folder);
LV_IMG_DECLARE(img_image);
LV_IMG_DECLARE(img_left);
LV_IMG_DECLARE(img_music);
LV_IMG_DECLARE(img_ok);
LV_IMG_DECLARE(img_play);
LV_IMG_DECLARE(img_right);
LV_IMG_DECLARE(img_settings);
LV_IMG_DECLARE(img_shut_down);
LV_IMG_DECLARE(img_star);
LV_IMG_DECLARE(img_test);
LV_IMG_DECLARE(img_up);
LV_IMG_DECLARE(img_user);
LV_IMG_DECLARE(img_video);
LV_IMG_DECLARE(img_volume);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the application system
 */
void lv_app_init(void)
{
	ll_init(&app_dsc_ll, sizeof(lv_app_dsc_t *));
	ll_init(&app_inst_ll, sizeof(lv_app_inst_t));

	lv_app_init_icons();
	lv_app_init_style();

	/*Create the desktop elements*/

	/*Shortcut area*/
	sc_page = lv_page_create(lv_scr_act(), NULL);
	lv_obj_set_style(sc_page, &app_style.sc_page_style);
	lv_obj_set_size(sc_page, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_pos(sc_page, 0, 0);
	sc_area = lv_btn_create(sc_page, NULL);
	lv_obj_set_style(sc_area, &app_style.sc_area_style);
	lv_rect_set_fit(sc_area, false, true);
	lv_rect_set_layout(sc_area, LV_RECT_LAYOUT_GRID);
	lv_btn_set_rel_action(sc_area, lv_app_sc_area_rel_action);
	lv_btn_set_pr_action(sc_area, lv_app_sc_area_pr_action);
	lv_page_glue_obj(sc_area, true);

	/*Menu on the top*/
	menuh = lv_rect_create(lv_scr_act(), NULL);
	lv_obj_set_size(menuh, LV_HOR_RES, app_style.menu_h);
	lv_obj_set_pos(menuh, 0, 0);
	lv_obj_set_style(menuh, &app_style.menu_style);

	app_btn = lv_btn_create(menuh, NULL);
	lv_obj_set_style(app_btn, &app_style.menu_btn_style);
	lv_obj_set_height(app_btn, app_style.menu_h);
	lv_rect_set_fit(app_btn, true, false);
	lv_btn_set_rel_action(app_btn, lv_app_menu_rel_action);
	lv_obj_t * app_label = lv_label_create(app_btn, NULL);
	lv_obj_set_style(app_label, &app_style.menu_btn_label_style);
	lv_obj_set_pos(app_btn, 0, 0);
	lv_label_set_text(app_label, "Apps");

	lv_app_refr_style();


	/*Initialize all application descriptors*/
	const lv_app_dsc_t ** dsc;
#if USE_LV_APP_EXAMPLE != 0
	dsc = ll_ins_head(&app_dsc_ll);
	*dsc = lv_app_example_init();
#endif
}

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param cstr a Create STRing which can give initial parameters to the application (NULL or "" if unused)
 * @return pointer to the opened application or NULL if any error occurred
 */
lv_app_inst_t * lv_app_run(const lv_app_dsc_t * app_dsc, const char * cstr)
{
	/*Add a new application and initialize it*/
	lv_app_inst_t * app;
	app = ll_ins_head(&app_inst_ll);
	app->dsc = app_dsc;
	app->app_data = dm_alloc(app_dsc->app_data_size);
	app->name = NULL;
	lv_app_rename(app, app_dsc->name);

	/*Call the application specific run function*/
	app_dsc->app_run(app, cstr);

	return app;
}

/**
 * Close a running application. Close the Window and the Shortcut too if opened.
 * @param app pointer to an application
 */
void lv_app_close(lv_app_inst_t * app)
{
	lv_app_win_close(app);
	lv_app_sc_close(app);

	app->dsc->app_close(app);

	dm_free(app->app_data);
	dm_free(app->name);
}

/**
 * Publish an event.
 * @param app pointer to an application which publishes the event
 * @param event an event from 'lv_app_event_t' enum
 */
void lv_app_event_send(lv_app_inst_t * app, lv_app_event_t event)
{

}

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_sc_open(lv_app_inst_t * app)
{
	/*Save the current position of the scrollable part of the page*/
	cord_t scrl_y = lv_obj_get_y(lv_page_get_scrable(sc_page));

	app->sc = lv_btn_create(sc_area, NULL);
	lv_obj_set_free_p(app->sc, app);
	lv_obj_set_style(app->sc, &app_style.sc_style);
	lv_obj_set_opa(app->sc, app_style.sc_opa);
	lv_obj_set_size(app->sc, LV_APP_SC_WIDTH, LV_APP_SC_HEIGHT);
	lv_rect_set_layout(app->sc, LV_RECT_LAYOUT_OFF);
	lv_btn_set_rel_action(app->sc, lv_app_sc_rel_action);
	lv_btn_set_pr_action(app->sc, lv_app_sc_pr_action);
	lv_page_glue_obj(app->sc, true);

	app->sc_data = dm_alloc(app->dsc->sc_data_size);
	app->dsc->sc_open(app, app->sc);

	app->sc_title = lv_label_create(app->sc, NULL);
	lv_obj_set_style(app->sc_title, &app_style.sc_title_style);
	lv_label_set_long_mode(app->sc_title, LV_LABEL_LONG_SCROLL);
	lv_label_set_text(app->sc_title, app->name);
	lv_obj_align_us(app->sc_title, NULL, LV_ALIGN_IN_TOP_MID, 0, app_style.sc_title_margin);

	/*Restore position of the scrollable part of the page*/
	lv_obj_set_y(lv_page_get_scrable(sc_page), scrl_y);
	lv_page_focus(sc_page, app->sc, true);

	return app->sc;
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
void lv_app_sc_close(lv_app_inst_t * app)
{
	if(app->sc == NULL) return;
	lv_obj_del(app->sc);
	app->sc = NULL;
	app->sc_title = NULL;
	dm_free(app->sc_data);
	app->sc_data = NULL;
}

/**
 * Open the application in a window
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_win_open(lv_app_inst_t * app)
{

	/*Close the app list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

	app->win = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_free_p(app->win, app);
	lv_obj_set_style(app->win, &app_style.win_style);

	lv_win_add_ctrl_btn(app->win, "U:/icon_down", lv_app_win_minim_action);
	lv_win_add_ctrl_btn(app->win, "U:/icon_close", lv_app_win_close_action);

	app->dsc->win_open(app, app->win);
#if LV_APP_ANIM_WIN_OPEN != 0
	anim_t a;
	a.act_time = 0;
	a.time = LV_APP_ANIM_WIN_OPEN;
	a.end_cb = NULL;
	a.playback = 0;
	a.repeat = 0;
	a.var = app->win;
	a.path = anim_get_path(ANIM_PATH_LIN);
#if LV_APP_ANIM_WIN_OPEN_COMPLEX != 0
	area_t cords;
	lv_obj_get_cords(app->sc, &cords);

	a.start = lv_obj_get_width(app->sc);
	a.end = LV_HOR_RES;
	a.fp = (anim_fp_t) lv_obj_set_width;
	anim_create(&a);

	a.start = lv_obj_get_height(app->sc);
	a.end = LV_VER_RES;
	a.fp = (anim_fp_t) lv_obj_set_height;
	anim_create(&a);

	a.start = cords.x1;
	a.end = 0;
	a.fp = (anim_fp_t) lv_obj_set_x;
	anim_create(&a);

	a.start = cords.y1;
	a.end = 0;
	a.fp = (anim_fp_t) lv_obj_set_y;
	anim_create(&a);
#endif /*LV_APP_ANIM_WIN_OPEN_COMPLEX*/

	a.start = OPA_TRANSP;
	a.end = OPA_COVER;
	a.fp = (anim_fp_t) lv_obj_set_opar;
	anim_create(&a);
#endif /*LV_APP_ANIM_WIN_OPEN*/

	return app->win;
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
void lv_app_win_close(lv_app_inst_t * app)
{
	if(app->win == NULL) return;

	lv_obj_del(app->win);
	app->win = NULL;
	dm_free(app->win_data);
	app->win_data = NULL;
}


void lv_app_refr_style(void)
{
	lv_obj_set_opa(menuh, app_style.menu_opa);
	lv_obj_set_opa(app_btn, app_style.menu_btn_opa);

	lv_obj_set_width(sc_area, LV_HOR_RES - 2 *
			   (app_style.sc_page_style.bg_rects.hpad +
			    app_style.sc_page_style.scrable_rects.hpad));
}

void lv_app_rename(lv_app_inst_t * app, const char * name)
{
	dm_free(app->name);
	app->name = dm_alloc(strlen(name) + 1);
	strcpy(app->name, name);

	if(app->sc != NULL) {
		lv_label_set_text(app->sc_title, app->name);
	}
}

const lv_app_dsc_t * lv_app_get_dsc(const char * name)
{
	const lv_app_dsc_t ** dsc;
	LL_READ(app_dsc_ll, dsc) {
		if(strcmp((*dsc)->name, name) == 0) {
			return *dsc;
		}
	}

	return NULL;
}

lv_app_style_t * lv_app_get_style(void)
{
	return &app_style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_action_res_t lv_app_menu_rel_action(lv_obj_t * app_btn, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}
	/*Create the app. list*/
	else {
		app_list = lv_list_create(lv_scr_act(), NULL);
		lv_obj_set_style(app_list, &app_style.app_list_style);
		lv_obj_set_opa(app_list, app_style.menu_opa);
		lv_obj_set_size(app_list, app_style.app_list_w, app_style.app_list_h);
		lv_obj_set_y(app_list, app_style.menu_h);

		lv_app_dsc_t ** dsc;
		lv_obj_t * elem;
		LL_READ(app_dsc_ll, dsc) {
			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

			elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
			lv_obj_set_free_p(elem, *dsc);
			lv_obj_set_opa(elem, app_style.menu_btn_opa);

		}
	}
	return LV_ACTION_RES_OK;
}

static lv_action_res_t lv_app_menu_elem_rel_action(lv_obj_t * app_elem_btn, lv_dispi_t * dispi)
{
	lv_app_dsc_t * dsc = lv_obj_get_free_p(app_elem_btn);

	lv_app_inst_t * app = lv_app_run(dsc, "");
	lv_app_sc_open(app);

	/*Close the app list*/
	lv_obj_del(app_list);
	app_list = NULL;

	return LV_ACTION_RES_INV;
}

static lv_action_res_t lv_app_sc_area_rel_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

	return LV_ACTION_RES_OK;
}

static lv_action_res_t lv_app_sc_area_pr_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

	return LV_ACTION_RES_OK;
}

static lv_action_res_t lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
	lv_page_focus(sc_page, sc, true);

	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}
	/*Else open the window of the shortcut*/
	else {
		lv_app_inst_t * app = lv_obj_get_free_p(sc);
		lv_app_win_open(app);
	}

	return LV_ACTION_RES_OK;
}

static lv_action_res_t lv_app_sc_pr_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

	return LV_ACTION_RES_OK;
}

static lv_action_res_t lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(close_btn);
	lv_app_inst_t * app = lv_obj_get_free_p(win);

#if LV_APP_ANIM_WIN_CLOSE != 0
	lv_obj_anim(app->win, LV_ANIM_FADE | ANIM_OUT, LV_APP_ANIM_WIN_CLOSE, 0, lv_app_win_close_anim_cb);
	lv_app_sc_close(app);
#else
	lv_app_close(app);
	return LV_ACTION_RES_INV;
#endif

	return LV_ACTION_RES_OK;
}


static lv_action_res_t lv_app_win_minim_action(lv_obj_t * close_btn, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(close_btn);
	lv_app_inst_t * app = lv_obj_get_free_p(win);

#if LV_APP_ANIM_WIN_CLOSE != 0
	lv_obj_anim(app->win, LV_ANIM_FLOAT_BOTTOM | ANIM_OUT, LV_APP_ANIM_WIN_CLOSE, 0, lv_app_win_minim_anim_cb);
#else
	lv_app_win_close(app);
	return LV_ACTION_RES_INV;
#endif

	return LV_ACTION_RES_OK;
}

static void lv_app_win_close_anim_cb(lv_obj_t * app_win)
{
	lv_app_inst_t * app = lv_obj_get_free_p(app_win);
	lv_app_close(app);
}

static void lv_app_win_minim_anim_cb(lv_obj_t * app_win)
{
	lv_app_inst_t * app = lv_obj_get_free_p(app_win);
	lv_app_win_close(app);
}

static void lv_app_init_icons(void)
{
	lv_img_create_file("icon_add", img_add);
	lv_img_create_file("icon_battery_empty", img_battery_empty);
	lv_img_create_file("icon_battery_full", img_battery_full);
	lv_img_create_file("icon_battery_half", img_battery_half);
	lv_img_create_file("icon_bubble", img_bubble);
	lv_img_create_file("icon_calendar", img_calendar);
	lv_img_create_file("icon_clock", img_clock);
	lv_img_create_file("icon_close", img_close);
	lv_img_create_file("icon_down", img_down);
	lv_img_create_file("icon_driver", img_driver);
	lv_img_create_file("icon_eject", img_eject);
	lv_img_create_file("icon_folder", img_folder);
	lv_img_create_file("icon_image", img_image);
	lv_img_create_file("icon_left", img_left);
	lv_img_create_file("icon_music", img_music);
	lv_img_create_file("icon_ok", img_ok);
	lv_img_create_file("icon_play", img_play);
	lv_img_create_file("icon_right", img_right);
	lv_img_create_file("icon_settings", img_settings);
	lv_img_create_file("icon_shut_down", img_shut_down);
	lv_img_create_file("icon_star", img_star);
	lv_img_create_file("icon_up", img_up);
	lv_img_create_file("icon_user", img_user);
	lv_img_create_file("icon_video", img_video);
	lv_img_create_file("icon_volume", img_volume);
}

static void lv_app_init_style(void)
{
	/*Coordinates*/
	app_style.menu_h = 40 * LV_DOWNSCALE;
	app_style.app_list_w = LV_HOR_RES / 3;
	app_style.app_list_h = (2 * LV_VER_RES) / 3;
	app_style.sc_title_margin = 2 * LV_DOWNSCALE;

	/*Fonts*/
	app_style.font_small = FONT_DEJAVU_20;
	app_style.font_medium = FONT_DEJAVU_30;
	app_style.font_large = FONT_DEJAVU_40;

	/*Opacity*/
	app_style.menu_opa = OPA_80;
	app_style.menu_btn_opa = OPA_50;
	app_style.sc_opa = OPA_70;

	/*Menu style*/
	lv_rects_get(LV_RECTS_DEF,&app_style.menu_style);
	app_style.menu_style.objs.color = COLOR_BLACK;
	app_style.menu_style.gcolor = COLOR_BLACK;
	app_style.menu_style.round = 0;
	app_style.menu_style.bwidth = 0;
	app_style.menu_style.light = 0;

	lv_btns_get(LV_BTNS_DEF,&app_style.menu_btn_style);
	memcpy(&app_style.menu_btn_style.rects, &app_style.menu_style, sizeof(lv_rects_t));
	app_style.menu_btn_style.flags[LV_BTN_STATE_REL].light_en = 0;
	app_style.menu_btn_style.flags[LV_BTN_STATE_PR].light_en = 0;

	app_style.menu_btn_style.flags[LV_BTN_STATE_REL].empty = 1;
	app_style.menu_btn_style.flags[LV_BTN_STATE_PR].empty = 0;

	app_style.menu_btn_style.mcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
	app_style.menu_btn_style.gcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
	app_style.menu_btn_style.mcolor[LV_BTN_STATE_PR] = COLOR_GRAY;
	app_style.menu_btn_style.gcolor[LV_BTN_STATE_PR] = COLOR_GRAY;

	lv_labels_get(LV_LABELS_BTN,&app_style.menu_btn_label_style);
	app_style.menu_btn_label_style.font = app_style.font_large;
	app_style.menu_btn_label_style.objs.color = COLOR_MAKE(0xd0, 0xe0, 0xf0);

	lv_imgs_get(LV_IMGS_DEF,&app_style.menu_btn_img_style);
	app_style.menu_btn_img_style.objs.color = COLOR_WHITE;
	app_style.menu_btn_img_style.recolor_opa = OPA_90;

	/*App list styles*/
	lv_lists_get(LV_LISTS_DEF,&app_style.app_list_style);
	memcpy(&app_style.app_list_style.liste_btns, &app_style.menu_btn_style, sizeof(lv_btns_t));
	memcpy(&app_style.app_list_style.bg_pages.bg_rects, &app_style.menu_style, sizeof(lv_rects_t));
	memcpy(&app_style.app_list_style.liste_labels, &app_style.menu_btn_label_style, sizeof(lv_labels_t));
	app_style.app_list_style.bg_pages.bg_rects.vpad = 0;
	app_style.app_list_style.bg_pages.bg_rects.hpad = 0;
	app_style.app_list_style.bg_pages.bg_rects.opad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.objs.transp = 1;
	app_style.app_list_style.bg_pages.scrable_rects.vpad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.hpad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.opad = 0;
	app_style.app_list_style.bg_pages.sb_rects.objs.color = COLOR_GRAY;
	app_style.app_list_style.bg_pages.sb_rects.gcolor = COLOR_GRAY;

	/*Shortcut area styles*/
	lv_btns_get(LV_BTNS_DEF,&app_style.sc_area_style);
	app_style.sc_area_style.flags[LV_BTN_STATE_REL].transp = 1;
	app_style.sc_area_style.flags[LV_BTN_STATE_PR].transp = 1;
	app_style.sc_area_style.rects.hpad = 20 * LV_STYLE_MULT;
	app_style.sc_area_style.rects.vpad = 50 * LV_STYLE_MULT;
	app_style.sc_area_style.rects.opad = 20 * LV_STYLE_MULT;

	lv_pages_get(LV_PAGES_DEF,&app_style.sc_page_style);
	app_style.sc_page_style.bg_rects.empty = 1;
	app_style.sc_page_style.bg_rects.round = 0;
	app_style.sc_page_style.bg_rects.bwidth = 0;
	app_style.sc_page_style.bg_rects.vpad = 0;
	app_style.sc_page_style.bg_rects.hpad = 0;
	app_style.sc_page_style.bg_rects.opad = 0;
	app_style.sc_page_style.scrable_rects.objs.transp = 1;
	app_style.sc_page_style.scrable_rects.vpad = 0;
	app_style.sc_page_style.scrable_rects.hpad = 0;
	app_style.sc_page_style.scrable_rects.opad = 0;

	/*Shortcut styles*/
	lv_btns_get(LV_BTNS_DEF,&app_style.sc_style);
	app_style.sc_style.mcolor[LV_BTN_STATE_REL] = COLOR_WHITE;
	app_style.sc_style.gcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x20, 0x30, 0x40);
	app_style.sc_style.bcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x40, 0x60, 0x80);
	app_style.sc_style.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xD0, 0xE0, 0xF0);
	app_style.sc_style.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x20, 0x30, 0x40);
	app_style.sc_style.bcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xB0, 0xD0, 0xF0);
	app_style.sc_style.rects.bopa = 70;
	app_style.sc_style.rects.bwidth = 1 * LV_STYLE_MULT;

	lv_labels_get(LV_LABELS_DEF,&app_style.sc_title_style);
	app_style.sc_title_style.font = app_style.font_small;
	app_style.sc_title_style.objs.color = COLOR_MAKE(0x20, 0x30, 0x40);
	app_style.sc_title_style.mid = 1;

	/*Window styles*/
	lv_wins_get(LV_WINS_DEF,&app_style.win_style);
	memcpy(&app_style.win_style.header, &app_style.menu_style, sizeof(lv_rects_t));
	memcpy(&app_style.win_style.title, &app_style.menu_btn_label_style, sizeof(lv_labels_t));
	memcpy(&app_style.win_style.ctrl_btn, &app_style.menu_btn_style, sizeof(lv_btns_t));
	memcpy(&app_style.win_style.ctrl_img, &app_style.menu_btn_img_style, sizeof(lv_imgs_t));
	app_style.win_style.header_on_content = 1;
	app_style.win_style.header_opa = app_style.menu_opa;
	app_style.win_style.ctrl_btn_opa = app_style.menu_btn_opa;
	app_style.win_style.header.vpad = 5 * LV_STYLE_MULT;
	app_style.win_style.header.hpad = 5 * LV_STYLE_MULT;
	app_style.win_style.header.opad = 5 * LV_STYLE_MULT;
	app_style.win_style.content.scrable_rects.vpad = app_style.win_style.ctrl_btn_h + 30;
}


