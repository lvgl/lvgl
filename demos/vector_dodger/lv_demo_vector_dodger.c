/**
 * @file lv_demo_vector_dodger.c
 *
 * Vector-based dodger game demo
 */

#include "lv_demo_vector_dodger.h"

/*****************************
 * Game configuration        *
 *****************************/

#if LV_USE_DEMO_VECTOR_DODGER && !LV_USE_VECTOR_GRAPHIC
    #error "LV_USE_VECTOR_GRAPHIC must be enabled to use this demo"
#endif

#if LV_USE_DEMO_VECTOR_DODGER && LV_USE_VECTOR_GRAPHIC

#define FINGER_OFFSET       50      /* Offset for finger touch */

#define VD_PLAYER_R         10      /* Player body radius */
#define VD_PLAYER_WINGSPAN  24      /* Player wing span */
#define VD_PLAYER_TRAIL_CNT 15
#define VD_MAX_OBS          14      /* Max obstacles on screen */
#define VD_OBS_MIN_SIZE     10
#define VD_OBS_MAX_SIZE     28
#define VD_BASE_SPEED       1.6f
#define VD_SPEED_INC        0.025f  /* Speed increase over time */
#define VD_ACCEL_KEY        2.2f
#define VD_FRICTION         0.36f   /* Inertia */
#define VD_SPAWN_COOLDOWN   220     /* Spawn interval (ms) */
#define VD_TIMER_PERIOD     16      /* ~60 FPS */
#define VD_SAFE_START_MS    1200    /* Initial safe period to avoid instant collision */
#define VD_BOUNDS_PADDING   2
#define VD_BG_STAR_COUNT    48

/* Colors */
#define VD_COLOR_THEME              lv_color_hex(0x2ee6a6)
#define VD_COLOR_THEME_BORDER       lv_color_hex(0x163d31)
#define VD_COLOR_THEME_DARK         lv_color_hex(0x0b1220)

/*****************************
 * Game state                *
 ******************************/

typedef struct {
    float x, y;          /* Center position */
    float vx, vy;        /* Velocity (affected by keyboard/drag) */
    uint8_t alive;       /* 1=alive */
} vd_player_t;

typedef struct {
    float x, y;          /* Top-left position */
    float w, h;          /* Dimensions (rectangular obstacle) */
    float vx;            /* Velocity X (moving left) */
    uint8_t used;        /* Whether obstacle exists */
} vd_obs_t;

typedef struct {
    /* Canvas and dimensions */
    lv_obj_t * canvas;
    lv_obj_t * restart_btn; /* Restart button */
    int32_t w, h;
    char text_buf[128];

    /* Player and obstacles */
    vd_player_t player;
    vd_obs_t obs[VD_MAX_OBS];

    /* Background stars (decoration only) */
    lv_point_t stars[VD_BG_STAR_COUNT];

    /* Timing/score */
    uint32_t start_ms;
    uint32_t last_spawn_ms;
    uint32_t last_update_ms;
    uint32_t score;          /* Score based on frames survived */
    uint8_t  game_over;

    /* Difficulty */
    float base_speed;

    /* Input buffer (for keyboard acceleration) */
    int key_left, key_right, key_up, key_down;

    /* Drag control */
    uint8_t dragging;
    lv_point_t drag_last;

    /* Improved trail effect */
    lv_fpoint_t trail[VD_PLAYER_TRAIL_CNT];    /* Trail position history with timestamps */
    uint32_t trail_time[VD_PLAYER_TRAIL_CNT];  /* Timestamps for each point */
    uint8_t trail_head;       /* Current trail position */
    uint8_t trail_count;      /* Number of active trail segments */
} vd_state_t;

static vd_state_t g_st;

/*****************************
 * Utils                     *
 *****************************/

static uint32_t vd_millis(void)
{
    return lv_tick_get();
}

static uint32_t vd_rand(uint32_t * seed)
{
    /* Simple LCG to avoid system rand() dependency */
    *seed = (*seed * 1664525u + 1013904223u);
    return *seed;
}

