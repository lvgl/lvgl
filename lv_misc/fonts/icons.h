#ifndef ICONS_H
#define ICONS_H

/*Use ISO8859-1 encoding in the IDE*/

#include "lv_conf.h"
#if  USE_FONT_ICONS != 0


#include <stdint.h>
#include "../font.h"

#define ICON_DRIVE	"a"
#define ICON_FILE	"b"
#define ICON_FOLDER	"c"
#define ICON_DELETE	"d"
#define ICON_SAVE	"e"
#define ICON_EDIT	"f"
#define ICON_OK		"g"
#define ICON_CLOSE	"h"
#define ICON_DOWN	"i"
#define ICON_LEFT	"j"
#define ICON_RIGHT	"k"
#define ICON_UP		"l"
#define ICON_BT		"m"
#define ICON_THERM	"n"
#define ICON_GPS	"o"
#define ICON_WARN	"p"
#define ICON_INFO	"q"
#define ICON_BATT1	"r"
#define ICON_BATT2	"s"
#define ICON_BATT3	"t"
#define ICON_BATT4	"u"
#define ICON_BATTCH	"v"
#define ICON_HELP	"w"
#define ICON_POWER	"x"
#define ICON_SETUP	"y"
#define ICON_WIFI	"z"

const font_t * icons_get_dsc(void);

#endif

#endif
