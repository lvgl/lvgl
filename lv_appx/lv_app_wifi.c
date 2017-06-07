/**
 * @file lv_app_wifi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_wifi.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_WIFI != 0

#include "../lv_app/lv_app_util/lv_app_kb.h"
#include "hal/wifi/wifi.h"
#include "misc/os/ptask.h"
#include "hal/systick/systick.h"
#include "misc/comm/wifimng.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define SSID_LIST_MAX_LENGTH    512 
#define WIFI_MONITOR_PERIOD     1000    /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
    char set_ssid[64];
    char set_pwd[64];
    char set_ip[32];
    char set_port[16];
}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{
    lv_obj_t * list;
    lv_obj_t * title;
    lv_obj_t * netw_ssid_ta;
    lv_obj_t * netw_pwd_ta;
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

static void wifi_state_monitor_task(void * param);

static lv_action_res_t netw_list_rel_action(lv_obj_t * btn, lv_dispi_t * dispi);
static lv_action_res_t netw_con_rel_action(lv_obj_t * btn, lv_dispi_t* dispi);
static lv_action_res_t netw_ssid_rel_action( lv_obj_t * ta, lv_dispi_t* dispi);
static lv_action_res_t netw_pwd_rel_action( lv_obj_t * ta, lv_dispi_t* dispi);
static lv_action_res_t tcp_ip_rel_action( lv_obj_t * ta, lv_dispi_t* dispi);
static lv_action_res_t tcp_port_rel_action( lv_obj_t * ta, lv_dispi_t* dispi);
static lv_action_res_t wifi_ap_select_action( lv_obj_t * ddlist, lv_dispi_t* dispi);

static void netw_ssid_kb_ok(lv_obj_t * ta);
static void netw_ssid_kb_close(lv_obj_t * ta);
static void netw_pwd_kb_ok(lv_obj_t * ta);
static void netw_pwd_kb_close(lv_obj_t * ta);
static void tcp_ip_kb_ok(lv_obj_t * ta);
static void tcp_ip_kb_close(lv_obj_t * ta);
static void tcp_port_kb_ok(lv_obj_t * ta);
static void tcp_port_kb_close(lv_obj_t * ta);

static void list_cb(wifi_state_t state, const char * txt);
static void tcp_transf_cb(wifi_state_t state, const char * txt);

static void win_title_refr(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "WiFi",
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

static char ssid_list[SSID_LIST_MAX_LENGTH];
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
const lv_app_dsc_t * lv_app_wifi_init(void)
{
    strcpy(ssid_list, "");
    
    wifimng_set_last_netw(LV_APP_WIFI_SSID_DEF, LV_APP_WIFI_PWD_DEF);
    wifimng_set_last_tcp(LV_APP_WIFI_IP_DEF, LV_APP_WIFI_PORT_DEF);
    
    ptask_create(wifi_state_monitor_task, WIFI_MONITOR_PERIOD, PTASK_PRIO_LOW, NULL);
    
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
    strcpy(adata->set_ssid, LV_APP_WIFI_SSID_DEF);
    strcpy(adata->set_pwd, LV_APP_WIFI_PWD_DEF);
    strcpy(adata->set_ip, LV_APP_WIFI_IP_DEF);
    strcpy(adata->set_port, LV_APP_WIFI_PORT_DEF);
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
        wifi_tcp_transf(data, size, tcp_transf_cb);
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
    
    wdata->list = lv_ddlist_create(win, NULL); 
    lv_obj_set_free_p(wdata->list, app);
    lv_ddlist_set_options_str(wdata->list, ssid_list);
    lv_ddlist_set_action(wdata->list, wifi_ap_select_action);
    
    lv_obj_t * list_btn = lv_btn_create(win, NULL);
    lv_obj_set_free_p(list_btn, app);
    lv_btn_set_rel_action(list_btn, netw_list_rel_action);
    lv_obj_t * label = lv_label_create(list_btn, NULL);
    lv_label_set_text(label, "List\nrefresh");
    
    lv_obj_t * ta_cont = lv_cont_create(win, NULL);
    lv_cont_set_fit(ta_cont, true, true);
    lv_cont_set_layout(ta_cont, LV_CONT_LAYOUT_COL_L);
    lv_obj_set_style(ta_cont, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
    
    wdata->netw_ssid_ta = lv_ta_create(ta_cont, NULL);
    lv_cont_set_fit(wdata->netw_ssid_ta, false, true);
    lv_obj_set_free_p(wdata->netw_ssid_ta, app);
    lv_page_set_rel_action(wdata->netw_ssid_ta, netw_ssid_rel_action);
    lv_ta_set_text(wdata->netw_ssid_ta, adata->set_ssid);
    lv_ta_set_cursor_show(wdata->netw_ssid_ta, false);
    
    wdata->netw_pwd_ta = lv_ta_create(ta_cont, wdata->netw_ssid_ta);
    lv_page_set_rel_action(wdata->netw_pwd_ta, netw_pwd_rel_action);
    lv_ta_set_text(wdata->netw_pwd_ta, adata->set_pwd);
  
    wdata->tcp_ip_ta = lv_ta_create(ta_cont, wdata->netw_ssid_ta);
    lv_page_set_rel_action(wdata->tcp_ip_ta, tcp_ip_rel_action);
    lv_ta_set_text(wdata->tcp_ip_ta, adata->set_ip);
    
    wdata->tcp_port_ta = lv_ta_create(ta_cont, wdata->netw_ssid_ta);
    lv_page_set_rel_action(wdata->tcp_port_ta, tcp_port_rel_action);
    lv_ta_set_text(wdata->tcp_port_ta, adata->set_port);
  
    
    lv_obj_t * con_btn = lv_btn_create(win, NULL);
    lv_obj_set_free_p(con_btn, app);
    lv_btn_set_rel_action(con_btn, netw_con_rel_action);
    label = lv_label_create(con_btn, NULL);
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

static void wifi_state_monitor_task(void * param)
{
    static wifimng_state_t state_prev = WIFIMNG_STATE_WAIT;
    wifimng_state_t state_act =  wifimng_get_state();
    
    if(state_prev != state_act && state_act == WIFIMNG_STATE_READY) {
        lv_app_notice_add("WiFi connected to:\n%s\n%s:%s", 
                           wifimng_get_last_ssid(), wifimng_get_last_ip(), wifimng_get_last_port());
    }
    
    
    state_prev = state_act;
}

static lv_action_res_t netw_list_rel_action(lv_obj_t * btn, lv_dispi_t* dispi)
{
    bool ret;
    ret = wifi_netw_list(list_cb);
    
    if(ret != false) {
        lv_app_notice_add("Listing WiFi networks");
    } else {
        lv_app_notice_add("Cannot list networks\nWiFi was busy. Try again");
        
    }
    return LV_ACTION_RES_OK;
}

static lv_action_res_t netw_con_rel_action(lv_obj_t * btn, lv_dispi_t* dispi)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_app_data_t * adata = app->app_data;
    
    wifimng_set_last_netw(adata->set_ssid, adata->set_pwd);
    wifimng_set_last_tcp(adata->set_ip, adata->set_port);
    wifimng_reconnect();
    lv_app_notice_add("Connecting to WiFi network\n%s, %s:%s", 
                      adata->set_ssid, adata->set_ip, adata->set_port);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t netw_ssid_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CURSOR_MANAGE, netw_ssid_kb_close ,netw_ssid_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t netw_pwd_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CURSOR_MANAGE, netw_pwd_kb_close ,netw_pwd_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t tcp_ip_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_TXT | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CURSOR_MANAGE, tcp_ip_kb_close ,tcp_ip_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t tcp_port_rel_action( lv_obj_t * ta, lv_dispi_t* dispi)
{
    lv_app_kb_open(ta, LV_APP_KB_MODE_NUM | LV_APP_KB_MODE_WIN_RESIZE | LV_APP_KB_MODE_CURSOR_MANAGE, tcp_port_kb_close ,tcp_port_kb_ok);
    return LV_ACTION_RES_OK;
}

static lv_action_res_t wifi_ap_select_action( lv_obj_t * ddlist, lv_dispi_t* dispi)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ddlist);
    my_app_data_t * adata = app->app_data;
    
    char ssid[256];
    lv_ddlist_get_selected_str(ddlist, ssid);
   
    my_win_data_t * wdata = app->win_data;
    lv_ta_set_text(wdata->netw_ssid_ta, ssid);
    strcpy(adata->set_ssid, ssid);
    
    return LV_ACTION_RES_OK;
}

static void netw_ssid_kb_ok(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_ssid, lv_ta_get_txt(ta));
}

static void netw_ssid_kb_close(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    lv_ta_set_text(ta, adata->set_ssid);
}

static void netw_pwd_kb_ok(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    strcpy(adata->set_pwd, lv_ta_get_txt(ta));
}

static void netw_pwd_kb_close(lv_obj_t * ta)
{
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * adata = app->app_data;
    lv_ta_set_text(ta, adata->set_pwd);
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

static void list_cb(wifi_state_t state, const char * txt)
{
    if(state == WIFI_STATE_READY) {
        if(txt[0] == '\0') {
        lv_app_notice_add("WiFi network list ready");
            return;
        }
        if(strlen(ssid_list) + strlen(txt) + 4 < sizeof(ssid_list)) {
            sprintf(ssid_list, "%s\n%s", ssid_list, txt);
        }
        
        lv_app_inst_t * app = lv_app_get_next(NULL, &my_app_dsc);
        while(app != NULL) {
            if(app->win_data != NULL) {
                my_win_data_t * wdata = app->win_data;
                lv_ddlist_set_options_str(wdata->list, ssid_list);
                
            }
            app = lv_app_get_next(app, &my_app_dsc);
        }
    } else if(state == WIFI_STATE_ERROR) {
        lv_app_notice_add("WiFi network list error\n%s", txt);
    }
}

static void tcp_transf_cb(wifi_state_t state, const char * txt)
{
    if(state == WIFI_STATE_READY) {
        uint16_t size =  txt[0] + ((txt[1] << 8) & 0xFF00);
        char buf[256];
        memcpy(buf, &txt[2], size);
        buf[size] = '\0';
        lv_app_com_send(app_act_com, LV_APP_COM_TYPE_CHAR, &txt[2], size);
    }else if(state == WIFI_STATE_ERROR) {
        lv_app_notice_add("WiFi TCP transfer error\n%s", txt);
        lv_app_notice_add("Reconnecting to WiFi...");
        wifimng_reconnect();
    }
    
    app_act_com = NULL;
}


static void win_title_refr(void)
{
    lv_app_inst_t * app;
    app = lv_app_get_next(NULL, &my_app_dsc);
    while(app != NULL) {
        if(app->win != NULL) {
            my_win_data_t * wdata = app->win_data;
            
            if(wifimng_get_state() != WIFIMNG_STATE_READY) lv_label_set_text(wdata->title, "Not connected");
            else {
                char buf[256];
                sprintf(buf, "%s - %s:%s", wifimng_get_last_ssid(), wifimng_get_last_ip(), wifimng_get_last_port());
                lv_label_set_text(wdata->title, buf);
            }
            lv_obj_set_width(wdata->title, lv_win_get_width(app->win));
        }
        app = lv_app_get_next(app, &my_app_dsc);
    }
}


#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_EXAMPLE != 0*/
