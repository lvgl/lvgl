#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if __WORDSIZE == 64
    #define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(NAME) TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_" NAME ".lp64.png")
#elif __WORDSIZE == 32
    #define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(NAME) TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_" NAME ".lp32.png")
#endif

#define OPTION_GENERATE_OUTLINE_DATA 0

/*
 * Generated vector ops string can use https://w-mai.github.io/vegravis/
 * to visualize the outline data.
 **/
#define OPTION_GENERATE_VECTOR_OPS_STRING 0

static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN =
    "鉴于对人类家庭所有成员的固有尊严及其平等的和不移的权利的承认，乃是世界自由、正义与和平的基础...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN =
    "Whereas recognition of the inherent dignity and of the equal and inalienable rights of all members of the human family is the foundation of freedom, justice and peace in the world...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP =
    "人間の家族のすべての構成員の固有の尊厳と平等で譲渡不能な権利とを承認することは、自由と正義と平和の基礎である...";

// Outline data for unicode '龘' (U+9F98)
static const lv_freetype_outline_event_param_t outline_data_U9F98[] = {
    {NULL, 1, {6792, 6889}, {0, 0}, {0, 0}},
    {NULL, 2, {7349, 6889}, {0, 0}, {0, 0}},
    {NULL, 2, {7014, 5915}, {0, 0}, {0, 0}},
    {NULL, 2, {6457, 5915}, {0, 0}, {0, 0}},
    {NULL, 2, {6792, 6889}, {0, 0}, {0, 0}},
    {NULL, 1, {2744, 5145}, {0, 0}, {0, 0}},
    {NULL, 2, {5251, 5145}, {0, 0}, {0, 0}},
    {NULL, 2, {5141, 4825}, {0, 0}, {0, 0}},
    {NULL, 2, {3158, 4825}, {0, 0}, {0, 0}},
    {NULL, 2, {2597, 3195}, {0, 0}, {0, 0}},
    {NULL, 2, {2073, 3195}, {0, 0}, {0, 0}},
    {NULL, 2, {2744, 5145}, {0, 0}, {0, 0}},
    {NULL, 1, {6511, 6070}, {0, 0}, {0, 0}},
    {NULL, 2, {9370, 6070}, {0, 0}, {0, 0}},
    {NULL, 2, {9091, 5259}, {0, 0}, {0, 0}},
    {NULL, 2, {6232, 5259}, {0, 0}, {0, 0}},
    {NULL, 2, {6336, 5562}, {0, 0}, {0, 0}},
    {NULL, 2, {8646, 5562}, {0, 0}, {0, 0}},
    {NULL, 2, {8719, 5775}, {0, 0}, {0, 0}},
    {NULL, 2, {6409, 5775}, {0, 0}, {0, 0}},
    {NULL, 2, {6511, 6070}, {0, 0}, {0, 0}},
    {NULL, 1, {6294, 5439}, {0, 0}, {0, 0}},
    {NULL, 2, {6851, 5439}, {0, 0}, {0, 0}},
    {NULL, 2, {6282, 3785}, {0, 0}, {0, 0}},
    {NULL, 4, {6304, 3600}, {6234, 3645}, {0, 0}},
    {NULL, 4, {6661, 3555}, {6375, 3555}, {0, 0}},
    {NULL, 4, {6927, 3555}, {6727, 3555}, {0, 0}},
    {NULL, 4, {7370, 3555}, {7128, 3555}, {0, 0}},
    {NULL, 4, {7825, 3555}, {7612, 3555}, {0, 0}},
    {NULL, 4, {8136, 3555}, {8038, 3555}, {0, 0}},
    {NULL, 4, {8349, 3579}, {8267, 3555}, {0, 0}},
    {NULL, 4, {8494, 3690}, {8432, 3604}, {0, 0}},
    {NULL, 4, {8626, 3957}, {8556, 3777}, {0, 0}},
    {NULL, 4, {8815, 3887}, {8702, 3916}, {0, 0}},
    {NULL, 4, {9030, 3842}, {8928, 3858}, {0, 0}},
    {NULL, 4, {8679, 3359}, {8873, 3482}, {0, 0}},
    {NULL, 4, {8092, 3236}, {8485, 3236}, {0, 0}},
    {NULL, 4, {7870, 3236}, {8034, 3236}, {0, 0}},
    {NULL, 4, {7498, 3236}, {7707, 3236}, {0, 0}},
    {NULL, 4, {7080, 3236}, {7289, 3236}, {0, 0}},
    {NULL, 4, {6711, 3236}, {6871, 3236}, {0, 0}},
    {NULL, 4, {6502, 3236}, {6551, 3236}, {0, 0}},
    {NULL, 4, {5783, 3334}, {5962, 3236}, {0, 0}},
    {NULL, 4, {5728, 3793}, {5604, 3432}, {0, 0}},
    {NULL, 2, {6294, 5439}, {0, 0}, {0, 0}},
    {NULL, 1, {5116, 6185}, {0, 0}, {0, 0}},
    {NULL, 2, {5549, 6038}, {0, 0}, {0, 0}},
    {NULL, 4, {5245, 5726}, {5386, 5874}, {0, 0}},
    {NULL, 4, {4975, 5464}, {5105, 5579}, {0, 0}},
    {NULL, 2, {4635, 5595}, {0, 0}, {0, 0}},
    {NULL, 4, {4893, 5894}, {4753, 5726}, {0, 0}},
    {NULL, 4, {5116, 6185}, {5033, 6062}, {0, 0}},
    {NULL, 1, {3299, 6046}, {0, 0}, {0, 0}},
    {NULL, 2, {3751, 6169}, {0, 0}, {0, 0}},
    {NULL, 4, {3824, 5894}, {3807, 6046}, {0, 0}},
    {NULL, 4, {3827, 5628}, {3842, 5743}, {0, 0}},
    {NULL, 2, {3332, 5497}, {0, 0}, {0, 0}},
    {NULL, 4, {3340, 5763}, {3351, 5603}, {0, 0}},
    {NULL, 4, {3299, 6046}, {3330, 5923}, {0, 0}},
    {NULL, 1, {4997, 5145}, {0, 0}, {0, 0}},
    {NULL, 2, {5521, 5145}, {0, 0}, {0, 0}},
    {NULL, 2, {4985, 3588}, {0, 0}, {0, 0}},
    {NULL, 4, {4863, 3362}, {4932, 3432}, {0, 0}},
    {NULL, 4, {4665, 3252}, {4794, 3293}, {0, 0}},
    {NULL, 4, {4332, 3203}, {4536, 3211}, {0, 0}},
    {NULL, 4, {3842, 3195}, {4129, 3195}, {0, 0}},
    {NULL, 4, {3849, 3359}, {3862, 3277}, {0, 0}},
    {NULL, 4, {3818, 3506}, {3837, 3441}, {0, 0}},
    {NULL, 4, {4137, 3506}, {3990, 3506}, {0, 0}},
    {NULL, 4, {4334, 3506}, {4285, 3506}, {0, 0}},
    {NULL, 4, {4418, 3522}, {4392, 3506}, {0, 0}},
    {NULL, 4, {4461, 3588}, {4445, 3539}, {0, 0}},
    {NULL, 2, {4997, 5145}, {0, 0}, {0, 0}},
    {NULL, 1, {6936, 6595}, {0, 0}, {0, 0}},
    {NULL, 2, {9853, 6595}, {0, 0}, {0, 0}},
    {NULL, 2, {9749, 6291}, {0, 0}, {0, 0}},
    {NULL, 2, {6832, 6291}, {0, 0}, {0, 0}},
    {NULL, 2, {6936, 6595}, {0, 0}, {0, 0}},
    {NULL, 1, {6393, 5038}, {0, 0}, {0, 0}},
    {NULL, 2, {8851, 5038}, {0, 0}, {0, 0}},
    {NULL, 2, {8758, 4768}, {0, 0}, {0, 0}},
    {NULL, 2, {6300, 4768}, {0, 0}, {0, 0}},
    {NULL, 2, {6393, 5038}, {0, 0}, {0, 0}},
    {NULL, 1, {6244, 4555}, {0, 0}, {0, 0}},
    {NULL, 2, {8644, 4555}, {0, 0}, {0, 0}},
    {NULL, 2, {8548, 4276}, {0, 0}, {0, 0}},
    {NULL, 2, {6148, 4276}, {0, 0}, {0, 0}},
    {NULL, 2, {6244, 4555}, {0, 0}, {0, 0}},
    {NULL, 1, {6055, 4055}, {0, 0}, {0, 0}},
    {NULL, 2, {8578, 4055}, {0, 0}, {0, 0}},
    {NULL, 2, {8485, 3785}, {0, 0}, {0, 0}},
    {NULL, 2, {5962, 3785}, {0, 0}, {0, 0}},
    {NULL, 2, {6055, 4055}, {0, 0}, {0, 0}},
    {NULL, 1, {2612, 4047}, {0, 0}, {0, 0}},
    {NULL, 2, {4881, 4047}, {0, 0}, {0, 0}},
    {NULL, 2, {4780, 3752}, {0, 0}, {0, 0}},
    {NULL, 2, {2511, 3752}, {0, 0}, {0, 0}},
    {NULL, 2, {2612, 4047}, {0, 0}, {0, 0}},
    {NULL, 1, {2798, 4588}, {0, 0}, {0, 0}},
    {NULL, 2, {5067, 4588}, {0, 0}, {0, 0}},
    {NULL, 2, {4966, 4293}, {0, 0}, {0, 0}},
    {NULL, 2, {2697, 4293}, {0, 0}, {0, 0}},
    {NULL, 2, {2798, 4588}, {0, 0}, {0, 0}},
    {NULL, 1, {2912, 6447}, {0, 0}, {0, 0}},
    {NULL, 2, {6197, 6447}, {0, 0}, {0, 0}},
    {NULL, 2, {6088, 6128}, {0, 0}, {0, 0}},
    {NULL, 2, {2803, 6128}, {0, 0}, {0, 0}},
    {NULL, 2, {2912, 6447}, {0, 0}, {0, 0}},
    {NULL, 1, {2438, 5710}, {0, 0}, {0, 0}},
    {NULL, 2, {6182, 5710}, {0, 0}, {0, 0}},
    {NULL, 2, {6072, 5390}, {0, 0}, {0, 0}},
    {NULL, 2, {2328, 5390}, {0, 0}, {0, 0}},
    {NULL, 2, {2438, 5710}, {0, 0}, {0, 0}},
    {NULL, 1, {4279, 6824}, {0, 0}, {0, 0}},
    {NULL, 2, {4810, 6914}, {0, 0}, {0, 0}},
    {NULL, 4, {4897, 6607}, {4863, 6783}, {0, 0}},
    {NULL, 4, {4937, 6308}, {4931, 6431}, {0, 0}},
    {NULL, 2, {4379, 6185}, {0, 0}, {0, 0}},
    {NULL, 4, {4360, 6500}, {4383, 6316}, {0, 0}},
    {NULL, 4, {4279, 6824}, {4338, 6685}, {0, 0}},
    {NULL, 1, {3419, 3129}, {0, 0}, {0, 0}},
    {NULL, 2, {3788, 3129}, {0, 0}, {0, 0}},
    {NULL, 2, {3450, 2146}, {0, 0}, {0, 0}},
    {NULL, 2, {3081, 2146}, {0, 0}, {0, 0}},
    {NULL, 2, {3419, 3129}, {0, 0}, {0, 0}},
    {NULL, 1, {1047, 1425}, {0, 0}, {0, 0}},
    {NULL, 2, {2350, 1425}, {0, 0}, {0, 0}},
    {NULL, 2, {2240, 1106}, {0, 0}, {0, 0}},
    {NULL, 2, {1281, 1106}, {0, 0}, {0, 0}},
    {NULL, 2, {698, -590}, {0, 0}, {0, 0}},
    {NULL, 2, {354, -590}, {0, 0}, {0, 0}},
    {NULL, 2, {1047, 1425}, {0, 0}, {0, 0}},
    {NULL, 1, {3137, 2310}, {0, 0}, {0, 0}},
    {NULL, 2, {4620, 2310}, {0, 0}, {0, 0}},
    {NULL, 2, {4339, 1491}, {0, 0}, {0, 0}},
    {NULL, 2, {2856, 1491}, {0, 0}, {0, 0}},
    {NULL, 2, {2957, 1786}, {0, 0}, {0, 0}},
    {NULL, 2, {4087, 1786}, {0, 0}, {0, 0}},
    {NULL, 2, {4166, 2015}, {0, 0}, {0, 0}},
    {NULL, 2, {3036, 2015}, {0, 0}, {0, 0}},
    {NULL, 2, {3137, 2310}, {0, 0}, {0, 0}},
    {NULL, 1, {2926, 1696}, {0, 0}, {0, 0}},
    {NULL, 2, {3286, 1696}, {0, 0}, {0, 0}},
    {NULL, 2, {2694, -25}, {0, 0}, {0, 0}},
    {NULL, 4, {2680, -184}, {2652, -147}, {0, 0}},
    {NULL, 4, {2840, -221}, {2709, -221}, {0, 0}},
    {NULL, 4, {3016, -221}, {2890, -221}, {0, 0}},
    {NULL, 4, {3282, -221}, {3143, -221}, {0, 0}},
    {NULL, 4, {3471, -221}, {3422, -221}, {0, 0}},
    {NULL, 4, {3640, -147}, {3578, -221}, {0, 0}},
    {NULL, 4, {3801, 188}, {3702, -74}, {0, 0}},
    {NULL, 4, {3914, 114}, {3844, 147}, {0, 0}},
    {NULL, 4, {4053, 66}, {3985, 82}, {0, 0}},
    {NULL, 4, {3772, -418}, {3913, -295}, {0, 0}},
    {NULL, 4, {3402, -541}, {3631, -541}, {0, 0}},
    {NULL, 4, {3275, -541}, {3378, -541}, {0, 0}},
    {NULL, 4, {3050, -541}, {3173, -541}, {0, 0}},
    {NULL, 4, {2828, -541}, {2927, -541}, {0, 0}},
    {NULL, 4, {2698, -541}, {2730, -541}, {0, 0}},
    {NULL, 4, {2308, -446}, {2394, -541}, {0, 0}},
    {NULL, 4, {2334, -25}, {2222, -352}, {0, 0}},
    {NULL, 2, {2926, 1696}, {0, 0}, {0, 0}},
    {NULL, 1, {2513, 2474}, {0, 0}, {0, 0}},
    {NULL, 2, {2769, 2335}, {0, 0}, {0, 0}},
    {NULL, 4, {2492, 2031}, {2622, 2171}, {0, 0}},
    {NULL, 4, {2255, 1794}, {2362, 1892}, {0, 0}},
    {NULL, 2, {2052, 1917}, {0, 0}, {0, 0}},
    {NULL, 4, {2292, 2199}, {2159, 2040}, {0, 0}},
    {NULL, 4, {2513, 2474}, {2425, 2359}, {0, 0}},
    {NULL, 1, {1445, 2367}, {0, 0}, {0, 0}},
    {NULL, 2, {1770, 2433}, {0, 0}, {0, 0}},
    {NULL, 4, {1801, 2166}, {1796, 2318}, {0, 0}},
    {NULL, 4, {1792, 1901}, {1807, 2015}, {0, 0}},
    {NULL, 2, {1441, 1810}, {0, 0}, {0, 0}},
    {NULL, 4, {1469, 2080}, {1465, 1925}, {0, 0}},
    {NULL, 4, {1445, 2367}, {1474, 2236}, {0, 0}},
    {NULL, 1, {2202, 1425}, {0, 0}, {0, 0}},
    {NULL, 2, {2554, 1425}, {0, 0}, {0, 0}},
    {NULL, 2, {1996, -197}, {0, 0}, {0, 0}},
    {NULL, 4, {1885, -426}, {1943, -352}, {0, 0}},
    {NULL, 4, {1723, -541}, {1827, -500}, {0, 0}},
    {NULL, 4, {1474, -586}, {1627, -582}, {0, 0}},
    {NULL, 4, {1091, -590}, {1321, -590}, {0, 0}},
    {NULL, 4, {1110, -430}, {1109, -516}, {0, 0}},
    {NULL, 4, {1103, -270}, {1111, -344}, {0, 0}},
    {NULL, 4, {1378, -279}, {1256, -279}, {0, 0}},
    {NULL, 4, {1542, -279}, {1501, -279}, {0, 0}},
    {NULL, 4, {1644, -197}, {1616, -279}, {0, 0}},
    {NULL, 2, {2202, 1425}, {0, 0}, {0, 0}},
    {NULL, 1, {3478, 2826}, {0, 0}, {0, 0}},
    {NULL, 2, {4862, 2826}, {0, 0}, {0, 0}},
    {NULL, 2, {4761, 2531}, {0, 0}, {0, 0}},
    {NULL, 2, {3377, 2531}, {0, 0}, {0, 0}},
    {NULL, 2, {3478, 2826}, {0, 0}, {0, 0}},
    {NULL, 1, {2946, 1278}, {0, 0}, {0, 0}},
    {NULL, 2, {4183, 1278}, {0, 0}, {0, 0}},
    {NULL, 2, {4091, 1008}, {0, 0}, {0, 0}},
    {NULL, 2, {2854, 1008}, {0, 0}, {0, 0}},
    {NULL, 2, {2946, 1278}, {0, 0}, {0, 0}},
    {NULL, 1, {2777, 786}, {0, 0}, {0, 0}},
    {NULL, 2, {4022, 786}, {0, 0}, {0, 0}},
    {NULL, 2, {3929, 516}, {0, 0}, {0, 0}},
    {NULL, 2, {2684, 516}, {0, 0}, {0, 0}},
    {NULL, 2, {2777, 786}, {0, 0}, {0, 0}},
    {NULL, 1, {2608, 295}, {0, 0}, {0, 0}},
    {NULL, 2, {3730, 295}, {0, 0}, {0, 0}},
    {NULL, 2, {3635, 16}, {0, 0}, {0, 0}},
    {NULL, 2, {2513, 16}, {0, 0}, {0, 0}},
    {NULL, 2, {2608, 295}, {0, 0}, {0, 0}},
    {NULL, 1, {834, 328}, {0, 0}, {0, 0}},
    {NULL, 2, {1989, 328}, {0, 0}, {0, 0}},
    {NULL, 2, {1887, 33}, {0, 0}, {0, 0}},
    {NULL, 2, {732, 33}, {0, 0}, {0, 0}},
    {NULL, 2, {834, 328}, {0, 0}, {0, 0}},
    {NULL, 1, {1019, 868}, {0, 0}, {0, 0}},
    {NULL, 2, {2174, 868}, {0, 0}, {0, 0}},
    {NULL, 2, {2073, 573}, {0, 0}, {0, 0}},
    {NULL, 2, {918, 573}, {0, 0}, {0, 0}},
    {NULL, 2, {1019, 868}, {0, 0}, {0, 0}},
    {NULL, 1, {1375, 2736}, {0, 0}, {0, 0}},
    {NULL, 2, {3128, 2736}, {0, 0}, {0, 0}},
    {NULL, 2, {3018, 2417}, {0, 0}, {0, 0}},
    {NULL, 2, {1265, 2417}, {0, 0}, {0, 0}},
    {NULL, 2, {1375, 2736}, {0, 0}, {0, 0}},
    {NULL, 1, {998, 1974}, {0, 0}, {0, 0}},
    {NULL, 2, {2899, 1974}, {0, 0}, {0, 0}},
    {NULL, 2, {2786, 1647}, {0, 0}, {0, 0}},
    {NULL, 2, {885, 1647}, {0, 0}, {0, 0}},
    {NULL, 2, {998, 1974}, {0, 0}, {0, 0}},
    {NULL, 1, {2067, 3056}, {0, 0}, {0, 0}},
    {NULL, 2, {2431, 3138}, {0, 0}, {0, 0}},
    {NULL, 4, {2479, 2887}, {2467, 3031}, {0, 0}},
    {NULL, 4, {2488, 2638}, {2491, 2744}, {0, 0}},
    {NULL, 2, {2086, 2515}, {0, 0}, {0, 0}},
    {NULL, 4, {2093, 2777}, {2100, 2630}, {0, 0}},
    {NULL, 4, {2067, 3056}, {2086, 2925}, {0, 0}},
    {NULL, 1, {7186, 3105}, {0, 0}, {0, 0}},
    {NULL, 2, {7563, 3105}, {0, 0}, {0, 0}},
    {NULL, 2, {7228, 2130}, {0, 0}, {0, 0}},
    {NULL, 2, {6851, 2130}, {0, 0}, {0, 0}},
    {NULL, 2, {7186, 3105}, {0, 0}, {0, 0}},
    {NULL, 1, {4742, 1401}, {0, 0}, {0, 0}},
    {NULL, 2, {6085, 1401}, {0, 0}, {0, 0}},
    {NULL, 2, {5975, 1081}, {0, 0}, {0, 0}},
    {NULL, 2, {4984, 1081}, {0, 0}, {0, 0}},
    {NULL, 2, {4401, -614}, {0, 0}, {0, 0}},
    {NULL, 2, {4049, -614}, {0, 0}, {0, 0}},
    {NULL, 2, {4742, 1401}, {0, 0}, {0, 0}},
    {NULL, 1, {6905, 2286}, {0, 0}, {0, 0}},
    {NULL, 2, {8446, 2286}, {0, 0}, {0, 0}},
    {NULL, 2, {8167, 1475}, {0, 0}, {0, 0}},
    {NULL, 2, {6626, 1475}, {0, 0}, {0, 0}},
    {NULL, 2, {6727, 1769}, {0, 0}, {0, 0}},
    {NULL, 2, {7899, 1769}, {0, 0}, {0, 0}},
    {NULL, 2, {7978, 1999}, {0, 0}, {0, 0}},
    {NULL, 2, {6806, 1999}, {0, 0}, {0, 0}},
    {NULL, 2, {6905, 2286}, {0, 0}, {0, 0}},
    {NULL, 1, {6693, 1671}, {0, 0}, {0, 0}},
    {NULL, 2, {7070, 1671}, {0, 0}, {0, 0}},
    {NULL, 2, {6473, -66}, {0, 0}, {0, 0}},
    {NULL, 4, {6462, -217}, {6431, -188}, {0, 0}},
    {NULL, 4, {6632, -246}, {6493, -246}, {0, 0}},
    {NULL, 4, {6804, -246}, {6673, -246}, {0, 0}},
    {NULL, 4, {7079, -246}, {6936, -246}, {0, 0}},
    {NULL, 4, {7280, -246}, {7222, -246}, {0, 0}},
    {NULL, 4, {7451, -176}, {7386, -246}, {0, 0}},
    {NULL, 4, {7615, 156}, {7517, -106}, {0, 0}},
    {NULL, 4, {7735, 86}, {7659, 115}, {0, 0}},
    {NULL, 4, {7878, 41}, {7811, 57}, {0, 0}},
    {NULL, 4, {7594, -450}, {7735, -328}, {0, 0}},
    {NULL, 4, {7217, -573}, {7454, -573}, {0, 0}},
    {NULL, 4, {7077, -573}, {7184, -573}, {0, 0}},
    {NULL, 4, {6844, -573}, {6971, -573}, {0, 0}},
    {NULL, 4, {6614, -573}, {6717, -573}, {0, 0}},
    {NULL, 4, {6479, -573}, {6512, -573}, {0, 0}},
    {NULL, 4, {6081, -479}, {6176, -573}, {0, 0}},
    {NULL, 4, {6099, -57}, {5987, -385}, {0, 0}},
    {NULL, 2, {6693, 1671}, {0, 0}, {0, 0}},
    {NULL, 1, {6249, 2449}, {0, 0}, {0, 0}},
    {NULL, 2, {6520, 2310}, {0, 0}, {0, 0}},
    {NULL, 4, {6242, 2015}, {6376, 2154}, {0, 0}},
    {NULL, 4, {5990, 1769}, {6109, 1876}, {0, 0}},
    {NULL, 2, {5778, 1892}, {0, 0}, {0, 0}},
    {NULL, 4, {6028, 2175}, {5887, 2015}, {0, 0}},
    {NULL, 4, {6249, 2449}, {6169, 2335}, {0, 0}},
    {NULL, 1, {5142, 2351}, {0, 0}, {0, 0}},
    {NULL, 2, {5473, 2408}, {0, 0}, {0, 0}},
    {NULL, 4, {5513, 2146}, {5510, 2302}, {0, 0}},
    {NULL, 4, {5506, 1884}, {5517, 1991}, {0, 0}},
    {NULL, 2, {5144, 1786}, {0, 0}, {0, 0}},
    {NULL, 4, {5164, 2060}, {5159, 1901}, {0, 0}},
    {NULL, 4, {5142, 2351}, {5170, 2220}, {0, 0}},
    {NULL, 1, {5930, 1401}, {0, 0}, {0, 0}},
    {NULL, 2, {6290, 1401}, {0, 0}, {0, 0}},
    {NULL, 2, {5735, -213}, {0, 0}, {0, 0}},
    {NULL, 4, {5626, -446}, {5678, -377}, {0, 0}},
    {NULL, 4, {5467, -565}, {5574, -516}, {0, 0}},
    {NULL, 4, {5201, -610}, {5354, -606}, {0, 0}},
    {NULL, 4, {4811, -614}, {5048, -614}, {0, 0}},
    {NULL, 4, {4828, -455}, {4828, -541}, {0, 0}},
    {NULL, 4, {4814, -295}, {4829, -369}, {0, 0}},
    {NULL, 4, {5098, -303}, {4967, -303}, {0, 0}},
    {NULL, 4, {5273, -295}, {5229, -303}, {0, 0}},
    {NULL, 4, {5372, -221}, {5347, -295}, {0, 0}},
    {NULL, 2, {5930, 1401}, {0, 0}, {0, 0}},
    {NULL, 1, {7254, 2802}, {0, 0}, {0, 0}},
    {NULL, 2, {8680, 2802}, {0, 0}, {0, 0}},
    {NULL, 2, {8579, 2507}, {0, 0}, {0, 0}},
    {NULL, 2, {7153, 2507}, {0, 0}, {0, 0}},
    {NULL, 2, {7254, 2802}, {0, 0}, {0, 0}},
    {NULL, 1, {6714, 1253}, {0, 0}, {0, 0}},
    {NULL, 2, {7992, 1253}, {0, 0}, {0, 0}},
    {NULL, 2, {7899, 983}, {0, 0}, {0, 0}},
    {NULL, 2, {6621, 983}, {0, 0}, {0, 0}},
    {NULL, 2, {6714, 1253}, {0, 0}, {0, 0}},
    {NULL, 1, {6545, 762}, {0, 0}, {0, 0}},
    {NULL, 2, {7831, 762}, {0, 0}, {0, 0}},
    {NULL, 2, {7738, 492}, {0, 0}, {0, 0}},
    {NULL, 2, {6452, 492}, {0, 0}, {0, 0}},
    {NULL, 2, {6545, 762}, {0, 0}, {0, 0}},
    {NULL, 1, {6376, 270}, {0, 0}, {0, 0}},
    {NULL, 2, {7548, 270}, {0, 0}, {0, 0}},
    {NULL, 2, {7455, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6283, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6376, 270}, {0, 0}, {0, 0}},
    {NULL, 1, {4536, 303}, {0, 0}, {0, 0}},
    {NULL, 2, {5732, 303}, {0, 0}, {0, 0}},
    {NULL, 2, {5631, 8}, {0, 0}, {0, 0}},
    {NULL, 2, {4435, 8}, {0, 0}, {0, 0}},
    {NULL, 2, {4536, 303}, {0, 0}, {0, 0}},
    {NULL, 1, {4722, 844}, {0, 0}, {0, 0}},
    {NULL, 2, {5918, 844}, {0, 0}, {0, 0}},
    {NULL, 2, {5819, 557}, {0, 0}, {0, 0}},
    {NULL, 2, {4623, 557}, {0, 0}, {0, 0}},
    {NULL, 2, {4722, 844}, {0, 0}, {0, 0}},
    {NULL, 1, {5072, 2720}, {0, 0}, {0, 0}},
    {NULL, 2, {6874, 2720}, {0, 0}, {0, 0}},
    {NULL, 2, {6761, 2392}, {0, 0}, {0, 0}},
    {NULL, 2, {4959, 2392}, {0, 0}, {0, 0}},
    {NULL, 2, {5072, 2720}, {0, 0}, {0, 0}},
    {NULL, 1, {4684, 1950}, {0, 0}, {0, 0}},
    {NULL, 2, {6650, 1950}, {0, 0}, {0, 0}},
    {NULL, 2, {6540, 1630}, {0, 0}, {0, 0}},
    {NULL, 2, {4574, 1630}, {0, 0}, {0, 0}},
    {NULL, 2, {4684, 1950}, {0, 0}, {0, 0}},
    {NULL, 1, {5766, 3023}, {0, 0}, {0, 0}},
    {NULL, 2, {6146, 3105}, {0, 0}, {0, 0}},
    {NULL, 4, {6200, 2855}, {6184, 2998}, {0, 0}},
    {NULL, 4, {6215, 2613}, {6216, 2712}, {0, 0}},
    {NULL, 2, {5807, 2499}, {0, 0}, {0, 0}},
    {NULL, 4, {5795, 2752}, {5810, 2605}, {0, 0}},
    {NULL, 4, {5766, 3023}, {5781, 2900}, {0, 0}},
    {NULL, 0, {0, 0}, {0, 0}, {0, 0}},
};

