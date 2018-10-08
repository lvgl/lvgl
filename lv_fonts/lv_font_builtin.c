/**
 * @file lv_font_built_in.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_font_builtin.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the built-in fonts
 */
void lv_font_builtin_init(void)
{
    /*DEJAVU 10*/
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10, NULL);
#endif

#if USE_LV_FONT_DEJAVU_10_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_latin_sup, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_10_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_cyrillic, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 10*/
#if USE_LV_FONT_SYMBOL_10 != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_symbol_10, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_symbol_10, NULL);
#endif
#endif


    /*DEJAVU 20*/
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20, NULL);
#endif

#if USE_LV_FONT_DEJAVU_20_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_latin_sup, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_20_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_cyrillic, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_dejavu_20_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 20*/
#if USE_LV_FONT_SYMBOL_20 != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_symbol_20, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20, NULL);
#endif
#endif


    /*DEJAVU 30*/
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30, NULL);
#endif

#if USE_LV_FONT_DEJAVU_30_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_latin_sup, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_30_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_cyrillic, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 30*/
#if USE_LV_FONT_SYMBOL_30 != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_symbol_30, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_symbol_30_basic, NULL);
#endif
#endif

    /*DEJAVU 40*/
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40, NULL);
#endif

#if USE_LV_FONT_DEJAVU_40_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_latin_sup, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_40_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_cyrillic, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 40*/
#if USE_LV_FONT_SYMBOL_40 != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_symbol_40, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_symbol_40, NULL);
#endif
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
