/**
 * @file lv_svg_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_svg_parser.h"
#if LV_USE_SVG

#include "../../../lvgl.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*********************
*      DEFINES
*********************/
#ifndef M_PI
    #define M_PI 3.1415926f
#endif

#define MAP_LEN(m) sizeof((m)) / sizeof((m[0]))

#define CHECK_AND_RESIZE_ATTRS(a) \
    do { \
        if((lv_array_size(&(a)) + 1) > lv_array_capacity(&(a))) { \
            lv_array_resize(&(a), (a).capacity << 1); \
        } \
    } while(0)

/**********************
*      TYPEDEFS
**********************/
static const struct _lv_svg_tag_map {
    const char * name;
    uint32_t name_len;
    lv_svg_tag_t tag;
} _svg_tag_map[] = {
    {"svg", 3, LV_SVG_TAG_SVG},
    {"use", 3, LV_SVG_TAG_USE},
    {"g", 1, LV_SVG_TAG_G},
    {"path", 4, LV_SVG_TAG_PATH},
    {"rect", 4, LV_SVG_TAG_RECT},
    {"circle", 6, LV_SVG_TAG_CIRCLE},
    {"ellipse", 7, LV_SVG_TAG_ELLIPSE},
    {"line", 4, LV_SVG_TAG_LINE},
    {"polyline", 8, LV_SVG_TAG_POLYLINE},
    {"polygon", 7, LV_SVG_TAG_POLYGON},
    {"solidColor", 10, LV_SVG_TAG_SOLID_COLOR},
    {"linearGradient", 14, LV_SVG_TAG_LINEAR_GRADIENT},
    {"radialGradient", 14, LV_SVG_TAG_RADIAL_GRADIENT},
    {"stop", 4, LV_SVG_TAG_STOP},
    {"defs", 4, LV_SVG_TAG_DEFS},
    {"image", 5, LV_SVG_TAG_IMAGE},
#if LV_USE_SVG_ANIMATION
    {"mpath", 5, LV_SVG_TAG_MPATH},
    {"set", 3, LV_SVG_TAG_SET},
    {"animate", 7, LV_SVG_TAG_ANIMATE},
    {"animateColor", 12, LV_SVG_TAG_ANIMATE_COLOR},
    {"animateTransform", 16, LV_SVG_TAG_ANIMATE_TRANSFORM},
    {"animateMotion", 13, LV_SVG_TAG_ANIMATE_MOTION},
#endif
    {"text", 4, LV_SVG_TAG_TEXT},
    {"tspan", 5, LV_SVG_TAG_TSPAN},
    {"textArea", 8, LV_SVG_TAG_TEXT_AREA},
};

static const struct _lv_svg_attr_map {
    const char * name;
    uint32_t name_len;
    lv_svg_attr_type_t attr;
} _svg_attr_map[] = {
    {"id", 2, LV_SVG_ATTR_ID},
    {"xml:id", 6, LV_SVG_ATTR_XML_ID},
    {"version", 7, LV_SVG_ATTR_VERSION},
    {"baseProfile", 11, LV_SVG_ATTR_BASE_PROFILE},
    {"viewBox", 7, LV_SVG_ATTR_VIEWBOX},
    {"preserveAspectRatio", 19, LV_SVG_ATTR_PRESERVE_ASPECT_RATIO},
    {"viewport-fill", 13, LV_SVG_ATTR_VIEWPORT_FILL},
    {"viewport-fill-opacity", 21, LV_SVG_ATTR_VIEWPORT_FILL_OPACITY},
    {"display", 7, LV_SVG_ATTR_DISPLAY},
    {"visibility", 10, LV_SVG_ATTR_VISIBILITY},
    {"x", 1, LV_SVG_ATTR_X},
    {"y", 1, LV_SVG_ATTR_Y},
    {"width", 5, LV_SVG_ATTR_WIDTH},
    {"height", 6, LV_SVG_ATTR_HEIGHT},
    {"rx", 2, LV_SVG_ATTR_RX},
    {"ry", 2, LV_SVG_ATTR_RY},
    {"cx", 2, LV_SVG_ATTR_CX},
    {"cy", 2, LV_SVG_ATTR_CY},
    {"r", 1, LV_SVG_ATTR_R},
    {"x1", 2, LV_SVG_ATTR_X1},
    {"y1", 2, LV_SVG_ATTR_Y1},
    {"x2", 2, LV_SVG_ATTR_X2},
    {"y2", 2, LV_SVG_ATTR_Y2},
    {"points", 6, LV_SVG_ATTR_POINTS},
    {"d", 1, LV_SVG_ATTR_D},
    {"pathLength", 10, LV_SVG_ATTR_PATH_LENGTH},
    {"xlink:href", 10, LV_SVG_ATTR_XLINK_HREF},
    {"fill", 4, LV_SVG_ATTR_FILL},
    {"fill-rule", 9, LV_SVG_ATTR_FILL_RULE},
    {"fill-opacity", 12, LV_SVG_ATTR_FILL_OPACITY},
    {"stroke", 6, LV_SVG_ATTR_STROKE},
    {"stroke-width", 12, LV_SVG_ATTR_STROKE_WIDTH},
    {"stroke-linecap", 14, LV_SVG_ATTR_STROKE_LINECAP},
    {"stroke-linejoin", 15, LV_SVG_ATTR_STROKE_LINEJOIN},
    {"stroke-miterlimit", 17, LV_SVG_ATTR_STROKE_MITER_LIMIT},
    {"stroke-dasharray", 16, LV_SVG_ATTR_STROKE_DASH_ARRAY},
    {"stroke-dashoffset", 17, LV_SVG_ATTR_STROKE_DASH_OFFSET},
    {"stroke-opacity", 14, LV_SVG_ATTR_STROKE_OPACITY},
    {"opacity", 7, LV_SVG_ATTR_OPACITY},
    {"solid-color", 11, LV_SVG_ATTR_SOLID_COLOR},
    {"solid-opacity", 13, LV_SVG_ATTR_SOLID_OPACITY},
    {"gradientUnits", 13, LV_SVG_ATTR_GRADIENT_UNITS},
    {"offset", 6, LV_SVG_ATTR_GRADIENT_STOP_OFFSET},
    {"stop-color", 10, LV_SVG_ATTR_GRADIENT_STOP_COLOR},
    {"stop-opacity", 12, LV_SVG_ATTR_GRADIENT_STOP_OPACITY},
    {"font-family", 11, LV_SVG_ATTR_FONT_FAMILY},
    {"font-style", 10, LV_SVG_ATTR_FONT_STYLE},
    {"font-variant", 12, LV_SVG_ATTR_FONT_VARIANT},
    {"font-weight", 11, LV_SVG_ATTR_FONT_WEIGHT},
    {"font-size", 9, LV_SVG_ATTR_FONT_SIZE},
    {"transform", 9, LV_SVG_ATTR_TRANSFORM},
    {"text-anchor", 11, LV_SVG_ATTR_TEXT_ANCHOR},
#if LV_USE_SVG_ANIMATION
    {"attributeName", 13, LV_SVG_ATTR_ATTRIBUTE_NAME},
    {"attributeType", 13, LV_SVG_ATTR_ATTRIBUTE_TYPE},
    {"begin", 5, LV_SVG_ATTR_BEGIN},
    {"end", 3, LV_SVG_ATTR_END},
    {"dur", 3, LV_SVG_ATTR_DUR},
    {"min", 3, LV_SVG_ATTR_MIN},
    {"max", 3, LV_SVG_ATTR_MAX},
    {"restart", 7, LV_SVG_ATTR_RESTART},
    {"repeatCount", 11, LV_SVG_ATTR_REPEAT_COUNT},
    {"repeatDur", 9, LV_SVG_ATTR_REPEAT_DUR},
    {"calcMode", 8, LV_SVG_ATTR_CALC_MODE},
    {"values", 6, LV_SVG_ATTR_VALUES},
    {"keyTimes", 8, LV_SVG_ATTR_KEY_TIMES},
    {"keySplines", 10, LV_SVG_ATTR_KEY_SPLINES},
    {"keyPoints", 9, LV_SVG_ATTR_KEY_POINTS},
    {"from", 4, LV_SVG_ATTR_FROM},
    {"to", 2, LV_SVG_ATTR_TO},
    {"by", 2, LV_SVG_ATTR_BY},
    {"additive", 8, LV_SVG_ATTR_ADDITIVE},
    {"accumulate", 10, LV_SVG_ATTR_ACCUMULATE},
    {"path", 4, LV_SVG_ATTR_PATH},
    {"rotate", 6, LV_SVG_ATTR_ROTATE},
    {"type", 4, LV_SVG_ATTR_TRANSFORM_TYPE},
#endif
};

static const struct _lv_svg_attr_aspect_ratio_map {
    const char * name;
    uint32_t align;
} _svg_attr_aspect_ratio_map[] = {
    {"xMinYMin", LV_SVG_ASPECT_RATIO_XMIN_YMIN},
    {"xMidYMin", LV_SVG_ASPECT_RATIO_XMID_YMIN},
    {"xMaxYMin", LV_SVG_ASPECT_RATIO_XMAX_YMIN},
    {"xMinYMid", LV_SVG_ASPECT_RATIO_XMIN_YMID},
    {"xMidYMid", LV_SVG_ASPECT_RATIO_XMID_YMID},
    {"xMaxYMid", LV_SVG_ASPECT_RATIO_XMAX_YMID},
    {"xMinYMax", LV_SVG_ASPECT_RATIO_XMIN_YMAX},
    {"xMidYMax", LV_SVG_ASPECT_RATIO_XMID_YMAX},
    {"xMaxYMax", LV_SVG_ASPECT_RATIO_XMAX_YMAX},
};

