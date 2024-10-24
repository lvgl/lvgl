#include "../../lvgl.h"

#if LV_BUILD_EXAMPLES


const LV_ATTRIBUTE_MEM_ALIGN uint8_t img_svg_data[] = "<svg width=\"12cm\" height=\"4cm\" viewBox=\"0 0 1200 400\">"
                             "<rect x=\"500\" y=\"100\" width=\"200\" height=\"200\" fill=\"red\" stroke=\"blue\" stroke-width=\"10\"/></svg>";

const lv_image_dsc_t img_svg_img = {
    .header.w = 450,
    .header.h = 150,
    .header.stride = 0,
    .header.cf = LV_COLOR_FORMAT_NATIVE,
    .data = img_svg_data,
    .data_size = sizeof(img_svg_data),
};

#endif /* LV_BUILD_EXAMPLES */
