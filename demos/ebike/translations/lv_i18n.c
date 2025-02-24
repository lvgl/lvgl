#include "./lv_i18n.h"
#include "../../../lvgl.h"

#if LV_USE_DEMO_EBIKE

/**
 * Define plural operands
 * http://unicode.org/reports/tr35/tr35-numbers.html#Operands
 */

/* Integer version, simplified */

static inline uint32_t op_n(int32_t val)
{
    return (uint32_t)(val < 0 ? -val : val);
}

static inline uint32_t op_i(uint32_t val)
{
    return val;
}

/* always zero, when decimal part not exists. */
static inline uint32_t op_v(uint32_t val)
{
    LV_UNUSED(val);
    return 0;
}

static inline uint32_t op_w(uint32_t val)
{
    LV_UNUSED(val);
    return 0;
}
static inline uint32_t op_f(uint32_t val)
{
    LV_UNUSED(val);
    return 0;
}

static inline uint32_t op_t(uint32_t val)
{
    LV_UNUSED(val);
    return 0;
}

static uint8_t en_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    LV_UNUSED(n);
    uint32_t i = op_i(n);
    LV_UNUSED(i);
    uint32_t v = op_v(n);
    LV_UNUSED(v);

    if(i == 1 && v == 0) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_lang = {
    .locale_name = "en",


    .locale_plural_fn = en_plural_fn
};

static lv_i18n_phrase_t ar_singulars[] = {
    {"Language", "لغة"},
    {"Title", "عنوان"},
    {"March 29", "29 مارس"},
    {"Battery", "البطارية"},
    {"Distance today", "المسافة اليوم"},
    {"Speed today", "السرعة اليوم"},
    {"Time today", "الوقت اليوم"},
    {"SETTINGS", "الإعدادات"},
    {"Bluetooth", "بلوتوث"},
    {"Lights", "أضواء"},
    {"Brightness", "السطوع"},
    {"Volume", "الحجم"},
    {"Max. speed", "الأعلى. السرعة"},
    {"Light level", "مستوى الضوء"},
    {"STATS", "إحصائيات"},
    {"March %d - March %d", "مارس %d - مارس %d"},
    {"Avg. speed", "متوسط ​​السرعة"},
    {"Distance", "المسافة"},
    {"Top speed", "السرعة القصوى"},
    {"March %d", "مارس %d"},
    {NULL, NULL} /* End mark */
};

static uint8_t ar_plural_fn(int32_t num)
{
    uint32_t n = op_n(num);
    LV_UNUSED(n);

    uint32_t n100 = n % 100;
    if(n == 0) return LV_I18N_PLURAL_TYPE_ZERO;
    if(n == 1) return LV_I18N_PLURAL_TYPE_ONE;
    if(n == 2) return LV_I18N_PLURAL_TYPE_TWO;
    if(3 <= n100 && n100 <= 10) return LV_I18N_PLURAL_TYPE_FEW;
    if(11 <= n100 && n100 <= 99) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ar_lang = {
    .locale_name = "ar",
    .singulars = ar_singulars,

    .locale_plural_fn = ar_plural_fn
};

static lv_i18n_phrase_t zh_singulars[] = {
    {"Language", "语言"},
    {"Title", "标题"},
    {"March 29", "3月29日"},
    {"Battery", "电池"},
    {"Distance today", "今日距离"},
    {"Speed today", "今天的速度"},
    {"Time today", "今天时间"},
    {"SETTINGS", "设置"},
    {"Bluetooth", "蓝牙"},
    {"Lights", "灯"},
    {"Brightness", "亮度"},
    {"Volume", "音量"},
    {"Max. speed", "最大速度"},
    {"Light level", "光照强度"},
    {"STATS", "统计"},
    {"March %d - March %d", "三月 %d - 三月 %d"},
    {"Avg. speed", "平均速度"},
    {"Distance", "距离"},
    {"Top speed", "最高时速"},
    {"March %d", "三月 %d"},
    {NULL, NULL} /* End mark */
};

static uint8_t zh_plural_fn(int32_t num)
{
    LV_UNUSED(num);

    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t zh_lang = {
    .locale_name = "zh",
    .singulars = zh_singulars,

    .locale_plural_fn = zh_plural_fn
};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_lang,
    &ar_lang,
    &zh_lang,
    NULL /* End mark */
};

/* Internal state */
static const lv_i18n_language_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        /* Found -> finish */
        if(lv_strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


static const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(lv_strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;

    /* Search in current locale */
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if(txt != NULL) return txt;
    }

    /* Try to fallback */
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    /* Repeat search for default locale */
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if(txt != NULL) return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;
    lv_i18n_plural_type_t ptype;

    /* Search in current locale */
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if(txt != NULL) return txt;
        }
    }

    /* Try to fallback */
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    /* Repeat search for default locale */
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if(txt != NULL) return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}

#endif /*#if LV_USE_DEMO_EBIKE*/