static const struct _lv_svg_color_map {
    const char * name;
    uint32_t name_len;
    uint32_t color;
} _svg_color_map[] = {
    {"aliceblue", 9, 0xf0f8ff},
    {"antiquewhite", 12, 0xfaebd7},
    {"aqua", 4, 0x00ffff},
    {"aquamarine", 10, 0x7fffd4},
    {"azure", 5, 0xf0ffff},
    {"beige", 5, 0xf5f5dc},
    {"bisque", 6, 0xffe4c4},
    {"black", 5, 0x000000},
    {"blanchedalmond", 14, 0xffebcd},
    {"blue", 4, 0x0000ff},
    {"blueviolet", 10, 0x8a2be2},
    {"brown", 5, 0xa52a2a},
    {"burlywood", 9, 0xdeb887},
    {"cadetblue", 9, 0x5f9ea0},
    {"chartreuse", 10, 0x7fff00},
    {"chocolate", 9, 0xd2691e},
    {"coral", 5, 0xff7f50},
    {"cornflowerblue", 14, 0x6495ed},
    {"cornsilk", 8, 0xfff8dc},
    {"crimson", 7, 0xdc143c},
    {"cyan", 4, 0x00ffff},
    {"darkblue", 8, 0x00008b},
    {"darkcyan", 8, 0x008b8b},
    {"darkgoldenrod", 13, 0xb8860b},
    {"darkgray", 8, 0xa9a9a9},
    {"darkgrey", 8, 0xa9a9a9},
    {"darkgreen", 9, 0x006400},
    {"darkkhaki", 9, 0xbdb76b},
    {"darkmagenta", 11, 0x8b008b},
    {"darkolivegreen", 14, 0x556b2f},
    {"darkorange", 10, 0xff8c00},
    {"darkorchid", 10, 0x9932cc},
    {"darkred", 7, 0x8b0000},
    {"darksalmon", 10, 0xe9967a},
    {"darkseagreen", 12, 0x8fbc8f},
    {"darkslateblue", 13, 0x483d8b},
    {"darkslategray", 13, 0x2f4f4f},
    {"darkslategrey", 13, 0x2f4f4f},
    {"darkturquoise", 13, 0x00ced1},
    {"darkviolet", 10, 0x9400d3},
    {"deeppink", 8, 0xff1493},
    {"deepskyblue", 11, 0x00bfff},
    {"dimgray", 7, 0x696969},
    {"dimgrey", 7, 0x696969},
    {"dodgerblue", 10, 0x1e90ff},
    {"firebrick", 9, 0xb22222},
    {"floralwhite", 11, 0xfffaf0},
    {"forestgreen", 11, 0x228b22},
    {"fuchsia", 7, 0xff00ff},
    {"gainsboro", 9, 0xdcdcdc},
    {"ghostwhite", 10, 0xf8f8ff},
    {"gold", 4, 0xffd700},
    {"goldenrod", 9, 0xdaa520},
    {"gray", 4, 0x808080},
    {"grey", 4, 0x808080},
    {"green", 5, 0x008000},
    {"greenyellow", 11, 0xadff2f},
    {"honeydew", 8, 0xf0fff0},
    {"hotpink", 7, 0xff69b4},
    {"indianred", 9, 0xcd5c5c},
    {"indigo", 6, 0x4b0082},
    {"ivory", 5, 0xfffff0},
    {"khaki", 5, 0xf0e68c},
    {"lavender", 8, 0xe6e6fa},
    {"lavenderblush", 13, 0xfff0f5},
    {"lawngreen", 9, 0x7cfc00},
    {"lemonchiffon", 12, 0xfffacd},
    {"lightblue", 9, 0xadd8e6},
    {"lightcoral", 10, 0xf08080},
    {"lightcyan", 9, 0xe0ffff},
    {"lightgoldenrodyellow", 20, 0xfafad2},
    {"lightgray", 9, 0xd3d3d3},
    {"lightgrey", 9, 0xd3d3d3},
    {"lightgreen", 10, 0x90ee90},
    {"lightpink", 9, 0xffb6c1},
    {"lightsalmon", 11, 0xffa07a},
    {"lightseagreen", 13, 0x20b2aa},
    {"lightskyblue", 12, 0x87cefa},
    {"lightslategray", 14, 0x778899},
    {"lightslategrey", 14, 0x778899},
    {"lightsteelblue", 14, 0xb0c4de},
    {"lightyellow", 11, 0xffffe0},
    {"lime", 4, 0x00ff00},
    {"limegreen", 9, 0x32cd32},
    {"linen", 5, 0xfaf0e6},
    {"magenta", 7, 0xff00ff},
    {"maroon", 6, 0x800000},
    {"mediumaquamarine", 16, 0x66cdaa},
    {"mediumblue", 10, 0x0000cd},
    {"mediumorchid", 12, 0xba55d3},
    {"mediumpurple", 12, 0x9370d8},
    {"mediumseagreen", 14, 0x3cb371},
    {"mediumslateblue", 15, 0x7b68ee},
    {"mediumspringgreen", 17, 0x00fa9a},
    {"mediumturquoise", 15, 0x48d1cc},
    {"mediumvioletred", 15, 0xc71585},
    {"midnightblue", 12, 0x191970},
    {"mintcream", 9, 0xf5fffa},
    {"mistyrose", 9, 0xffe4e1},
    {"moccasin", 8, 0xffe4b5},
    {"navajowhite", 11, 0xffdead},
    {"navy", 4, 0x000080},
    {"oldlace", 7, 0xfdf5e6},
    {"olive", 5, 0x808000},
    {"olivedrab", 9, 0x6b8e23},
    {"orange", 6, 0xffa500},
    {"orangered", 9, 0xff4500},
    {"orchid", 6, 0xda70d6},
    {"palegoldenrod", 13, 0xeee8aa},
    {"palegreen", 9, 0x98fb98},
    {"paleturquoise", 13, 0xafeeee},
    {"palevioletred", 13, 0xd87093},
    {"papayawhip", 10, 0xffefd5},
    {"peachpuff", 9, 0xffdab9},
    {"peru", 4, 0xcd853f},
    {"pink", 4, 0xffc0cb},
    {"plum", 4, 0xdda0dd},
    {"powderblue", 10, 0xb0e0e6},
    {"purple", 6, 0x800080},
    {"red", 3, 0xff0000},
    {"rosybrown", 9, 0xbc8f8f},
    {"royalblue", 9, 0x4169e1},
    {"saddlebrown", 11, 0x8b4513},
    {"salmon", 6, 0xfa8072},
    {"sandybrown", 10, 0xf4a460},
    {"seagreen", 8, 0x2e8b57},
    {"seashell", 8, 0xfff5ee},
    {"sienna", 6, 0xa0522d},
    {"silver", 6, 0xc0c0c0},
    {"skyblue", 7, 0x87ceeb},
    {"slateblue", 9, 0x6a5acd},
    {"slategray", 9, 0x708090},
    {"slategrey", 9, 0x708090},
    {"snow", 4, 0xfffafa},
    {"springgreen", 11, 0x00ff7f},
    {"steelblue", 9, 0x4682b4},
    {"tan", 3, 0xd2b48c},
    {"teal", 4, 0x008080},
    {"thistle", 7, 0xd8bfd8},
    {"tomato", 6, 0xff6347},
    {"turquoise", 9, 0x40e0d0},
    {"violet", 6, 0xee82ee},
    {"wheat", 5, 0xf5deb3},
    {"white", 5, 0xffffff},
    {"whitesmoke", 10, 0xf5f5f5},
    {"yellow", 6, 0xffff00},
    {"yellowgreen", 11, 0x9acd32},
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_svg_tag_t _get_svg_tag_type(const _lv_svg_token_t * token)
{
    uint32_t len = MAP_LEN(_svg_tag_map);
    uint32_t token_len = SVG_TOKEN_LEN(token);

    for(uint32_t i = 0; i < len; i++) {
        if(token_len == _svg_tag_map[i].name_len && strncmp(_svg_tag_map[i].name, token->start, token_len) == 0) {
            return _svg_tag_map[i].tag;
        }
    }
    return LV_SVG_TAG_INVALID;
}

static bool _process_end_tag(_lv_svg_parser_t * parser, lv_svg_tag_t tag, const _lv_svg_token_t * token)
{
    if(parser->state == LV_SVG_PARSER_IGNORE) {
        uint32_t len = SVG_TOKEN_LEN(token);
        if((parser->ignore_len == len) && strncmp(parser->ignore_name, token->start, len) == 0) {
            parser->state = LV_SVG_PARSER_PROCESS;
            lv_free(parser->ignore_name);
            parser->ignore_name = NULL;
            parser->ignore_len = 0;
        }
        return true;
    }

    if(parser->cur_node->type != tag) {
        LV_LOG_ERROR("svg tag does not match in pairs!");
        return false;
    }

    if(parser->cur_node != parser->doc_root) {
        parser->cur_node = (lv_svg_node_t *)LV_TREE_NODE(parser->cur_node)->parent;
    }
    return true;
}

static lv_svg_attr_type_t _get_svg_attr_type(const char * attr_start, const char * attr_end)
{
    uint32_t len = MAP_LEN(_svg_attr_map);
    uint32_t attr_len = attr_end - attr_start;

    for(uint32_t i = 0; i < len; i++) {
        if(attr_len == _svg_attr_map[i].name_len && strncmp(_svg_attr_map[i].name, attr_start, attr_len) == 0) {
            return _svg_attr_map[i].attr;
        }
    }
    return LV_SVG_ATTR_INVALID;
}

static void _process_string(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start, const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    char * str = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(str);
    lv_memcpy(str, val_start, len);
    str[len] = '\0';
    attr->value.sval = str;
}

static void _process_xlink(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start, const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    if(*val_start == '#') {
        val_start++;
    }

    uint32_t len = val_end - val_start;
    char * str = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(str);
    lv_memcpy(str, val_start, len);
    str[len] = '\0';
    attr->value.sval = str;
}

static bool _is_number_begin(char ch)
{
    return ch != 0 && strchr("0123456789+-.", ch) != NULL;
}

static const char * _skip_space(const char * str, const char * str_end)
{
    while((str < str_end) && isspace(*str)) {
        ++str;
    }
    return str;
}

static bool _is_separators(char c)
{
    return  c == ',' || c == '\t' || c == '\n' || c == '\r';
}

static const char * _skip_space_and_separators(const char * str, const char * str_end)
{
    while((str < str_end) && (isspace(*str) || _is_separators(*str))) {
        ++str;
    }
    return str;
}

static const char * _parse_number(const char * str, const char * str_end, float * val)
{
    if(!str) {
        return NULL;
    }
    // skip loading
    while((str < str_end) && !_is_number_begin(*str)) {
        ++str;
    }

    if(str == str_end) { // parse fail
        return NULL;
    }

    char * end = NULL;
    *val = strtof(str, &end);
    return end;
}

static const char * _parse_length(const char * str, const char * str_end, int32_t dpi, float * val)
{
    str = _parse_number(str, str_end, val);
    if(str) {
        uint32_t len = str_end - str;
        if(len > 0) {
            if(len == 1 && (*str == '%')) {
                // percentage
                *val *= 0.01f;
            }
            else if(len == 2) {
                if(str[0] == 'p' && str[1] == 't') { // pt
                    *val = *val / 72.0f * (float)dpi;
                }
                else if(str[0] == 'p' && str[1] == 'c') {   // pc
                    *val = *val / 6.0f * (float)dpi;
                }
                else if(str[0] == 'i' && str[1] == 'n') {   // in
                    *val = *val * (float)dpi;
                }
                else if(str[0] == 'm' && str[1] == 'm') {   // mm
                    *val = *val / 25.4f * (float)dpi;
                }
                else if(str[0] == 'c' && str[1] == 'm') {   // cm
                    *val = *val / 2.54f * (float)dpi;
                }
                else if(str[0] == 'e' && str[1] == 'm') {   // em
                    *val = *val * 16.0f; // FIXME: browser default font size
                }
                else if(str[0] == 'e' && str[1] == 'x') {   // ex
                    *val = *val * 16.0f * 0.52f;
                }
            }
        }
        str += len;
    }
    return str;
}

static const char * _parse_color(const char * str, const char * str_end, uint32_t * val)
{
    if(!str) {
        return NULL;
    }

    const char * ptr = str;
    while((ptr < str_end) && (*ptr != ')')) { // calc letters end
        ++ptr;
    }

    uint32_t len = ptr - str;
    uint8_t r = 0, g = 0, b = 0;

    if(*str == '#') {
        if(len == 4) { // three digit hex format '#rgb'
            if(isxdigit(str[1]) && isxdigit(str[2]) && isxdigit(str[3])) {
                char st[3] = {0};
                st[0] = st[1] = str[1];
                r = (uint8_t)strtol(st, NULL, 16);
                st[0] = st[1] = str[2];
                g = (uint8_t)strtol(st, NULL, 16);
                st[0] = st[1] = str[3];
                b = (uint8_t)strtol(st, NULL, 16);
            }
        }
        else if(len == 7) {    // six digit hex format '#rrggbb'
            if(isxdigit(str[1]) && isxdigit(str[2]) && isxdigit(str[3])
               && isxdigit(str[4]) && isxdigit(str[5]) && isxdigit(str[6])) {
                char st[3] = {0};
                st[0] = str[1];
                st[1] = str[2];
                r = (uint8_t)strtol(st, NULL, 16);
                st[0] = str[3];
                st[1] = str[4];
                g = (uint8_t)strtol(st, NULL, 16);
                st[0] = str[5];
                st[1] = str[6];
                b = (uint8_t)strtol(st, NULL, 16);
            }
        }
        // make color
        *val = (r << 16) + (g << 8) + b;
    }
    else if(len > 5 && strncmp(str, "rgba(", 5) == 0) {
        str += 5;
        bool valid_color = true;
        float vals[3] = {0};
        uint8_t alpha = 255;

        for(int i = 0; i < 3; i++) {
            str = _parse_number(str, ptr, &vals[i]);
            if(!str) valid_color = false;

            if(*str == '%') {
                vals[i] *= 2.56f;
            }
        }

        float a = 0.0f;
        str = _parse_number(str, ptr, &a);
        if(str) {
            if(*str == '%') {
                a *= 2.56f;
            }
            else if(a >= 0.0f && a <= 1.0f) {
                a *= 255.0f;
            }
            alpha = (uint8_t)a;
        }

        if(valid_color) {
            r = (uint8_t)vals[0];
            g = (uint8_t)vals[1];
            b = (uint8_t)vals[2];
        }
        // make color
        *val = (alpha << 24) + (r << 16) + (g << 8) + b;
    }
    else if(len > 4 && strncmp(str, "rgb(", 4) == 0) {
        str += 4;
        bool valid_color = true;
        float vals[3] = {0};

        for(int i = 0; i < 3; i++) {
            str = _parse_number(str, ptr, &vals[i]);
            if(!str) valid_color = false;

            if(*str == '%') {
                vals[i] *= 2.56f;
            }
        }

        if(valid_color) {
            r = (uint8_t)vals[0];
            g = (uint8_t)vals[1];
            b = (uint8_t)vals[2];
        }
        // make color
        *val = (r << 16) + (g << 8) + b;
    }
    else {   // color keyword
        uint32_t map_len = MAP_LEN(_svg_color_map);
        for(uint32_t i = 0; i < map_len; i++) {
            if(len == _svg_color_map[i].name_len && strncmp(_svg_color_map[i].name, str, len) == 0) {
                *val = _svg_color_map[i].color;
            }
        }
    }
    return ++ptr;
}

static void _multiply_matrix(lv_svg_matrix_t * matrix, const lv_svg_matrix_t * mul)
{
    // TODO: use NEON to optimize this function on ARM architecture.
    lv_svg_matrix_t tmp;

    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            tmp.m[y][x] = (matrix->m[y][0] * mul->m[0][x])
                          + (matrix->m[y][1] * mul->m[1][x])
                          + (matrix->m[y][2] * mul->m[2][x]);
        }
    }

    lv_memcpy(matrix, &tmp, sizeof(lv_svg_matrix_t));
}

