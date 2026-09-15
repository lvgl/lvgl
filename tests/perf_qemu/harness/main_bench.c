/**
 * @file main_bench.c
 * The LVGL benchmark demo on bare metal, per scene and in total.
 *
 * The counts come from the display's RENDER_START and RENDER_READY events, so they are the
 * rendering only and exclude the demo's own scene setup. Each frame is charged to
 * lv_demo_benchmark_get_scene_act(), and the reported figure is the mean over that scene's
 * frames, because the demo plays each scene for a fixed number of milliseconds rather than
 * a fixed number of frames.
 */
#include "bench.h"

/*The demo's scene table is not this file's to know the size of, so this is a ceiling*/
#define MAX_SCENES 64

static uint64_t g_instr[MAX_SCENES];
static uint32_t g_count[MAX_SCENES];
static uint64_t g_frame_start;
static bool g_done;

static void render_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_RENDER_START) {
        g_frame_start = plat_instr();
    }
    else if(code == LV_EVENT_RENDER_READY) {
        uint32_t scene = lv_demo_benchmark_get_scene_act();
        if(scene < MAX_SCENES) {
            g_instr[scene] += plat_instr() - g_frame_start;
            g_count[scene]++;
        }
    }
}

/*The demo's names have spaces and capitals; the selection and the #SC lines use neither*/
static void scene_label(char * out, const char * name)
{
    char * p = out;
    while(*name) {
        char c = *name++;
        if(c == ' ') c = '_';
        else if(c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        *p++ = c;
    }
    *p = '\0';
}

/**
 * @return the scene the run was asked for, or -1 for all of them
 */
static int32_t selected_scene(void)
{
    if(bench_all_scenes()) return -1;

    uint32_t i;
    for(i = 0; i < MAX_SCENES; i++) {
        const char * name = lv_demo_benchmark_get_scene_name(i);
        if(name == NULL) break;
        char label[96];
        scene_label(label, name);
        if(bench_wanted(label)) return (int32_t)i;
    }
    return -2;
}

static void demo_end_cb(const lv_demo_benchmark_summary_t * summary)
{
    LV_UNUSED(summary);
    g_done = true;
}

int main(void)
{
    lv_display_t * disp = bench_init();
    lv_display_add_event_cb(disp, render_event_cb, LV_EVENT_RENDER_START, NULL);
    lv_display_add_event_cb(disp, render_event_cb, LV_EVENT_RENDER_READY, NULL);

    int32_t only = selected_scene();
    if(only == -2) {
        plat_flush(plat_str(plat_str(plat_str(plat_ob, "#ERR\tno scene matched "), bench_only), "\n"));
        sh_exit();
    }

    lv_demo_benchmark_set_end_cb(demo_end_cb);
    if(only < 0) lv_demo_benchmark();
    else lv_demo_benchmark_scene((uint32_t)only);

    while(!g_done) {
        lv_timer_handler();
        bench_passes++;
    }

    uint64_t total = 0;
    uint64_t frames = 0;
    uint32_t i;
    for(i = 0; i < MAX_SCENES; i++) {
        const char * name = lv_demo_benchmark_get_scene_name(i);
        if(name == NULL) break;
        if(!g_count[i]) continue;
        char label[96];
        scene_label(label, name);
        bench_print_scene(label, g_instr[i] / g_count[i]);
        total += g_instr[i];
        frames += g_count[i];
    }
    bench_print_total(total, frames);
    bench_print_mem();
    sh_exit();
    return 0;
}
