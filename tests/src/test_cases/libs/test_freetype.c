#if LV_BUILD_TEST
#include "lvgl.h"
#include "src/libs/freetype/lv_freetype_private.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
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
    {NULL, 1, {5888, 6889}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6445, 6889}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6238, 5915}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5681, 5915}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5888, 6889}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2069, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4576, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4508, 4825}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2525, 4825}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2178, 3195}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1654, 3195}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2069, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5714, 6070}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8573, 6070}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8401, 5259}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5542, 5259}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5606, 5562}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7916, 5562}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7962, 5775}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5652, 5775}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5714, 6070}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5580, 5439}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6137, 5439}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5786, 3785}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5832, 3600}, {5756, 3645}, {0, 0}, {0, 0}},
    {NULL, 4, {6195, 3555}, {5909, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {6461, 3555}, {6261, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {6904, 3555}, {6662, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {7359, 3555}, {7146, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {7670, 3555}, {7572, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {7880, 3579}, {7801, 3555}, {0, 0}, {0, 0}},
    {NULL, 4, {8010, 3690}, {7959, 3604}, {0, 0}, {0, 0}},
    {NULL, 4, {8107, 3957}, {8061, 3777}, {0, 0}, {0, 0}},
    {NULL, 4, {8305, 3887}, {8188, 3916}, {0, 0}, {0, 0}},
    {NULL, 4, {8526, 3842}, {8422, 3858}, {0, 0}, {0, 0}},
    {NULL, 4, {8238, 3359}, {8416, 3482}, {0, 0}, {0, 0}},
    {NULL, 4, {7668, 3236}, {8061, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {7446, 3236}, {7610, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {7074, 3236}, {7283, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {6656, 3236}, {6865, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {6287, 3236}, {6447, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {6078, 3236}, {6127, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {5345, 3334}, {5538, 3236}, {0, 0}, {0, 0}},
    {NULL, 4, {5230, 3793}, {5153, 3432}, {0, 0}, {0, 0}},
    {NULL, 2, {5580, 5439}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4305, 6185}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4756, 6038}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {4494, 5726}, {4616, 5874}, {0, 0}, {0, 0}},
    {NULL, 4, {4258, 5464}, {4373, 5579}, {0, 0}, {0, 0}},
    {NULL, 2, {3901, 5595}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {4120, 5894}, {4002, 5726}, {0, 0}, {0, 0}},
    {NULL, 4, {4305, 6185}, {4238, 6062}, {0, 0}, {0, 0}},
    {NULL, 1, {2506, 6046}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2941, 6169}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {3051, 5894}, {3014, 6046}, {0, 0}, {0, 0}},
    {NULL, 4, {3088, 5628}, {3089, 5743}, {0, 0}, {0, 0}},
    {NULL, 2, {2610, 5497}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {2584, 5763}, {2616, 5603}, {0, 0}, {0, 0}},
    {NULL, 4, {2506, 6046}, {2553, 5923}, {0, 0}, {0, 0}},
    {NULL, 1, {4322, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4846, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4515, 3588}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {4421, 3362}, {4481, 3432}, {0, 0}, {0, 0}},
    {NULL, 4, {4238, 3252}, {4362, 3293}, {0, 0}, {0, 0}},
    {NULL, 4, {3912, 3203}, {4115, 3211}, {0, 0}, {0, 0}},
    {NULL, 4, {3423, 3195}, {3710, 3195}, {0, 0}, {0, 0}},
    {NULL, 4, {3409, 3359}, {3433, 3277}, {0, 0}, {0, 0}},
    {NULL, 4, {3358, 3506}, {3385, 3441}, {0, 0}, {0, 0}},
    {NULL, 4, {3677, 3506}, {3530, 3506}, {0, 0}, {0, 0}},
    {NULL, 4, {3874, 3506}, {3825, 3506}, {0, 0}, {0, 0}},
    {NULL, 4, {3956, 3522}, {3932, 3506}, {0, 0}, {0, 0}},
    {NULL, 4, {3991, 3588}, {3980, 3539}, {0, 0}, {0, 0}},
    {NULL, 2, {4322, 5145}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6071, 6595}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8988, 6595}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8923, 6291}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6006, 6291}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6071, 6595}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5732, 5038}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8190, 5038}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8132, 4768}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5674, 4768}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5732, 5038}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5646, 4555}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8046, 4555}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7987, 4276}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5587, 4276}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5646, 4555}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5523, 4055}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8046, 4055}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7989, 3785}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5466, 3785}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5523, 4055}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2081, 4047}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4350, 4047}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4288, 3752}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2019, 3752}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2081, 4047}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2196, 4588}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4465, 4588}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4402, 4293}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2133, 4293}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2196, 4588}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2066, 6447}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5351, 6447}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5284, 6128}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1999, 6128}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2066, 6447}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {1689, 5710}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5433, 5710}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5365, 5390}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1621, 5390}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1689, 5710}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {3383, 6824}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3903, 6914}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {4030, 6607}, {3973, 6783}, {0, 0}, {0, 0}},
    {NULL, 4, {4110, 6308}, {4087, 6431}, {0, 0}, {0, 0}},
    {NULL, 2, {3568, 6185}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {3507, 6500}, {3554, 6316}, {0, 0}, {0, 0}},
    {NULL, 4, {3383, 6824}, {3461, 6685}, {0, 0}, {0, 0}},
    {NULL, 1, {3008, 3129}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3377, 3129}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3168, 2146}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2799, 2146}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3008, 3129}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {860, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2163, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2095, 1106}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1136, 1106}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {776, -590}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {432, -590}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {860, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2834, 2310}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4317, 2310}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4143, 1491}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2660, 1491}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2723, 1786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3853, 1786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3901, 2015}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2771, 2015}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2834, 2310}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2703, 1696}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3063, 1696}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2698, -25}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {2705, -184}, {2672, -147}, {0, 0}, {0, 0}},
    {NULL, 4, {2869, -221}, {2738, -221}, {0, 0}, {0, 0}},
    {NULL, 4, {3045, -221}, {2919, -221}, {0, 0}, {0, 0}},
    {NULL, 4, {3311, -221}, {3172, -221}, {0, 0}, {0, 0}},
    {NULL, 4, {3500, -221}, {3451, -221}, {0, 0}, {0, 0}},
    {NULL, 4, {3659, -147}, {3607, -221}, {0, 0}, {0, 0}},
    {NULL, 4, {3776, 188}, {3711, -74}, {0, 0}, {0, 0}},
    {NULL, 4, {3899, 114}, {3824, 147}, {0, 0}, {0, 0}},
    {NULL, 4, {4044, 66}, {3974, 82}, {0, 0}, {0, 0}},
    {NULL, 4, {3826, -418}, {3951, -295}, {0, 0}, {0, 0}},
    {NULL, 4, {3473, -541}, {3702, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {3346, -541}, {3449, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {3121, -541}, {3244, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {2899, -541}, {2998, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {2769, -541}, {2801, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {2366, -446}, {2465, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {2338, -25}, {2268, -352}, {0, 0}, {0, 0}},
    {NULL, 2, {2703, 1696}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2189, 2474}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2462, 2335}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {2225, 2031}, {2337, 2171}, {0, 0}, {0, 0}},
    {NULL, 4, {2019, 1794}, {2114, 1892}, {0, 0}, {0, 0}},
    {NULL, 2, {1800, 1917}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {2003, 2199}, {1892, 2040}, {0, 0}, {0, 0}},
    {NULL, 4, {2189, 2474}, {2115, 2359}, {0, 0}, {0, 0}},
    {NULL, 1, {1134, 2367}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1451, 2433}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {1517, 2166}, {1492, 2318}, {0, 0}, {0, 0}},
    {NULL, 4, {1543, 1901}, {1542, 2015}, {0, 0}, {0, 0}},
    {NULL, 2, {1204, 1810}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {1196, 2080}, {1212, 1925}, {0, 0}, {0, 0}},
    {NULL, 4, {1134, 2367}, {1180, 2236}, {0, 0}, {0, 0}},
    {NULL, 1, {2015, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2367, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2022, -197}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {1941, -426}, {1989, -352}, {0, 0}, {0, 0}},
    {NULL, 4, {1794, -541}, {1893, -500}, {0, 0}, {0, 0}},
    {NULL, 4, {1551, -586}, {1703, -582}, {0, 0}, {0, 0}},
    {NULL, 4, {1169, -590}, {1399, -590}, {0, 0}, {0, 0}},
    {NULL, 4, {1166, -430}, {1176, -516}, {0, 0}, {0, 0}},
    {NULL, 4, {1139, -270}, {1156, -344}, {0, 0}, {0, 0}},
    {NULL, 4, {1415, -279}, {1293, -279}, {0, 0}, {0, 0}},
    {NULL, 4, {1579, -279}, {1538, -279}, {0, 0}, {0, 0}},
    {NULL, 4, {1670, -197}, {1653, -279}, {0, 0}, {0, 0}},
    {NULL, 2, {2015, 1425}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {3108, 2826}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4492, 2826}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4429, 2531}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3045, 2531}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3108, 2826}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2779, 1278}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4016, 1278}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3958, 1008}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2721, 1008}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2779, 1278}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2674, 786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3919, 786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3862, 516}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2617, 516}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2674, 786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {2570, 295}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3692, 295}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {3632, 16}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2510, 16}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2570, 295}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {791, 328}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1946, 328}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1883, 33}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {728, 33}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {791, 328}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {905, 868}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2060, 868}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1998, 573}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {843, 573}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {905, 868}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {1016, 2736}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2769, 2736}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2701, 2417}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {948, 2417}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1016, 2736}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {739, 1974}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2640, 1974}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2570, 1647}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {669, 1647}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {739, 1974}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {1666, 3056}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {2019, 3138}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {2100, 2887}, {2069, 3031}, {0, 0}, {0, 0}},
    {NULL, 4, {2142, 2638}, {2131, 2744}, {0, 0}, {0, 0}},
    {NULL, 2, {1756, 2515}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {1729, 2777}, {1755, 2630}, {0, 0}, {0, 0}},
    {NULL, 4, {1666, 3056}, {1703, 2925}, {0, 0}, {0, 0}},
    {NULL, 1, {6779, 3105}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7156, 3105}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6949, 2130}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6572, 2130}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6779, 3105}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4558, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5901, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5833, 1081}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4842, 1081}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4481, -614}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4129, -614}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4558, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6605, 2286}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8146, 2286}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7974, 1475}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6433, 1475}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6495, 1769}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7667, 1769}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7716, 1999}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6544, 1999}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6605, 2286}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6474, 1671}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6851, 1671}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6482, -66}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {6491, -217}, {6456, -188}, {0, 0}, {0, 0}},
    {NULL, 4, {6665, -246}, {6526, -246}, {0, 0}, {0, 0}},
    {NULL, 4, {6837, -246}, {6706, -246}, {0, 0}, {0, 0}},
    {NULL, 4, {7112, -246}, {6969, -246}, {0, 0}, {0, 0}},
    {NULL, 4, {7313, -246}, {7255, -246}, {0, 0}, {0, 0}},
    {NULL, 4, {7474, -176}, {7419, -246}, {0, 0}, {0, 0}},
    {NULL, 4, {7594, 156}, {7530, -106}, {0, 0}, {0, 0}},
    {NULL, 4, {7723, 86}, {7643, 115}, {0, 0}, {0, 0}},
    {NULL, 4, {7873, 41}, {7803, 57}, {0, 0}, {0, 0}},
    {NULL, 4, {7653, -450}, {7778, -328}, {0, 0}, {0, 0}},
    {NULL, 4, {7292, -573}, {7529, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {7152, -573}, {7259, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {6919, -573}, {7046, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {6689, -573}, {6792, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {6554, -573}, {6587, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {6144, -479}, {6251, -573}, {0, 0}, {0, 0}},
    {NULL, 4, {6107, -57}, {6037, -385}, {0, 0}, {0, 0}},
    {NULL, 2, {6474, 1671}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5928, 2449}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6217, 2310}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5978, 2015}, {6094, 2154}, {0, 0}, {0, 0}},
    {NULL, 4, {5758, 1769}, {5863, 1876}, {0, 0}, {0, 0}},
    {NULL, 2, {5530, 1892}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5742, 2175}, {5622, 2015}, {0, 0}, {0, 0}},
    {NULL, 4, {5928, 2449}, {5862, 2335}, {0, 0}, {0, 0}},
    {NULL, 1, {4834, 2351}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5157, 2408}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5232, 2146}, {5208, 2302}, {0, 0}, {0, 0}},
    {NULL, 4, {5258, 1884}, {5256, 1991}, {0, 0}, {0, 0}},
    {NULL, 2, {4910, 1786}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {4894, 2060}, {4910, 1901}, {0, 0}, {0, 0}},
    {NULL, 4, {4834, 2351}, {4879, 2220}, {0, 0}, {0, 0}},
    {NULL, 1, {5746, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6106, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5763, -213}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5684, -446}, {5728, -377}, {0, 0}, {0, 0}},
    {NULL, 4, {5541, -565}, {5641, -516}, {0, 0}, {0, 0}},
    {NULL, 4, {5280, -610}, {5433, -606}, {0, 0}, {0, 0}},
    {NULL, 4, {4891, -614}, {5128, -614}, {0, 0}, {0, 0}},
    {NULL, 4, {4888, -455}, {4899, -541}, {0, 0}, {0, 0}},
    {NULL, 4, {4852, -295}, {4878, -369}, {0, 0}, {0, 0}},
    {NULL, 4, {5138, -303}, {5007, -303}, {0, 0}, {0, 0}},
    {NULL, 4, {5311, -295}, {5269, -303}, {0, 0}, {0, 0}},
    {NULL, 4, {5401, -221}, {5385, -295}, {0, 0}, {0, 0}},
    {NULL, 2, {5746, 1401}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6887, 2802}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8313, 2802}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {8250, 2507}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6824, 2507}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6887, 2802}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6549, 1253}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7827, 1253}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7770, 983}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6492, 983}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6549, 1253}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6445, 762}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7731, 762}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7674, 492}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6388, 492}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6445, 762}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {6340, 270}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7512, 270}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {7455, 0}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6283, 0}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6340, 270}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4496, 303}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5692, 303}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5630, 8}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4434, 8}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4496, 303}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4611, 844}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5807, 844}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5746, 557}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4550, 557}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4611, 844}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4715, 2720}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6517, 2720}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6447, 2392}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4645, 2392}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4715, 2720}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {4428, 1950}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6394, 1950}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {6326, 1630}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4360, 1630}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {4428, 1950}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 1, {5370, 3023}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {5739, 3105}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5825, 2855}, {5790, 2998}, {0, 0}, {0, 0}},
    {NULL, 4, {5872, 2613}, {5860, 2712}, {0, 0}, {0, 0}},
    {NULL, 2, {5479, 2499}, {0, 0}, {0, 0}, {0, 0}},
    {NULL, 4, {5434, 2752}, {5469, 2605}, {0, 0}, {0, 0}},
    {NULL, 4, {5370, 3023}, {5400, 2900}, {0, 0}, {0, 0}},
    {NULL, 0, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
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
    lv_obj_clean(lv_screen_active());
}