static int vd_irand_range(uint32_t * seed, int lo, int hi)
{
    uint32_t r = vd_rand(seed);
    int span = hi - lo + 1;
    return lo + (int)(r % (uint32_t)span);
}

static float vd_frand01(uint32_t * seed)
{
    return (float)(vd_rand(seed) & 0xFFFF) / 65535.0f;
}

static int rect_circle_intersect(float rx, float ry, float rw, float rh,
                                 float cx, float cy, float cr)
{
    /* AABB vs Circle collision detection */
    float nearest_x = (cx < rx) ? rx : (cx > rx + rw ? rx + rw : cx);
    float nearest_y = (cy < ry) ? ry : (cy > ry + rh ? ry + rh : cy);
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy) <= cr * cr;
}

/*****************************
 * Spawning & Reset          *
 *****************************/

static void vd_reset(vd_state_t * st)
{
    lv_memzero(st->obs, sizeof(st->obs));
    st->player.x = st->w * 0.25f;
    st->player.y = st->h * 0.5f;
    st->player.vx = st->player.vy = 0.0f;
    st->player.alive = 1;

    st->game_over = 0;
    st->score = 0;
    st->base_speed = VD_BASE_SPEED;
    st->start_ms = vd_millis();
    st->last_spawn_ms = st->start_ms;
    st->last_update_ms = st->start_ms;
    st->key_left = st->key_right = st->key_up = st->key_down = 0;
    st->dragging = 0;

    /* Hide restart button */
    if(st->restart_btn) lv_obj_add_flag(st->restart_btn, LV_OBJ_FLAG_HIDDEN);

    /* Generate background stars (fixed seed for consistent look) */
    uint32_t seed = 0x114514u;
    for(int i = 0; i < VD_BG_STAR_COUNT; i++) {
        st->stars[i].x = vd_irand_range(&seed, 0, st->w - 1);
        st->stars[i].y = vd_irand_range(&seed, 0, st->h - 1);
    }

    /* Initialize trail */
    st->trail_head = 0;
    st->trail_count = 0;
    for(int i = 0; i < VD_PLAYER_TRAIL_CNT; i++) {
        st->trail[i].x = st->player.x - VD_PLAYER_R;
        st->trail[i].y = st->player.y;
        st->trail_time[i] = st->start_ms;
    }
}

static void vd_spawn(vd_state_t * st)
{
    /* Find an empty slot */
    int idx = -1;
    for(int i = 0; i < VD_MAX_OBS; i++) {
        if(!st->obs[i].used) {
            idx = i;
            break;
        }
    }
    if(idx < 0) return;

    /* Random properties (spawn from right, move left) */
    uint32_t seed = vd_millis() ^ 0x114514u;
    float h = (float)vd_irand_range(&seed, VD_OBS_MIN_SIZE, VD_OBS_MAX_SIZE);
    float w = (float)vd_irand_range(&seed, VD_OBS_MIN_SIZE, VD_OBS_MAX_SIZE + 8);
    float y = (float)vd_irand_range(&seed, VD_BOUNDS_PADDING, st->h - (int)h - VD_BOUNDS_PADDING);

    float speed = st->base_speed + vd_frand01(&seed) * st->base_speed * 0.8f;

    st->obs[idx].x = (float)st->w + (float)vd_irand_range(&seed, 0, st->w / 3);
    st->obs[idx].y = y;
    st->obs[idx].w = w;
    st->obs[idx].h = h;
    st->obs[idx].vx = -speed;
    st->obs[idx].used = 1;
}

/*****************************
 * Game update               *
 *****************************/

