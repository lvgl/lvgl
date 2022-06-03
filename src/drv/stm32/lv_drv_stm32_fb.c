/**to_
 * @file lv_drv_stm32_fb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_stm32_fb.h"
#include "stm32f7xx_hal.h"
#if LV_USE_DRV_STM32_LTDC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t send_image(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf);

static void DMA_Config(void);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_COLOR_DEPTH == 16
    typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    typedef uint32_t uintpixel_t;
#endif

static DMA_HandleTypeDef      DmaHandle;
static int32_t            	  y_to_flush;
static lv_area_t			  area_to_flush;
static const lv_color_t * 	  buf_to_flush;
static lv_disp_drv_t * 		  disp_drv_to_flush;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_drv_stm32_layer_init(lv_drv_stm32_layer_t * drv)
{
	static bool inited = false;
	if(!inited) {
		DMA_Config();
		inited = true;
	}

    lv_memset_00(drv, sizeof(*drv));
}

lv_disp_drv_t * lv_drv_stm32_layer_create(lv_drv_stm32_layer_t * drv)
{
    drv->base.send_image_cb = send_image;

	LTDC_LayerCfgTypeDef  layer_cfg;

	/* Layer Init */
	layer_cfg.WindowX0 = drv->area.x1;
	layer_cfg.WindowX1 = drv->area.x2;
	layer_cfg.WindowY0 = drv->area.y1;
	layer_cfg.WindowY1 = drv->area.y2;

#if LV_COLOR_DEPTH == 16
	layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
	layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
#endif

	layer_cfg.FBStartAdress = (uint32_t)drv->frame_buffer;
	layer_cfg.Alpha = 255;
	layer_cfg.Alpha0 = 0;
	layer_cfg.Backcolor.Blue = 0;
	layer_cfg.Backcolor.Green = 0;
	layer_cfg.Backcolor.Red = 0;
	layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	layer_cfg.ImageWidth = lv_area_get_width(&drv->area);
	layer_cfg.ImageHeight = lv_area_get_height(&drv->area);

	HAL_LTDC_ConfigLayer(drv->ltdc_handler, &layer_cfg, drv->index);

    lv_disp_drv_t * disp_drv = lv_mem_alloc(sizeof(lv_disp_drv_t));
    LV_ASSERT_MALLOC(disp_drv);

    lv_disp_draw_buf_t * draw_buf = lv_mem_alloc(sizeof(lv_disp_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);

    if(disp_drv == NULL || draw_buf == NULL) {
        lv_mem_free(disp_drv);
        lv_mem_free(draw_buf);
        return NULL;
    }

    lv_disp_drv_init(disp_drv);

    lv_disp_draw_buf_init(draw_buf, drv->draw_buf1, drv->draw_buf2, drv->draw_buf_size);
    disp_drv->draw_buf = draw_buf;
    disp_drv->user_data = drv;

    return disp_drv;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t send_image(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf)
{
	/*Wait until DMA is busy*/
	while(disp_drv_to_flush);

    lv_drv_stm32_layer_t * layer_drv = (lv_drv_stm32_layer_t *) drv;

    area_to_flush = *area;
    y_to_flush = area->y1;
    buf_to_flush = buf;
    disp_drv_to_flush = disp_drv;

    SCB_CleanInvalidateDCache();
    SCB_InvalidateICache();

    HAL_StatusTypeDef err;
    uint32_t length = lv_area_get_width(area);
#if LV_COLOR_DEPTH == 32
    length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
    lv_coord_t fb_w = lv_area_get_width(&layer_drv->area);
    err = HAL_DMA_Start_IT(&DmaHandle, (uint32_t)buf_to_flush,
                           (uint32_t)&layer_drv->frame_buffer[y_to_flush * fb_w + area->x1],
                           length);
    if(err != HAL_OK) return LV_RES_INV;

    return LV_RES_OK;
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef * han)
{
    y_to_flush ++;

    if(y_to_flush > area_to_flush.y2) {
        SCB_CleanInvalidateDCache();
        SCB_InvalidateICache();
        lv_disp_flush_ready(disp_drv_to_flush);
        disp_drv_to_flush = NULL;
    }
    else {
        uint32_t length = lv_area_get_width(&area_to_flush);
        buf_to_flush += length;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        lv_drv_stm32_layer_t * layer_drv = disp_drv_to_flush->user_data;
		lv_coord_t fb_w = lv_area_get_width(&layer_drv->area);
        if(HAL_DMA_Start_IT(han, (uint32_t)buf_to_flush, (uint32_t)&layer_drv->frame_buffer[y_to_flush * fb_w + area_to_flush.x1],
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
	LV_ASSERT_MSG(0, "DMA transfer error");
}


static void DMA_Config(void)
{
    /*## -1- Enable DMA2 clock #################################################*/
    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-2- Select the DMA functional Parameters ###############################*/
    DmaHandle.Init.Channel = DMA_CHANNEL_0;                   /* DMA_CHANNEL_0                    */
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
    DmaHandle.Instance = DMA2_Stream0;

    /*##-4- Initialize the DMA stream ##########################################*/
    if(HAL_DMA_Init(&DmaHandle) != HAL_OK) {
        while(1) {
        }
    }

    /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream0_IRQHandler(void)
{
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(&DmaHandle);
}

#endif
