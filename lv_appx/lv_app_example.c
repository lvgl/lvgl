/**
 * @file lv_app_example.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_example.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0

#include "../lv_app/lv_app_sup.h"
#include "misc/os/ptask.h"
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
}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{

}my_win_data_t;

/*Application specific data for a shortcut of this application*/
typedef struct
{
    lv_obj_t * label;
}my_sc_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void my_app_run(lv_app_inst_t * app, const char * cstr);
static void my_app_close(lv_app_inst_t * app);
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec, lv_app_com_type_t type , const void * data, uint32_t len);
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc);
static void my_sc_close(lv_app_inst_t * app);
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win);
static void my_win_close(lv_app_inst_t * app);

static void task(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Example",
	.mode = LV_APP_MODE_NONE,
	.app_run = my_app_run,
	.app_close = my_app_close,
	.com_rec = my_com_rec,
	.win_open = my_win_open,
	.win_close = my_win_close,
	.sc_open = my_sc_open,
	.sc_close = my_sc_close,
	.app_data_size = sizeof(my_app_data_t),
	.sc_data_size = sizeof(my_sc_data_t),
	.win_data_size = sizeof(my_win_data_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const lv_app_dsc_t * lv_app_example_init(void)
{
    ptask_create(task, 200, PTASK_PRIO_MID);

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
	((my_app_data_t *)app->app_data)->txt = cstr;	/*Save the create string*/
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
 * Read the data have been sent to this application
 * @param app_send pointer to an application which sent the message
 * @param app_rec pointer to an application which is receiving the message
 * @param type type of data from 'lv_app_com_type_t' enum
 * @param data pointer to the sent data
 * @param len length of 'data' in bytes
 */
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec,
                       lv_app_com_type_t type , const void * data, uint32_t len)
{
	if(type == LV_APP_COM_TYPE_STR) {      /*data: string*/
	    my_sc_data_t * sc_data = app_rec->sc_data;
	    if (sc_data->label != NULL) {
	        lv_label_set_text(sc_data->label, data);
	        lv_obj_align(sc_data->label , NULL,LV_ALIGN_CENTER, 0, 0);

	    }
	}
	else if(type == LV_APP_COM_TYPE_BIN) { /*data: array of 'int32_t' */

	}
}

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @param sc pointer to an object where the application
 *           can create content of the shortcut
 */
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc)
{
    my_sc_data_t * sc_data = app->sc_data;

    sc_data->label = lv_label_create(sc, NULL);
	lv_label_set_text(sc_data->label, ((my_app_data_t *)app->app_data)->txt);
	lv_obj_set_style(sc_data->label, lv_labels_get(LV_LABELS_DEF, NULL));
	lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
static void my_sc_close(lv_app_inst_t * app)
{

}

lv_action_res_t kb_open(lv_obj_t * ta, lv_dispi_t * dispi);
/**
 * Open the application in a window
 * @param app pointer to an application
 * @param win pointer to a window object where
 *            the application can create content
 */
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win)
{
    my_sc_data_t * win_data = app->win_data;


    win_data->label = lv_label_create(win, NULL);
	lv_label_set_text(win_data->label, ((my_app_data_t *)app->app_data)->txt);


	lv_obj_t * ta;
	ta = lv_ta_create(win, NULL);
    lv_obj_set_size_us(ta, 200, 100);
	lv_obj_set_pos_us(ta, 20, 200);
	lv_page_set_rel_action(ta, kb_open);
	lv_obj_set_free_p(ta, app);
}

void kb_ok(lv_obj_t * ta) {
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    const char * txt = lv_ta_get_txt(ta);
    lv_app_com_send(app, LV_APP_COM_TYPE_STR, txt, strlen(txt) + 1);
}

lv_action_res_t kb_open(lv_obj_t * ta, lv_dispi_t * dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT, NULL, kb_ok);
    return LV_ACTION_RES_OK;
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
static void my_win_close(lv_app_inst_t * app)
{

}


static void task(void)
{

    return;
    dm_defrag();

    dm_mon_t mon;
    dm_monitor(&mon);

    lv_app_inst_t * app;
    app = lv_app_get_next_app(NULL, &my_app_dsc);

    while(app != NULL) {
        char buf[256];
        sprintf(buf, "Mem. total: %d\nMem. free: %d (%d)\nFrag: %d%%",
                DM_MEM_SIZE, mon.size_free, mon.size_free * 100 / DM_MEM_SIZE, mon.pct_frag);

        if(app->sc_data != NULL) {
            my_sc_data_t * sc_data = app->sc_data;
            lv_label_set_text(sc_data->label, buf);
            lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
        }

        if(app->win_data != NULL) {
            my_sc_data_t * win_data = app->win_data;
            lv_label_set_text(win_data->label, buf);
      //      lv_obj_align(win_data->label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
        }
        app = lv_app_get_next_app(app, &my_app_dsc);
    }


}

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0*/
