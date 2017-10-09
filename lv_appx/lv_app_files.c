/**
 * @file lv_app_example.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_files.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_FILES != 0

#include <stdio.h>
#include "misc/os/ptask.h"
#include "../lv_app/lv_app_util/lv_app_kb.h"
#include "../lv_app/lv_app_util/lv_app_notice.h"

/*********************
 *      DEFINES
 *********************/
#define LV_APP_FILES_CHUNK_MIN_SIZE     32
#define LV_APP_FILES_CHUNK_MIN_TIME     10
#define LV_APP_FILES_CHUNK_MAX_TIME     10000

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
    char path[LV_APP_FILES_PATH_MAX_LEN];
    char fn[LV_APP_FILES_FN_MAX_LEN];
    fs_file_t file;
    uint8_t file_cnt;
    uint16_t chunk_delay;
    uint16_t chunk_size;
    uint8_t send_fn     :1;
    uint8_t send_size   :1;
    uint8_t send_crc    :1;
    uint8_t send_in_prog :1;
    ptask_t * send_task;
}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{
    lv_obj_t * file_list;
}my_win_data_t;

/*Application specific data for a shortcut of this application*/
typedef struct
{
    lv_obj_t * label;
}my_sc_data_t;

typedef enum
{
    SEND_SETTINGS_FN,
    SEND_SETTINGS_SIZE,
    SEND_SETTINGS_CRC,
    SEND_SETTINGS_CHUNK_SIZE,
    SEND_SETTINGS_CHUNK_DELAY,
}send_settings_id_t;

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
static void my_conf_open(lv_app_inst_t * app, lv_obj_t * conf_win);

