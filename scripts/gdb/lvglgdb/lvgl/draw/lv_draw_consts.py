"""
Auto-generated draw constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

DRAW_TASK_TYPE_NAMES = {
    0: "NONE",
    1: "FILL",
    2: "BORDER",
    3: "BOX_SHADOW",
    4: "LETTER",
    5: "LABEL",
    6: "IMAGE",
    7: "LAYER",
    8: "LINE",
    9: "ARC",
    10: "TRIANGLE",
    11: "MASK_RECTANGLE",
    12: "MASK_BITMAP",
    13: "BLUR",
    14: "VECTOR",
    15: "3D",
}

DRAW_TASK_STATE_NAMES = {
    0: "BLOCKED",
    1: "WAITING",
    2: "QUEUED",
    3: "IN_PROGRESS",
    4: "FINISHED",
}

DRAW_UNIT_TYPE_NAMES = {
    "DAVE2D": "lv_draw_dave2d_unit_t",
    "DMA2D": "lv_draw_dma2d_unit_t",
    "ESP_PPA": "lv_draw_ppa_unit_t",
    "G2D": "lv_draw_g2d_unit_t",
    "NANOVG": "lv_draw_nanovg_unit_t",
    "NEMA_GFX": "lv_draw_nema_gfx_unit_t",
    "NXP_PXP": "lv_draw_pxp_unit_t",
    "OPENGLES": "lv_draw_opengles_unit_t",
    "SDL": "lv_draw_sdl_unit_t",
    "SW": "lv_draw_sw_unit_t",
    "SW_ARM2D": "lv_draw_sw_unit_t",
    "VG_LITE": "lv_draw_vg_lite_unit_t",
}
