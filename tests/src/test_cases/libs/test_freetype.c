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
    {NULL, 1, {1273, 1292}, {0, 0}, {0, 0}},
    {NULL, 2, {1378, 1292}, {0, 0}, {0, 0}},
    {NULL, 2, {1315, 1109}, {0, 0}, {0, 0}},
    {NULL, 2, {1210, 1109}, {0, 0}, {0, 0}},
    {NULL, 2, {1273, 1292}, {0, 0}, {0, 0}},
    {NULL, 1, {515, 965}, {0, 0}, {0, 0}},
    {NULL, 2, {985, 965}, {0, 0}, {0, 0}},
    {NULL, 2, {964, 905}, {0, 0}, {0, 0}},
    {NULL, 2, {592, 905}, {0, 0}, {0, 0}},
    {NULL, 2, {487, 599}, {0, 0}, {0, 0}},
    {NULL, 2, {389, 599}, {0, 0}, {0, 0}},
    {NULL, 2, {515, 965}, {0, 0}, {0, 0}},
    {NULL, 1, {1220, 1138}, {0, 0}, {0, 0}},
    {NULL, 2, {1756, 1138}, {0, 0}, {0, 0}},
    {NULL, 2, {1704, 986}, {0, 0}, {0, 0}},
    {NULL, 2, {1168, 986}, {0, 0}, {0, 0}},
    {NULL, 2, {1188, 1043}, {0, 0}, {0, 0}},
    {NULL, 2, {1622, 1043}, {0, 0}, {0, 0}},
    {NULL, 2, {1635, 1083}, {0, 0}, {0, 0}},
    {NULL, 2, {1201, 1083}, {0, 0}, {0, 0}},
    {NULL, 2, {1220, 1138}, {0, 0}, {0, 0}},
    {NULL, 1, {1180, 1020}, {0, 0}, {0, 0}},
    {NULL, 2, {1285, 1020}, {0, 0}, {0, 0}},
    {NULL, 2, {1178, 710}, {0, 0}, {0, 0}},
    {NULL, 4, {1182, 675}, {1169, 684}, {0, 0}},
    {NULL, 4, {1249, 667}, {1195, 667}, {0, 0}},
    {NULL, 4, {1298, 667}, {1261, 667}, {0, 0}},
    {NULL, 4, {1381, 667}, {1336, 667}, {0, 0}},
    {NULL, 4, {1467, 667}, {1427, 667}, {0, 0}},
    {NULL, 4, {1525, 667}, {1507, 667}, {0, 0}},
    {NULL, 4, {1565, 671}, {1550, 667}, {0, 0}},
    {NULL, 4, {1592, 692}, {1581, 676}, {0, 0}},
    {NULL, 4, {1617, 742}, {1604, 708}, {0, 0}},
    {NULL, 4, {1652, 728}, {1631, 734}, {0, 0}},
    {NULL, 4, {1693, 720}, {1674, 723}, {0, 0}},
    {NULL, 4, {1627, 630}, {1663, 653}, {0, 0}},
    {NULL, 4, {1518, 607}, {1591, 607}, {0, 0}},
    {NULL, 4, {1476, 607}, {1507, 607}, {0, 0}},
    {NULL, 4, {1406, 607}, {1445, 607}, {0, 0}},
    {NULL, 4, {1328, 607}, {1367, 607}, {0, 0}},
    {NULL, 4, {1259, 607}, {1289, 607}, {0, 0}},
    {NULL, 4, {1220, 607}, {1229, 607}, {0, 0}},
    {NULL, 4, {1084, 625}, {1118, 607}, {0, 0}},
    {NULL, 4, {1073, 711}, {1050, 644}, {0, 0}},
    {NULL, 2, {1180, 1020}, {0, 0}, {0, 0}},
    {NULL, 1, {960, 1160}, {0, 0}, {0, 0}},
    {NULL, 2, {1040, 1132}, {0, 0}, {0, 0}},
    {NULL, 4, {983, 1073}, {1009, 1101}, {0, 0}},
    {NULL, 4, {933, 1025}, {958, 1046}, {0, 0}},
    {NULL, 2, {869, 1049}, {0, 0}, {0, 0}},
    {NULL, 4, {917, 1105}, {891, 1074}, {0, 0}},
    {NULL, 4, {960, 1160}, {944, 1137}, {0, 0}},
    {NULL, 1, {619, 1134}, {0, 0}, {0, 0}},
    {NULL, 2, {704, 1157}, {0, 0}, {0, 0}},
    {NULL, 4, {717, 1105}, {714, 1134}, {0, 0}},
    {NULL, 4, {718, 1055}, {720, 1077}, {0, 0}},
    {NULL, 2, {624, 1031}, {0, 0}, {0, 0}},
    {NULL, 4, {626, 1081}, {628, 1051}, {0, 0}},
    {NULL, 4, {619, 1134}, {625, 1111}, {0, 0}},
    {NULL, 1, {937, 965}, {0, 0}, {0, 0}},
    {NULL, 2, {1035, 965}, {0, 0}, {0, 0}},
    {NULL, 2, {934, 673}, {0, 0}, {0, 0}},
    {NULL, 4, {911, 630}, {924, 644}, {0, 0}},
    {NULL, 4, {875, 610}, {899, 617}, {0, 0}},
    {NULL, 4, {812, 600}, {851, 602}, {0, 0}},
    {NULL, 4, {721, 599}, {774, 599}, {0, 0}},
    {NULL, 4, {722, 629}, {724, 614}, {0, 0}},
    {NULL, 4, {716, 657}, {720, 645}, {0, 0}},
    {NULL, 4, {776, 657}, {748, 657}, {0, 0}},
    {NULL, 4, {813, 657}, {804, 657}, {0, 0}},
    {NULL, 4, {828, 660}, {824, 657}, {0, 0}},
    {NULL, 4, {836, 673}, {833, 664}, {0, 0}},
    {NULL, 2, {937, 965}, {0, 0}, {0, 0}},
    {NULL, 1, {1301, 1236}, {0, 0}, {0, 0}},
    {NULL, 2, {1847, 1236}, {0, 0}, {0, 0}},
    {NULL, 2, {1828, 1180}, {0, 0}, {0, 0}},
    {NULL, 2, {1282, 1180}, {0, 0}, {0, 0}},
    {NULL, 2, {1301, 1236}, {0, 0}, {0, 0}},
    {NULL, 1, {1199, 945}, {0, 0}, {0, 0}},
    {NULL, 2, {1660, 945}, {0, 0}, {0, 0}},
    {NULL, 2, {1642, 894}, {0, 0}, {0, 0}},
    {NULL, 2, {1181, 894}, {0, 0}, {0, 0}},
    {NULL, 2, {1199, 945}, {0, 0}, {0, 0}},
    {NULL, 1, {1171, 854}, {0, 0}, {0, 0}},
    {NULL, 2, {1621, 854}, {0, 0}, {0, 0}},
    {NULL, 2, {1603, 802}, {0, 0}, {0, 0}},
    {NULL, 2, {1153, 802}, {0, 0}, {0, 0}},
    {NULL, 2, {1171, 854}, {0, 0}, {0, 0}},
    {NULL, 1, {1135, 760}, {0, 0}, {0, 0}},
    {NULL, 2, {1608, 760}, {0, 0}, {0, 0}},
    {NULL, 2, {1591, 710}, {0, 0}, {0, 0}},
    {NULL, 2, {1118, 710}, {0, 0}, {0, 0}},
    {NULL, 2, {1135, 760}, {0, 0}, {0, 0}},
    {NULL, 1, {490, 759}, {0, 0}, {0, 0}},
    {NULL, 2, {915, 759}, {0, 0}, {0, 0}},
    {NULL, 2, {896, 703}, {0, 0}, {0, 0}},
    {NULL, 2, {471, 703}, {0, 0}, {0, 0}},
    {NULL, 2, {490, 759}, {0, 0}, {0, 0}},
    {NULL, 1, {525, 860}, {0, 0}, {0, 0}},
    {NULL, 2, {950, 860}, {0, 0}, {0, 0}},
    {NULL, 2, {931, 805}, {0, 0}, {0, 0}},
    {NULL, 2, {506, 805}, {0, 0}, {0, 0}},
    {NULL, 2, {525, 860}, {0, 0}, {0, 0}},
    {NULL, 1, {547, 1209}, {0, 0}, {0, 0}},
    {NULL, 2, {1162, 1209}, {0, 0}, {0, 0}},
    {NULL, 2, {1141, 1149}, {0, 0}, {0, 0}},
    {NULL, 2, {526, 1149}, {0, 0}, {0, 0}},
    {NULL, 2, {547, 1209}, {0, 0}, {0, 0}},
    {NULL, 1, {457, 1071}, {0, 0}, {0, 0}},
    {NULL, 2, {1159, 1071}, {0, 0}, {0, 0}},
    {NULL, 2, {1139, 1011}, {0, 0}, {0, 0}},
    {NULL, 2, {437, 1011}, {0, 0}, {0, 0}},
    {NULL, 2, {457, 1071}, {0, 0}, {0, 0}},
    {NULL, 1, {802, 1279}, {0, 0}, {0, 0}},
    {NULL, 2, {902, 1296}, {0, 0}, {0, 0}},
    {NULL, 4, {918, 1239}, {912, 1272}, {0, 0}},
    {NULL, 4, {926, 1183}, {925, 1206}, {0, 0}},
    {NULL, 2, {821, 1160}, {0, 0}, {0, 0}},
    {NULL, 4, {817, 1218}, {822, 1184}, {0, 0}},
    {NULL, 4, {802, 1279}, {813, 1253}, {0, 0}},
    {NULL, 1, {641, 587}, {0, 0}, {0, 0}},
    {NULL, 2, {710, 587}, {0, 0}, {0, 0}},
    {NULL, 2, {646, 402}, {0, 0}, {0, 0}},
    {NULL, 2, {577, 402}, {0, 0}, {0, 0}},
    {NULL, 2, {641, 587}, {0, 0}, {0, 0}},
    {NULL, 1, {196, 267}, {0, 0}, {0, 0}},
    {NULL, 2, {441, 267}, {0, 0}, {0, 0}},
    {NULL, 2, {420, 207}, {0, 0}, {0, 0}},
    {NULL, 2, {240, 207}, {0, 0}, {0, 0}},
    {NULL, 2, {131, -111}, {0, 0}, {0, 0}},
    {NULL, 2, {66, -111}, {0, 0}, {0, 0}},
    {NULL, 2, {196, 267}, {0, 0}, {0, 0}},
    {NULL, 1, {588, 433}, {0, 0}, {0, 0}},
    {NULL, 2, {866, 433}, {0, 0}, {0, 0}},
    {NULL, 2, {813, 280}, {0, 0}, {0, 0}},
    {NULL, 2, {535, 280}, {0, 0}, {0, 0}},
    {NULL, 2, {554, 335}, {0, 0}, {0, 0}},
    {NULL, 2, {766, 335}, {0, 0}, {0, 0}},
    {NULL, 2, {781, 378}, {0, 0}, {0, 0}},
    {NULL, 2, {569, 378}, {0, 0}, {0, 0}},
    {NULL, 2, {588, 433}, {0, 0}, {0, 0}},
    {NULL, 1, {548, 318}, {0, 0}, {0, 0}},
    {NULL, 2, {616, 318}, {0, 0}, {0, 0}},
    {NULL, 2, {505, -5}, {0, 0}, {0, 0}},
    {NULL, 4, {502, -34}, {497, -28}, {0, 0}},
    {NULL, 4, {533, -41}, {508, -41}, {0, 0}},
    {NULL, 4, {566, -41}, {542, -41}, {0, 0}},
    {NULL, 4, {616, -41}, {590, -41}, {0, 0}},
    {NULL, 4, {651, -41}, {642, -41}, {0, 0}},
    {NULL, 4, {682, -27}, {671, -41}, {0, 0}},
    {NULL, 4, {712, 35}, {694, -14}, {0, 0}},
    {NULL, 4, {734, 21}, {721, 28}, {0, 0}},
    {NULL, 4, {760, 12}, {747, 15}, {0, 0}},
    {NULL, 4, {707, -78}, {734, -55}, {0, 0}},
    {NULL, 4, {638, -101}, {681, -101}, {0, 0}},
    {NULL, 4, {614, -101}, {633, -101}, {0, 0}},
    {NULL, 4, {572, -101}, {595, -101}, {0, 0}},
    {NULL, 4, {530, -101}, {549, -101}, {0, 0}},
    {NULL, 4, {506, -101}, {512, -101}, {0, 0}},
    {NULL, 4, {432, -83}, {449, -101}, {0, 0}},
    {NULL, 4, {437, -5}, {416, -66}, {0, 0}},
    {NULL, 2, {548, 318}, {0, 0}, {0, 0}},
    {NULL, 1, {472, 464}, {0, 0}, {0, 0}},
    {NULL, 2, {520, 438}, {0, 0}, {0, 0}},
    {NULL, 4, {467, 381}, {492, 407}, {0, 0}},
    {NULL, 4, {423, 336}, {443, 355}, {0, 0}},
    {NULL, 2, {384, 359}, {0, 0}, {0, 0}},
    {NULL, 4, {429, 412}, {404, 382}, {0, 0}},
    {NULL, 4, {472, 464}, {455, 442}, {0, 0}},
    {NULL, 1, {271, 444}, {0, 0}, {0, 0}},
    {NULL, 2, {332, 456}, {0, 0}, {0, 0}},
    {NULL, 4, {338, 406}, {337, 435}, {0, 0}},
    {NULL, 4, {336, 356}, {339, 378}, {0, 0}},
    {NULL, 2, {271, 339}, {0, 0}, {0, 0}},
    {NULL, 4, {275, 390}, {275, 361}, {0, 0}},
    {NULL, 4, {271, 444}, {276, 419}, {0, 0}},
    {NULL, 1, {413, 267}, {0, 0}, {0, 0}},
    {NULL, 2, {479, 267}, {0, 0}, {0, 0}},
    {NULL, 2, {374, -37}, {0, 0}, {0, 0}},
    {NULL, 4, {353, -80}, {364, -66}, {0, 0}},
    {NULL, 4, {323, -101}, {343, -94}, {0, 0}},
    {NULL, 4, {277, -110}, {306, -109}, {0, 0}},
    {NULL, 4, {205, -111}, {248, -111}, {0, 0}},
    {NULL, 4, {208, -81}, {208, -97}, {0, 0}},
    {NULL, 4, {206, -51}, {208, -65}, {0, 0}},
    {NULL, 4, {258, -52}, {235, -52}, {0, 0}},
    {NULL, 4, {289, -52}, {282, -52}, {0, 0}},
    {NULL, 4, {308, -37}, {303, -52}, {0, 0}},
    {NULL, 2, {413, 267}, {0, 0}, {0, 0}},
    {NULL, 1, {652, 530}, {0, 0}, {0, 0}},
    {NULL, 2, {912, 530}, {0, 0}, {0, 0}},
    {NULL, 2, {893, 475}, {0, 0}, {0, 0}},
    {NULL, 2, {633, 475}, {0, 0}, {0, 0}},
    {NULL, 2, {652, 530}, {0, 0}, {0, 0}},
    {NULL, 1, {553, 240}, {0, 0}, {0, 0}},
    {NULL, 2, {785, 240}, {0, 0}, {0, 0}},
    {NULL, 2, {767, 189}, {0, 0}, {0, 0}},
    {NULL, 2, {535, 189}, {0, 0}, {0, 0}},
    {NULL, 2, {553, 240}, {0, 0}, {0, 0}},
    {NULL, 1, {521, 147}, {0, 0}, {0, 0}},
    {NULL, 2, {754, 147}, {0, 0}, {0, 0}},
    {NULL, 2, {736, 97}, {0, 0}, {0, 0}},
    {NULL, 2, {503, 97}, {0, 0}, {0, 0}},
    {NULL, 2, {521, 147}, {0, 0}, {0, 0}},
    {NULL, 1, {489, 55}, {0, 0}, {0, 0}},
    {NULL, 2, {699, 55}, {0, 0}, {0, 0}},
    {NULL, 2, {681, 3}, {0, 0}, {0, 0}},
    {NULL, 2, {471, 3}, {0, 0}, {0, 0}},
    {NULL, 2, {489, 55}, {0, 0}, {0, 0}},
    {NULL, 1, {156, 61}, {0, 0}, {0, 0}},
    {NULL, 2, {373, 61}, {0, 0}, {0, 0}},
    {NULL, 2, {354, 6}, {0, 0}, {0, 0}},
    {NULL, 2, {137, 6}, {0, 0}, {0, 0}},
    {NULL, 2, {156, 61}, {0, 0}, {0, 0}},
    {NULL, 1, {191, 163}, {0, 0}, {0, 0}},
    {NULL, 2, {408, 163}, {0, 0}, {0, 0}},
    {NULL, 2, {389, 108}, {0, 0}, {0, 0}},
    {NULL, 2, {172, 108}, {0, 0}, {0, 0}},
    {NULL, 2, {191, 163}, {0, 0}, {0, 0}},
    {NULL, 1, {257, 513}, {0, 0}, {0, 0}},
    {NULL, 2, {586, 513}, {0, 0}, {0, 0}},
    {NULL, 2, {566, 453}, {0, 0}, {0, 0}},
    {NULL, 2, {237, 453}, {0, 0}, {0, 0}},
    {NULL, 2, {257, 513}, {0, 0}, {0, 0}},
    {NULL, 1, {187, 370}, {0, 0}, {0, 0}},
    {NULL, 2, {543, 370}, {0, 0}, {0, 0}},
    {NULL, 2, {522, 309}, {0, 0}, {0, 0}},
    {NULL, 2, {166, 309}, {0, 0}, {0, 0}},
    {NULL, 2, {187, 370}, {0, 0}, {0, 0}},
    {NULL, 1, {387, 573}, {0, 0}, {0, 0}},
    {NULL, 2, {455, 588}, {0, 0}, {0, 0}},
    {NULL, 4, {464, 541}, {462, 568}, {0, 0}},
    {NULL, 4, {466, 495}, {467, 515}, {0, 0}},
    {NULL, 2, {391, 472}, {0, 0}, {0, 0}},
    {NULL, 4, {392, 520}, {393, 493}, {0, 0}},
    {NULL, 4, {387, 573}, {391, 548}, {0, 0}},
    {NULL, 1, {1347, 582}, {0, 0}, {0, 0}},
    {NULL, 2, {1418, 582}, {0, 0}, {0, 0}},
    {NULL, 2, {1355, 399}, {0, 0}, {0, 0}},
    {NULL, 2, {1284, 399}, {0, 0}, {0, 0}},
    {NULL, 2, {1347, 582}, {0, 0}, {0, 0}},
    {NULL, 1, {889, 263}, {0, 0}, {0, 0}},
    {NULL, 2, {1141, 263}, {0, 0}, {0, 0}},
    {NULL, 2, {1121, 203}, {0, 0}, {0, 0}},
    {NULL, 2, {935, 203}, {0, 0}, {0, 0}},
    {NULL, 2, {825, -115}, {0, 0}, {0, 0}},
    {NULL, 2, {759, -115}, {0, 0}, {0, 0}},
    {NULL, 2, {889, 263}, {0, 0}, {0, 0}},
    {NULL, 1, {1294, 429}, {0, 0}, {0, 0}},
    {NULL, 2, {1583, 429}, {0, 0}, {0, 0}},
    {NULL, 2, {1531, 276}, {0, 0}, {0, 0}},
    {NULL, 2, {1242, 276}, {0, 0}, {0, 0}},
    {NULL, 2, {1261, 332}, {0, 0}, {0, 0}},
    {NULL, 2, {1481, 332}, {0, 0}, {0, 0}},
    {NULL, 2, {1496, 375}, {0, 0}, {0, 0}},
    {NULL, 2, {1276, 375}, {0, 0}, {0, 0}},
    {NULL, 2, {1294, 429}, {0, 0}, {0, 0}},
    {NULL, 1, {1255, 313}, {0, 0}, {0, 0}},
    {NULL, 2, {1326, 313}, {0, 0}, {0, 0}},
    {NULL, 2, {1214, -12}, {0, 0}, {0, 0}},
    {NULL, 4, {1211, -40}, {1206, -35}, {0, 0}},
    {NULL, 4, {1244, -46}, {1217, -46}, {0, 0}},
    {NULL, 4, {1275, -46}, {1251, -46}, {0, 0}},
    {NULL, 4, {1327, -46}, {1300, -46}, {0, 0}},
    {NULL, 4, {1365, -46}, {1354, -46}, {0, 0}},
    {NULL, 4, {1397, -33}, {1385, -46}, {0, 0}},
    {NULL, 4, {1428, 29}, {1409, -20}, {0, 0}},
    {NULL, 4, {1450, 16}, {1436, 22}, {0, 0}},
    {NULL, 4, {1478, 8}, {1465, 11}, {0, 0}},
    {NULL, 4, {1424, -84}, {1450, -61}, {0, 0}},
    {NULL, 4, {1353, -108}, {1398, -108}, {0, 0}},
    {NULL, 4, {1327, -108}, {1347, -108}, {0, 0}},
    {NULL, 4, {1283, -108}, {1307, -108}, {0, 0}},
    {NULL, 4, {1240, -108}, {1259, -108}, {0, 0}},
    {NULL, 4, {1215, -108}, {1221, -108}, {0, 0}},
    {NULL, 4, {1140, -90}, {1158, -108}, {0, 0}},
    {NULL, 4, {1143, -11}, {1122, -72}, {0, 0}},
    {NULL, 2, {1255, 313}, {0, 0}, {0, 0}},
    {NULL, 1, {1172, 459}, {0, 0}, {0, 0}},
    {NULL, 2, {1223, 433}, {0, 0}, {0, 0}},
    {NULL, 4, {1171, 378}, {1196, 404}, {0, 0}},
    {NULL, 4, {1123, 332}, {1146, 352}, {0, 0}},
    {NULL, 2, {1084, 355}, {0, 0}, {0, 0}},
    {NULL, 4, {1130, 408}, {1104, 378}, {0, 0}},
    {NULL, 4, {1172, 459}, {1157, 438}, {0, 0}},
    {NULL, 1, {965, 441}, {0, 0}, {0, 0}},
    {NULL, 2, {1026, 452}, {0, 0}, {0, 0}},
    {NULL, 4, {1034, 402}, {1034, 432}, {0, 0}},
    {NULL, 4, {1032, 353}, {1034, 373}, {0, 0}},
    {NULL, 2, {964, 335}, {0, 0}, {0, 0}},
    {NULL, 4, {968, 386}, {967, 356}, {0, 0}},
    {NULL, 4, {965, 441}, {969, 416}, {0, 0}},
    {NULL, 1, {1111, 263}, {0, 0}, {0, 0}},
    {NULL, 2, {1179, 263}, {0, 0}, {0, 0}},
    {NULL, 2, {1075, -40}, {0, 0}, {0, 0}},
    {NULL, 4, {1055, -84}, {1065, -71}, {0, 0}},
    {NULL, 4, {1025, -106}, {1045, -97}, {0, 0}},
    {NULL, 4, {975, -114}, {1004, -114}, {0, 0}},
    {NULL, 4, {902, -115}, {946, -115}, {0, 0}},
    {NULL, 4, {905, -85}, {905, -101}, {0, 0}},
    {NULL, 4, {903, -55}, {905, -69}, {0, 0}},
    {NULL, 4, {955, -57}, {931, -57}, {0, 0}},
    {NULL, 4, {989, -55}, {980, -57}, {0, 0}},
    {NULL, 4, {1007, -41}, {1002, -55}, {0, 0}},
    {NULL, 2, {1111, 263}, {0, 0}, {0, 0}},
    {NULL, 1, {1360, 525}, {0, 0}, {0, 0}},
    {NULL, 2, {1627, 525}, {0, 0}, {0, 0}},
    {NULL, 2, {1609, 470}, {0, 0}, {0, 0}},
    {NULL, 2, {1342, 470}, {0, 0}, {0, 0}},
    {NULL, 2, {1360, 525}, {0, 0}, {0, 0}},
    {NULL, 1, {1259, 235}, {0, 0}, {0, 0}},
    {NULL, 2, {1499, 235}, {0, 0}, {0, 0}},
    {NULL, 2, {1481, 184}, {0, 0}, {0, 0}},
    {NULL, 2, {1241, 184}, {0, 0}, {0, 0}},
    {NULL, 2, {1259, 235}, {0, 0}, {0, 0}},
    {NULL, 1, {1227, 143}, {0, 0}, {0, 0}},
    {NULL, 2, {1468, 143}, {0, 0}, {0, 0}},
    {NULL, 2, {1451, 92}, {0, 0}, {0, 0}},
    {NULL, 2, {1210, 92}, {0, 0}, {0, 0}},
    {NULL, 2, {1227, 143}, {0, 0}, {0, 0}},
    {NULL, 1, {1196, 51}, {0, 0}, {0, 0}},
    {NULL, 2, {1416, 51}, {0, 0}, {0, 0}},
    {NULL, 2, {1398, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1178, 0}, {0, 0}, {0, 0}},
    {NULL, 2, {1196, 51}, {0, 0}, {0, 0}},
    {NULL, 1, {851, 57}, {0, 0}, {0, 0}},
    {NULL, 2, {1075, 57}, {0, 0}, {0, 0}},
    {NULL, 2, {1056, 2}, {0, 0}, {0, 0}},
    {NULL, 2, {832, 2}, {0, 0}, {0, 0}},
    {NULL, 2, {851, 57}, {0, 0}, {0, 0}},
    {NULL, 1, {885, 158}, {0, 0}, {0, 0}},
    {NULL, 2, {1109, 158}, {0, 0}, {0, 0}},
    {NULL, 2, {1091, 104}, {0, 0}, {0, 0}},
    {NULL, 2, {867, 104}, {0, 0}, {0, 0}},
    {NULL, 2, {885, 158}, {0, 0}, {0, 0}},
    {NULL, 1, {951, 510}, {0, 0}, {0, 0}},
    {NULL, 2, {1289, 510}, {0, 0}, {0, 0}},
    {NULL, 2, {1268, 449}, {0, 0}, {0, 0}},
    {NULL, 2, {930, 449}, {0, 0}, {0, 0}},
    {NULL, 2, {951, 510}, {0, 0}, {0, 0}},
    {NULL, 1, {879, 366}, {0, 0}, {0, 0}},
    {NULL, 2, {1247, 366}, {0, 0}, {0, 0}},
    {NULL, 2, {1226, 306}, {0, 0}, {0, 0}},
    {NULL, 2, {858, 306}, {0, 0}, {0, 0}},
    {NULL, 2, {879, 366}, {0, 0}, {0, 0}},
    {NULL, 1, {1081, 567}, {0, 0}, {0, 0}},
    {NULL, 2, {1152, 582}, {0, 0}, {0, 0}},
    {NULL, 4, {1162, 535}, {1159, 562}, {0, 0}},
    {NULL, 4, {1165, 490}, {1166, 508}, {0, 0}},
    {NULL, 2, {1089, 468}, {0, 0}, {0, 0}},
    {NULL, 4, {1087, 516}, {1090, 488}, {0, 0}},
    {NULL, 4, {1081, 567}, {1084, 544}, {0, 0}},
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
    outline_data = (lv_ll_t *)lv_font_get_glyph_bitmap(font_italic, &g, 0x9F98, NULL);

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