static void win_load_file_list(lv_app_inst_t * app);
static void win_create_list(lv_app_inst_t * app);
static lv_action_res_t win_up_action(lv_obj_t * up, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_next_action(lv_obj_t * next, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_prev_action(lv_obj_t * prev, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_drv_action(lv_obj_t * drv, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_folder_action(lv_obj_t * folder, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_file_action(lv_obj_t * file, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_send_rel_action(lv_obj_t * send, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_send_settings_element_rel_action(lv_obj_t * element, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_back_action(lv_obj_t * back, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_del_rel_action(lv_obj_t * del, lv_indev_proc_t * indev_proc);
static lv_action_res_t win_del_lpr_action(lv_obj_t * del, lv_indev_proc_t * indev_proc);
static void send_settings_kb_close_action(lv_obj_t * ta);
static void send_settings_kb_ok_action(lv_obj_t * ta);
static void start_send(lv_app_inst_t * app, const char * path);
static void send_task(void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Files",
	.mode = LV_APP_MODE_NONE,
	.app_run = my_app_run,
	.app_close = my_app_close,
	.com_rec = my_com_rec,
	.win_open = my_win_open,
	.win_close = my_win_close,
	.sc_open = my_sc_open,
	.sc_close = my_sc_close,
    .conf_open = my_conf_open,
	.app_data_size = sizeof(my_app_data_t),
	.sc_data_size = sizeof(my_sc_data_t),
	.win_data_size = sizeof(my_win_data_t),
};

static lv_style_t style_sc_label;
static lv_style_t style_btn_symbol;


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
const lv_app_dsc_t * lv_app_files_init(void)
{
    lv_app_style_t * app_style = lv_app_style_get();
    memcpy(&style_sc_label, &app_style->sc_rec_rel, sizeof(lv_style_t));
    style_sc_label.font = font_get(LV_APP_FONT_LARGE);

    lv_style_get(LV_STYLE_BTN_REL, &style_btn_symbol);
    style_btn_symbol.font = font_get(LV_IMG_DEF_SYMBOL_FONT);
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
    my_app_data_t * app_data = app->app_data;
    app_data->file_cnt = 0;
    app_data->path[0] = '\0';
    app_data->fn[0] = '\0';
    app_data->send_fn = 0;
    app_data->send_size = 0;
    app_data->send_crc = 0;
    app_data->chunk_size = LV_APP_FILES_CHUNK_DEF_SIZE;
    app_data->chunk_delay = LV_APP_FILES_CHUNK_DEF_TIME;
    app_data->send_in_prog = 0;

    app_data->send_task = ptask_create(send_task, LV_APP_FILES_CHUNK_DEF_TIME, PTASK_PRIO_OFF, app);
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
    my_app_data_t * app_data = app->app_data;
    ptask_del(app_data->send_task);
    if(app_data->send_in_prog != 0) fs_close(&app_data->file);

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
    if(type == LV_APP_COM_TYPE_CHAR) {
        /*Check for file query. E.g. "U:/file.txt?"*/
        const char * path = data;
        if(path[size - 1] == '?') {
            if(size > LV_APP_FILES_PATH_MAX_LEN + LV_APP_FILES_FN_MAX_LEN) {
                lv_app_notice_add("Can not send file:\ntoo long path");
            }

            char path_fn[LV_APP_FILES_PATH_MAX_LEN + LV_APP_FILES_FN_MAX_LEN];
            memcpy(path_fn, data, size - 1); /*-1 to ignore the '?' at the end*/
            path_fn[size - 1] = '\0';
            start_send(app_rec, path_fn);
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
    my_app_data_t * app_data = app->app_data;


    sc_data->label = lv_label_create(sc, NULL);
    lv_obj_set_style(sc_data->label, &style_sc_label);
    lv_label_set_text(sc_data->label, fs_get_last(app_data->path));
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
    my_win_data_t * win_data = app->win_data;
    my_app_data_t * app_data = app->app_data;

    app_data->file_cnt = 0;
    win_data->file_list = NULL;

    lv_win_set_title(win, app_data->path);

    win_load_file_list(app);
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
static void my_win_close(lv_app_inst_t * app)
{

}

/**
 * Create objects to configure the applications
 * @param app pointer to an application which settings should be created
 * @param conf_win pointer to a window where the objects can be created
 *                (the window has the proper layout)
 */
static void my_conf_open(lv_app_inst_t * app, lv_obj_t * conf_win)
{
    my_app_data_t * app_data = app->app_data;

    /*Create check boxes*/
    lv_obj_t * cb;

    /*Send file name check box*/
    cb = lv_cb_create(conf_win, NULL);
    lv_cb_set_text(cb, "Send file name");
    lv_obj_set_free_num(cb, SEND_SETTINGS_FN);
    lv_obj_set_free_p(cb, app);
    lv_btn_set_rel_action(cb, win_send_settings_element_rel_action);
    if(app_data->send_fn != 0) lv_btn_set_state(cb, LV_BTN_STATE_TREL);
    else lv_btn_set_state(cb, LV_BTN_STATE_REL);

    /*Send size check box*/
    cb = lv_cb_create(conf_win, cb);
    lv_cb_set_text(cb, "Send size");
    lv_obj_set_free_num(cb, SEND_SETTINGS_SIZE);
    if(app_data->send_size != 0) lv_btn_set_state(cb, LV_BTN_STATE_TREL);
    else lv_btn_set_state(cb, LV_BTN_STATE_REL);

    /*Send CRC check box*/
    cb = lv_cb_create(conf_win, cb);
    lv_cb_set_text(cb, "Send CRC");
    lv_obj_set_free_num(cb, SEND_SETTINGS_CRC);
    if(app_data->send_crc != 0) lv_btn_set_state(cb, LV_BTN_STATE_TREL);
    else lv_btn_set_state(cb, LV_BTN_STATE_REL);

    /*Create a text area to type chunk size*/
    lv_obj_t * val_set_h;
    val_set_h = lv_cont_create(conf_win, NULL);
    lv_obj_set_style(val_set_h, lv_style_get(LV_STYLE_PLAIN_COLOR, NULL));
    lv_obj_set_click(val_set_h, false);
    lv_cont_set_fit(val_set_h, true, true);
    lv_cont_set_layout(val_set_h, LV_CONT_LAYOUT_ROW_M);

    lv_obj_t * label;
    label = lv_label_create(val_set_h, NULL);
    lv_label_set_text(label, "Chunk size");

    lv_obj_t * ta;
    char buf[32];
    ta = lv_ta_create(val_set_h, NULL);
    lv_cont_set_fit(ta, false, true);
    lv_obj_set_free_num(ta, SEND_SETTINGS_CHUNK_SIZE);
    lv_obj_set_free_p(ta, app);
    lv_page_set_rel_action(ta, win_send_settings_element_rel_action);
    sprintf(buf, "%d", app_data->chunk_size);
    lv_ta_set_text(ta, buf);

    /*Create a text area to type the chunk delay*/
    val_set_h = lv_cont_create(conf_win, val_set_h);

    label = lv_label_create(val_set_h, NULL);
    lv_label_set_text(label, "Inter-chunk delay");

    ta = lv_ta_create(val_set_h, ta);
    lv_obj_set_free_num(ta, SEND_SETTINGS_CHUNK_DELAY);
    sprintf(buf, "%d", app_data->chunk_delay);
    lv_ta_set_text(ta, buf);
}

/*--------------------
 * OTHER FUNCTIONS
 ---------------------*/
/**
 * Create an mpty list on the window. 'win_load_file_list' will fill it.
 * @param app pointer to a Files application
 */
static void win_create_list(lv_app_inst_t * app)
{
    my_win_data_t * win_data = app->win_data;

    /*Delete the previous list*/
    if(win_data->file_list != NULL) {
      lv_obj_del(win_data->file_list);
    }

    /*Create a new list*/
    win_data->file_list = lv_list_create(app->win, NULL);
    lv_obj_set_width(win_data->file_list, lv_win_get_width(app->win));
    lv_list_set_style_img(win_data->file_list, &style_btn_symbol);
    lv_obj_set_style(lv_page_get_scrl(win_data->file_list), lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
    lv_obj_set_drag_parent(win_data->file_list, true);
    lv_obj_set_drag_parent(lv_page_get_scrl(win_data->file_list), true);
    lv_cont_set_fit(win_data->file_list, false, true);
    lv_cont_set_layout(lv_page_get_scrl(win_data->file_list), LV_CONT_LAYOUT_COL_L);
}

/**
 * Load the file list from the current path on the window
 * @param app pointer to a Files application
 */
static void win_load_file_list(lv_app_inst_t * app)
{
    my_app_data_t * app_data = app->app_data;
    my_win_data_t * win_data = app->win_data;

    /*Create a new list*/
    win_create_list(app);

    fs_res_t res = FS_RES_OK;

      /*At empty path show the drivers */
    lv_obj_t * liste;
    if(app_data->path[0] == '\0') {
        char drv[16];
        char buf[2];
        fs_get_letters(drv);
        uint8_t i;
        for(i = 0; drv[i] != '\0'; i++) {
            buf[0] = drv[i];
            buf[1] = '\0';
            liste = lv_list_add(win_data->file_list, SYMBOL_DRIVE, buf, win_drv_action);
            lv_obj_set_free_p(liste, app);
        }
    }
    /*List the files/folders with fs interface*/
    else {
        liste = lv_list_add(win_data->file_list, SYMBOL_UP, "Up", win_up_action);
        lv_obj_set_free_p(liste, app);

        fs_readdir_t rd;
        res = fs_readdir_init(&rd, app_data->path);
        if(res != FS_RES_OK) {
            lv_app_notice_add("Can not read the\npath in Files");
        return;
        }

        /*At not first page add prev. page button */
        if(app_data->file_cnt != 0) {
            liste = lv_list_add(win_data->file_list, SYMBOL_LEFT, "Previous page", win_prev_action);
            lv_obj_set_free_p(liste, app);
        }

        char fn[LV_APP_FILES_FN_MAX_LEN];

        /*Read the files from the previous pages*/
        uint16_t file_cnt = 0;
        while(file_cnt <= app_data->file_cnt) {
            res = fs_readdir(&rd, fn);
            if(res != FS_RES_OK ){
                lv_app_notice_add("Can not read\nthe path in Files");
                return;
            }
            file_cnt ++;
        }

        /*Add list elements from the files and folders*/
        while(res == FS_RES_OK && fn[0] != '\0') {
            if(fn[0] == '/') { /*Add a folder*/
                lv_obj_t * liste;
                liste = lv_list_add(win_data->file_list, SYMBOL_FOLDER, &fn[1], win_folder_action);
                lv_obj_set_free_p(liste, app);
                app_data->file_cnt ++;
            }
            /*Add a file*/
            else {
                liste = lv_list_add(win_data->file_list, SYMBOL_FILE, fn, win_file_action);
                lv_obj_set_free_p(liste, app);
                app_data->file_cnt ++;
            }

            /*Get the next element*/
            res = fs_readdir(&rd, fn);

            /*Show only LV_APP_FSEL_MAX_FILE elements and add a Next page button*/
            if(app_data->file_cnt != 0 && app_data->file_cnt % LV_APP_FILES_PAGE_SIZE == 0) {
                liste = lv_list_add(win_data->file_list, SYMBOL_RIGHT, "Next page", win_next_action);
                lv_obj_set_free_p(liste, app);
                break;
            }
        }

      /*Close the read directory*/
      fs_readdir_close(&rd);
    }

    if(res != FS_RES_OK) {
        lv_app_notice_add("Can not read\nthe path in Files");
    }

    /*Focus to the top of the list*/
    lv_obj_set_y(lv_page_get_scrl(win_data->file_list), 0);
    return;
}


/**
 * Called when the Up list element is released to step one level
 * @param up pointer to the Up button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_up_action(lv_obj_t * up, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(up);
    my_app_data_t * app_data = app->app_data;
    fs_up(app_data->path);
    app_data->file_cnt = 0;
    lv_win_set_title(app->win, app_data->path);

    my_sc_data_t * sc_data = app->sc_data;
    if(sc_data != NULL) {
        lv_label_set_text(sc_data->label, fs_get_last(app_data->path));
        lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
    }

    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Next list element is released to go to the next page
 * @param next pointer to the Next button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_next_action(lv_obj_t * next, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(next);
    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Prev list element is released to previous page
 * @param prev pointer to the Prev button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_prev_action(lv_obj_t * prev, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(prev);
    my_app_data_t * app_data = app->app_data;
    if(app_data->file_cnt <= 2 * LV_APP_FILES_PAGE_SIZE) app_data->file_cnt = 0;
    else if(app_data->file_cnt % LV_APP_FILES_PAGE_SIZE == 0) {
        app_data->file_cnt -= 2 * LV_APP_FILES_PAGE_SIZE;
    } else {
        app_data->file_cnt = ((app_data->file_cnt / LV_APP_FILES_PAGE_SIZE) - 1) * LV_APP_FILES_PAGE_SIZE;
    }

    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}


/**
 * Called when the Driver list element is released to step into a driver
 * @param drv pointer to the Driver button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_drv_action(lv_obj_t * drv, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(drv);
    my_app_data_t * app_data = app->app_data;
    sprintf(app_data->path, "%s:", lv_list_get_element_text(drv));
    app_data->file_cnt = 0;
    lv_win_set_title(app->win, app_data->path);
    my_sc_data_t * sc_data = app->sc_data;
    if(sc_data != NULL) {
        lv_label_set_text(sc_data->label, fs_get_last(app_data->path));
        lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
    }

    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}


/**
 * Called when a folder list element is released to enter into it
 * @param folder pointer to a folder button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_folder_action(lv_obj_t * folder, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(folder);
    my_app_data_t * app_data = app->app_data;
    sprintf(app_data->path, "%s/%s", app_data->path, lv_list_get_element_text(folder));
    app_data->file_cnt = 0;

    lv_win_set_title(app->win, app_data->path);
    my_sc_data_t * sc_data = app->sc_data;
    if(sc_data != NULL) {
        lv_label_set_text(sc_data->label, fs_get_last(app_data->path));
        lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
    }


    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}


/**
 * Called when a file list element is released to show the list of operation on it
 * @param file pointer to a file button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_file_action(lv_obj_t * file, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(file);
    my_app_data_t * app_data = app->app_data;
    my_win_data_t * win_data = app->win_data;

    sprintf(app_data->fn, "%s", lv_list_get_element_text(file));

    win_create_list(app);

    /*Create the list of operations*/
    lv_obj_t * liste;
    liste = lv_list_add(win_data->file_list, SYMBOL_LEFT, "Back", win_back_action);
    lv_obj_set_free_p(liste, app);

    /*Send button*/
    liste = lv_list_add(win_data->file_list, NULL, "Send", win_send_rel_action);
    lv_obj_set_free_p(liste, app);

    /*Delete button*/
    liste = lv_list_add(win_data->file_list, NULL, "Delete", win_del_rel_action);
    lv_btn_set_lpr_action(liste, win_del_lpr_action);
    lv_obj_set_free_p(liste, app);

    return LV_ACTION_RES_INV;
}

/**
 * Called when the Back list element is released to when a file chosen to
 * go back to the file list from file operation
 * @param back pointer to the back button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t win_back_action(lv_obj_t * up, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(up);
    my_app_data_t * app_data = app->app_data;

    app_data->file_cnt = 0;
    win_load_file_list(app);
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Send list element is released to send the file
 * @param sed pointer to the Up button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_OK because the list is NOT deleted in the function
 */
static lv_action_res_t win_send_rel_action(lv_obj_t * send, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(send);
    my_app_data_t * app_data = app->app_data;

    if(app_data->send_in_prog != 0) {
        lv_app_notice_add("File sending\nin progress");
        return LV_ACTION_RES_OK;
    }

    char path_fn[LV_APP_FILES_PATH_MAX_LEN + LV_APP_FILES_FN_MAX_LEN];
    sprintf(path_fn, "%s/%s", app_data->path, app_data->fn);
    start_send(app, path_fn);

    return LV_ACTION_RES_OK;
}

/**
 * Called when a send settings element is released
 * @param element pointer to a chekbox or text area
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_OK because the list is NOT deleted in the function
 */
static lv_action_res_t win_send_settings_element_rel_action(lv_obj_t * element, lv_indev_proc_t * indev_proc)
{
    send_settings_id_t id = lv_obj_get_free_num(element);
    lv_app_inst_t * app = lv_obj_get_free_p(element);
    my_app_data_t * app_data = app->app_data;

    if(id == SEND_SETTINGS_FN) {
        app_data->send_fn = lv_btn_get_state(element) == LV_BTN_STATE_REL ? 0 : 1;
    } else if(id == SEND_SETTINGS_SIZE) {
        app_data->send_size = lv_btn_get_state(element) == LV_BTN_STATE_REL ? 0 : 1;
    } else if(id == SEND_SETTINGS_CRC) {
        app_data->send_crc = lv_btn_get_state(element) == LV_BTN_STATE_REL ? 0 : 1;

        /*TODO CRC sending is not supported yet*/
        if(app_data->send_crc != 0) {
            lv_app_notice_add("CRC sending is\nnot supported yet");
        }
    } else if(id == SEND_SETTINGS_CHUNK_SIZE) {
        lv_app_kb_open(element, LV_APP_KB_MODE_NUM | LV_APP_KB_MODE_WIN_RESIZE, send_settings_kb_close_action, send_settings_kb_ok_action);
    } else if(id == SEND_SETTINGS_CHUNK_DELAY) {
        lv_app_kb_open(element, LV_APP_KB_MODE_NUM | LV_APP_KB_MODE_WIN_RESIZE, send_settings_kb_close_action, send_settings_kb_ok_action);
    }

    return LV_ACTION_RES_OK;
}


/**
 * Called when the Delete list element is released.
 * It will show a notification to long press the Delete button to remove the file
 * @param del pointer to the back button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_OK because the list is NOT deleted in the function
 */
static lv_action_res_t win_del_rel_action(lv_obj_t * del, lv_indev_proc_t * indev_proc)
{
    lv_app_notice_add("Press long the Delete button\n"
                      "to remove the file");

    return LV_ACTION_RES_OK;
}
/**
 * Called when the Delete list element is long pressed to remove a file
 * @param del pointer to the Delete button
 * @param indev_proc pointer to the caller display input
 * @return LV_ACTION_RES_OK because the list is NOT deleted in the function
 */
static lv_action_res_t win_del_lpr_action(lv_obj_t * del, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(del);
    my_app_data_t * app_data = app->app_data;

    char path_fn[LV_APP_FILES_PATH_MAX_LEN + LV_APP_FILES_FN_MAX_LEN];
    sprintf(path_fn, "%s/%s", app_data->path, app_data->fn);

    fs_res_t res = fs_remove(path_fn);
    if(res == FS_RES_OK) lv_app_notice_add("%s deleted", app_data->fn);
    else lv_app_notice_add("Can not delete\n%s", app_data->fn);

    return LV_ACTION_RES_OK;
}

/**
 * Called when a send setting is typed and 'Close' pressed on the App. keyboard.
 * The function reverts the original value in the text area.
 * @param ta pointer to a text area
 */
static void send_settings_kb_close_action(lv_obj_t * ta)
{
    send_settings_id_t id = lv_obj_get_free_num(ta);
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * app_data = app->app_data;

    char buf[32];
    buf[0] = '\0';

    if(id == SEND_SETTINGS_CHUNK_DELAY) {
        sprintf(buf, "%d", app_data->chunk_size);
    } else if(id == SEND_SETTINGS_CHUNK_SIZE) {
        sprintf(buf, "%d", app_data->chunk_size);
    }
    lv_ta_set_text(ta, buf);
}

/**
 * Called when a send setting is typed and 'Ok' pressed on the App. keyboard.
 * The function saves teh new value.
 * @param ta pointer to a text area
 */
static void send_settings_kb_ok_action(lv_obj_t * ta)
{
    send_settings_id_t id = lv_obj_get_free_num(ta);
    lv_app_inst_t * app = lv_obj_get_free_p(ta);
    my_app_data_t * app_data = app->app_data;

    int num;
    sscanf(lv_ta_get_txt(ta), "%d", &num);

    if(id == SEND_SETTINGS_CHUNK_DELAY) {
        if(num > LV_APP_FILES_CHUNK_MAX_TIME) num = LV_APP_FILES_CHUNK_MAX_TIME;
        if(num < LV_APP_FILES_CHUNK_MIN_TIME) num = LV_APP_FILES_CHUNK_MIN_TIME;
        app_data->chunk_delay = (uint16_t) num;
    } else if(id == SEND_SETTINGS_CHUNK_SIZE) {
        if(num > LV_APP_FILES_CHUNK_MAX_SIZE) num = LV_APP_FILES_CHUNK_MAX_SIZE;
        if(num < LV_APP_FILES_CHUNK_MIN_SIZE) num = LV_APP_FILES_CHUNK_MIN_SIZE;
        app_data->chunk_size= (uint16_t) num;
    }

}

/**
 * Start the sending of a file
 * @param app pointer to a Files application
 * @param path path of the file to send
 */
static void start_send(lv_app_inst_t * app, const char * path)
{
    my_app_data_t * app_data = app->app_data;

    /*Open the file*/
    fs_res_t res = fs_open(&app_data->file, path, FS_MODE_RD);
    if(res == FS_RES_OK) {
        app_data->send_in_prog = 1;

        /*Send the header*/
        if(app_data->send_fn != 0) {
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, app_data->path, strlen(app_data->path));
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "/", 1);
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, app_data->fn, strlen(app_data->fn));
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "\n", 1);
        }

        if(app_data->send_size != 0) {
            char buf[64];
            uint32_t size;
            fs_size(&app_data->file, &size);
            sprintf(buf,"%d", (int) size);
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, buf, strlen(buf));
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "\n", 1);
        }
        if(app_data->send_crc != 0) {
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "0x0000", 6);
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "\n", 1);
        }

        /*Add an extra \n to separate the header from the file data*/
        if(app_data->send_fn != 0 || app_data->send_size != 0 || app_data->send_crc != 0) {
            lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, "\n", 1);
        } 
    }

    /*If an error occurred  close the file*/
    if(res != FS_RES_OK) {
        fs_close(&app_data->file);
        ptask_set_prio(app_data->send_task, PTASK_PRIO_OFF);
        app_data->send_in_prog = 0;
        lv_app_notice_add("Can not send\nthe file in Files");
    }
    /*If no error show notification, start the sender task and refresh the shortcut*/
    else {
       /*Start the sender task*/
       ptask_set_period(app_data->send_task, app_data->chunk_delay);
       ptask_reset(app_data->send_task);
       ptask_set_prio(app_data->send_task, PTASK_PRIO_HIGH);
       lv_app_notice_add("Sending\n%s", fs_get_last(path));

       /*Refresh the shortcut with the percentage of the sending*/
       if(app->sc_data != NULL) {
           my_sc_data_t * sc_data = app->sc_data;

           uint32_t size;
           fs_size(&app_data->file, &size);
           uint32_t pos;
           fs_tell(&app_data->file, &pos);

           int pct = (uint32_t) (pos * 100) / size;

           char buf[256];
           sprintf(buf, "Sending\n%d%%", pct);
           lv_label_set_text(sc_data->label, buf);
           lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
       }
   }
}

/**
 * Periodically send the next chunk of the file
 * @param app pointer to a Files application
 */
static void send_task(void * param)
{
    lv_app_inst_t * app = param;
    my_app_data_t * app_data = app->app_data;

    if(app_data->send_in_prog == 0) return;

    /*Read a chunk*/
    uint32_t rn;
    char rd_buf[LV_APP_FILES_CHUNK_MAX_SIZE];
    fs_res_t res = fs_read(&app_data->file, rd_buf, app_data->chunk_size, &rn);
    if(res == FS_RES_OK) {
       app_data->send_in_prog = 1;
       lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, rd_buf, rn);
    }

    /*If the read failed close the file and show an error*/
    if(res != FS_RES_OK) {
       fs_close(&app_data->file);
       app_data->send_in_prog = 0;
       lv_app_notice_add("Can not send\nthe file in Files");
    }
    /*If the read was successful*/
    else {
        my_sc_data_t * sc_data = app->sc_data;

        /*If the file is read close it a show a notification*/
        if(rn < app_data->chunk_size) {
            lv_app_notice_add("File sent");
            fs_close(&app_data->file);
            app_data->send_in_prog = 0;

            /*Refresh the shortut*/
            if(sc_data != NULL) {
                lv_label_set_text(sc_data->label, fs_get_last(app_data->path));
                lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
            }
        }
        /*If the file is not sent yet refresh the shortcut with percentage of sending*/
        else {
            if(sc_data != NULL) {
                uint32_t size;
                fs_size(&app_data->file, &size);
                uint32_t pos;
                fs_tell(&app_data->file, &pos);

                uint8_t pct = (uint32_t) (pos * 100) / size;

                char buf[256];
                sprintf(buf, "Sending\n%d%%", pct);
                lv_label_set_text(sc_data->label, buf);
                lv_obj_align(sc_data->label, NULL, LV_ALIGN_CENTER, 0, 0);
            }
        }
    }
}

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_FILES != 0*/