static void freetype_outline_event_cb(lv_event_t * e);

#if OPTION_GENERATE_VECTOR_OPS_STRING
static void vegravis_generate_vector_ops_string(lv_freetype_outline_event_param_t * param, char * buf,
                                                uint32_t buf_len);
#endif

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_freetype_bitmap_rendering_test(void)
{
#if LV_USE_FREETYPE
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);
    lv_font_t * font_normal = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    lv_font_t * font_normal_small = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                            12,
                                                            LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font_italic || !font_normal || !font_normal_small) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    /*Create style with the new font*/
    static lv_style_t style_italic;
    lv_style_init(&style_italic);
    lv_style_set_text_font(&style_italic, font_italic);
    lv_style_set_text_align(&style_italic, LV_TEXT_ALIGN_CENTER);

    static lv_style_t style_normal;
    lv_style_init(&style_normal);
    lv_style_set_text_font(&style_normal, font_normal);
    lv_style_set_text_align(&style_normal, LV_TEXT_ALIGN_CENTER);

    static lv_style_t style_normal_small;
    lv_style_init(&style_normal_small);
    lv_style_set_text_font(&style_normal_small, font_normal_small);

    /*Create a label with the new style*/
    lv_obj_t * label0 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label0, &style_italic, 0);
    lv_obj_set_width(label0, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label0, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN);
    lv_obj_align(label0,  LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label1, &style_normal, 0);
    lv_obj_set_width(label1, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label1, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN);
    lv_obj_align_to(label1, label0, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label2, &style_normal_small, 0);
    lv_obj_set_width(label2, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label2, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT("1");

#else
    TEST_PASS();
#endif
}

