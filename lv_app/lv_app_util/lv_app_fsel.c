/**
 * @file lv_app_fsel.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_fsel.h"
#if LV_APP_ENABLE != 0

#include <stdio.h>
#include "lv_app_notice.h"

/*********************
 *      DEFINES
 *********************/
#define LV_APP_FSEL_FN_MAX_LEN    128
#define LV_APP_FSEL_PATH_MAX_LEN  256
#define LV_APP_FSEL_MAX_FILE      8

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fsel_refr(void);
static lv_action_res_t fsel_close_action(lv_obj_t * close, lv_dispi_t * dispi);
static lv_action_res_t fsel_up_action(lv_obj_t * up, lv_dispi_t * dispi);
static lv_action_res_t fsel_next_action(lv_obj_t * next, lv_dispi_t * dispi);
static lv_action_res_t fsel_prev_action(lv_obj_t * prev, lv_dispi_t * dispi);
static lv_action_res_t fsel_drv_action(lv_obj_t * drv, lv_dispi_t * dispi);
static lv_action_res_t fsel_drv_lpr_action(lv_obj_t * drv, lv_dispi_t * dispi);
static lv_action_res_t fsel_folder_action(lv_obj_t * folder, lv_dispi_t * dispi);
static lv_action_res_t fsel_folder_lpr_action(lv_obj_t * folder, lv_dispi_t * dispi);
static lv_action_res_t fsel_file_action(lv_obj_t * file, lv_dispi_t * dispi);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * fsel_filter;
static char fsel_path[LV_APP_FSEL_PATH_MAX_LEN];
static uint16_t fsel_file_cnt;
static lv_obj_t * fsel_win;
static lv_obj_t * fsel_list;
static lv_lists_t fsel_lists;
static void *  fsel_param;
static void (*fsel_ok_action)(void *, const char *);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the File selector utility
 */
void lv_app_fsel_init(void)
{
    lv_app_style_t * app_style = lv_app_style_get();

    lv_lists_get(LV_LISTS_DEF, &fsel_lists);

    memcpy(&fsel_lists.liste_labels, &app_style->menu_btn_label_style, sizeof(lv_labels_t));
    fsel_lists.bg_pages.bg_rects.empty = 1;
    fsel_lists.bg_pages.bg_rects.bwidth = 0;

    fsel_lists.liste_imgs.objs.color = COLOR_WHITE;
    fsel_lists.liste_imgs.recolor_opa = OPA_70;
}

/**
 * Open the File selector
 * @param path start path
 * @param filter show only files with a specific extension, e.g. "wav".
 *               "/" means filter to folders.
 * @param param a free parameter which will be added to 'ok_action'
 * @param ok_action an action to call when a file or folder is chosen
 */
