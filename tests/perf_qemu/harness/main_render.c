/**
 * @file main_render.c
 * Every lv_demo_render scene at both opacities, timed and dumped.
 *
 * The scenes are static, so the screen is invalidated before each pass to force a full
 * redraw, and each scene is rendered once untimed first so that a lazily built cache is
 * not charged to the first measured pass.
 *
 * After timing, the rendered buffer is written to the host over semihosting as
 * <cf>/<scene>_opa_<opa>.bin, and compare_refs.py diffs it against the reference image
 * in tests/ref_imgs/draw/render/<cf>. The rows are written packed, with the draw buffer's
 * stride removed, so the host needs nothing but the header to read it.
 */
#include "bench.h"

/*Enough passes that a per-scene fixed cost does not dominate, few enough to stay quick*/
#define RENDER_REPS 8

static const lv_opa_t g_opas[] = { 255, BENCH_OPA_LOW };

/*"LVD1", w, h, cf, stride. Rows are written at their real stride rather than packed,
 *because L8, AL88 and I1 do not have a whole number of bytes per pixel to pack to, and
 *because the host has to index them exactly the way lv_test_screenshot_compare.c does.*/
static uint32_t g_hdr[5] = { 0x3144564Cu, BENCH_HOR_RES, BENCH_VER_RES, 0, 0 };

static void dump_buf(const lv_draw_buf_t * draw_buf, const char * scene, uint32_t opa)
{
    char name[128];
    char * p = plat_str(name, BENCH_CF_NAME);
    p = plat_str(p, "/");
    p = plat_str(p, scene);
    p = plat_str(p, "_opa_");
    p = plat_u64(p, opa);
    p = plat_str(p, ".bin");
    *p = '\0';

    long h = sh_open_w(name);
    if(h < 0) {
        plat_flush(plat_str(plat_str(plat_ob, "#ERR\tcannot open "), name));
        return;
    }

    uint32_t stride = draw_buf->header.stride;
    g_hdr[3] = draw_buf->header.cf;
    g_hdr[4] = stride;
    sh_write(h, g_hdr, sizeof(g_hdr));

    /*An I1 buffer starts with an 8 byte palette that the pixels follow, which is what
     *lv_test_screenshot_compare.c skips before it reads the rows. Skipping it here too
     *means the file is h rows of stride bytes for every format, and the host needs no
     *special case.*/
    const uint8_t * px = draw_buf->data;
    if(draw_buf->header.cf == LV_COLOR_FORMAT_I1) px += 8;
    sh_write(h, px, (size_t)stride * draw_buf->header.h);
    sh_close(h);
}

int main(void)
{
    lv_display_t * disp = bench_init();

    uint64_t total = 0;
    uint32_t o;
    for(o = 0; o < sizeof(g_opas) / sizeof(g_opas[0]); o++) {
        uint32_t id;
        for(id = 0; id < LV_DEMO_RENDER_SCENE_NUM; id++) {
            const char * scene = lv_demo_render_get_scene_name((lv_demo_render_scene_t)id);

            char label[96];
            char * lp = plat_str(label, scene);
            lp = plat_str(lp, "_opa_");
            lp = plat_u64(lp, g_opas[o]);
            *lp = '\0';
            if(!bench_wanted(label)) continue;

            lv_demo_render((lv_demo_render_scene_t)id, g_opas[o]);
            lv_obj_invalidate(lv_screen_active());
            lv_refr_now(disp);
            bench_passes++;

            /*plat_instr() is read every pass, not just around the loop. On Cortex-M it
             *accumulates a 24 bit SysTick delta, which wraps after a few hundred million
             *instructions, so the gap between two reads has to stay well inside that. One
             *frame is; eight of the heaviest scene would be uncomfortably close.*/
            uint64_t t0 = plat_instr();
            uint32_t r;
            for(r = 0; r < RENDER_REPS; r++) {
                lv_obj_invalidate(lv_screen_active());
                lv_refr_now(disp);
                bench_passes++;
                plat_instr();
            }
            uint64_t instr = (plat_instr() - t0) / RENDER_REPS;
            total += instr;
            bench_print_scene(label, instr);

            dump_buf(lv_display_get_buf_active(disp), scene, g_opas[o]);
        }
    }

    if(!total) {
        plat_flush(plat_str(plat_str(plat_str(plat_ob, "#ERR\tno scene matched "), bench_only), "\n"));
    }
    bench_print_total(total, 0);
    bench_print_mem();
    sh_exit();
    return 0;
}
