/**
 * @file lv_app_gsm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_gsm.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_GSM != 0

#include "../lv_app/lv_app_util/lv_app_kb.h"
#include "hal/gsm/gsm.h"
#include "misc/os/ptask.h"
#include "hal/systick/systick.h"
#include "misc/comm/gsmmng.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define GSM_MONITOR_PERIOD     1000    /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
    char set_apn[128];
    char set_ip[32];
    char set_port[16];
    uint8_t * last_msg_dp;
    uint16_t last_msg_size;
}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{
    lv_obj_t * title;
    lv_obj_t * netw_apn_ta;
    lv_obj_t * tcp_ip_ta;
    lv_obj_t * tcp_port_ta;
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

static void gsm_state_monitor_task(void * param);

static lv_action_res_t netw_con_rel_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t netw_apn_rel_action( lv_obj_t * ta, lv_dispi_t * dispi);
static lv_action_res_t tcp_ip_rel_action( lv_obj_t * ta, lv_dispi_t * dispi);
static lv_action_res_t tcp_port_rel_action( lv_obj_t * ta, lv_dispi_t * dispi);

static void netw_apn_kb_ok(lv_obj_t * ta);
static void netw_apn_kb_close(lv_obj_t * ta);
static void tcp_ip_kb_ok(lv_obj_t * ta);
static void tcp_ip_kb_close(lv_obj_t * ta);
static void tcp_port_kb_ok(lv_obj_t * ta);
static void tcp_port_kb_close(lv_obj_t * ta);

static void tcp_transf_cb(gsm_state_t state, const char * txt);

static void win_title_refr(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "GSM",
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

static lv_app_inst_t * app_act_com;

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
const lv_app_dsc_t * lv_app_gsm_init(void)
{
    
    gsmmng_set_last_apn(LV_APP_GSM_APN_DEF);
    gsmmng_set_last_tcp(LV_APP_GSM_IP_DEF, LV_APP_GSM_PORT_DEF);
    
    ptask_create(gsm_state_monitor_task, GSM_MONITOR_PERIOD, PTASK_PRIO_LOW, NULL);
    
    return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to a lv_app_example_conf_t structure with configuration data or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
static void my_app_run(lv_app_inst_t * app, void * conf)
{
    /*Initialize the application*/
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_apn, LV_APP_GSM_APN_DEF);
    strcpy(adata->set_ip, LV_APP_GSM_IP_DEF);
    strcpy(adata->set_port, LV_APP_GSM_PORT_DEF);
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
        app_act_com = app_rec;
        gsm_tcp_transf(data, size, tcp_transf_cb);
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
    my_app_data_t * adata = app->app_data;
    my_win_data_t * wdata = app->win_data;
    
    wdata->title = lv_label_create(win, NULL);
       
    lv_obj_t * ta_cont = lv_cont_create(win, NULL);
    lv_cont_set_fit(ta_cont, true, true);
    lv_cont_set_layout(ta_cont, LV_CONT_LAYOUT_COL_L);
    lv_obj_set_style(ta_cont, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
    
    wdata->netw_apn_ta = lv_ta_create(ta_cont, NULL);
    lv_cont_set_fit(wdata->netw_apn_ta, false, true);
    lv_obj_set_free_p(wdata->netw_apn_ta, app);
    lv_obj_set_width(wdata->netw_apn_ta, LV_DPI * 3);
    lv_page_set_rel_action(wdata->netw_apn_ta, netw_apn_rel_action);
    lv_ta_set_text(wdata->netw_apn_ta, adata->set_apn);
    lv_ta_set_cursor_show(wdata->netw_apn_ta, false);
  
    wdata->tcp_ip_ta = lv_ta_create(ta_cont, wdata->netw_apn_ta);
    lv_page_set_rel_action(wdata->tcp_ip_ta, tcp_ip_rel_action);
    lv_ta_set_text(wdata->tcp_ip_ta, adata->set_ip);
    
    wdata->tcp_port_ta = lv_ta_create(ta_cont, wdata->netw_apn_ta);
    lv_page_set_rel_action(wdata->tcp_port_ta, tcp_port_rel_action);
    lv_ta_set_text(wdata->tcp_port_ta, adata->set_port);
   
    lv_obj_t * con_btn = lv_btn_create(win, NULL);
    lv_obj_set_free_p(con_btn, app);
    lv_btn_set_rel_action(con_btn, netw_con_rel_action);
    lv_obj_t * label = lv_label_create(con_btn, NULL);
    lv_label_set_text(label, "Connect");
   
    lv_cont_set_layout(lv_page_get_scrl(lv_win_get_page(win)), LV_CONT_LAYOUT_PRETTY); 
    
    win_title_refr();
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

static void gsm_state_monitor_task(void * param)
{
    static gsmmng_state_t state_prev = GSMMNG_STATE_WAIT;
    gsmmng_state_t state_act =  gsmmng_get_state();
    
    if(state_prev != state_act && state_act == GSMMNG_STATE_READY) {
        lv_app_notice_add("GSM connected to:\n%s\n%s:%s", 
                           gsmmng_get_last_apn(), gsmmng_get_last_ip(), gsmmng_get_last_port());
        win_title_refr();
    }
      
    /* The GSM should be busy if there is sg. to send. 
     * It means fail during last send. Try again*/
    if(app_act_com != NULL) {
        if(gsm_busy() ==  false && state_act == GSMMNG_STATE_READY) {
            /*Try to send the message again*/
            lv_app_notice_add("Resend GSM message");
            my_app_data_t * adata = app_act_com->app_data;
            gsm_tcp_transf(adata->last_msg_dp, adata->last_msg_size, tcp_transf_cb);
        }
    }
    
    state_prev = state_act;
}

static lv_action_res_t netw_con_rel_action(lv_obj_t * btn, lv_dispi_t* dispi)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_app_data_t * adata = app->app_data;
    
    gsmmng_set_last_apn(adata->set_apn);
    gsmmng_set_last_tcp(adata->set_ip, adata->set_port);
    gsmmng_reconnect();
    lv_app_notice_add("Connecting to GSM network\n%s, %s:%s", 
                      adata->set_apn, adata->set_ip, adata->set_port);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t netw_apn_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CUR_MANAGE, netw_apn_kb_close ,netw_apn_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t tcp_ip_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CUR_MANAGE, tcp_ip_kb_close ,tcp_ip_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t tcp_port_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_NUM | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CUR_MANAGE, tcp_port_kb_close ,tcp_port_kb_ok);
    return LV_ACTION_RES_OK;
}

