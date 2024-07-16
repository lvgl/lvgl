/**
 * This file contains the main function for the profiling program. It initialises the resources
 * and calls the profiling functions with those resources. Other functions and files may be used
 * to implement the desired procedures but the calling of the profiling function must be done in main.
 * This can be changed but it necessitates modifications in the script that calculates the execution
 * times and maps the addresses to the function names (found in SO3). However this hurts performance
 * a lot (analysis goes from taking seconds to several minutes)
 */

#include "prof_test_p.h"
#include <stdio.h>

int main(int argc, char const * argv[])
{
    uint32_t t = 256,
             u0 = 0,
             u1 = 50,
             u2 = 954,
             u3 = LV_BEZIER_VAL_MAX,
             bezier_res1,
             bezier_res2;
    lv_area_t intersect_res1,
              intersect_res2,
              a1 = {0, 0, 4, 3},
              a2 = {3, 1, 6, 4};

    printf("Profiling LVGL\n");
    printf("\tProfiling lv_bezier3\n");
    printf("\t\tCubic bezier curve with parameters (%d;%d;%d;%d), t = %d\n", u0, u1, u2, u3, t);
    bezier_res1 = lv_bezier3(t, u0, u1, u2, u3);
    printf("\t\tWithout profiling: res = %d\n", bezier_res1);
    bezier_res2 = prof_lv_bezier3(t, u0, u1, u2, u3);
    printf("\t\tWith profiling: res = %d\n", bezier_res2);

    printf("\tProfiling _lv_area_intersect\n");
    printf("\t\tArea a1 (%d;%d)->(%d;%d)\n",
           (int)(a1.x1),
           (int)(a1.y1),
           (int)(a1.x2),
           (int)(a1.y2));
    printf("\t\tArea a2 (%d;%d)->(%d;%d)\n",
           (int)(a2.x1),
           (int)(a2.y1),
           (int)(a2.x2),
           (int)(a2.y2));
    printf("\t\tCalculating intersection\n");
    _lv_area_intersect(&intersect_res1, &a1, &a2);
    printf("\t\tWithout profiling: res = (%d;%d)->(%d;%d)\n",
           (int)(intersect_res1.x1),
           (int)(intersect_res1.y1),
           (int)(intersect_res1.x2),
           (int)(intersect_res1.y2));
    prof_lv_area_intersect(&intersect_res2, &a1, &a2);
    printf("\t\tWith profiling: res = (%d;%d)->(%d;%d)\n",
           (int)(intersect_res2.x1),
           (int)(intersect_res2.y1),
           (int)(intersect_res2.x2),
           (int)(intersect_res2.y2));

    return 0;
}