static void vd_update(vd_state_t * st, uint32_t now_ms)
{
    if(st->game_over) return;

    uint32_t dt_ms = now_ms - st->last_update_ms;
    st->last_update_ms = now_ms;
    float dt = dt_ms / LV_DEF_REFR_PERIOD; /* Frame time normalized to 16ms */

    /* Keyboard input -> velocity */
    float ax = (st->key_right - st->key_left) * VD_ACCEL_KEY;
    float ay = (st->key_down - st->key_up) * VD_ACCEL_KEY;

    st->player.vx = (st->player.vx + ax) * VD_FRICTION;
    st->player.vy = (st->player.vy + ay) * VD_FRICTION;

    /* Drag takes priority: direct follow with smoothing */
    if(st->dragging) {
        float tx = st->drag_last.x;
        float ty = st->drag_last.y;
        float x = st->player.x;
        float y = st->player.y;
        st->player.x = st->player.x * 0.7f + tx * 0.3f;
        st->player.y = st->player.y * 0.7f + ty * 0.3f;
        /* Reduce inertia to avoid jitter */
        st->player.vx = st->player.x - x;
        st->player.vy = st->player.y - y;
    }
    else {
        st->player.x += st->player.vx * dt;
        st->player.y += st->player.vy * dt;
    }

    /* Boundary checks */
    if(st->player.x < VD_BOUNDS_PADDING + VD_PLAYER_R) st->player.x = VD_BOUNDS_PADDING + VD_PLAYER_R;
    if(st->player.y < VD_BOUNDS_PADDING + VD_PLAYER_R) st->player.y = VD_BOUNDS_PADDING + VD_PLAYER_R;
    if(st->player.x > st->w - VD_BOUNDS_PADDING - VD_PLAYER_R) st->player.x = st->w - VD_BOUNDS_PADDING - VD_PLAYER_R;
    if(st->player.y > st->h - VD_BOUNDS_PADDING - VD_PLAYER_R) st->player.y = st->h - VD_BOUNDS_PADDING - VD_PLAYER_R;

    /* Obstacle movement and cleanup */
    for(int i = 0; i < VD_MAX_OBS; i++) {
        if(!st->obs[i].used) continue;
        st->obs[i].x += st->obs[i].vx * dt;

        if(st->obs[i].x + st->obs[i].w < -8) {
            st->obs[i].used = 0;
        }
    }

    /* Spawn new obstacles (gradually faster) */
    if(now_ms - st->last_spawn_ms >= VD_SPAWN_COOLDOWN) {
        st->last_spawn_ms = now_ms;
        vd_spawn(st);
        st->base_speed += VD_SPEED_INC; /* Gradually increase speed */
    }

    /* Collision detection (skip initial safe period) */
    if(now_ms - st->start_ms > VD_SAFE_START_MS) {
        for(int i = 0; i < VD_MAX_OBS; i++) {
            if(!st->obs[i].used) continue;
            if(rect_circle_intersect(st->obs[i].x, st->obs[i].y, st->obs[i].w, st->obs[i].h,
                                     st->player.x, st->player.y, VD_PLAYER_R)) {
                st->game_over = 1;
                st->player.alive = 0;
                /* Show restart button when game over */
                if(st->restart_btn) {
                    lv_obj_remove_flag(st->restart_btn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_move_foreground(st->restart_btn);
                }
                break;
            }
        }
    }

    /* Update trail every frame */
    if(now_ms - st->trail_time[st->trail_head] > LV_DEF_REFR_PERIOD) { // 30ms between points
        st->trail[st->trail_head].x = st->player.x - VD_PLAYER_R;
        st->trail[st->trail_head].y = st->player.y;
        st->trail_time[st->trail_head] = now_ms;
        st->trail_head = (st->trail_head + 1) % VD_PLAYER_TRAIL_CNT;
        if(st->trail_count < VD_PLAYER_TRAIL_CNT) st->trail_count++;
    }

    /* Scoring */
    st->score++;
}

/*****************************
 * Vector Drawing Functions  *
 *****************************/

static void draw_airplane_shape(lv_vector_path_t * path, float x, float y, float size, bool is_enemy)
{
    /* Create airplane shape pointing right (for player) or left (for enemies) */
    if(is_enemy) {
        /* Enemy airplane shape (pointing left) */
        lv_vector_path_move_to(path, &(lv_fpoint_t) {
            x - size / 2, y
        }); // nose
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x + size, y - size / 2
        }); // top wing
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x + size * 0.7f, y
        }); // mid back
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x + size, y + size / 2
        }); // bottom wing
        lv_vector_path_close(path);
    }
    else {
        /* Player airplane shape (pointing right) */
        lv_vector_path_move_to(path, &(lv_fpoint_t) {
            x + VD_PLAYER_R, y
        }); // nose
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x - size, y - size / 2
        }); // top wing
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x - size * 0.7f, y
        }); // mid back
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            x - size, y + size / 2
        }); // bottom wing
        lv_vector_path_close(path);
    }
}

