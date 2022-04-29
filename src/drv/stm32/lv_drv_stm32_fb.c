/**
 * @file lv_drv_stm32_fb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_stm32_fb.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t disp_flush(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf);

/**********************
 *  STATIC VARIABLES
 **********************/
static LTDC_HandleTypeDef  hLtdcHandler;

#if LV_COLOR_DEPTH == 16
    typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    typedef uint32_t uintpixel_t;
#endif

static DMA_HandleTypeDef  DmaHandle;
static int32_t            x1_flush;
static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_drv_stm32_fb_init(lv_drv_esp_t * drv)
{
    static bool inited = false;
    if(!inited) {
        LCD_Config();
        DMA_Config();
        inited = true;
    }

    lv_memset_00(drv, sizeof(*drv));
    drv->base.send_cmd_cb = reset_dev;
    drv->base.send_cmd_cb = send_cmd;
    drv->base.read_data_cb = read_data;
    drv->base.disp_flush_cb = disp_flush;

}

void lv_drv_stm32_fb_create(lv_drv_stm32_fb_t * drv)
{
    LTDC_LayerCfgTypeDef  layer_cfg;

    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = drv->width;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = drv->height;

#if LV_COLOR_DEPTH == 16
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
#endif

    layer_cfg.FBStartAdress = (uint32_t)drv->fb;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = drv->width;
    layer_cfg.ImageHeight = drv->height;

    HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static lv_res_t disp_flush(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf)
{
    lv_drv_stm32_fb_t * stm_drv = (lv_drv_stm32_fb_t *) drv;

    int32_t x1 = area->x1;
    int32_t x2 = area->x2;
    int32_t y1 = area->y1;
    int32_t y2 = area->y2;

    /*Return if the area is out the screen*/
    if(x2 < 0) return LV_RES_OK;
    if(y2 < 0) return LV_RES_OK;
    if(x1 > stm_drv->width - 1) return LV_RES_OK;
    if(y1 > stm_drv->height - 1) return LV_RES_OK;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > stm_drv->width - 1 ? stm_drv->width - 1 : x2;
    int32_t act_y2 = y2 > stm_drv->height - 1 ? stm_drv->height - 1 : y2;

    x1_flush = act_x1;
    y1_flush = act_y1;
    x2_flush = act_x2;
    y2_fill = act_y2;
    y_fill_act = act_y1;
    buf_to_flush = buf;

    SCB_CleanInvalidateDCache();
    SCB_InvalidateICache();
    /*##-7- Start the DMA transfer using the interrupt mode #*/
    /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
    /* Enable All the DMA interrupts */
    HAL_StatusTypeDef err;
    uint32_t length = (x2_flush - x1_flush + 1);
#if LV_COLOR_DEPTH == 32
    length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
    err = HAL_DMA_Start_IT(&DmaHandle, (uint32_t)buf_to_flush,
                           (uint32_t)&stm_drv->frame_buffer[y_fill_act * stm_drv->width + x1_flush],
                           length);
    if(err != HAL_OK) return LV_RES_INV;
}



/**
 * Clock and pins should be set be the user
 * @return
 */
static void LCD_Init(void)
{
    /* Select the used LCD */

    /* The RK043FN48H LCD 480x272 is selected */
    /* Timing Configuration */
    hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
    hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
    hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
    hLtdcHandler.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

    /* Initialize the LCD pixel width and pixel height */
    hLtdcHandler.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
    hLtdcHandler.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

    /* Background value */
    hLtdcHandler.Init.Backcolor.Blue = 0;
    hLtdcHandler.Init.Backcolor.Green = 0;
    hLtdcHandler.Init.Backcolor.Red = 0;

    /* Polarity */
    hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hLtdcHandler.Instance = LTDC;

    HAL_LTDC_Init(&hLtdcHandler);

}

static void DMA_Config(void)
{
    /*## -1- Enable DMA2 clock #################################################*/
    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-2- Select the DMA functional Parameters ###############################*/
    DmaHandle.Init.Channel = CPY_BUF_DMA_CHANNEL;                   /* DMA_CHANNEL_0                    */
    DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;                /* M2M transfer mode                */
    DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;                     /* Peripheral increment mode Enable */
    DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                        /* Memory increment mode Enable     */
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Peripheral data alignment : 16bit */
    DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;      /* memory data alignment : 16bit     */
    DmaHandle.Init.Mode = DMA_NORMAL;                               /* Normal DMA mode                  */
    DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;                    /* priority level : high            */
    DmaHandle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;                  /* FIFO mode enabled                */
    DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL; /* FIFO threshold: 1/4 full   */
    DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;                    /* Memory burst                     */
    DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;                 /* Peripheral burst                 */

    /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
    DmaHandle.Instance = CPY_BUF_DMA_STREAM;

    /*##-4- Initialize the DMA stream ##########################################*/
    if(HAL_DMA_Init(&DmaHandle) != HAL_OK) {
        while(1) {
        }
    }

    /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    HAL_NVIC_SetPriority(CPY_BUF_DMA_STREAM_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(CPY_BUF_DMA_STREAM_IRQ);
}


/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef * han)
{
    y_fill_act ++;

    if(y_fill_act > y2_fill) {
        SCB_CleanInvalidateDCache();
        SCB_InvalidateICache();
        lv_disp_flush_ready(&disp_drv);
    }
    else {
        uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += x2_flush - x1_flush + 1;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        if(HAL_DMA_Start_IT(han, (uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * TFT_HOR_RES + x1_flush],
                            length) != HAL_OK) {
            while(1);   /*Halt on error*/
        }
    }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void DMA_TransferError(DMA_HandleTypeDef * han)
{

}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void CPY_BUF_DMA_STREAM_IRQHANDLER(void)
{
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(&DmaHandle);
}
