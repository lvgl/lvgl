#ifndef SYMBOL_30_H
#define SYMBOL_30_H

/*Use ISO8859-1 encoding in the IDE*/

#include "lv_conf.h"
#if  USE_FONT_SYMBOL_30 != 0


#include <stdint.h>
#include "../font.h"

#define SYMBOL_30_DRIVE		"a"
#define SYMBOL_30_FILE		"b"
#define SYMBOL_30_FOLDER	"c"
#define SYMBOL_30_DELETE	"d"
#define SYMBOL_30_SAVE		"e"
#define SYMBOL_30_EDIT		"f"
#define SYMBOL_30_OK		"g"
#define SYMBOL_30_CLOSE		"h"
#define SYMBOL_30_DOWN		"i"
#define SYMBOL_30_LEFT		"j"
#define SYMBOL_30_RIGHT		"k"
#define SYMBOL_30_UP		"l"
#define SYMBOL_30_BT		"m"
#define SYMBOL_30_THERM		"n"
#define SYMBOL_30_GPS		"o"
#define SYMBOL_30_WARN		"p"
#define SYMBOL_30_INFO		"q"
#define SYMBOL_30_BATT1		"r"
#define SYMBOL_30_BATT2		"s"
#define SYMBOL_30_BATT3		"t"
#define SYMBOL_30_BATT4		"u"
#define SYMBOL_30_BATTCH	"v"
#define SYMBOL_30_HELP		"w"
#define SYMBOL_30_POWER		"x"
#define SYMBOL_30_SETUP		"y"
#define SYMBOL_30_WIFI		"z"

const font_t * symbol_30_get_dsc(void);

#endif

#endif