static const char * _parse_matrix(const char * str, const char * str_end, lv_svg_transform_type_t type,
                                  lv_svg_matrix_t * matrix)
{
    // skip loading
    while((str < str_end) && *str != '(') {
        ++str;
    }

    if(str == str_end) { // parse fail
        return str;
    }

    const char * ptr = str;
    switch(type) {
        case LV_SVG_TRANSFORM_TYPE_MATRIX: {
                float vals[6] = {0};
                for(int i = 0; i < 6; i++) {
                    ptr = _parse_number(ptr, str_end, &vals[i]);
                    if(!ptr) return str;
                    str = ptr;
                }

                lv_svg_matrix_t mt = {{
                        {vals[0], vals[2], vals[4]},
                        {vals[1], vals[3], vals[5]},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                _multiply_matrix(matrix, &mt);
            }
            break;
        case LV_SVG_TRANSFORM_TYPE_TRANSLATE: {
                float tx = 0.0f, ty = 0.0f;
                ptr = _parse_number(ptr, str_end, &tx);
                if(!ptr) return str;
                str = ptr;

                ptr = _skip_space(ptr, str_end);
                if(*ptr != ')') {
                    ptr = _parse_number(ptr, str_end, &ty);
                    if(ptr) str = ptr;
                }

                lv_svg_matrix_t tlm = {{
                        {1.0f, 0.0f, tx},
                        {0.0f, 1.0f, ty},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                _multiply_matrix(matrix, &tlm);
            }
            break;
        case LV_SVG_TRANSFORM_TYPE_ROTATE: {
                float degree = 0.0f, cx = 0.0f, cy = 0.0f;
                bool trans = false;

                ptr = _parse_number(ptr, str_end, &degree);
                if(!ptr) return str;
                str = ptr;

                ptr = _skip_space(ptr, str_end);
                if(*ptr != ')') {
                    ptr = _parse_number(ptr, str_end, &cx);
                    ptr = _parse_number(ptr, str_end, &cy);
                    if(ptr) {
                        trans = true;
                        str = ptr;
                    }
                }

                float radian = degree / 180.0f * (float)M_PI;
                float cos_r = cosf(radian);
                float sin_r = sinf(radian);

                lv_svg_matrix_t rtm = {{
                        {cos_r, -sin_r, 0.0f},
                        {sin_r, cos_r, 0.0f},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                if(!trans) {
                    _multiply_matrix(matrix, &rtm);
                }
                else {
                    lv_svg_matrix_t tlm = {{
                            {1.0f, 0.0f, cx},
                            {0.0f, 1.0f, cy},
                            {0.0f, 0.0f, 1.0f},
                        }
                    };

                    _multiply_matrix(matrix, &tlm);
                    _multiply_matrix(matrix, &rtm);

                    tlm.m[0][2] = -cx;
                    tlm.m[1][2] = -cy;
                    _multiply_matrix(matrix, &tlm);
                }
            }
            break;
        case LV_SVG_TRANSFORM_TYPE_SCALE: {
                float sx = 0.0f, sy = 0.0f;
                ptr = _parse_number(ptr, str_end, &sx);
                if(!ptr) return str;
                str = ptr;

                sy = sx;

                ptr = _skip_space(ptr, str_end);
                if(*ptr != ')') {
                    ptr = _parse_number(ptr, str_end, &sy);
                    if(ptr) str = ptr;
                }

                lv_svg_matrix_t scm = {{
                        {sx, 0.0f, 0.0f},
                        {0.0f, sy, 0.0f},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                _multiply_matrix(matrix, &scm);
            }
            break;
        case LV_SVG_TRANSFORM_TYPE_SKEW_X: {
                float degree = 0.0f;
                ptr = _parse_number(ptr, str_end, &degree);
                if(!ptr) return str;
                str = ptr;

                float radian = degree / 180.0f * (float)M_PI;
                float tan = tanf(radian);

                lv_svg_matrix_t skm = {{
                        {1.0f, tan, 0.0f},
                        {0.0f, 1.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                _multiply_matrix(matrix, &skm);
            }
            break;
        case LV_SVG_TRANSFORM_TYPE_SKEW_Y: {
                float degree = 0.0f;
                ptr = _parse_number(ptr, str_end, &degree);
                if(!ptr) return str;
                str = ptr;

                float radian = degree / 180.0f * (float)M_PI;
                float tan = tanf(radian);

                lv_svg_matrix_t skm = {{
                        {1.0f, 0.0f, 0.0f},
                        {tan, 1.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f},
                    }
                };

                _multiply_matrix(matrix, &skm);
            }
            break;
    }
    return str;
}

static void _process_view_box(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                              const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    if(len >= 4 && strncmp(val_start, "none", 4) == 0) {
        attr->val_type = LV_SVG_ATTR_VALUE_DATA;
        attr->class_type = LV_SVG_ATTR_VALUE_NONE;
        return;
    }

    float * vals = lv_malloc_zeroed(sizeof(float) * 4);
    LV_ASSERT_MALLOC(vals);
    const char * ptr = val_start;
    for(int i = 0; i < 4; i++) {
        ptr = _parse_number(ptr, val_end, &vals[i]);
        if(!ptr) {
            attr->val_type = LV_SVG_ATTR_VALUE_DATA;
            attr->class_type = LV_SVG_ATTR_VALUE_NONE;
            lv_free(vals);
            return;
        }
    }
    attr->value.val = vals;
}

static void _process_points_value(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                  const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t list_cap = 4;
    lv_svg_attr_values_list_t * list = lv_malloc(sizeof(lv_svg_point_t) * list_cap + sizeof(uint32_t));
    LV_ASSERT_MALLOC(list);

    float val_number = 0.0f;
    const char * ptr = val_start;
    uint32_t point_cnt = 0;

    while(ptr < val_end) {
        if(point_cnt == list_cap) {
            list_cap = list_cap << 1;
            list = (lv_svg_attr_values_list_t *)lv_realloc(list, sizeof(lv_svg_point_t) * list_cap + sizeof(uint32_t));
            LV_ASSERT_MALLOC(list);
        }
        lv_svg_point_t * pt = (lv_svg_point_t *)(&list->data) + point_cnt;
        val_number = 0.0f;
        ptr = _parse_number(ptr, val_end, &val_number);
        pt->x = val_number;
        val_number = 0.0f;
        ptr = _parse_number(ptr, val_end, &val_number);
        pt->y = val_number;
        if(!ptr) break;
        ++point_cnt;
    }

    list->length = point_cnt;
    attr->value.val = list;
}

static int _get_path_point_count(char cmd)
{
    switch(cmd) {
        case 'M':
        case 'm':
        case 'L':
        case 'l':
        case 'H':
        case 'h':
        case 'V':
        case 'v':
        case 'Z':
        case 'z':
            return 1;
        case 'C':
        case 'c':
        case 'S':
        case 's':
            return 3;
        case 'Q':
        case 'q':
        case 'T':
        case 't':
            return 2;
        default:
            return 0;
    }
}

static bool _is_relative_cmd(char cmd)
{
    switch(cmd) {
        case 'm':
        case 'l':
        case 'h':
        case 'v':
        case 'c':
        case 's':
        case 'q':
        case 't':
        case 'z':
            return true;
        case 'M':
        case 'L':
        case 'H':
        case 'V':
        case 'C':
        case 'S':
        case 'Q':
        case 'T':
        case 'Z':
        default:
            return false;
    }
}

static bool _is_path_cmd(char ch)
{
    return ch != 0 && strchr("MLHVCSQTZmlhvcsqtz", ch) != NULL;
}

static void _process_path_value(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t list_cap = 4;
    uint32_t list_size = sizeof(lv_svg_point_t) * list_cap + sizeof(uint32_t) * list_cap + sizeof(uint32_t);
    lv_svg_attr_values_list_t * list = lv_malloc(list_size);
    LV_ASSERT_MALLOC(list);

    uint32_t cmd_cnt = 0;
    uint32_t cur_size = 0;
    char cur_cmd = 0;
    lv_svg_point_t cur_point = {0, 0};
    lv_svg_point_t cur_ctrlPoint = {0, 0};
    lv_svg_point_t first_point = {0, 0};

    const char * ptr = val_start;
    uint8_t * data_ptr = (uint8_t *)(&list->data);

    while(ptr < val_end) {
        ptr = _skip_space_and_separators(ptr, val_end);
        if(ptr == val_end) break;

        char ch = *ptr;
        if(_is_number_begin(ch)) {
            if(cur_cmd != 0) {
                if(cur_cmd == 'M') {
                    ch = 'L';
                }
                else if(cur_cmd == 'm') {
                    ch = 'l';
                }
                else {
                    ch = cur_cmd;
                }
            }
            else {
                break;
            }
        }
        else if(_is_path_cmd(ch)) {
            ++ptr;
        }
        else {
            break;
        }

        int point_count = _get_path_point_count(ch);
        uint32_t mem_inc = sizeof(lv_svg_point_t) * point_count + sizeof(uint32_t);

        if((cur_size + mem_inc) > (list_size - sizeof(uint32_t))) {
            list_cap = list_cap << 1;
            list_size = sizeof(lv_svg_point_t) * list_cap + sizeof(uint32_t) * list_cap + sizeof(uint32_t);
            list = (lv_svg_attr_values_list_t *)lv_realloc(list, list_size);
            LV_ASSERT_MALLOC(list);
        }

        data_ptr = (uint8_t *)(&list->data) + cur_size;
        lv_svg_attr_path_value_t * path_seg = (lv_svg_attr_path_value_t *)data_ptr;

        bool relative = _is_relative_cmd(ch);

        switch(ch) {
            case 'm':
            case 'M': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    float xval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &xval);
                    float yval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &yval);
                    if(relative) {
                        xval += cur_point.x;
                        yval += cur_point.y;
                    }
                    path_seg->cmd = LV_SVG_PATH_CMD_MOVE_TO;
                    point->x = xval;
                    point->y = yval;
                    cur_point.x = xval;
                    cur_point.y = yval;
                    first_point.x = xval;
                    first_point.y = yval;
                }
                break;
            case 'L':
            case 'l': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    float xval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &xval);
                    float yval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &yval);
                    if(relative) {
                        xval += cur_point.x;
                        yval += cur_point.y;
                    }
                    path_seg->cmd = LV_SVG_PATH_CMD_LINE_TO;
                    point->x = xval;
                    point->y = yval;
                    cur_point.x = xval;
                    cur_point.y = yval;
                }
                break;
            case 'H':
            case 'h': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    float xval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &xval);
                    if(relative) {
                        xval += cur_point.x;
                    }
                    path_seg->cmd = LV_SVG_PATH_CMD_LINE_TO;
                    point->x = xval;
                    point->y = cur_point.y;
                    cur_point.x = xval;
                }
                break;
            case 'V':
            case 'v': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    float yval = 0.0f;
                    ptr = _parse_number(ptr, val_end, &yval);
                    if(relative) {
                        yval += cur_point.y;
                    }
                    path_seg->cmd = LV_SVG_PATH_CMD_LINE_TO;
                    point->x = cur_point.x;
                    point->y = yval;
                    cur_point.y = yval;
                }
                break;
            case 'C':
            case 'c': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    for(int i = 0; i < 3; i++) {
                        float xval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &xval);
                        float yval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &yval);
                        if(relative) {
                            xval += cur_point.x;
                            yval += cur_point.y;
                        }
                        path_seg->cmd = LV_SVG_PATH_CMD_CURVE_TO;
                        point[i].x = xval;
                        point[i].y = yval;
                    }

                    cur_ctrlPoint.x = point[1].x;
                    cur_ctrlPoint.y = point[1].y;
                    cur_point.x = point[2].x;
                    cur_point.y = point[2].y;
                }
                break;
            case 'S':
            case 's': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);

                    if(cur_cmd == 'C' || cur_cmd == 'c' || cur_cmd == 'S' || cur_cmd == 's') {
                        point[0].x = cur_point.x * 2 - cur_ctrlPoint.x;
                        point[0].y = cur_point.y * 2 - cur_ctrlPoint.y;
                    }
                    else {
                        point[0].x = cur_point.x;
                        point[0].y = cur_point.y;
                    }

                    for(int i = 1; i < 3; i++) {
                        float xval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &xval);
                        float yval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &yval);
                        if(relative) {
                            xval += cur_point.x;
                            yval += cur_point.y;
                        }
                        path_seg->cmd = LV_SVG_PATH_CMD_CURVE_TO;
                        point[i].x = xval;
                        point[i].y = yval;
                    }

                    cur_ctrlPoint.x = point[1].x;
                    cur_ctrlPoint.y = point[1].y;
                    cur_point.x = point[2].x;
                    cur_point.y = point[2].y;
                }
                break;
            case 'Q':
            case 'q': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    for(int i = 0; i < 2; i++) {
                        float xval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &xval);
                        float yval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &yval);
                        if(relative) {
                            xval += cur_point.x;
                            yval += cur_point.y;
                        }
                        path_seg->cmd = LV_SVG_PATH_CMD_QUAD_TO;
                        point[i].x = xval;
                        point[i].y = yval;
                    }

                    cur_ctrlPoint.x = point[0].x;
                    cur_ctrlPoint.y = point[0].y;
                    cur_point.x = point[1].x;
                    cur_point.y = point[1].y;
                }
                break;
            case 'T':
            case 't': {
                    lv_svg_point_t * point = (lv_svg_point_t *)(&path_seg->data);
                    if(cur_cmd == 'Q' || cur_cmd == 'q' || cur_cmd == 'T' || cur_cmd == 't') {
                        point[0].x = cur_point.x * 2 - cur_ctrlPoint.x;
                        point[0].y = cur_point.y * 2 - cur_ctrlPoint.y;
                    }
                    else {
                        point[0].x = cur_point.x;
                        point[0].y = cur_point.y;
                    }

                    for(int i = 1; i < 2; i++) {
                        float xval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &xval);
                        float yval = 0.0f;
                        ptr = _parse_number(ptr, val_end, &yval);
                        if(relative) {
                            xval += cur_point.x;
                            yval += cur_point.y;
                        }
                        path_seg->cmd = LV_SVG_PATH_CMD_QUAD_TO;
                        point[i].x = xval;
                        point[i].y = yval;
                    }

                    cur_ctrlPoint.x = point[0].x;
                    cur_ctrlPoint.y = point[0].y;
                    cur_point.x = point[1].x;
                    cur_point.y = point[1].y;
                }
                break;
            case 'Z':
            case 'z': {
                    path_seg->cmd = LV_SVG_PATH_CMD_CLOSE;
                    cur_point.x = first_point.x;
                    cur_point.y = first_point.y;
                }
                break;
        }

        if(!ptr) break;
        cur_size += mem_inc;
        cur_cmd = ch;
        ++cmd_cnt;
    }

    list->length = cmd_cnt;
    attr->value.val = list;
}

