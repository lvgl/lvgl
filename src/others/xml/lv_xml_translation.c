/**
 * @file lv_xml_translation.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#if LV_USE_XML

#include "lv_xml_widget.h"
#include "lv_xml_parser.h"
#include "../../libs/expat/expat.h"
#include "../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * tag;
    const char ** translations; /**< Translations for each language*/
} translation_dsc_t;

typedef struct {
    const char ** languages;
    uint32_t language_cnt;
    uint32_t is_pointer; /*In the union translations_p is used*/
    const char ** tag_p;
    const char ** translation_p; /*E.g. {{"a", "b"}, {"c", "d"}}*/
    lv_array_t translation_array;
} language_pack_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void start_handler(void * user_data, const char * name, const char ** attrs);
static void end_handler(void * user_data, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t packs_ll;
static const char * selected_lang = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_translation_init(void)
{
    lv_ll_init(&packs_ll, sizeof(language_pack_t));
}


lv_result_t lv_xml_translation_register_from_array(const char * languages[], const char * tags[],
                                                   const char * translations[])
{
    language_pack_t * pack = lv_ll_ins_head(&packs_ll);
    lv_memzero(pack, sizeof(language_pack_t));
    pack->is_pointer = 1;

    /*Count the languages*/
    while(languages[pack->language_cnt]) {
        pack->language_cnt++;
    }

    pack->languages = languages;
    pack->tag_p = tags;
    pack->translation_p = translations;
    return LV_RESULT_OK;
}

lv_result_t lv_xml_translation_register_from_file(const char * path)
{
    lv_fs_res_t fs_res;
    lv_fs_file_t f;
    fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Couldn't open %s", path);
        return LV_RESULT_INVALID;
    }

    /* Determine file size */
    lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    uint32_t file_size = 0;
    lv_fs_tell(&f, &file_size);
    lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

    /* Create the buffer */
    char * xml_buf = lv_malloc(file_size + 1);
    if(xml_buf == NULL) {
        LV_LOG_WARN("Memory allocation failed for file %s (%d bytes)", path, file_size + 1);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Read the file content  */
    uint32_t rn;
    lv_fs_read(&f, xml_buf, file_size, &rn);
    if(rn != file_size) {
        LV_LOG_WARN("Couldn't read %s fully", path);
        lv_free(xml_buf);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Null-terminate the buffer */
    xml_buf[rn] = '\0';

    /* Register the component */
    lv_result_t res = lv_xml_translation_register_from_data(xml_buf);

    /* Housekeeping */
    lv_free(xml_buf);
    lv_fs_close(&f);

    return res;
}

lv_result_t lv_xml_translation_register_from_data(const char * xml_def)
{
    language_pack_t * pack = lv_ll_ins_head(&packs_ll);
    lv_memzero(pack, sizeof(language_pack_t));
    pack->is_pointer = 0;
    lv_array_init(&pack->translation_array, 16, sizeof(translation_dsc_t));

    /* Parse the XML to extract metadata */
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, pack);
    XML_SetElementHandler(parser, start_handler, end_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }
    XML_ParserFree(parser);
    return LV_RESULT_OK;
}

void lv_xml_set_language(const char * lang)
{
    if(selected_lang) lv_free((void *)selected_lang);
    selected_lang = lv_strdup(lang);
}

const char * lv_xml_get_translation(const char * tag)
{
    language_pack_t * pack;
    bool lang_found = false;
    LV_LL_READ(&packs_ll, pack) {
        uint32_t lang;
        for(lang = 0; lang < pack->language_cnt; lang++) {
            /*Does this pack contains the language?*/
            if(lv_streq(pack->languages[lang], selected_lang)) {
                lang_found = true;
                /*Find the tag*/
                if(pack->is_pointer) {
                    uint32_t t;
                    for(t = 0; pack->tag_p[t]; t++) {
                        if(lv_streq(pack->tag_p[t], tag)) {
                            /*Find the "row" of the tag */
                            const char ** tr_row = pack->translation_p + pack->language_cnt * t;
                            const char * tr = tr_row[lang];
                            if(tr) return tr; /*Found directly*/

                            tr = tr_row[0];
                            if(tr) {
                                /*Use the first translation as fall back*/
                                LV_LOG_WARN("`%s` tag is not found for `%s` language. Using `%s` instead.",
                                            tag, selected_lang, pack->languages[0]);
                                return tr;
                            }

                            LV_LOG_WARN("`%s` tag is not found. Using the tag as translation.", tag);
                            return tag; /*Return the tag as a worst case option*/
                        }
                    }
                }
                else {
                    size_t trans_cnt = lv_array_size(&pack->translation_array);
                    size_t i;
                    for(i = 0; i < trans_cnt; i++) {
                        translation_dsc_t * tr_dsc = lv_array_at(&pack->translation_array, i);
                        if(lv_streq(tr_dsc->tag, tag)) {
                            const char * tr = tr_dsc->translations[lang];
                            if(tr) return tr; /*Found directly*/

                            tr = tr_dsc->translations[0];
                            if(tr) {
                                /*Use the first translation as fall back*/
                                LV_LOG_WARN("`%s` tag is not found for `%s` language. Using `%s` instead.",
                                            tag, selected_lang, pack->languages[0]);
                                return tr;
                            }

                            LV_LOG_WARN("`%s` tag is not found. Using the tag as translation.", tag);
                            return tag; /*Return the tag as a worst case option*/
                        }
                    }
                }
            }
        }
    }

    if(lang_found) {
        LV_LOG_WARN("`%s` tag is not found, using the tag as translation.", tag);
    }
    else {
        LV_LOG_WARN("`%s` language is not found, using the `%s` as translation.", selected_lang, tag);
    }

    return tag;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void start_handler(void * user_data, const char * name, const char ** attrs)
{
    language_pack_t * pack = user_data;

    if(lv_streq(name, "translations")) {
        const char * languages = lv_xml_get_value_of(attrs, "languages");
        char buf[256];
        lv_strlcpy(buf, languages, sizeof(buf));
        char * bufp = buf;

        while(bufp[0]) {
            lv_xml_split_str(&bufp, ' ');
            pack->language_cnt++;
        }

        pack->languages = lv_malloc(pack->language_cnt * sizeof(const char *));

        lv_strlcpy(buf, languages, sizeof(buf));
        bufp = buf;
        uint32_t i = 0;
        while(bufp[0]) {
            const char * lang = lv_xml_split_str(&bufp, ' ');
            pack->languages[i] = lv_strdup(lang);
            i++;
        }
    }
    else if(lv_streq(name, "translation")) {
        if(pack->language_cnt == 0 || pack->languages == NULL) {
            LV_LOG_WARN("`No languages were found, <translations languages=\"...\" was not set>`");
            return;
        }
        const char * tag_name = lv_xml_get_value_of(attrs, "tag");
        if(tag_name == NULL) {
            LV_LOG_WARN("`tag` is missing from the translation");
            return;
        }

        translation_dsc_t tag;
        tag.tag = lv_strdup(tag_name);
        tag.translations = lv_malloc(pack->language_cnt * sizeof(const char *));
        lv_array_push_back(&pack->translation_array, &tag);

        uint32_t i;
        for(i = 0; i < pack->language_cnt; i++) {
            const char * trans = lv_xml_get_value_of(attrs, pack->languages[i]);
            if(trans == NULL) {
                tag.translations[i] = NULL;
                LV_LOG_WARN("`%s` language is missing from tag `%s`", pack->languages[i], tag_name);
                continue;
            }
            tag.translations[i] = lv_strdup(trans);
        }
    }
}

static void end_handler(void * user_data, const char * name)
{
    LV_UNUSED(user_data);
    LV_UNUSED(name);
}

#endif /* LV_USE_XML */
