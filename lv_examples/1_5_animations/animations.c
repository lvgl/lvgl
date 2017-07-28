/*
 *
    static lv_style_t s1;
    lv_style_get(LV_STYLE_BTN_REL, &s1);

    static lv_style_t s2;
    lv_style_get(LV_STYLE_BTN_REL, &s2);
    s2.radius = 30;
    s2.mcolor = COLOR_RED;
    s2.bwidth = 8;
    s2.opa = 50;
    s2.hpad = 80;
    //s2.font = font_get(FONT_DEJAVU_60);


    lv_style_anim_t a;
    a.act_time = -1000;
    a.time = 1000;
    a.playback = 1;
    a.playback_pause = 300;
    a.repeat = 1;
    a.repeat_pause = 300;
    a.end_cb = NULL;
    a.style_anim = lv_style_get(LV_STYLE_BTN_REL, NULL);
    a.style_start = &s1;
    a.style_end = &s2;
    lv_style_anim_create(&a);
 */