static void _process_gradient_units(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                    const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    int32_t val = 0;

    if(len == 14 && strncmp(val_start, "userSpaceOnUse", 14) == 0) {
        val = LV_SVG_GRADIENT_UNITS_USER_SPACE;
    }
    else {
        val = LV_SVG_GRADIENT_UNITS_OBJECT;
    }
    attr->value.ival = val;
}

static void _process_paint_dasharray(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                     const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;

    if(len >= 4 && strncmp(val_start, "none", 4) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_NONE;
        return;
    }
    else if(len >= 7 && strncmp(val_start, "inherit", 7) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_INHERIT;
        return;
    }
    else {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;

        uint32_t list_cap = 4;
        lv_svg_attr_values_list_t * list = lv_malloc(sizeof(float) * list_cap + sizeof(uint32_t));
        LV_ASSERT_MALLOC(list);

        uint32_t count = 0;
        const char * ptr = val_start;

        while(ptr < val_end) {
            if(count == list_cap) {
                list_cap = list_cap << 1;
                list = lv_realloc(list, sizeof(float) * list_cap + sizeof(uint32_t));
                LV_ASSERT_MALLOC(list);
            }
            float * val = (float *)(&list->data) + count;
            ptr = _parse_number(ptr, val_end, val);
            if(!ptr) break;
            ++count;
        }

        list->length = count;
        attr->value.val = list;
    }
}

