/**
 * @file lv_linux_drm_common.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_linux_drm.h"

#if LV_USE_LINUX_DRM

#include <dirent.h>
#include <xf86drmMode.h>
#include <unistd.h>
#include <fcntl.h>

#include "../../../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/

#define LV_DRM_CLASS_DIR "/sys/class/drm"
#define LV_DRM_CARD_DIR "/dev/dri"
#define LV_DRM_CARD_PATH "/dev/dri/card"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static char * find_by_class(void);
static char * find_by_drm_dev(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char * lv_linux_drm_find_device_path(void)
{
    char * card_path;
    if((card_path = find_by_class()))
        return card_path;
    return find_by_drm_dev();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static char * find_by_drm_dev(void)
{
    DIR * d = opendir(LV_DRM_CARD_DIR);
    if(!d) {
        return NULL;
    }
    struct dirent * ent;
    while((ent = readdir(d)) != NULL) {
        if(!lv_strcmp(ent->d_name, ".")
           || !lv_strcmp(ent->d_name, "..")
           || lv_strncmp(ent->d_name, "card", 4))
            continue;

        const size_t buf_size = lv_strlen(LV_DRM_CARD_DIR "/") + lv_strlen(ent->d_name) + 1;
        char * card_path = lv_zalloc(buf_size);
        if(!card_path)
            continue;
        lv_snprintf(card_path, buf_size, LV_DRM_CARD_DIR "/%s", ent->d_name);
        int fd = open(card_path, O_RDWR);

        if(fd < 0)
            goto open_fail;

        if(fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)
            goto get_res_fail;

        drmModeRes * res = drmModeGetResources(fd);
        if(!res)
            goto get_res_fail;

        for(int i = 0; i < res->count_connectors; i++) {
            drmModeConnector * conn = drmModeGetConnector(fd, res->connectors[i]);
            if(!conn)
                continue;
            if(conn->connection == DRM_MODE_CONNECTED) {
                drmModeFreeConnector(conn);
                drmModeFreeResources(res);
                close(fd);
                closedir(d);
                return card_path;
            }
            drmModeFreeConnector(conn);
        }
        drmModeFreeResources(res);
get_res_fail:
        close(fd);
open_fail:
        lv_free(card_path);

    }

    closedir(d);
    return NULL;
}

static char * find_by_class(void)
{
    DIR * d = opendir(LV_DRM_CLASS_DIR);
    if(!d) {
        return NULL;
    }

    struct dirent * ent;
    while((ent = readdir(d)) != NULL) {
        if(lv_strcmp(ent->d_name, ".") == 0 || lv_strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        /* connector dirs look like card0-HDMI-A-1, card0-eDP-1, etc. */
        bool is_card = lv_strncmp(ent->d_name, "card", 4) == 0;
        bool is_connected = lv_strchr(ent->d_name, '-') != NULL;

        if(!is_card || !is_connected) {
            continue;
        }

        const size_t buf_size = lv_strlen(LV_DRM_CARD_PATH) + 3;
        char * card_path = lv_zalloc(buf_size);
        if(!card_path)
            continue;
        if(ent->d_name[5] != '-') {
            /* Double digit card*/
            lv_snprintf(card_path, buf_size, LV_DRM_CARD_PATH "%c%c", ent->d_name[4], ent->d_name[5]);
        }
        else {
            lv_snprintf(card_path, buf_size, LV_DRM_CARD_PATH "%c", ent->d_name[4]);
        }
        closedir(d);
        return card_path;
    }

    closedir(d);
    return NULL;

}

int32_t lv_linux_drm_mode_get_horizontal_resolution(const lv_linux_drm_mode_t * mode)
{
    if(!mode) {
        return 0;
    }
    return mode->hdisplay;
}

int32_t lv_linux_drm_mode_get_vertical_resolution(const lv_linux_drm_mode_t * mode)
{
    if(!mode) {
        return 0;
    }
    return mode->vdisplay;
}

int32_t lv_linux_drm_mode_get_refresh_rate(const lv_linux_drm_mode_t * mode)
{
    if(!mode) {
        return 0;
    }
    return mode->vrefresh;
}

bool lv_linux_drm_mode_is_preferred(const lv_linux_drm_mode_t * mode)
{
    if(!mode) {
        return false;
    }
    return (mode->type & DRM_MODE_TYPE_PREFERRED) != 0;
}

#endif /*LV_USE_LINUX_DRM*/
