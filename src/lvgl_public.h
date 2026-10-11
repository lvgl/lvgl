#ifndef LVGL_PUBLIC_H
#define LVGL_PUBLIC_H

/* By default we want to include the full public API here
 * but we need to guard any .S file that might include this else
 * it will fail to compile*/
#ifndef __ASSEMBLY__
    #include "../include/lvgl/lvgl.h"
#else
    #include "../include/lvgl/config/lv_conf_internal.h"
#endif /*__ASSEMBLY__*/


#endif /*LVGL_PUBLIC_H*/
