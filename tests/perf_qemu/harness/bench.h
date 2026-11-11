/**
 * @file bench.h
 * What the two benchmark mains share: the display, the clock and the colour format.
 *
 * The display comes from lv_test_display_create(), the same 800x480 direct mode display
 * the unit tests render through, because the reference images in tests/ref_imgs/draw/render
 * were produced on it. Rendering through anything else would report config differences as
 * pixel differences.
 *
 * The clock advances one fixed step per main loop pass rather than tracking the instruction
 * counter. That matters: with a real clock a faster build renders more frames in the same
 * scene and lands on different content, and the two sides stop being comparable.
 */
#ifndef BENCH_H
#define BENCH_H

#include "plat.h"

#include "lvgl.h"
#include "demos/lv_demos.h"

/*The resolution the reference images were rendered at, see tests/src/lv_test_init.h*/
#define BENCH_HOR_RES 800
#define BENCH_VER_RES 480

/*One tick step per main loop pass, in ms*/
#define BENCH_FRAME_MS 16u

/*One per directory under tests/ref_imgs/draw/render. compare_refs.py has to be able to
 *expand each of these the way lv_test_screenshot_compare.c does, so the two lists match.*/
#define PERF_QEMU_CF_rgb565                1
#define PERF_QEMU_CF_rgb565_swapped        2
#define PERF_QEMU_CF_rgb888                3
#define PERF_QEMU_CF_xrgb8888              4
#define PERF_QEMU_CF_argb8888              5
#define PERF_QEMU_CF_argb8888_premultiplied 6
#define PERF_QEMU_CF_l8                    7
#define PERF_QEMU_CF_al88                  8
#define PERF_QEMU_CF_i1                    9

#if PERF_QEMU_CF == PERF_QEMU_CF_rgb565
    #define BENCH_CF      LV_COLOR_FORMAT_RGB565
    #define BENCH_CF_NAME "rgb565"
#elif PERF_QEMU_CF == PERF_QEMU_CF_rgb565_swapped
    #define BENCH_CF      LV_COLOR_FORMAT_RGB565_SWAPPED
    #define BENCH_CF_NAME "rgb565_swapped"
#elif PERF_QEMU_CF == PERF_QEMU_CF_rgb888
    #define BENCH_CF      LV_COLOR_FORMAT_RGB888
    #define BENCH_CF_NAME "rgb888"
#elif PERF_QEMU_CF == PERF_QEMU_CF_xrgb8888
    #define BENCH_CF      LV_COLOR_FORMAT_XRGB8888
    #define BENCH_CF_NAME "xrgb8888"
#elif PERF_QEMU_CF == PERF_QEMU_CF_argb8888
    #define BENCH_CF      LV_COLOR_FORMAT_ARGB8888
    #define BENCH_CF_NAME "argb8888"
#elif PERF_QEMU_CF == PERF_QEMU_CF_argb8888_premultiplied
    #define BENCH_CF      LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED
    #define BENCH_CF_NAME "argb8888_premultiplied"
#elif PERF_QEMU_CF == PERF_QEMU_CF_l8
    #define BENCH_CF      LV_COLOR_FORMAT_L8
    #define BENCH_CF_NAME "l8"
#elif PERF_QEMU_CF == PERF_QEMU_CF_al88
    #define BENCH_CF      LV_COLOR_FORMAT_AL88
    #define BENCH_CF_NAME "al88"
#elif PERF_QEMU_CF == PERF_QEMU_CF_i1
    #define BENCH_CF      LV_COLOR_FORMAT_I1
    #define BENCH_CF_NAME "i1"
#else
    #error "define PERF_QEMU_CF to one of the PERF_QEMU_CF_* values above"
#endif

/*Every format has reference images at full opacity and at one partial value. That partial
 *value is 192 for I1 and 128 for the rest, following the per-format tests in
 *tests/src/test_cases/draw, so that every scene rendered here has a reference to compare
 *against. Rendering I1 at 128 produces 19 images that nothing was ever stored for.*/
#if PERF_QEMU_CF == PERF_QEMU_CF_i1
    #define BENCH_OPA_LOW 192
#else
    #define BENCH_OPA_LOW 128
#endif

static uint32_t bench_passes;

static uint32_t bench_tick_cb(void)
{
    return bench_passes * BENCH_FRAME_MS;
}

/*The scene selection, from -semihosting-config arg=. "*" means every scene; otherwise it
 *is one scene's name, so that measuring a single scene does not need its own image.
 *perf_qemu.py always passes one, because with no arg QEMU answers SYS_GET_CMDLINE with
 *the kernel's path, which would look like a scene name that matches nothing.*/
static char bench_only[96];

static inline void bench_read_selection(void)
{
    sh_cmdline(bench_only, sizeof(bench_only));
}

/**
 * Was the run asked for every scene rather than one of them?
 */
static inline bool bench_all_scenes(void)
{
    return bench_only[0] == '\0' || (bench_only[0] == '*' && bench_only[1] == '\0');
}

/**
 * Is this scene one the run was asked for?
 * @param name  the scene's label, as it appears on the #SC line
 */
static inline bool bench_wanted(const char * name)
{
    if(bench_all_scenes()) return true;

    /*A selection without an _opa_ suffix takes every opacity of that scene*/
    const char * a = bench_only;
    const char * b = name;
    while(*a && *a == *b) {
        a++;
        b++;
    }
    if(*a != '\0') return false;
    return *b == '\0' || (b[0] == '_' && b[1] == 'o' && b[2] == 'p' && b[3] == 'a');
}

static inline lv_display_t * bench_init(void)
{
    plat_timer_init();
    lv_init();
    lv_tick_set_cb(bench_tick_cb);

    bench_read_selection();

    lv_display_t * disp = lv_test_display_create(BENCH_HOR_RES, BENCH_VER_RES);
    lv_display_set_color_format(disp, BENCH_CF);
    return disp;
}

/*One line per measurement, scraped by perf_qemu.py*/
static inline void bench_print_scene(const char * name, uint64_t instr)
{
    char * p = plat_str(plat_ob, "#SC\t");
    p = plat_str(p, name);
    p = plat_str(p, "\t");
    p = plat_u64(p, instr);
    plat_flush(plat_str(p, "\n"));
}

static inline void bench_print_total(uint64_t instr, uint64_t frames)
{
    char * p = plat_str(plat_ob, "#TOTAL\t");
    p = plat_u64(p, instr);
    p = plat_str(p, "\t");
    p = plat_u64(p, frames);
    plat_flush(plat_str(p, "\n"));
}

/*The peak of LVGL's own heap. A heap too small for the scene does not fail loudly with
 *the assertions off, it just draws the wrong thing, so the number is always reported and
 *perf_qemu.py refuses a run that came close to the limit.*/
static inline void bench_print_mem(void)
{
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    char * p = plat_str(plat_ob, "#MEM\t");
    p = plat_u64(p, mon.max_used);
    p = plat_str(p, "\t");
    p = plat_u64(p, mon.total_size);
    plat_flush(plat_str(p, "\n"));
}

#endif /*BENCH_H*/
