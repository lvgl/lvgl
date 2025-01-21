#include "../../../lvgl.h"

#if LV_BUILD_TEST && LV_USE_SVG

#ifndef LV_ATTRIBUTE_MEM_ALIGN
    #define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_TEST__IMAGE_SVG
    #define LV_ATTRIBUTE_IMAGE_TEST_IMAGE_SVG
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_TEST_IMAGE_SVG uint8_t img_svg_data[] = "<svg width=\"12cm\" height=\"4cm\" viewBox=\"0 0 1200 400\">"
                             "<rect x=\"350\" y=\"100\" width=\"200\" height=\"200\" fill=\"red\" stroke=\"blue\" stroke-width=\"10\"/></svg>";

const lv_image_dsc_t test_image_svg = {
    .header.w = 250,
    .header.h = 250,
    .header.stride = 0,
    .header.cf = LV_COLOR_FORMAT_NATIVE,
    .data = img_svg_data,
    .data_size = sizeof(img_svg_data) - 1, // ignore '\0'
};

#endif /* LV_BUILD_TEST */
