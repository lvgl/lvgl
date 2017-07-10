/**
 * @file lv_app_ethernet.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_ethernet.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_ETHERNET != 0

#include "../lv_app/lv_app_util/lv_app_kb.h"
#include "hal/eth/eth.h"
#include "misc/os/ptask.h"
#include "hal/systick/systick.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define ETH_MONITOR_PERIOD     1000    /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
    uint8_t * last_msg_dp;
    uint16_t last_msg_size;
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

static void eth_state_monitor_task(void * param);
static void tcp_transf_cb(eth_state_t state, const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Ethernet",
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

static lv_app_inst_t * app_act_com = NULL;

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
const lv_app_dsc_t * lv_app_ethernet_init(void)
{
    ptask_create(eth_state_monitor_task, ETH_MONITOR_PERIOD, PTASK_PRIO_LOW, NULL);
	return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to a lv_app_ethernet_conf_t structure with configuration data or NULL if unused
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
        eth_tcp_transf(data, size, tcp_transf_cb);
        app_act_com = app_rec;
        my_app_data_t * adata = app_act_com->app_data;
        if(adata->last_msg_dp != NULL) dm_free(adata->last_msg_dp); 
        
        adata->last_msg_dp = dm_alloc(size);
        memcpy(adata->last_msg_dp, data, size);
        adata->last_msg_size = size;
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

static void eth_state_monitor_task(void * param)
{   
    /* The eth. should be busy if there is sg. to send. 
     * It means fail during last send. Try again*/
    if(app_act_com != NULL && eth_busy() ==  false) {
        /*Try to send the message again*/
        lv_app_notice_add("Resend Ethernet message");
        my_app_data_t * adata = app_act_com->app_data;
        eth_tcp_transf(adata->last_msg_dp, adata->last_msg_size, tcp_transf_cb);
    }
}

static void tcp_transf_cb(eth_state_t state, const char * txt)
{
    if(state == ETH_STATE_OK) {
        uint16_t size =  txt[0] + ((txt[1] << 8) & 0xFF00);
        char buf[256];
        memcpy(buf, &txt[2], size);
        buf[size] = '\0';
        lv_app_com_send(app_act_com, LV_APP_COM_TYPE_CHAR, &txt[2], size);
        my_app_data_t * adata = app_act_com->app_data;
        dm_free(adata->last_msg_dp);
        adata->last_msg_dp = NULL;
        adata->last_msg_size = 0;
        app_act_com = NULL;
    }else if(state == ETH_STATE_ERROR) {
        lv_app_notice_add("Ethernet TCP transfer error\n%s", txt);
    }
    
}
#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_ETHERNET != 0*/
