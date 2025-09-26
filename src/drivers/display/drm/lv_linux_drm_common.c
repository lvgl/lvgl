
#include "lv_linux_drm.h"

#if LV_USE_LINUX_DRM

#include <xf86drm.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * lv_linux_drm_find_device_path(void)
{
    const int max_cards = 8;
    char path[64];
    int found_index = 0;
    for(int ci = 0; ci < max_cards; ++ci) {
        snprintf(path, sizeof(path), "/sys/class/drm");
        DIR * d = opendir(path);
        if(!d) continue;
        struct dirent * ent;
        int found_connected = 0;
        while((ent = readdir(d)) != NULL) {
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            // connector dirs look like card0-HDMI-A-1, card0-eDP-1, etc.
            if(strncmp(ent->d_name, "card", 4) != 0 || strchr(ent->d_name, '-') == NULL) continue;
            found_connected = 1;
            found_index = ent->d_name[4] - '0';
            break;
        }
        closedir(d);
        if(found_connected) {
            char * out = malloc(strlen("/dev/dri/card") + 4);
            if(!out) return NULL;
            snprintf(out, strlen("/dev/dri/card") + 4, "/dev/dri/card%d", found_index);
            return out;
        }
    }
    // fallback: prefer card0 if exists, else first cardN
    for(int ci = 0; ci < max_cards; ++ci) {
        snprintf(path, sizeof(path), "/dev/dri/card%d", ci);
        if(access(path, R_OK) == 0) {
            char * out = malloc(strlen(path) + 1);
            if(!out) return NULL;
            strcpy(out, path);
            return out;
        }
    }
    return NULL;
}

int lv_linux_drm_open_by_module_name(void)
{
    static const char * drm_modules[] = {
        "vc4",
        "i915",
        "imx-drm",
        "nouveau",
        "radeon",
        "vmgfx",
        "omapdrm",
        "exynos",
        "pl111",
        "msm",
        "meson",
        "rockchip",
        "sun4i-drm",
        "stm",
    };

    int fd = -1;
    size_t num_modules = sizeof(drm_modules) / sizeof(drm_modules[0]);

    for(size_t m = 0; m < num_modules; ++m) {
        fd = drmOpen(drm_modules[m], NULL);
        if(fd < 0) {
            LV_LOG_INFO("Failed to open DRM module '%s'", drm_modules[m]);
            continue;
        }
        LV_LOG_INFO("Opened DRM module '%s'", drm_modules[m]);
        break;
    }

    return fd;
}

#endif /*LV_USE_LINUX_DRM*/
