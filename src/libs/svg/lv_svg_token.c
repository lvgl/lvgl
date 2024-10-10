/**
 * @file lv_svg_token.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_svg_token.h"
#if LV_USE_SVG

#include "../../../lvgl.h"
#include <ctype.h>
#include <string.h>

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/*
 *  tag mask     quote mask   tag   search  comment  doc type  xml inst
 * |   0 0 0   |    0 0     |  0  |   0   |    0   |    0    |    0    |
 */
enum {
    SVG_TAG_MASK   = (1 << 3) - 1,
    SVG_QUOTE_MASK = (1 << 5) - (1 << 3),
    SVG_TAG        = 1 << 5,
    SVG_SEARCH     = 1 << 6,
    SVG_COMMENT    = 1 << 7,
    SVG_DOCTYPE    = 1 << 8,
    SVG_XMLINST    = 1 << 9,
};
typedef uint32_t _lv_svg_parser_bits_t;

enum {
    SVG_NO_QUOTE = 0,
    SVG_SINGLE_QUOTE = 1,
    SVG_DOUBLE_QUOTE = 2,
};
typedef uint32_t _lv_svg_parser_quote_t;

enum {
    SVG_NO_TAG       = 0,
    SVG_TAG_NAME     = 1,
    SVG_ATTR_START   = 2,
    SVG_ATTR_NAME    = 3,
    SVG_SEARCH_EQUAL = 4,
    SVG_SEARCH_VALUE = 5,
    SVG_QUOTE_VALUE  = 6,
    SVG_VALUE        = 7,
};
typedef uint32_t _lv_svg_parser_tag_state_t;

typedef struct {
    uint32_t flags;
    const char * cur;
    const char * end;
} _lv_svg_parser_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void _set_state(_lv_svg_parser_state_t * state, uint32_t bit)
{
    state->flags |= bit;
}

static void _clear_state(_lv_svg_parser_state_t * state, uint32_t bit)
{
    state->flags &= ~bit;
}

static bool _is_state(_lv_svg_parser_state_t * state, uint32_t bit)
{
    return state->flags & bit;
}

static void _set_tag_state(_lv_svg_parser_state_t * state, uint32_t bit)
{
    state->flags = (state->flags & ~SVG_TAG_MASK) | bit;
}

static void _set_quote_state(_lv_svg_parser_state_t * state, uint32_t bit)
{
    state->flags = (state->flags & ~SVG_QUOTE_MASK) | (bit << 3);
}

static bool _special_handle(_lv_svg_parser_state_t * state)
{
    return state->flags & (SVG_TAG | SVG_SEARCH | SVG_TAG_MASK | SVG_COMMENT | SVG_DOCTYPE | SVG_XMLINST);
}

static void _lv_svg_token_init(_lv_svg_token_t * token)
{
    token->start = NULL;
    token->end = NULL;
    token->type = LV_SVG_TOKEN_CONTENT;
    token->flat = false;
    token->cur_attr = NULL;
    lv_array_init(&token->attrs, LV_ARRAY_DEFAULT_CAPACITY, sizeof(_lv_svg_token_attr_t));
}

static void _lv_svg_token_reset(_lv_svg_token_t * token)
{
    token->start = NULL;
    token->end = NULL;
    token->type = LV_SVG_TOKEN_CONTENT;
    token->flat = false;
    token->cur_attr = NULL;
    lv_array_clear(&token->attrs);
}

static bool _lv_svg_token_process(_lv_svg_token_t * token, svg_token_process cb, void * data)
{
    if(!token->start || SVG_TOKEN_LEN(token) == 0)
        return true;

    bool ret = cb(token, data);
    _lv_svg_token_reset(token);
    return ret;
}

static _lv_svg_token_attr_t * _new_svg_attr(_lv_svg_token_t * token)
{
    if((lv_array_size(&token->attrs) + 1) > lv_array_capacity(&token->attrs)) {
        lv_array_resize(&token->attrs, token->attrs.capacity << 1);
    }

    token->attrs.size++;
    _lv_svg_token_attr_t * attr = lv_array_at(&token->attrs, token->attrs.size - 1);
    lv_memset(attr, 0, sizeof(_lv_svg_token_attr_t));
    return attr;
}

static void _svg_parser_xml_inst(_lv_svg_parser_state_t * state, _lv_svg_token_t * token)
{
    LV_UNUSED(token);

    while(state->cur <= state->end) {
        char ch = *(state->cur);
        if(ch == '>' && (*(state->cur - 1)) == '?') {
            _clear_state(state, SVG_XMLINST);
            state->cur++;
            break;
        }
        state->cur++;
    }
}

