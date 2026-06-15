#warning Include public headers from the `src` folder is deprecated. This file will be removed soon. To ensure your application keeps working, you can choose to include the new header in <include/lvgl/config/lv_conf_internal.h> or include the main header file <include/lvgl/lvgl.h>
#include "../include/lvgl/lvgl.h"
#ifndef LV_USE_CHECK_OBJ_CLASSTYPE
    #ifdef CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
        #define LV_USE_CHECK_OBJ_CLASSTYPE CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
    #else
        #define LV_USE_CHECK_OBJ_CLASSTYPE 1
    #endif
#endif

#ifndef LV_USE_CHECK_OBJ_VALIDITY
    #ifdef CONFIG_LV_USE_CHECK_OBJ_VALIDITY
        #define LV_USE_CHECK_OBJ_VALIDITY CONFIG_LV_USE_CHECK_OBJ_VALIDITY
    #else
        #define LV_USE_CHECK_OBJ_VALIDITY 1
    #endif
#endif