static void vd_draw_player(lv_layer_t * layer, vd_state_t * st)
{
    if(!st->player.alive) return;

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    draw_airplane_shape(path, st->player.x, st->player.y, VD_PLAYER_WINGSPAN, false);

    lv_vector_dsc_t * vdsc = lv_vector_dsc_create(layer);
    lv_vector_dsc_set_fill_color(vdsc, VD_COLOR_THEME);
    lv_vector_dsc_set_stroke_color(vdsc, VD_COLOR_THEME_BORDER);
    lv_vector_dsc_set_stroke_width(vdsc, VD_PLAYER_WINGSPAN * 0.2f);
    lv_vector_dsc_set_stroke_opa(vdsc, LV_OPA_80);

    lv_vector_dsc_add_path(vdsc, path);
    lv_draw_vector(vdsc);

    lv_vector_path_delete(path);
    lv_vector_dsc_delete(vdsc);
}

static void vd_draw_obstacles(lv_layer_t * layer, vd_state_t * st)
{
    for(int i = 0; i < VD_MAX_OBS; i++) {
        if(!st->obs[i].used) continue;

        lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

        /* Draw enemy airplane at obstacle position */
        float center_x = st->obs[i].x + st->obs[i].w / 2;
        float center_y = st->obs[i].y + st->obs[i].h / 2;
        float size = (st->obs[i].w + st->obs[i].h) / 3; // Approximate size

        draw_airplane_shape(path, center_x, center_y, size, 1);

        lv_vector_dsc_t * vdsc = lv_vector_dsc_create(layer);

        /* Color based on size/speed */
        uint8_t g = (uint8_t)LV_CLAMP(48, 200, (int)(80 + st->obs[i].w * 4));
        lv_vector_dsc_set_fill_color(vdsc, lv_color_make(0xFF, g, 0x40));
        lv_vector_dsc_set_stroke_color(vdsc, lv_color_hex(0x101010));
        lv_vector_dsc_set_stroke_width(vdsc, 1.0f);
        lv_vector_dsc_set_stroke_opa(vdsc, LV_OPA_50);

        lv_vector_dsc_add_path(vdsc, path);
        lv_draw_vector(vdsc);

        lv_vector_path_delete(path);
        lv_vector_dsc_delete(vdsc);
    }
}

static void vd_draw_background(lv_layer_t * layer, vd_state_t * st)
{
    /* Solid background */
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_append_rect(path, &(lv_area_t) {
        0, 0, st->w - 1, st->h - 1
    }, 0, 0);

    lv_vector_dsc_t * vdsc = lv_vector_dsc_create(layer);
    lv_vector_dsc_set_fill_color(vdsc, VD_COLOR_THEME_DARK);
    lv_vector_dsc_add_path(vdsc, path);
    lv_draw_vector(vdsc);
    lv_vector_path_delete(path);
    lv_vector_dsc_delete(vdsc);

    /* Stars */
    path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    uint32_t seed = 0x114514u;
    for(int i = 0; i < VD_BG_STAR_COUNT; i++) {
        st->stars[i].x -= (int32_t)(vd_irand_range(&seed, 128, 255) * st->base_speed / 255.0f);
        st->stars[i].y -= (int32_t)(st->player.vy * 0.6 * vd_irand_range(&seed, 128, 255) / 255);
        if(st->stars[i].x < 0) st->stars[i].x = st->w;
    }
    for(int i = 0; i < VD_BG_STAR_COUNT; i++) {
        lv_vector_path_append_rect(path,
        &(lv_area_t) {
            st->stars[i].x, st->stars[i].y, st->stars[i].x + vd_irand_range(&seed, 1, 3), st->stars[i].y + vd_irand_range(&seed, 1,
                                                                                                                          3)
        },
        1, 1);
    }

    vdsc = lv_vector_dsc_create(layer);
    lv_vector_dsc_set_fill_color(vdsc, lv_color_hex(0xcde5ff));
    lv_vector_dsc_add_path(vdsc, path);
    lv_draw_vector(vdsc);
    lv_vector_path_delete(path);
    lv_vector_dsc_delete(vdsc);
}