static void test_freetype_with_render_mode(lv_freetype_font_render_mode_t render_mode, const char * screenshot_name)
{
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      render_mode,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);
    TEST_ASSERT_NOT_NULL(font_italic);
    lv_font_t * font_normal = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      render_mode,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_normal);
    lv_font_t * font_normal_small = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                            render_mode,
                                                            12,
                                                            LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_normal_small);

    /* Emoji is only supported in bitmap mode */
    lv_font_t * font_emoji = lv_freetype_font_create("../examples/libs/freetype/NotoColorEmoji-32.subset.ttf",
                                                     LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                     12,
                                                     LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_emoji);

    lv_font_t * font_path_error = lv_freetype_font_create("ERROR_PATH", render_mode, 24,
                                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    font_path_error = lv_freetype_font_create("", render_mode, 24, LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    font_path_error = lv_freetype_font_create(NULL, render_mode, 24, LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    lv_font_t * font_size_error = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                          render_mode,
                                                          0,
                                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_size_error);

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

    static lv_style_t style_normal_emoji;
    lv_style_init(&style_normal_emoji);
    lv_style_set_text_font(&style_normal_emoji, font_emoji);
    lv_style_set_text_align(&style_normal_emoji, LV_TEXT_ALIGN_CENTER);

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

    /* test emoji rendering
     * emoji font does not contain normal characters, use fallback to render them */
    font_emoji->fallback = font_normal;

    lv_obj_t * label_emoji = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_emoji, &style_normal_emoji, 0);
    lv_obj_set_width(label_emoji, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label_emoji, "FreeType Emoji test: 😀");
    lv_obj_align_to(label_emoji, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot_name);

    lv_obj_clean(lv_screen_active());
    lv_style_reset(&style_italic);
    lv_style_reset(&style_normal);
    lv_style_reset(&style_normal_small);
    lv_style_reset(&style_normal_emoji);
    lv_freetype_font_delete(font_italic);
    lv_freetype_font_delete(font_normal);
    lv_freetype_font_delete(font_normal_small);
    lv_freetype_font_delete(font_emoji);
}

