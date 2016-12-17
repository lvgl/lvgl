/**
 * @file lv_app_example.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_example.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
	const char * txt;
}app_data_t;

/*Application specific data a window of this application*/
typedef struct
{

}win_data_t;

/*Application specific data for a shortcut of this application*/
typedef struct
{

}sc_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void my_app_run(lv_app_inst_t * app, const char * cstr);
static void my_app_close(lv_app_inst_t * app);
static void my_event_read(lv_app_inst_t * app, lv_app_event_t event);
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc);
static void my_sc_close(lv_app_inst_t * app);
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win);
static void my_win_close(lv_app_inst_t * app);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Example",
	.mode = LV_APP_MODE_NONE,
	.app_run = my_app_run,
	.app_close = my_app_close,
	.event_read = my_event_read,
	.win_open = my_win_open,
	.win_close = my_win_close,
	.sc_open = my_sc_open,
	.sc_close = my_sc_close,
	.app_data_size = sizeof(app_data_t),
	.sc_data_size = sizeof(sc_data_t),
	.win_data_size = sizeof(win_data_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const lv_app_dsc_t * lv_app_example_init(void)
{
	return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param cstr a Create STRing which can give initial parameters to the application (NULL or "" if unused)
 * @return pointer to the opened application or NULL if any error occurred
 */
static void my_app_run(lv_app_inst_t * app, const char * cstr)
{
	/*Check the create string*/
	if(cstr == NULL || cstr[0] == '\0') {
		cstr = "No create\nstring";
	}

	/*Initialize the application*/
	((app_data_t *)app->app_data)->txt = cstr;	/*Save the create string*/
	char buf[256];
	sprintf(buf,"%s - %s", my_app_dsc.name, cstr);
	lv_app_rename(app, buf);
}

/**
 * Close a running application.
 * Close the Window and the Shortcut too if opened.
 * Free all the allocated memory by this application.
 * @param app pointer to an application
 */
static void my_app_close(lv_app_inst_t * app)
{

}

/**
 * Publish an event.
 * @param app pointer to an application which publishes the event
 * @param event an event from 'lv_app_event_t' enum
 */
static void my_event_read(lv_app_inst_t * app, lv_app_event_t event)
{

}

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @param sc pointer to an object where the application
 *           can create content of the shortcut
 */
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc)
{
	lv_obj_t * label;
	label = lv_label_create(sc, NULL);
	lv_label_set_text(label, ((app_data_t *)app->app_data)->txt);
	lv_obj_set_style(label, lv_labels_get(LV_LABELS_BTN, NULL));
	lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
static void my_sc_close(lv_app_inst_t * app)
{

}

/**
 * Open the application in a window
 * @param app pointer to an application
 * @param win pointer to a window object where
 *            the application can create content
 */
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win)
{
	lv_obj_t * label;
	label = lv_label_create(win, NULL);
	lv_label_set_text(label, ((app_data_t *)app->app_data)->txt);
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
static void my_win_close(lv_app_inst_t * app)
{

}