static void vd_draw_hud(lv_layer_t * layer, vd_state_t * st)
{
    /* Score display */
    char buf[64];
    lv_snprintf(buf, sizeof(buf), "SCORE: %u", (unsigned)st->score);

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = VD_COLOR_THEME;  /* Game theme color */
    dsc.opa = LV_OPA_COVER;
    dsc.text = buf;
    dsc.font = &lv_font_montserrat_16;  /* Use consistent font */

    lv_area_t a = { .x1 = 8, .y1 = 6, .x2 = st->w - 9, .y2 = 40 };
    lv_draw_label(layer, &dsc, &a);

    if(st->game_over) {
        lv_snprintf(st->text_buf, sizeof(st->text_buf), "GAME OVER SCORE: %" LV_PRIu32, st->score);
        /* Enhanced game over text styling */
        lv_draw_label_dsc_t game_over_dsc;
        lv_draw_label_dsc_init(&game_over_dsc);
        game_over_dsc.color = lv_color_hex(0xff4040);  /* Red color for game over */
        game_over_dsc.opa = LV_OPA_COVER;
        game_over_dsc.text = st->text_buf;
        game_over_dsc.font = &lv_font_montserrat_36;  /* Larger font for emphasis */

        /* Calculate text dimensions */
        lv_point_t txt_size;
        lv_text_attributes_t attr;
        lv_text_attributes_init(&attr);
        attr.letter_space = game_over_dsc.letter_space;
        attr.line_space = game_over_dsc.line_space;
        attr.text_flags = game_over_dsc.flag;
        attr.max_width = LV_COORD_MAX;

        lv_text_get_size(&txt_size, st->text_buf, game_over_dsc.font, &attr);

        /* Create background rectangle */
        lv_area_t bg_area;
        bg_area.x1 = 0;
        bg_area.y1 = st->h / 2 - txt_size.y / 2 - 30;
        bg_area.x2 = st->w;
        bg_area.y2 = bg_area.y1 + txt_size.y + 60;

        /* Draw background rectangle */
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = VD_COLOR_THEME_DARK;  /* Dark background */
        rect_dsc.bg_opa = LV_OPA_80;  /* Semi-transparent */
        rect_dsc.radius = 20;  /* Rounded corners */
        lv_draw_rect(layer, &rect_dsc, &bg_area);

        /* Position text in the center of the background */
        lv_area_t game_over_area;
        game_over_area.x1 = (st->w - txt_size.x) / 2;
        game_over_area.y1 = st->h / 2 - txt_size.y / 2 - 10;
        game_over_area.x2 = game_over_area.x1 + txt_size.x;
        game_over_area.y2 = game_over_area.y1 + txt_size.y;

        lv_draw_label(layer, &game_over_dsc, &game_over_area);
    }
}