void test_freetype_outline_rendering_test(void)
{
#if LV_USE_FREETYPE
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);

    if(!font_italic) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    /*Setup outline event for generating outline drawing data*/
    lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, NULL);

    lv_font_glyph_dsc_t g;

    lv_font_get_glyph_dsc(font_italic, &g, 0x9F98, '\0');

    const lv_ll_t * outline_data;
    outline_data = (lv_ll_t *)lv_font_get_glyph_bitmap(&g, 0x9F98, NULL);

    uint32_t i = 0;
    lv_freetype_outline_event_param_t * param;
    _LV_LL_READ(outline_data, param) {
#if OPTION_GENERATE_OUTLINE_DATA
        /*FOR Generate outline data*/
#if OPTION_GENERATE_VECTOR_OPS_STRING
        char buf[1024];
        vegravis_generate_vector_ops_string(param, buf, sizeof(buf));
        TEST_PRINTF("%s", buf);
#else
        TEST_PRINTF("{NULL, %d, {%d, %d}, {%d, %d}, {%d, %d}}, ", param->type, param->to.x, param->to.y, param->control1.x,
                    param->control1.y, param->control2.x, param->control2.y);
#endif
#endif
        TEST_ASSERT_EQUAL(param->type, outline_data_U9F98[i].type);
        TEST_ASSERT_EQUAL(param->to.x, outline_data_U9F98[i].to.x);
        TEST_ASSERT_EQUAL(param->to.y, outline_data_U9F98[i].to.y);
        TEST_ASSERT_EQUAL(param->control1.x, outline_data_U9F98[i].control1.x);
        TEST_ASSERT_EQUAL(param->control1.y, outline_data_U9F98[i].control1.y);
        TEST_ASSERT_EQUAL(param->control2.x, outline_data_U9F98[i].control2.x);
        TEST_ASSERT_EQUAL(param->control2.y, outline_data_U9F98[i].control2.y);
        i++;
    }

    font_italic->release_glyph(font_italic, &g);

    lv_freetype_font_delete(font_italic);