static void _process_font_attrs(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                const char * val_end, int32_t dpi)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;

    if(len >= 7 && strncmp(val_start, "inherit", 7) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_INHERIT;
        return;
    }

    if(type == LV_SVG_ATTR_FONT_SIZE && _is_number_begin(*val_start)) {
        float val_number = 0.0f;
        val_start = _parse_length(val_start, val_end, dpi, &val_number);
        attr->value.fval = val_number;
    }
    else {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;

        char * str = lv_malloc(len + 1);
        LV_ASSERT_MALLOC(str);
        lv_memcpy(str, val_start, len);
        str[len] = '\0';
        attr->value.sval = str;
    }
}

static void _process_paint_attrs(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                 const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;

    if(len >= 7 && strncmp(val_start, "inherit", 7) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_INHERIT;
        return;
    }

    if(type == LV_SVG_ATTR_FILL_RULE) {
        int32_t val = 0;
        if(strncmp(val_start, "evenodd", 7) == 0) {
            val = LV_SVG_FILL_EVENODD;
        }
        else {
            val = LV_SVG_FILL_NONZERO;
        }
        attr->value.ival = val;
    }
    else if(type == LV_SVG_ATTR_STROKE_LINECAP) {
        int32_t val = 0;
        if(strncmp(val_start, "round", 5) == 0) {
            val = LV_SVG_LINE_CAP_ROUND;
        }
        else if(strncmp(val_start, "square", 6) == 0) {
            val = LV_SVG_LINE_CAP_SQUARE;
        }
        else {
            val = LV_SVG_LINE_CAP_BUTT;
        }
        attr->value.ival = val;
    }
    else if(type == LV_SVG_ATTR_STROKE_LINEJOIN) {
        int32_t val = 0;
        if(strncmp(val_start, "round", 5) == 0) {
            val = LV_SVG_LINE_JOIN_ROUND;
        }
        else if(strncmp(val_start, "bevel", 5) == 0) {
            val = LV_SVG_LINE_JOIN_BEVEL;
        }
        else {
            val = LV_SVG_LINE_JOIN_MITER;
        }
        attr->value.ival = val;
    }
    else if(type == LV_SVG_ATTR_STROKE_WIDTH) {
        float val = 1.0f;
        val_start = _parse_number(val_start, val_end, &val);
        if(val < 0.0f) {
            val = 0.0f;
        }
        attr->value.fval = val;
    }
    else if(type == LV_SVG_ATTR_STROKE_MITER_LIMIT) {
        float val = 4.0f;
        val_start = _parse_number(val_start, val_end, &val);
        if(val < 1.0f) {
            val = 1.0f;
        }
        attr->value.ival = (int32_t)val;
    }
    else if(type == LV_SVG_ATTR_STROKE_DASH_OFFSET) {
        float val = 0.0f;
        val_start = _parse_number(val_start, val_end, &val);
        attr->value.fval = val;
    }
    else if(type == LV_SVG_ATTR_GRADIENT_STOP_OFFSET) {
        float val = 0.0f;
        val_start = _parse_number(val_start, val_end, &val);
        attr->value.fval = val;
    }
}

static void _process_paint(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                           const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    if(len >= 4 && strncmp(val_start, "none", 4) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_NONE;
        return;
    }
    else if(len >= 7 && strncmp(val_start, "inherit", 7) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_INHERIT;
        return;
    }
    else if(len > 4 && strncmp(val_start, "url(", 4) == 0) {
        // parse url
        const char * ptr = val_start + 4;
        const char * url_start = NULL;
        const char * url_end = NULL;

        ptr = _skip_space(ptr, val_end);
        if(ptr == val_end) {
            attr->class_type = LV_SVG_ATTR_VALUE_NONE;
            return;
        }

        if(*ptr == '#') {
            url_start = ptr + 1;
        }

        while((ptr < val_end) && !isspace(*ptr) && *ptr != ')') {
            ++ptr;
        }

        url_end = ptr;
        if(url_start && url_end) {
            attr->val_type = LV_SVG_ATTR_VALUE_PTR;
            len = url_end - url_start;
            char * node_id = lv_malloc(len + 1);
            LV_ASSERT_MALLOC(node_id);
            lv_memcpy(node_id, url_start, len);
            node_id[len] = '\0';
            attr->value.sval = node_id;
        }
        return;
    }
    else {
#if LV_USE_SVG_ANIMATION
        if(len == 6) {
            if(strncmp(val_start, "freeze", 6) == 0) {
                attr->value.ival = LV_SVG_ANIM_FREEZE;
                return;
            }
            else if(strncmp(val_start, "remove", 6) == 0) {
                attr->value.ival = LV_SVG_ANIM_REMOVE;
                return;
            }
        }
#endif
        // parse color
        uint32_t color = 0;
        _parse_color(val_start, val_end, &color);
        attr->value.uval = color;
        return;
    }
}

static void _process_opacity_value(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                   const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;

    if(len >= 7 && strncmp(val_start, "inherit", 7) == 0) {
        attr->class_type = LV_SVG_ATTR_VALUE_INHERIT;
        return;
    }

    float val_number = 1.0f;
    val_start = _parse_number(val_start, val_end, &val_number);

    if(val_number < 0.0f) val_number = 0.0f;
    else if(val_number > 1.0f) val_number = 1.0f;

    attr->value.fval = val_number;
}

static void _process_length_value(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                  const char * val_end, int32_t dpi)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    float val_number = 0.0f;
    val_start = _parse_length(val_start, val_end, dpi, &val_number);
    attr->value.fval = val_number;
}

