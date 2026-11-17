/**
 * @file lv_port_disp_template.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>

#include "gd32h7xx.h"
#include "gd32h759i_lcd_eval.h"
#if LV_USE_DRAW_IPA && LV_USE_DRAW_IPA_INTERRUPT
    #include "../../src/draw/gigadevice/ipa/lv_draw_ipa.h"
    #if LV_USE_OS == LV_OS_FREERTOS
        #include "FreeRTOS.h"
    #endif
#endif
/*********************
 *      DEFINES
 *********************/
#define LV_HOR_RES_MAX          (480)
#define LV_VER_RES_MAX          (272)
#define USE_IPA_INT    1
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
/* TLI buf */
__ALIGNED(32) lv_color_t buf_disp[LV_HOR_RES_MAX * LV_VER_RES_MAX];

#if (USE_IPA_INT == 1)
volatile uint8_t g_ipa_flag = 0;
static lv_display_t * g_disp_drv;
#endif
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_display_set_flush_cb(disp, disp_flush);

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_1[LV_HOR_RES_MAX * 100 * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_2[LV_HOR_RES_MAX * 100 * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

#if LV_USE_DRAW_IPA && LV_USE_DRAW_IPA_INTERRUPT && LV_USE_OS == LV_OS_FREERTOS
    nvic_irq_enable(IPA_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
#else
    nvic_irq_enable(IPA_IRQn, 2, 0);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
        /*You code here*/
    /* ipa init */
    rcu_periph_clock_enable(RCU_IPA);
    /* initilize the LCD and layers */
    lcd_init();
#if(LV_BUF_TYPE==3)
    lcd_layer_init(LCD_LAYER_BACKGROUND, (uint32_t)buf_2_1, LV_HOR_RES_MAX, LV_VER_RES_MAX, LAYER_PPF_RGB565);
#else
    lcd_layer_init(LCD_LAYER_BACKGROUND, (uint32_t)buf_disp, LV_HOR_RES_MAX, LV_VER_RES_MAX, LAYER_PPF_RGB565);
#endif
    lcd_layer_enable(LCD_LAYER_BACKGROUND);
    lcd_layer_set(LCD_LAYER_BACKGROUND,(uint32_t)buf_disp);
    lcd_transparency_set(255);
//    lcd_clear(LCD_COLOR_BLACK);
    tli_enable();
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
//    disp_clean_dcache(disp_drv);
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#if (USE_IPA_INT == 0)
    uint32_t time_out;
#endif
    uint16_t offline;
    offline = LV_HOR_RES_MAX - (area->x2 - area->x1 + 1);
    
    while(1 == g_ipa_flag){
    }
    
    IPA_CTL = 0x0;
    IPA_FMADDR = (uint32_t)px_map;
    IPA_DMADDR = (uint32_t)buf_disp + 2*(LV_HOR_RES_MAX*area->y1+area->x1);
    IPA_FLOFF = 0;
    IPA_DLOFF = offline;
    IPA_FPCTL = FOREGROUND_PPF_RGB565;
    IPA_IMS = (uint32_t)((area->x2 - area->x1 + 1) << 16) | (uint16_t)(area->y2 - area->y1 + 1);
    
#if (USE_IPA_INT == 1)
    g_disp_drv = disp_drv;
    /* enable IPA interrupt */
    g_ipa_flag = 1;
    IPA_CTL |= IPA_CTL_FTFIE | IPA_CTL_TAEIE | IPA_CTL_WCFIE;
    IPA_CTL |= IPA_CTL_TEN;
#else
    IPA_CTL |= IPA_CTL_TEN;
    /* Wait until transfer is done */
    while (IPA_CTL & IPA_CTL_TEN)
    {
        if(time_out++ >= 0XFFFFFFFF) break;
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
#endif
}

void IPA_IRQHandler(void)
{
    if(RESET != ipa_interrupt_flag_get(IPA_INT_FLAG_FTF)){
        if(0U != (IPA_CTL & IPA_CTL_FTFIE)){
            IPA_CTL &= (uint32_t)(~IPA_CTL_FTFIE);
            ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);
            
            if(1U == g_ipa_flag){
                g_ipa_flag = 0U;
                /* IMPORTANT!!!
                 * Inform the graphics library that you are ready with the flushing*/
                lv_disp_flush_ready(g_disp_drv);
            }
#if LV_USE_DRAW_IPA && LV_USE_DRAW_IPA_INTERRUPT
            else {
                lv_draw_ipa_transfer_complete_interrupt_handler();
            }
#endif
        }
    }
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