void lv_app_fsel_open(const char * path, const char * filter, void * param, void (*ok_action)(void *, const char *))
{
    /*Save the paramteres*/
    strcpy(fsel_path, path);
    fsel_filter = filter;
    fsel_file_cnt = 0;
    fsel_param = param;
    fsel_ok_action = ok_action;

    /*Create a window for the File selector*/
    lv_app_style_t * app_style = lv_app_style_get();
    fsel_win = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(fsel_win, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style(fsel_win, &app_style->win_style);

    lv_win_add_ctrl_btn(fsel_win, "U:/icon_close", fsel_close_action);

    fsel_refr(); /*Refresh the list*/

    /*Show instruction when first open with folder filter*/
    static bool first_folder_call = false;
    if(first_folder_call == false && fsel_filter[0] == '/') {
        lv_app_notice_add("Press a folder long\nto choose it!");
        first_folder_call = true;
    }
}

/**
 * Close the File selector
 */
void lv_app_fsel_close(void)
{
    if(fsel_win != NULL) {
            lv_obj_del(fsel_win);
    }

    fsel_win = NULL;
    fsel_list = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Refresh the file list according to the current path and filter
 */
static void fsel_refr(void)
{
    /*Delete the previous list*/
    if(fsel_list != NULL) {
        lv_obj_del(fsel_list);
    }

    lv_win_set_title(fsel_win, fsel_path);

    /*Create a new list*/
    lv_wins_t * wins = lv_obj_get_style(fsel_win);
    fsel_list = lv_list_create(fsel_win, NULL);
    lv_obj_set_size(fsel_list, LV_HOR_RES -  2 * (wins->content.bg_rects.hpad + wins->content.scrable_rects.hpad), LV_VER_RES -
                    wins->content.bg_rects.vpad - 2 * wins->content.scrable_rects.vpad);
    lv_obj_align(fsel_list, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style(fsel_list, &fsel_lists);

    fs_res_t res = FS_RES_OK;

    /*At empty path show the drivers */
    if(fsel_path[0] == '\0') {
        char drv[16];
        char buf[2];
        lv_obj_t * liste;
        fs_get_letters(drv);
        uint8_t i;
        for(i = 0; drv[i] != '\0'; i++) {
            buf[0] = drv[i];
            buf[1] = '\0';
            liste = lv_list_add(fsel_list, "U:/icon_driver", buf, fsel_drv_action);
            /*Add long press action to choose the driver as a folder*/
            if(fsel_filter[0] == '/') lv_btn_set_lpr_action(liste, fsel_drv_lpr_action);
        }
        fsel_file_cnt = 0;

    }
    /*List the files/folders with fs iterface*/
    else {
        lv_list_add(fsel_list, "U:/icon_up", "Up", fsel_up_action);

        fs_readdir_t rd;
        res = fs_readdir_init(&rd, fsel_path);
        if(res != FS_RES_OK) {
            lv_app_notice_add("Can not read the path\nin File selector");
            return;
        }

        /*At not first page add prev. page button */
        if(fsel_file_cnt != 0) {
            lv_list_add(fsel_list, "U:/icon_left", "Previous page", fsel_prev_action);
        }

        char fn[LV_APP_FSEL_FN_MAX_LEN];

        /*Read the files from the previous pages*/
        uint16_t file_cnt = 0;
        while(file_cnt <= fsel_file_cnt) {
            res = fs_readdir(&rd, fn);
            if(res != FS_RES_OK || fn[0] == '\0'){
                lv_app_notice_add("Can not read the path\nin File selector");
                return;
            }
            file_cnt ++;
        }

        /*Add list elements from the files and folders*/
        while(res == FS_RES_OK && fn[0] != '\0') {
            if(fn[0] == '/') { /*Add a folder*/
                lv_obj_t * liste;
                liste = lv_list_add(fsel_list, "U:/icon_folder", fn, fsel_folder_action);
                /*Add long press action to choose a folder*/
                if(fsel_filter[0] == '/') lv_btn_set_lpr_action(liste, fsel_folder_lpr_action);

                fsel_file_cnt ++;
                file_cnt ++;
            }else if(strcmp(fs_get_ext(fn), fsel_filter) == 0 &&
                     fsel_filter[0] != '/') { /*Add a file if it is not filtered*/
                lv_list_add(fsel_list, "U:/icon_file", fn, fsel_file_action);
                fsel_file_cnt ++;
                file_cnt ++;
            }

            /*Get the next element*/
            res = fs_readdir(&rd, fn);

            /*Show only LV_APP_FSEL_MAX_FILE elements and add a Next page button*/
            if(fsel_file_cnt != 0 && fsel_file_cnt % LV_APP_FSEL_MAX_FILE == 0) {
                lv_list_add(fsel_list, "U:/icon_right", "Next page", fsel_next_action);
                break;
            }
        }

        /*Close the read directory*/
        fs_readdir_close(&rd);
    }

    if(res != FS_RES_OK) {
        lv_app_notice_add("Can not read the path\nin File selector");
    }
    return;
}

/**
 * Called when the File selector window close button is released
 * @param close pointer to the close button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the window is deleted in the function
 */
static lv_action_res_t fsel_close_action(lv_obj_t * close, lv_dispi_t * dispi)
{
    lv_app_fsel_close();
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Up list element is released to step one level
 * @param up pointer to the Up button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_up_action(lv_obj_t * up, lv_dispi_t * dispi)
{
    fs_up(fsel_path);

    fsel_refr();
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Next list element is released to go to the next page
 * @param next pointer to the Next button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_next_action(lv_obj_t * next, lv_dispi_t * dispi)
{
    fsel_refr();
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Prev list element is released to previous page
 * @param prev pointer to the Prev button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_prev_action(lv_obj_t * prev, lv_dispi_t * dispi)
{
    if(fsel_file_cnt <= 2 * LV_APP_FSEL_MAX_FILE) fsel_file_cnt = 0;
    else if(fsel_file_cnt % LV_APP_FSEL_MAX_FILE == 0) {
        fsel_file_cnt -= 2 * LV_APP_FSEL_MAX_FILE;
    } else {
        fsel_file_cnt = ((fsel_file_cnt / LV_APP_FSEL_MAX_FILE) - 1) * LV_APP_FSEL_MAX_FILE;
    }

    fsel_refr();
    return LV_ACTION_RES_INV;
}


/**
 * Called when the Driver list element is released to step into a driver
 * @param drv pointer to the Driver button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_drv_action(lv_obj_t * drv, lv_dispi_t * dispi)
{
    sprintf(fsel_path, "%s:", lv_list_element_get_txt(drv));

    fsel_refr();
    return LV_ACTION_RES_INV;
}

/**
 * Called when the Driver list element is long pressed to choose it
 * @param drv pointer to the Driver button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_drv_lpr_action(lv_obj_t * drv, lv_dispi_t * dispi)
{
    sprintf(fsel_path, "%s:", lv_list_element_get_txt(drv));

    if(fsel_ok_action != NULL) {
        fsel_ok_action(fsel_param, fsel_path);
    }

    lv_app_fsel_close();

    return LV_ACTION_RES_INV;
}

/**
 * Called when a folder list element is released to enter into it
 * @param folder pointer to a folder button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_folder_action(lv_obj_t * folder, lv_dispi_t * dispi)
{
    sprintf(fsel_path, "%s/%s", fsel_path, lv_list_element_get_txt(folder));

    fsel_refr();
    return LV_ACTION_RES_INV;
}

/**
 * Called when a folder list element is long pressed to choose it
 * @param folder pointer to a folder button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_folder_lpr_action(lv_obj_t * folder, lv_dispi_t * dispi)
{
    sprintf(fsel_path, "%s/%s", fsel_path, lv_list_element_get_txt(folder));

    if(fsel_ok_action != NULL) {
        fsel_ok_action(fsel_param, fsel_path);
    }

    lv_app_fsel_close();

    return LV_ACTION_RES_INV;
}

/**
 * Called when a file list element is long pressed to choose it
 * @param file pointer to a file button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is deleted in the function
 */
static lv_action_res_t fsel_file_action(lv_obj_t * file, lv_dispi_t * dispi)
{
    sprintf(fsel_path, "%s/%s", fsel_path, lv_list_element_get_txt(file));

    if(fsel_ok_action != NULL) {
        fsel_ok_action(fsel_param, fsel_path);
    }

    lv_app_fsel_close();

    return LV_ACTION_RES_INV;
}

#endif /*LV_APP_ENABLE != 0*/