static void _process_transform(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                               const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_PTR;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    if(len >= 4 && strncmp(val_start, "none", 4) == 0) {
        attr->val_type = LV_SVG_ATTR_VALUE_DATA;
        attr->class_type = LV_SVG_ATTR_VALUE_NONE;
        return;
    }

    lv_svg_matrix_t * matrix = lv_malloc_zeroed(sizeof(lv_svg_matrix_t));
    LV_ASSERT_MALLOC(matrix);
    matrix->m[0][0] = matrix->m[1][1] = matrix->m[2][2] = 1.0f; // identity

    const char * ptr = val_start;
    while(ptr < val_end) {
        ptr = _skip_space(ptr, val_end);
        if(ptr == val_end) break;

        len = val_end - ptr;

        if(len >= 9 && strncmp(ptr, "translate", 9) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_TRANSLATE, matrix);
        }
        else if(len >= 6 && strncmp(ptr, "matrix", 6) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_MATRIX, matrix);
        }
        else if(len >= 6 && strncmp(ptr, "rotate", 6) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_ROTATE, matrix);
        }
        else if(len >= 5 && strncmp(ptr, "scale", 5) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_SCALE, matrix);
        }
        else if(len >= 5 && strncmp(ptr, "skewX", 5) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_SKEW_X, matrix);
        }
        else if(len >= 5 && strncmp(ptr, "skewY", 5) == 0) {
            ptr = _parse_matrix(ptr, val_end, LV_SVG_TRANSFORM_TYPE_SKEW_Y, matrix);
        }

        ++ptr;
    }
    attr->value.val = matrix;
}

static void _process_preserve_aspect_ratio(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                           const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    lv_svg_aspect_ratio_t ratio = LV_SVG_ASPECT_RATIO_XMID_YMID;
    uint32_t len = MAP_LEN(_svg_attr_aspect_ratio_map);

    for(uint32_t i = 0; i < len; i++) {
        if(strncmp(_svg_attr_aspect_ratio_map[i].name, val_start, 8) == 0) {
            ratio = _svg_attr_aspect_ratio_map[i].align;
            val_start += 8;
            break;
        }
        else if(strncmp("none", val_start, 4) == 0) {
            ratio = LV_SVG_ASPECT_RATIO_NONE;
            val_start += 4;
            break;
        }
    }

    if(ratio != LV_SVG_ASPECT_RATIO_NONE) {
        len = val_end - val_start;
        if(len > 4) {
            val_start = _skip_space(val_start, val_end);
            if(strncmp(val_start, "meet", 4) == 0) {
                ratio |= LV_SVG_ASPECT_RATIO_OPT_MEET;
            }
            else if(strncmp(val_start, "slice", 5) == 0) {
                ratio |= LV_SVG_ASPECT_RATIO_OPT_SLICE;
            }
        }
    }
    attr->value.uval = ratio;
}

#if LV_USE_SVG_ANIMATION
typedef void(*_parse_list_cb)(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start, const char * val_end,
                              int32_t dpi, void * data);

static uint32_t _parse_anim_value_list(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start,
                                       const char * val_end, int32_t dpi, _parse_list_cb cb, void * data)
{
    uint32_t count = 0;
    val_start = _skip_space(val_start, val_end);
    const char * ptr = val_start;

    while(ptr != val_end) {
        if(*ptr == ';') {
            cb(node, attr, val_start, ptr, dpi, data);
            val_start = ++ptr;
            val_start = _skip_space(val_start, val_end);
            count++;
        }
        else {
            ++ptr;
        }
    }
    if(val_start < val_end) {
        cb(node, attr, val_start, ptr, dpi, data);
        count++;
    }
    return count;
}

static const char * _parse_clock_time(const char * str, const char * str_end, float * val)
{
    str = _parse_number(str, str_end, val);
    if(str) {
        uint32_t len = str_end - str;
        if(len > 0) {
            if(len >= 2 && str[0] == 'm' && str[1] == 's') {
                *val = roundf(*val);
            }
            else {
                *val = roundf(*val * 1000.0f);
            }
        }
        else {
            *val = roundf(*val * 1000.0f);
        }
        str += len;
        return str;
    }
    *val = roundf(*val * 1000.0f);
    return str;
}

static void _process_clock_time(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    if(len == 10 && strncmp(val_start, "indefinite", 10) == 0) {
        attr->value.fval = 0.0f;
        return;
    }

    float val_number = 0.0f;
    val_start = _parse_clock_time(val_start, val_end, &val_number);
    attr->value.fval = val_number; // ms
}

static void _process_anim_attr_number(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                      const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    if(type == LV_SVG_ATTR_REPEAT_COUNT) {
        uint32_t len = val_end - val_start;
        if(len == 10 && strncmp(val_start, "indefinite", 10) == 0) {
            attr->value.uval = 0;
            return;
        }

        float val_number = 0.0f;
        val_start = _parse_number(val_start, val_end, &val_number);
        attr->value.uval = (uint32_t)val_number;
    }
    else {   // LV_SVG_ATTR_ROTATE
        uint32_t len = val_end - val_start;
        if(len == 4 && strncmp(val_start, "auto", 4) == 0) {
            attr->class_type =
                LV_SVG_ATTR_VALUE_INHERIT; // rotated over time by the angle of the direction (i.e., directional tangent vector) of the motion path
            attr->value.fval = 0.0f;
            return;
        }
        else if(len == 12 && strncmp(val_start, "auto-reverse", 12) == 0) {
            attr->class_type =
                LV_SVG_ATTR_VALUE_INHERIT; // rotated over time by the angle of the direction (i.e., directional tangent vector) of the motion path plus 180 degrees.
            attr->value.fval = 180.0f;
            return;
        }

        float val_number = 0.0f;
        val_start = _parse_number(val_start, val_end, &val_number);
        attr->value.fval = val_number;
    }
}

static void _process_anim_attr_names(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                     const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;
    attr->value.ival = _get_svg_attr_type(val_start, val_end);
}

static void _process_anim_attr_options(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                       const char * val_end)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    uint32_t len = val_end - val_start;
    switch(type) {
        case LV_SVG_ATTR_RESTART: {
                if(len == 6 && strncmp(val_start, "always", 6) == 0) {
                    attr->value.ival = LV_SVG_ANIM_RESTART_ALWAYS;
                    return;
                }
                else if(len == 13 && strncmp(val_start, "whenNotActive", 13) == 0) {
                    attr->value.ival = LV_SVG_ANIM_RESTART_WHEN_NOT_ACTIVE;
                    return;
                }
                else if(len == 5 && strncmp(val_start, "never", 5) == 0) {
                    attr->value.ival = LV_SVG_ANIM_RESTART_NEVER;
                    return;
                }
            }
            break;
        case LV_SVG_ATTR_CALC_MODE: {
                if(len == 6 && strncmp(val_start, "linear", 6) == 0) {
                    attr->value.ival = LV_SVG_ANIM_CALC_MODE_LINEAR;
                    return;
                }
                else if(len == 5 && strncmp(val_start, "paced", 5) == 0) {
                    attr->value.ival = LV_SVG_ANIM_CALC_MODE_PACED;
                    return;
                }
                else if(len == 6 && strncmp(val_start, "spline", 6) == 0) {
                    attr->value.ival = LV_SVG_ANIM_CALC_MODE_SPLINE;
                    return;
                }
                else if(len == 8 && strncmp(val_start, "discrete", 8) == 0) {
                    attr->value.ival = LV_SVG_ANIM_CALC_MODE_DISCRETE;
                    return;
                }
            }
            break;
        case LV_SVG_ATTR_ADDITIVE: {
                if(len == 7 && strncmp(val_start, "replace", 7) == 0) {
                    attr->value.ival = LV_SVG_ANIM_ADDITIVE_REPLACE;
                    return;
                }
                else if(len == 3 && strncmp(val_start, "sum", 3) == 0) {
                    attr->value.ival = LV_SVG_ANIM_ADDITIVE_SUM;
                    return;
                }
            }
            break;
        case LV_SVG_ATTR_ACCUMULATE: {
                if(len == 4 && strncmp(val_start, "none", 4) == 0) {
                    attr->value.ival = LV_SVG_ANIM_ACCUMULATE_NONE;
                    return;
                }
                else if(len == 3 && strncmp(val_start, "sum", 3) == 0) {
                    attr->value.ival = LV_SVG_ANIM_ACCUMULATE_SUM;
                    return;
                }
            }
            break;
        case LV_SVG_ATTR_TRANSFORM_TYPE: {
                if(len == 9 && strncmp(val_start, "translate", 9) == 0) {
                    attr->value.ival = LV_SVG_TRANSFORM_TYPE_TRANSLATE;
                    return;
                }
                else if(len == 5 && strncmp(val_start, "scale", 5) == 0) {
                    attr->value.ival = LV_SVG_TRANSFORM_TYPE_SCALE;
                    return;
                }
                else if(len == 6 && strncmp(val_start, "rotate", 6) == 0) {
                    attr->value.ival = LV_SVG_TRANSFORM_TYPE_ROTATE;
                    return;
                }
                else if(len == 5 && strncmp(val_start, "skewX", 5) == 0) {
                    attr->value.ival = LV_SVG_TRANSFORM_TYPE_SKEW_X;
                    return;
                }
                else if(len == 5 && strncmp(val_start, "skewY", 5) == 0) {
                    attr->value.ival = LV_SVG_TRANSFORM_TYPE_SKEW_Y;
                    return;
                }
            }
            break;
    }
    attr->value.ival = 0;
}

