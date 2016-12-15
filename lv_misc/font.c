/**
 * @file font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "font.h"
#include "fonts/dejavu_10.h"
#include "fonts/dejavu_14.h"
#include "fonts/dejavu_20.h"
#include "fonts/dejavu_30.h"
#include "fonts/dejavu_40.h"
#include "fonts/dejavu_60.h"
#include "fonts/dejavu_80.h"

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
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the font from its id 
 * @param font_id: the id of a font (an element of font_types_t enum)
 * @return pointer to a font descriptor
 */
const font_t * font_get(font_types_t font_id)
{
    const font_t * font_p = NULL;
    
    switch(font_id)
    {
#if USE_FONT_DEJAVU_8 != 0
        case FONT_DEJAVU_8:
            font_p = dejavu_8_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_10 != 0
        case FONT_DEJAVU_10:
            font_p = dejavu_10_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_14 != 0
        case FONT_DEJAVU_14:
            font_p = dejavu_14_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_20 != 0
        case FONT_DEJAVU_20:
            font_p = dejavu_20_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_30 != 0
        case FONT_DEJAVU_30:
            font_p = dejavu_30_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_40 != 0
        case FONT_DEJAVU_40:
            font_p = dejavu_40_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_60 != 0
        case FONT_DEJAVU_60:
            font_p = dejavu_60_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_80 != 0
        case FONT_DEJAVU_80:
            font_p = dejavu_80_get_dsc();
            break;
#endif
        default:
            font_p = NULL;
    }
    
    return font_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
         
         