static void vd_draw_trail(lv_layer_t * layer, vd_state_t * st)
{
    if(!st->player.alive || st->trail_count < 3) return;

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_HIGH);

    /* Start from the oldest point (tail) */
    int start_idx = (st->trail_head - st->trail_count + VD_PLAYER_TRAIL_CNT) % VD_PLAYER_TRAIL_CNT;
    lv_vector_path_move_to(path, &st->trail[start_idx]);

    /* Draw smooth cubic Bézier curve through points */
    for(int i = 1; i < st->trail_count - 1; i++) {
        int prev_idx = (start_idx + i - 1) % VD_PLAYER_TRAIL_CNT;
        int curr_idx = (start_idx + i) % VD_PLAYER_TRAIL_CNT;
        int next_idx = (start_idx + i + 1) % VD_PLAYER_TRAIL_CNT;

        /* Calculate control points for smooth curve */
        lv_fpoint_t cp1 = {
            (st->trail[curr_idx].x + st->trail[next_idx].x) / 2,
            (st->trail[curr_idx].y + st->trail[next_idx].y) / 2
        };

        lv_fpoint_t cp2 = {
            (st->trail[curr_idx].x + st->trail[prev_idx].x) / 2,
            (st->trail[curr_idx].y + st->trail[prev_idx].y) / 2
        };

        lv_vector_path_cubic_to(path, &cp1, &cp2, &st->trail[curr_idx]);
    }

    /* Gradient descriptor for fading trail */
    lv_grad_stop_t stops[3] = {
        {VD_COLOR_THEME, LV_OPA_10, 0},
        {VD_COLOR_THEME, LV_OPA_40, 128},
        {VD_COLOR_THEME, LV_OPA_80, 255}
    };

    lv_vector_dsc_t * vdsc = lv_vector_dsc_create(layer);
    lv_vector_dsc_set_fill_linear_gradient(vdsc,
                                           st->trail[(st->trail_head - st->trail_count + VD_PLAYER_TRAIL_CNT) % VD_PLAYER_TRAIL_CNT].x,
                                           st->trail[(st->trail_head - st->trail_count + VD_PLAYER_TRAIL_CNT) % VD_PLAYER_TRAIL_CNT].y,
                                           st->trail[(st->trail_head - 1 + VD_PLAYER_TRAIL_CNT) % VD_PLAYER_TRAIL_CNT].x,
                                           st->trail[(st->trail_head - 1 + VD_PLAYER_TRAIL_CNT) % VD_PLAYER_TRAIL_CNT].y);
    lv_vector_dsc_set_fill_gradient_color_stops(vdsc, stops, 3);
    lv_vector_dsc_set_fill_opa(vdsc, LV_OPA_COVER);

    lv_vector_dsc_add_path(vdsc, path);
    lv_draw_vector(vdsc);

    lv_vector_path_delete(path);
    lv_vector_dsc_delete(vdsc);
}

static void vd_painting_cb(lv_obj_t * obj, lv_layer_t * layer, void * user_data)
{
    LV_UNUSED(obj);
    vd_state_t * st = (vd_state_t *)user_data;

    /* Draw all game elements */
    vd_draw_background(layer, st);
    vd_draw_trail(layer, st);
    vd_draw_obstacles(layer, st);
    vd_draw_player(layer, st);
    vd_draw_hud(layer, st);
}

static void vd_painting_end_cb(lv_obj_t * obj, void * user_data)
{
    LV_UNUSED(obj);
    LV_UNUSED(user_data);
}

/*****************************
 * Event handlers            *
 *****************************/

static void vd_on_pointer(lv_event_t * e)
{
    vd_state_t * st = (vd_state_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED) {
        st->dragging = 1;
    }
    else if(code == LV_EVENT_RELEASED) {
        st->dragging = 0;
    }
    else if(code == LV_EVENT_PRESSING) {
        /* Get current pointer position */
        lv_indev_t * indev = lv_indev_active();
        if(indev) {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            st->drag_last = p;
            st->drag_last.y -= FINGER_OFFSET;
        }
        else {
            /* Fallback for some platforms */
            const lv_point_t * pt = lv_event_get_param(e);
            if(pt) st->drag_last = *pt;
            st->drag_last.y -= FINGER_OFFSET;
        }
    }
}

