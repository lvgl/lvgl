# LVGL Over NEMA® | GFX
Use TSi's (aka Think Silicon) acceleration library NEMA® | GFX to off-load LVGL-CPU rendering to any TSi GPU (NEMA® Pico, NEMA® VG, NEOX®).
NEMA® | GFX Library is a low level library that interfaces directly with the NEMA® GPUs and provides a software abstraction layer to organize and employ drawing commands with ease and efficiency. The target of NEMA® | GFX is to be able to be used as a back-end to existing APIs (such as OpenGL® , DirectFB or any proprietary one) but also to expose higher level drawing functions, so as to be used as a standalone Graphics API. Its small footprint, efficient design and lack of any external dependencies, makes it ideal for use in embedded applications. By leveraging NEMA®’s sophisticated architecture, it allows great performance with minimum CPU/MCU usage and power consumption.

This implementation accelerates lvgl version 8.3.

## LVGL features supported
This implementation makes use of the function pointers (hooks) provided by lvgl in file `lv_draw_sw.c`. Currently the following functions are being overriden and accelerated through the GPU:

```c
void lv_draw_nema_gfx_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_nema_gfx_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_nema_gfx_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_nema_gfx_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_nema_gfx_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_nema_gfx_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

void lv_draw_nema_gfx_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                       uint32_t letter);
```
In these functions, there are cases of features which are either not supported or not accelerated yet. For those cases the program execution falls back to LVGL-CPU rendering.

The source code of this implementation lies in `lvgl/src/draw/nema_gfx` folder.

### Features Supported
    -LV_IMG_CF_TRUE_COLOR images operations
    -Texture Transformation
    -Blending Operations
    -Fonts
    -Antialiasing
    -Shapes drawing

## Usage
Copy file `lv_conf_template.h` as `lv_conf.h` simply next to the `lvgl` folder and enable the use of NemaGFX through the corresponding define.

```c
#define LV_USE_NEMA_GFX 1
```
Build the lvgl library for your system and link the example with NEMA® | GFX Library provided.

# About Think-Silicon
Think-Silicon, an Applied Materials Company, is a leading provider of ultra-low power GPU IP for embedded systems with over 15 years of experience designing IP for extremely small, ultra-low power, high-performance 2D and 3D graphics, video, display and machine learning-accelerated microprocessor cores.
Our high-performance graphics IP technology targets ultra-low power and area-limited devices, such as wearables, home control appliances and home entertainment devices.
More about Think-Silicon, an Applied Materials Company, can be found at https://www.think-silicon.com/


