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
static bool lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi);
static bool lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi);


/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t app_dsc_ll; /*Store a pointer to the app descriptors*/
static ll_dsc_t app_inst_ll; /*Store the running apps*/
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

	/*Create images for the window control buttons*/
	lv_img_create_file("close",img_close);

	/*Initialize all application descriptors*/
	const lv_app_dsc_t ** dsc;
#if USE_LV_APP_EXAMPLE
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
	app->sc = lv_btn_create(lv_scr_act(), NULL);
	lv_obj_set_free_p(app->sc, app);
	lv_btn_set_rel_action(app->sc, lv_app_sc_rel_action);

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
	app->win = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_free_p(app->win, app);
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


const lv_app_dsc_t * lv_app_dsc_get(const char * name)
{
	const lv_app_dsc_t ** dsc;
	LL_READ(app_dsc_ll, dsc) {
		if(strcmp((*dsc)->name, name) == 0) {
			return *dsc;
		}
	}

	return NULL;


}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
	lv_app_inst_t * app = lv_obj_get_free_p(sc);
	lv_app_win_open(app);
	return true;
}

static bool lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(close_btn);
	lv_app_inst_t * app = lv_obj_get_free_p(win);
	lv_app_win_close(app);
	return false;
}