static void vd_on_size_changed(lv_event_t * e)
{
    vd_state_t * st = (vd_state_t *)lv_event_get_user_data(e);

    st->w = lv_obj_get_width(st->canvas);
    st->h = lv_obj_get_height(st->canvas);

    /* Generate background stars (fixed seed for consistent look) */
    uint32_t seed = 0x114514u;
    for(int i = 0; i < VD_BG_STAR_COUNT; i++) {
        st->stars[i].x = vd_irand_range(&seed, 0, st->w - 1);
        st->stars[i].y = vd_irand_range(&seed, 0, st->h - 1);
    }
}

static void restart_btn_event_cb(lv_event_t * e)
{
    vd_state_t * st = (vd_state_t *)lv_event_get_user_data(e);
    vd_reset(st);
    lv_obj_invalidate(st->canvas);
}

/*****************************
 * Timer                     *
 *****************************/

static void vd_timer_cb(lv_timer_t * t)
{
    vd_state_t * st = (vd_state_t *)lv_timer_get_user_data(t);
    uint32_t now = vd_millis();
    vd_update(st, now);
    lv_obj_invalidate(st->canvas);
}

/*****************************
 * Public entry              *
 *****************************/

void lv_demo_vector_dodger(void)
{
    /* Use parent dimensions */
    lv_obj_t * scr = lv_screen_active();
    const int32_t w = lv_obj_get_width(scr);
    const int32_t h = lv_obj_get_height(scr);

    /* Create Canvas (DIRECT mode) */
    lv_obj_t * canvas = lv_canvas_create(scr);
    lv_obj_set_size(canvas, LV_PCT(100), LV_PCT(100));
    lv_obj_center(canvas);

    /* Create restart button (initially hidden) */
    lv_obj_t * btn = lv_button_create(canvas);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);

    /* Style the button to match game theme */
    lv_obj_set_style_bg_color(btn, VD_COLOR_THEME, 0);  /* Green background */
    lv_obj_set_style_border_color(btn, VD_COLOR_THEME_BORDER, 0);
    lv_obj_set_style_border_width(btn, 2, 0);
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_shadow_color(btn, VD_COLOR_THEME, 0);
    lv_obj_set_style_shadow_width(btn, 5, 0);
    lv_obj_set_style_shadow_spread(btn, 2, 0);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "RESTART");
    lv_obj_center(label);

    /* Style the label */
    lv_obj_set_style_text_color(label, VD_COLOR_THEME_DARK, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);

    /* Allocate and initialize game state */
    lv_memzero(&g_st, sizeof(g_st));
    g_st.canvas = canvas;
    g_st.restart_btn = btn;
    g_st.w = w;
    g_st.h = h;

    /* Game initialization */
    vd_reset(&g_st);

    /* Setup DIRECT mode drawing callbacks */
    lv_canvas_set_mode(canvas, LV_CANVAS_MODE_DIRECT);
    lv_canvas_set_painting_cb(canvas, vd_painting_cb);
    lv_canvas_set_painting_end_cb(canvas, vd_painting_end_cb);
    lv_canvas_set_painting_data(canvas, &g_st);

    /* Input events */
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(canvas, vd_on_pointer, LV_EVENT_PRESSED, &g_st);
    lv_obj_add_event_cb(canvas, vd_on_pointer, LV_EVENT_PRESSING, &g_st);
    lv_obj_add_event_cb(canvas, vd_on_pointer, LV_EVENT_RELEASED, &g_st);
    lv_obj_add_event_cb(canvas, vd_on_size_changed, LV_EVENT_SIZE_CHANGED, &g_st);

    /* Restart button event */
    lv_obj_add_event_cb(btn, restart_btn_event_cb, LV_EVENT_CLICKED, &g_st);

    /* Game update timer */
    lv_timer_t * timer = lv_timer_create(vd_timer_cb, VD_TIMER_PERIOD, &g_st);
    LV_UNUSED(timer);

    /* Initial redraw */
    lv_obj_invalidate(canvas);
}

#endif /*LV_USE_DEMO_VECTOR_GRAPHIC*/
