/**
 * @file lv_app_benchmark.c
 *
 */

/*TODO
 * Win: - Complex GUI with: bg, buttons, text, lines, few recolored images
 *      - Buttons: Bg ON/OFF, Alpha: ON/OFF, Start (make a scr refr), Continuous (benchmark)
 *      - Text: last px num, last refr_time, last px/us, avg px/us
 * Shorcut: - Show last px/us
 *
 * Use: lv_refr_set_monitor_cb();
 * Ignore results if px_num == shortcut_label_size because not its test is the goal
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_benchmark.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0

#include "../lv_app/lv_app_util/lv_app_kb.h"
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
static void my_app_run(lv_app_inst_t * app, void * conf);
static void my_app_close(lv_app_inst_t * app);
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec, lv_app_com_type_t type , const void * data, uint32_t size);
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc);
static void my_sc_close(lv_app_inst_t * app);
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win);
static void my_win_close(lv_app_inst_t * app);

static lv_action_res_t ta_rel_action(lv_obj_t * ta, lv_dispi_t * dispi);
static void kb_ok_action(lv_obj_t * ta);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Benchamrk",
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

/**
 * Initialize the application
 * @return pointer to the application descriptor of this application
 */
const lv_app_dsc_t * lv_app_benchmark_init(void)
{
	return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to a lv_app_benchmark_conf_t structure with configuration data or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
static void my_app_run(lv_app_inst_t * app, void * conf)
{
    /*Initialize the application*/
}

/**
 * Close a running application.
 * Close the Window and the Shortcut too if opened.
 * Free all the allocated memory by this application.
 * @param app pointer to an application
 */
static void my_app_close(lv_app_inst_t * app)
{
    /*No dynamically allocated data in 'my_app_data'*/
}

/**
 * Read the data have been sent to this application
 * @param app_send pointer to an application which sent the message
 * @param app_rec pointer to an application which is receiving the message
 * @param type type of data from 'lv_app_com_type_t' enum
 * @param data pointer to the sent data
 * @param size length of 'data' in bytes
 */
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec,
                       lv_app_com_type_t type , const void * data, uint32_t size)
{
	if(type == LV_APP_COM_TYPE_CHAR) {      /*data: string*/
	    my_sc_data_t * sc_data = app_rec->sc_data;
	    if (sc_data->label != NULL) {
	        lv_label_set_text_array(sc_data->label, data, size);
	        lv_obj_align(sc_data->label , NULL,LV_ALIGN_CENTER, 0, 0);
	    }
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
	lv_label_set_text(sc_data->label, "Empty");
	lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
static void my_sc_close(lv_app_inst_t * app)
{
    /*No dynamically allocated data in 'my_sc_data'*/
}


/**
 * Open the application in a window
 * @param app pointer to an application
 * @param win pointer to a window object where
 *            the application can create content
 */
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win)
{
	lv_obj_t * ta;
	ta = lv_ta_create(win, NULL);
    lv_obj_set_size_us(ta, 200, 100);
	lv_obj_set_pos_us(ta, 0, 0);
    lv_obj_set_free_p(ta, app);
	lv_page_set_rel_action(ta, ta_rel_action);
	lv_ta_set_text(ta, "Write a text to send to the other applications");
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
static void my_win_close(lv_app_inst_t * app)
{

}

/*--------------------
 * OTHER FUNCTIONS
 ---------------------*/

/**
 * Called when the text area on the window is released to open the app. keyboard
 * @param ta pointer to the text area on the window
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK because the text area is not deleted
 */
static lv_action_res_t ta_rel_action(lv_obj_t * ta, lv_dispi_t * dispi)
{
    lv_ta_set_text(ta, ""); /*Clear the ta*/
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE, NULL, kb_ok_action);
    return LV_ACTION_RES_OK;
}

/**
 * Called when the "Ok" button is  pressed on the app. keyboard
 * @param ta pointer to the text area assigned to the app. kexboard
 */
static void kb_ok_action(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    const char * txt = lv_ta_get_txt(ta);
    lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, txt, strlen(txt));
}

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0*/
