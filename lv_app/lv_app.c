/**
 * @file lv_app.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app.h"
#include "lvgl/lv_objx/lv_btn.h"
#include "lvgl/lv_objx/lv_win.h"
#include "img_conf.h"

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


static void lv_app_init_style(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t app_dsc_ll; /*Store a pointer to the app descriptors*/
static ll_dsc_t app_inst_ll; /*Store the running apps*/

static lv_obj_t * menuh; 	/*Holder of the menu on the top*/
static lv_obj_t * app_btn;  /*The "Apps" button on the menu*/
static lv_obj_t * sys_apph; /*Holder of the system app. buttons*/
static lv_obj_t * app_list;
static lv_obj_t * sc_area;

static lv_app_style_t app_style;

LV_IMG_DECLARE(img_close);

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

	lv_app_init_style();

	/*Create the desktop elements*/

	/*Shortcut area*/
	lv_obj_t * sc_page = lv_page_create(lv_scr_act(), NULL);
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

	/*Create images for the window control buttons*/
	//lv_img_create_file("close",img_close);

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
	app->sc = lv_btn_create(sc_area, NULL);
	lv_obj_set_free_p(app->sc, app);
	lv_obj_set_size(app->sc, app_style.sc_w, app_style.sc_h);
	lv_btn_set_rel_action(app->sc, lv_app_sc_rel_action);
	lv_btn_set_pr_action(app->sc, lv_app_sc_pr_action);
	lv_page_glue_obj(app->sc, true);

	app->sc_data = dm_alloc(app->dsc->sc_data_size);
	app->dsc->sc_open(app, app->sc);

	return app->sc;
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
void lv_app_sc_close(lv_app_inst_t * app)
{

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

	lv_win_add_ctrl_btn(app->win, "U:/close", lv_app_win_close_action);

	app->dsc->win_open(app, app->win);

	return app->win;
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
void lv_app_win_close(lv_app_inst_t * app)
{
	lv_obj_del(app->win);
}


void lv_app_refr_style(void)
{
	lv_obj_set_opa(menuh, app_style.menu_opa);
	lv_obj_set_opa(app_btn, app_style.menu_btn_opa);

	lv_obj_set_width(sc_area, LV_HOR_RES - 2 *
			   (app_style.sc_page_style.bg_rects.hpad +
			    app_style.sc_page_style.scrable_rects.hpad));
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
	lv_app_win_close(app);
	return LV_ACTION_RES_INV;
}

static void lv_app_init_style(void)
{
	/*Coordinates*/
	app_style.menu_h = 40 * LV_DOWNSCALE;
	app_style.sc_w = LV_HOR_RES / 4;
	app_style.sc_h = LV_VER_RES / 3;
	app_style.app_list_w = LV_HOR_RES / 3;
	app_style.app_list_h = (2 * LV_VER_RES) / 3;

	/*Fonts*/
	app_style.font_small = FONT_DEJAVU_20;
	app_style.font_medium = FONT_DEJAVU_30;
	app_style.font_large = FONT_DEJAVU_40;

	/*Opacity*/
	app_style.menu_opa = OPA_90;
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

	lv_imgs_get(LV_IMGS_DEF,&app_style.menu_btn_img_style);

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

	lv_labels_get(LV_LABELS_DEF,&app_style.sc_title_style);

	/*Window styles*/
	lv_wins_get(LV_WINS_DEF,&app_style.win_style);
	memcpy(&app_style.win_style.header, &app_style.menu_style, sizeof(lv_rects_t));
	memcpy(&app_style.win_style.title, &app_style.menu_btn_label_style, sizeof(lv_labels_t));
	memcpy(&app_style.win_style.ctrl_btn, &app_style.menu_btn_style, sizeof(lv_btns_t));
	app_style.win_style.header_on_content = 1;
	app_style.win_style.header_opa = app_style.menu_opa;
	app_style.win_style.ctrl_btn_opa = app_style.menu_btn_opa;
	app_style.win_style.header.vpad = 5 * LV_STYLE_MULT;
	app_style.win_style.header.hpad = 5 * LV_STYLE_MULT;
	app_style.win_style.header.opad = 5 * LV_STYLE_MULT;
	app_style.win_style.content.scrable_rects.vpad = app_style.win_style.ctrl_btn_h + 30;
}