void test_freetype_render_bitmap(void)
{
    test_freetype_with_render_mode(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, "libs/freetype_render_bitmap" EXT_NAME);
}

void test_freetype_render_outline(void)
{
#if LV_USE_DRAW_VG_LITE
    /* VG-Lite support rendering outline */
    test_freetype_with_render_mode(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, "libs/freetype_render_outline.png");
    LV_UNUSED(outline_data_U9F98);
    LV_UNUSED(freetype_outline_event_cb);
#else
    /* Outline rendering not supported, compare outline data only */
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);

    TEST_ASSERT_NOT_NULL(font_italic);

    /*Setup outline event for generating outline drawing data*/
    lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, NULL);

    lv_font_glyph_dsc_t g;

    lv_font_get_glyph_dsc(font_italic, &g, 0x9F98, '\0');

    const lv_ll_t * outline_data;
    outline_data = (lv_ll_t *) lv_font_get_glyph_bitmap(&g, NULL);

    uint32_t i = 0;
    lv_freetype_outline_event_param_t * param;
    LV_LL_READ(outline_data, param) {
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
#endif
}

static void freetype_outline_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);
    switch(code) {
        case LV_EVENT_CREATE:
            param->outline = lv_malloc_zeroed(sizeof(lv_ll_t));
            lv_ll_init(param->outline, sizeof(lv_freetype_outline_event_param_t));
            break;
        case LV_EVENT_DELETE:
            lv_ll_clear(param->outline);
            lv_free(param->outline);
            break;
        case LV_EVENT_INSERT: {
                void * entry = lv_ll_ins_tail(param->outline);
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

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freetype_render_bitmap(void)
{
}

void test_freetype_render_outline(void)
{
}

#endif /*LV_USE_FREETYPE*/

#endif