static void _svg_parser_comment(_lv_svg_parser_state_t * state, _lv_svg_token_t * token)
{
    LV_UNUSED(token);

    while(state->cur <= state->end) {
        char ch = *(state->cur);
        if(ch == '>' && (*(state->cur - 1)) == '-' && (*(state->cur - 2)) == '-') {
            _clear_state(state, SVG_COMMENT);
            state->cur++;
            break;
        }
        state->cur++;
    }
}

static void _svg_parser_doctype(_lv_svg_parser_state_t * state, _lv_svg_token_t * token)
{
    LV_UNUSED(token);

    //TODO: processing DTD type
    while(state->cur <= state->end) {
        char ch = *(state->cur);
        if(ch == '>') {
            _clear_state(state, SVG_DOCTYPE);
            state->cur++;
            break;
        }
        state->cur++;
    }
}

static bool _svg_parser_tag(_lv_svg_parser_state_t * state, _lv_svg_token_t * token, svg_token_process cb, void * data)
{
    while(state->cur <= state->end) {
        switch(state->flags & SVG_TAG_MASK) {
            case SVG_NO_TAG: {
                    if(!_lv_svg_token_process(token, cb, data)) {
                        return false;
                    }
                    state->cur++;
                }
                return true;
            case SVG_TAG_NAME: {
                    char ch = *(state->cur);
                    if(ch == '/') {
                        token->type = LV_SVG_TOKEN_END;
                        state->cur++;
                        if(!token->start) {
                            token->start = state->cur;
                        }
                        continue;
                    }
                    else if(ch == '>' || isspace(ch)) {
                        token->end = state->cur;
                        _set_tag_state(state, SVG_ATTR_START);
                        continue;
                    }
                    else {
                        if(!token->start) {
                            token->type = LV_SVG_TOKEN_BEGIN;
                            token->start = state->cur;
                        }
                        state->cur++;
                        continue;
                    }
                }
                break;
            case SVG_ATTR_START: {
                    char ch = *(state->cur);
                    if(!isspace(ch) && ch != '\'' && ch != '\"') {
                        if(ch == '/') {
                            token->flat = true;
                            state->cur++;
                            continue;
                        }
                        if(ch == '>') {
                            _set_tag_state(state, SVG_NO_TAG);
                        }
                        else {
                            token->cur_attr = NULL;
                            _set_tag_state(state, SVG_ATTR_NAME);
                        }
                        continue;
                    }
                }
                break;
            case SVG_ATTR_NAME: {
                    if(!token->cur_attr) {
                        token->cur_attr = _new_svg_attr(token);
                    }
                    char ch = *(state->cur);
                    if(isspace(ch) || ch == '=' || ch == '/' || ch == '>') {
                        token->cur_attr->name_end = state->cur;
                        _set_tag_state(state, SVG_SEARCH_EQUAL);
                        continue;
                    }
                    else {
                        if(!token->cur_attr->name_start) {
                            token->cur_attr->name_start = state->cur;
                        }
                        state->cur++;
                        continue;
                    }
                }
                break;
            case SVG_SEARCH_EQUAL: {
                    char ch = *(state->cur);
                    if(!isspace(ch) && ch != '/' && ch != '\'' && ch != '\"') {
                        if(ch == '=') {
                            _set_tag_state(state, SVG_SEARCH_VALUE);
                        }
                        else {
                            // attr name has empty value
                            token->cur_attr = NULL;
                            _set_tag_state(state, SVG_ATTR_START);
                            continue;
                        }
                    }
                }
                break;
            case SVG_SEARCH_VALUE: {
                    char ch = *(state->cur);
                    if(!isspace(ch)) {
                        if(ch == '\'' || ch == '\"') {
                            if(ch == '\'') {
                                _set_quote_state(state, SVG_SINGLE_QUOTE);
                            }
                            else {
                                _set_quote_state(state, SVG_DOUBLE_QUOTE);
                            }
                            _set_tag_state(state, SVG_QUOTE_VALUE);
                        }
                        else {
                            _set_tag_state(state, SVG_VALUE);
                            continue;
                        }
                    }
                }
                break;
            case SVG_QUOTE_VALUE: {
                    char ch = *(state->cur);
                    if((ch == '\'' && ((state->flags & SVG_QUOTE_MASK) >> 3) == SVG_SINGLE_QUOTE)
                       || (ch == '\"' && ((state->flags & SVG_QUOTE_MASK) >> 3) == SVG_DOUBLE_QUOTE)) {
                        if(!token->cur_attr->value_start) {
                            token->cur_attr->value_start = state->cur;
                        }
                        token->cur_attr->value_end = state->cur;
                        _set_quote_state(state, SVG_NO_QUOTE);
                        _set_tag_state(state, SVG_ATTR_START);
                        continue;
                    }
                    else {
                        if(!token->cur_attr->value_start) {
                            token->cur_attr->value_start = state->cur;
                        }
                        state->cur++;
                        continue;
                    }
                }
                break;
            case SVG_VALUE: {
                    char ch = *(state->cur);
                    if(isspace(ch) || ch == '>' || ch == '/') {
                        if(!token->cur_attr->value_start) {
                            token->cur_attr->value_start = state->cur;
                        }
                        token->cur_attr->value_end = state->cur;
                        _set_quote_state(state, SVG_NO_QUOTE);
                        _set_tag_state(state, SVG_ATTR_START);
                        continue;
                    }
                    else {
                        if(!token->cur_attr->value_start) {
                            token->cur_attr->value_start = state->cur;
                        }
                        state->cur++;
                        continue;
                    }
                }
                break;
        }
        state->cur++;
    }
    return true;
}

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool _lv_svg_tokenizer(const char * svg_data, uint32_t data_len, svg_token_process cb, void * data)
{
    LV_ASSERT_NULL(svg_data);
    LV_ASSERT(data_len > 0);
    LV_ASSERT_NULL(cb);
    LV_ASSERT_NULL(data);

    _lv_svg_token_t token;
    _lv_svg_token_init(&token);
    _lv_svg_parser_state_t state = {
        .flags = 0,
        .cur = svg_data,
        .end = svg_data + data_len,
    };

    while(state.cur <= state.end) {
        char ch = *(state.cur);
        if(ch == '\r' || ch == '\n') { // skip LR character
            state.cur++;
            continue;
        }
        else if(_special_handle(&state)) {
            if(_is_state(&state, SVG_TAG)) {
                _clear_state(&state, SVG_TAG);
                switch(ch) {
                    case '/': // end tag
                        _set_tag_state(&state, SVG_TAG_NAME);
                        break;
                    case '!': {
                            // <!-- comment or <!DOCTYPE>
                            _set_state(&state, SVG_SEARCH); // get more character
                            state.cur++;
                        }
                        break;
                    case '?': {
                            // xml instruction
                            _set_state(&state, SVG_XMLINST);
                            state.cur++;
                        }
                        break;
                    default: {
                            if(isalpha(ch)) {
                                _set_tag_state(&state, SVG_TAG_NAME);
                            }
                            else {
                                LV_LOG_ERROR("svg document parser error!");
                                lv_array_deinit(&token.attrs);
                                return false;
                            }
                        }
                }
                // process token
                if(!_lv_svg_token_process(&token, cb, data)) {
                    LV_LOG_ERROR("svg document parser error!");
                    lv_array_deinit(&token.attrs);
                    return false;
                }
            }
            else if(_is_state(&state, SVG_SEARCH)) {
                if(ch == '-' || isalpha(ch)) {
                    if(!token.start) {
                        token.start = state.cur;
                    }
                    token.end = state.cur;
                }
                else {
                    // processing as a normal tag name.
                    _clear_state(&state, SVG_SEARCH);
                    _set_tag_state(&state, SVG_TAG_NAME);
                    continue;
                }

                if(((token.end - token.start) == 1) && (token.start[0] == '-') && (token.start[1] == '-')) {
                    // is <!-- comment start
                    _clear_state(&state, SVG_SEARCH);
                    token.start = token.end = NULL;
                    _set_state(&state, SVG_COMMENT);
                }
                else if(((token.end - token.start) == 6) && (strncmp(token.start, "DOCTYPE", 7) == 0)) {
                    _clear_state(&state, SVG_SEARCH);
                    token.start = token.end = NULL;
                    _set_state(&state, SVG_DOCTYPE);
                }
                state.cur++;
            }
            else if(_is_state(&state, SVG_COMMENT)) {
                _svg_parser_comment(&state, &token);
            }
            else if(_is_state(&state, SVG_DOCTYPE)) {
                _svg_parser_doctype(&state, &token);
            }
            else if(_is_state(&state, SVG_TAG_MASK)) {
                if(!_svg_parser_tag(&state, &token, cb, data)) {
                    LV_LOG_ERROR("svg document parser error!");
                    lv_array_deinit(&token.attrs);
                    return false;
                }
            }
            else if(_is_state(&state, SVG_XMLINST)) {
                _svg_parser_xml_inst(&state, &token);
            }
        }
        else {
            switch(ch) {
                case '<': {
                        _set_state(&state, SVG_TAG); // start a new tag
                        state.cur++;
                    }
                    break;
                default: {
                        if(!token.start) {
                            token.start = state.cur;
                        }
                        if(state.cur == state.end) {
                            goto finish;
                        }
                        token.end = ++state.cur;
                    }
            }
        }
    }

finish:
    lv_array_deinit(&token.attrs);
    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_SVG*/