#else
    TEST_PASS();
#endif
}

static void freetype_outline_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);
    switch(code) {
        case LV_EVENT_CREATE:
            param->outline = lv_malloc_zeroed(sizeof(lv_ll_t));
            _lv_ll_init(param->outline, sizeof(lv_freetype_outline_event_param_t));
            break;
        case LV_EVENT_DELETE:
            _lv_ll_clear(param->outline);
            lv_free(param->outline);
            break;
        case LV_EVENT_INSERT: {
                void * entry = _lv_ll_ins_tail(param->outline);
                lv_memcpy(entry, param, sizeof(lv_freetype_outline_event_param_t));
                break;
            }
        default:
            LV_LOG_WARN("unknown event code: %d", code);
            break;
    }
}

#if OPTION_GENERATE_VECTOR_OPS_STRING
static void vegravis_generate_vector_ops_string(lv_freetype_outline_event_param_t * param, char * buf, uint32_t buf_len)
{
    float x, y, p1x, p1y, p2x, p2y;

    x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
    y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
    p1x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.x);
    p1y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.y);
    p2x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.x);
    p2y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.y);

    switch(param->type) {
        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_snprintf(buf, buf_len, "MOVE, %f, %f, ", x, x);
            break;
        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_snprintf(buf, buf_len, "LINE, %f, %f, ", x, y);
            break;
        case LV_FREETYPE_OUTLINE_CONIC_TO:
            lv_snprintf(buf, buf_len, "QUAD, %f, %f, %f, %f, ", p1x, p1y, x, y);
            break;
        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            lv_snprintf(buf, buf_len, "CUBI, %f, %f, %f, %f, %f, %f, ", p1x, p1y, p2x, p2y, x, y);
            break;
        case LV_FREETYPE_OUTLINE_END:
            lv_snprintf(buf, buf_len, "END, ");
            break;
        default:
            break;
    }
}
#endif

#endif