static void _parse_anim_value(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start, const char * val_end,
                              int32_t dpi)
{
    if(node->type == LV_SVG_TAG_ANIMATE || node->type == LV_SVG_TAG_SET) {
        float val_number = 0.0f;
        val_start = _parse_length(val_start, val_end, dpi, &val_number);
        attr->value.fval = val_number;
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_COLOR) {
        uint32_t color = 0;
        val_start = _parse_color(val_start, val_end, &color);
        attr->value.uval = color;
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_TRANSFORM) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        lv_svg_attr_values_list_t * list = lv_malloc(sizeof(float) * 4 + sizeof(uint32_t));
        LV_ASSERT_MALLOC(list);

        float val_number = 0.0f;
        uint32_t cnt = 0;
        const char * ptr = val_start;

        while((ptr < val_end) && (cnt < 3)) {
            float * val = (float *)(&list->data) + cnt;

            val_number = 0.0f;
            ptr = _parse_number(ptr, val_end, &val_number);
            *val = val_number;

            if(!ptr) break;
            ++cnt;
        }

        list->length = cnt;
        attr->value.val = list;
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_MOTION) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        lv_svg_attr_values_list_t * list = lv_malloc(sizeof(lv_svg_point_t) + sizeof(uint32_t));
        LV_ASSERT_MALLOC(list);

        lv_svg_point_t * pt = (lv_svg_point_t *)(&list->data);

        float val_number = 0.0f;
        val_start = _parse_number(val_start, val_end, &val_number);
        pt->x = val_number;
        val_number = 0.0f;
        val_start = _parse_number(val_start, val_end, &val_number);
        pt->y = val_number;

        list->length = 1;
        attr->value.val = list;
    }
}

struct _parse_value_list_context {
    uint32_t mem_size;
    uint32_t list_count;
    lv_svg_attr_values_list_t * list;
};

struct _transform_values_list {
    uint32_t length;
    float data[4];
};

#define GET_NEXT_VALUE_PTR(ptr, ctx, type) \
    do { \
        lv_svg_attr_values_list_t * list = ctx->list; \
        if(!list) { \
            ctx->mem_size = sizeof(type) * 4 + sizeof(uint32_t);\
            ctx->list = lv_malloc_zeroed(ctx->mem_size); \
            LV_ASSERT_MALLOC(ctx->list); \
            ptr = (type *)(&(ctx->list->data)); \
            ctx->list_count = 1; \
        } else { \
            uint32_t mem = sizeof(type) * (ctx->list_count + 1) + sizeof(uint32_t); \
            if(ctx->mem_size < mem) { \
                ctx->mem_size = (ctx->list_count << 1) * sizeof(type) + sizeof(uint32_t); \
                ctx->list = (lv_svg_attr_values_list_t *)lv_realloc(ctx->list, ctx->mem_size); \
                LV_ASSERT_MALLOC(ctx->list); \
            } \
            ptr = (type *)(&(ctx->list->data)) + ctx->list_count; \
            ctx->list_count++; \
        } \
    } while(0)

static void _anim_values_cb(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start, const char * val_end,
                            int32_t dpi, void * data)
{
    LV_UNUSED(attr);
    struct _parse_value_list_context * ctx = (struct _parse_value_list_context *)data;

    if(node->type == LV_SVG_TAG_ANIMATE || node->type == LV_SVG_TAG_SET) {
        float * val_number = NULL;
        GET_NEXT_VALUE_PTR(val_number, ctx, float);
        val_start = _parse_length(val_start, val_end, dpi, val_number);
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_COLOR) {
        uint32_t * color = NULL;
        GET_NEXT_VALUE_PTR(color, ctx, uint32_t);
        val_start = _parse_color(val_start, val_end, color);
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_TRANSFORM) {
        struct _transform_values_list * trans_vals = NULL;
        GET_NEXT_VALUE_PTR(trans_vals, ctx, struct _transform_values_list);

        uint32_t cnt = 0;
        const char * ptr = val_start;

        while((ptr < val_end) && (cnt < 3)) {
            float * val = &(trans_vals->data[cnt]);
            ptr = _parse_number(ptr, val_end, val);
            if(!ptr) break;
            ++cnt;
        }

        trans_vals->length = cnt;
    }
    else if(node->type == LV_SVG_TAG_ANIMATE_MOTION) {
        lv_svg_point_t * point = NULL;
        GET_NEXT_VALUE_PTR(point, ctx, lv_svg_point_t);
        val_start = _parse_number(val_start, val_end, &point->x);
        val_start = _parse_number(val_start, val_end, &point->y);
    }
    ctx->list->length = ctx->list_count;
}

static void _anim_keys_cb(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start, const char * val_end,
                          int32_t dpi, void * data)
{
    LV_UNUSED(node);
    LV_UNUSED(attr);
    LV_UNUSED(dpi);
    struct _parse_value_list_context * ctx = (struct _parse_value_list_context *)data;

    float * val_number = NULL;
    GET_NEXT_VALUE_PTR(val_number, ctx, float);
    val_start = _parse_number(val_start, val_end, val_number);

    ctx->list->length = ctx->list_count;
}

static void _anim_key_splines_cb(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start,
                                 const char * val_end, int32_t dpi, void * data)
{
    LV_UNUSED(node);
    LV_UNUSED(attr);
    LV_UNUSED(dpi);
    struct _parse_value_list_context * ctx = (struct _parse_value_list_context *)data;

    lv_svg_point_t * point = NULL;
    GET_NEXT_VALUE_PTR(point, ctx, lv_svg_point_t);
    val_start = _parse_number(val_start, val_end, &point->x);
    val_start = _parse_number(val_start, val_end, &point->y);

    GET_NEXT_VALUE_PTR(point, ctx, lv_svg_point_t);
    val_start = _parse_number(val_start, val_end, &point->x);
    val_start = _parse_number(val_start, val_end, &point->y);

    ctx->list->length = ctx->list_count;
}

static void _anim_begin_end_cb(lv_svg_node_t * node, lv_svg_attr_t * attr, const char * val_start,
                               const char * val_end, int32_t dpi, void * data)
{
    LV_UNUSED(node);
    LV_UNUSED(attr);
    LV_UNUSED(dpi);
    struct _parse_value_list_context * ctx = (struct _parse_value_list_context *)data;

    // offset-value
    float * val_number = NULL;
    GET_NEXT_VALUE_PTR(val_number, ctx, float);
    val_start = _parse_clock_time(val_start, val_end, val_number);

    //FIXME: not support begin-end type
    // syncbase-value
    // event-value
    // repeat-value
    // accessKey-value
    // indefinite

    ctx->list->length = ctx->list_count;
}

static void _process_anim_attr_values(lv_svg_node_t * node, lv_svg_attr_type_t type, const char * val_start,
                                      const char * val_end, int32_t dpi)
{
    CHECK_AND_RESIZE_ATTRS(node->attrs);

    node->attrs.size++;
    lv_svg_attr_t * attr = lv_array_at(&node->attrs, node->attrs.size - 1);
    attr->id = type;
    attr->val_type = LV_SVG_ATTR_VALUE_DATA;
    attr->class_type = LV_SVG_ATTR_VALUE_INITIAL;

    if(type == LV_SVG_ATTR_VALUES) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        struct _parse_value_list_context ctx = {.mem_size = 0, .list_count = 0, .list = NULL};
        _parse_anim_value_list(node, attr, val_start, val_end, dpi, _anim_values_cb, &ctx);
        attr->value.val = ctx.list;
    }
    else if(type == LV_SVG_ATTR_KEY_TIMES || type == LV_SVG_ATTR_KEY_POINTS) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        struct _parse_value_list_context ctx = {.mem_size = 0, .list_count = 0, .list = NULL};
        _parse_anim_value_list(node, attr, val_start, val_end, dpi, _anim_keys_cb, &ctx);
        attr->value.val = ctx.list;
    }
    else if(type == LV_SVG_ATTR_KEY_SPLINES) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        struct _parse_value_list_context ctx = {.mem_size = 0, .list_count = 0, .list = NULL};
        _parse_anim_value_list(node, attr, val_start, val_end, dpi, _anim_key_splines_cb, &ctx);
        attr->value.val = ctx.list;
    }
    else if(type == LV_SVG_ATTR_BEGIN || type == LV_SVG_ATTR_END) {
        attr->val_type = LV_SVG_ATTR_VALUE_PTR;
        struct _parse_value_list_context ctx = {.mem_size = 0, .list_count = 0, .list = NULL};
        _parse_anim_value_list(node, attr, val_start, val_end, dpi, _anim_begin_end_cb, &ctx);
        attr->value.val = ctx.list;
    }
    else {
        _parse_anim_value(node, attr, val_start, val_end, dpi);
    }
}

#endif