static void netw_apn_kb_ok(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_apn, lv_ta_get_txt(ta));
}

static void netw_apn_kb_close(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    lv_ta_set_text(ta, adata->set_apn);
}

static void tcp_ip_kb_ok(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_ip, lv_ta_get_txt(ta));
}

static void tcp_ip_kb_close(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    lv_ta_set_text(ta, adata->set_ip);
}

static void tcp_port_kb_ok(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_port, lv_ta_get_txt(ta));
}

static void tcp_port_kb_close(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    lv_ta_set_text(ta, adata->set_port);
}

static void tcp_transf_cb(gsm_state_t state, const char * txt)
{
    if(state == GSM_STATE_OK) {
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
    }else if(state == GSM_STATE_ERROR) {
        lv_app_notice_add("GSM TCP transfer error\n%s", txt);
        lv_app_notice_add("Reconnecting to GSM...");
        gsmmng_reconnect();
    }
}


static void win_title_refr(void)
{
    lv_app_inst_t * app;
    app = lv_app_get_next(NULL, &my_app_dsc);
    while(app != NULL) {
        if(app->win != NULL) {
            my_win_data_t * wdata = app->win_data;
            
            if(gsmmng_get_state() == GSMMNG_STATE_IDLE) {
                lv_label_set_text(wdata->title, "Not connected");
            } else if(gsmmng_get_state() == GSMMNG_STATE_READY) {
                lv_label_set_text(wdata->title, "Connecting ...");
            } else {
                char buf[256];
                sprintf(buf, "%s - %s:%s", gsmmng_get_last_apn(), gsmmng_get_last_ip(), gsmmng_get_last_port());
                lv_label_set_text(wdata->title, buf);
            }
            lv_obj_set_width(wdata->title, lv_win_get_width(app->win));
        }
        app = lv_app_get_next(app, &my_app_dsc);
    }
}


#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0*/