static void _process_attrs_tag(_lv_svg_parser_t * parser, lv_svg_node_t * node, const _lv_svg_token_t * token)
{
    uint32_t len = lv_array_size(&token->attrs);
    for(uint32_t i = 0; i < len; i++) {
        _lv_svg_token_attr_t * tok_attr = lv_array_at(&token->attrs, i);
        lv_svg_attr_type_t type = _get_svg_attr_type(tok_attr->name_start, tok_attr->name_end);

        tok_attr->value_start = _skip_space(tok_attr->value_start, tok_attr->value_end);
        uint32_t value_len = tok_attr->value_end - tok_attr->value_start;
        if(value_len == 0) {
            continue; // skip empty value attribute
        }

        if(type == LV_SVG_ATTR_XML_ID || type == LV_SVG_ATTR_ID) { // get xml:id
            char * str = lv_malloc(value_len + 1);
            LV_ASSERT_MALLOC(str);
            lv_memcpy(str, tok_attr->value_start, value_len);
            str[value_len] = '\0';
            node->xml_id = str;
            continue;
        }

        switch(type) {
            case LV_SVG_ATTR_VERSION:
            case LV_SVG_ATTR_BASE_PROFILE:
                _process_string(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_VIEWBOX:
                _process_view_box(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_PRESERVE_ASPECT_RATIO:
                _process_preserve_aspect_ratio(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_X:
            case LV_SVG_ATTR_Y:
            case LV_SVG_ATTR_WIDTH:
            case LV_SVG_ATTR_HEIGHT:
            case LV_SVG_ATTR_RX:
            case LV_SVG_ATTR_RY:
            case LV_SVG_ATTR_CX:
            case LV_SVG_ATTR_CY:
            case LV_SVG_ATTR_R:
            case LV_SVG_ATTR_X1:
            case LV_SVG_ATTR_Y1:
            case LV_SVG_ATTR_X2:
            case LV_SVG_ATTR_Y2:
            case LV_SVG_ATTR_PATH_LENGTH:
                _process_length_value(node, type, tok_attr->value_start, tok_attr->value_end, parser->dpi);
                break;
            case LV_SVG_ATTR_OPACITY:
            case LV_SVG_ATTR_FILL_OPACITY:
            case LV_SVG_ATTR_STROKE_OPACITY:
            case LV_SVG_ATTR_SOLID_OPACITY:
            case LV_SVG_ATTR_VIEWPORT_FILL_OPACITY:
            case LV_SVG_ATTR_GRADIENT_STOP_OPACITY:
                _process_opacity_value(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_POINTS:
                _process_points_value(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_D:
#if LV_USE_SVG_ANIMATION
            case LV_SVG_ATTR_PATH:
#endif
                _process_path_value(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_TRANSFORM:
                _process_transform(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_FILL:
            case LV_SVG_ATTR_STROKE:
            case LV_SVG_ATTR_VIEWPORT_FILL:
            case LV_SVG_ATTR_SOLID_COLOR:
            case LV_SVG_ATTR_GRADIENT_STOP_COLOR:
                _process_paint(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_FILL_RULE:
            case LV_SVG_ATTR_STROKE_LINECAP:
            case LV_SVG_ATTR_STROKE_LINEJOIN:
            case LV_SVG_ATTR_STROKE_WIDTH:
            case LV_SVG_ATTR_STROKE_MITER_LIMIT:
            case LV_SVG_ATTR_STROKE_DASH_OFFSET:
            case LV_SVG_ATTR_GRADIENT_STOP_OFFSET:
                _process_paint_attrs(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_STROKE_DASH_ARRAY:
                _process_paint_dasharray(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_GRADIENT_UNITS:
                _process_gradient_units(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_FONT_FAMILY:
            case LV_SVG_ATTR_FONT_STYLE:
            case LV_SVG_ATTR_FONT_VARIANT:
            case LV_SVG_ATTR_FONT_WEIGHT:
            case LV_SVG_ATTR_FONT_SIZE:
                _process_font_attrs(node, type, tok_attr->value_start, tok_attr->value_end, parser->dpi);
                break;
            case LV_SVG_ATTR_XLINK_HREF:
                _process_xlink(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
#if LV_USE_SVG_ANIMATION
            case LV_SVG_ATTR_DUR:
            case LV_SVG_ATTR_MIN:
            case LV_SVG_ATTR_MAX:
            case LV_SVG_ATTR_REPEAT_DUR:
                _process_clock_time(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_ATTRIBUTE_NAME:
                _process_anim_attr_names(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_FROM:
            case LV_SVG_ATTR_TO:
            case LV_SVG_ATTR_BY:
            case LV_SVG_ATTR_VALUES:
            case LV_SVG_ATTR_KEY_TIMES:
            case LV_SVG_ATTR_KEY_POINTS:
            case LV_SVG_ATTR_KEY_SPLINES:
            case LV_SVG_ATTR_BEGIN:
            case LV_SVG_ATTR_END:
                _process_anim_attr_values(node, type, tok_attr->value_start, tok_attr->value_end, parser->dpi);
                break;
            case LV_SVG_ATTR_ROTATE:
            case LV_SVG_ATTR_REPEAT_COUNT:
                _process_anim_attr_number(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_RESTART:
            case LV_SVG_ATTR_CALC_MODE:
            case LV_SVG_ATTR_ADDITIVE:
            case LV_SVG_ATTR_ACCUMULATE:
            case LV_SVG_ATTR_TRANSFORM_TYPE:
                _process_anim_attr_options(node, type, tok_attr->value_start, tok_attr->value_end);
                break;
            case LV_SVG_ATTR_ATTRIBUTE_TYPE:
#endif
            case LV_SVG_ATTR_DISPLAY:
            case LV_SVG_ATTR_VISIBILITY:
            case LV_SVG_ATTR_TEXT_ANCHOR:
                // not support yet
                break;
        }
    }
}

static bool _process_begin_tag(_lv_svg_parser_t * parser, lv_svg_tag_t tag, const _lv_svg_token_t * token)
{
    if(parser->state == LV_SVG_PARSER_IGNORE) {
        // ignore ignored tokens
        return true;
    }

    if(token->type == LV_SVG_TOKEN_CONTENT) {
        uint32_t len = SVG_TOKEN_LEN(token);
        char * content = lv_malloc(len + 1);
        LV_ASSERT_MALLOC(content);
        lv_memcpy(content, token->start, len);
        content[len] = '\0';
        lv_svg_node_t * node = lv_svg_node_create(parser->cur_node);
        node->xml_id = content;
        node->type = LV_SVG_TAG_CONTENT;
        return true;
    }

    // begin invalid tag
    if(tag == LV_SVG_TAG_INVALID) {
        if(!token->flat) {
            parser->state = LV_SVG_PARSER_IGNORE;
            uint32_t len = SVG_TOKEN_LEN(token);
            parser->ignore_name = lv_malloc(len + 1);
            LV_ASSERT_MALLOC(parser->ignore_name);
            parser->ignore_len = len;
            lv_memcpy(parser->ignore_name, token->start, len);
            parser->ignore_name[len] = '\0';
        }
        return true;
    }

    // create new node
    lv_svg_node_t * node = lv_svg_node_create(parser->cur_node);
    node->type = tag;
    _process_attrs_tag(parser, node, token);

    if(!parser->doc_root) { // root node
        parser->doc_root = node;
    }
    if(!token->flat) { // FIXME: not leaf node
        parser->cur_node = node;
    }
    return true;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void _lv_svg_parser_init(_lv_svg_parser_t * parser)
{
    LV_ASSERT_NULL(parser);
    lv_memzero(parser, sizeof(_lv_svg_parser_t));
    parser->state = LV_SVG_PARSER_PROCESS;
    parser->ignore_name = NULL;
    parser->ignore_len = 0;
    parser->dpi = 96; // FIXME: Is it right ?
    parser->doc_root = NULL;
    parser->cur_node = NULL;
}

void _lv_svg_parser_deinit(_lv_svg_parser_t * parser)
{
    LV_ASSERT_NULL(parser);
    if(parser->ignore_name) {
        lv_free(parser->ignore_name);
        parser->ignore_name = NULL;
        parser->ignore_len = 0;
    }

    if(parser->doc_root) {
        lv_svg_node_delete(parser->doc_root);
    }
    parser->doc_root = parser->cur_node = NULL;
}

bool _lv_svg_parser_is_finish(_lv_svg_parser_t * parser)
{
    LV_ASSERT_NULL(parser);
    return (parser->doc_root != NULL)
           && (parser->cur_node == parser->doc_root)
           && (parser->state != LV_SVG_PARSER_IGNORE);
}

bool _lv_svg_parser_token(_lv_svg_parser_t * parser, const _lv_svg_token_t * token)
{
    LV_ASSERT_NULL(parser);
    LV_ASSERT_NULL(token);
    lv_svg_tag_t tag = _get_svg_tag_type(token);

    if(parser->doc_root == NULL) {
        if(!(tag == LV_SVG_TAG_SVG && token->type == LV_SVG_TOKEN_BEGIN)) {
            LV_LOG_ERROR("root element in svg document must be <svg>!");
            return false;
        }
    }

    if(token->type == LV_SVG_TOKEN_END) {
        return _process_end_tag(parser, tag, token);
    }

    return _process_begin_tag(parser, tag, token);
}

#if LV_USE_SVG_DEBUG
#include <stdio.h>
void _lv_svg_dump_tree(lv_svg_node_t * root, int depth)
{
    if(!root) {
        return;
    }

    for(int i = 0; i < depth; i++) {
        printf("  ");
    }
    if(root->type == LV_SVG_TAG_CONTENT) {
        printf("content: [%s]\n", root->xml_id);
    }
    else {
        printf("tag <%s>", _svg_tag_map[root->type - 1].name);
        if(root->xml_id) {
            printf(" - id [%s]", root->xml_id);
        }
        printf("\n");
    }

    uint32_t len = lv_array_size(&root->attrs);
    for(uint32_t i = 0; i < len; i++) {
        for(int j = 0; j < depth; j++) {
            printf("  ");
        }
        lv_svg_attr_t * attr = lv_array_at(&root->attrs, i);
        printf("   attr <%s>\n", _svg_attr_map[attr->id - 1].name);
    }

    lv_tree_node_t * tree_root = (lv_tree_node_t *)root;

    for(uint32_t i = 0; i < tree_root->child_cnt; i++) {
        ++depth;
        _lv_svg_dump_tree((lv_svg_node_t *)tree_root->children[i], depth);
        --depth;
    }
}
#endif /*LV_USE_SVG_DEBUG*/

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_SVG*/
