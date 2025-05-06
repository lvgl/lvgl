#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE
/*
@file    EVE_target.c
@brief   target specific functions for plain C targets
@version 5.0
@date    2023-10-01
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- changed the DMA buffer from uin8_t to uint32_t
- added a section for Arduino-ESP32
- corrected the clock-divider settings for ESP32
- added DMA to ARDUINO_METRO_M4 target
- added DMA to ARDUINO_NUCLEO_F446RE target
- added DMA to Arduino-ESP32 target
- added a native ESP32 target with DMA
- added an ARDUINO_TEENSY41 target with DMA support
- added DMA for the Raspberry Pi Pico - RP2040
- added ARDUINO_TEENSY35 to the ARDUINO_TEENSY41 target
- transferred the little experimental STM32 code I had over from my experimental branch
- added S32K144 support including DMA
- split up this file in EVE_target.c for the plain C targets and EVE_target.cpp for the Arduino C++ targets
- converted all TABs to SPACEs
- split the ATSAMC21 and ATSAMx51 targets into separate sections
- added more defines for ATSAMC21 and ATSAMx51 - chip crises...
- added DMA support for the GD32C103 target
- fixed the ESP32 target to work with the ESP32-S3 as well
- basic maintenance: checked for violations of white space and indent rules
- changed the code for ATSAMC21 and ATSAMx51 targets to use EVE_SPI_SERCOM
- ESP32: fix for building with ESP-IDF 5.x
- ESP32: changed a couple of {0} to {}
- ESP32: changed back a couple of {} to {0} as C and C++ are not the same thing...
- Bugfix issue #89: ESP32 not initializing if GPIO pin number > 31
- moved the include for EVE_target.h to avoid the empty translation unit warning
 from -Wpedantic when building for Arduino

 */

#include "EVE_target.h"

#if !defined (ARDUINO)

#include "EVE_commands.h"

#if defined (__GNUC__)

/* ################################################################## */
/* ################################################################## */

#if defined (__SAMC21E18A__) \
    || defined (__SAMC21J18A__) \
    || defined (__SAMC21J17A__) \
    || defined (__SAMC21J16A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

void DELAY_MS(uint16_t val)
{
    while (val > 0U)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __asm__ volatile ("nop");
        }
        val--;
    }
}

#if defined (EVE_DMA)

static DmacDescriptor dmadescriptor __attribute__((aligned(16)));
static DmacDescriptor dmawriteback __attribute__((aligned(16)));
uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;

void EVE_init_dma(void)
{
    DMAC->CTRL.reg = 0;
    while (DMAC->CTRL.bit.DMAENABLE);
    DMAC->CTRL.bit.SWRST = 1;
    while (DMAC->CTRL.bit.SWRST); /* wait for the software-reset to be complete */

    DMAC->BASEADDR.reg = (uint32_t) &dmadescriptor;
    DMAC->WRBADDR.reg = (uint32_t) &dmawriteback;

    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_TRIGSRC(EVE_SPI_DMA_TRIGGER); /* beat-transfer, SERCOM0 TX Trigger, level 0, channel-event input / output disabled */
    DMAC->CHID.reg = EVE_DMA_CHANNEL; /* select channel */
    DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */

    dmadescriptor.BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID; /* increase source-address, beat-size = 8-bit */
    dmadescriptor.DSTADDR.reg = (uint32_t) &EVE_SPI_SERCOM->SPI.DATA.reg;
    dmadescriptor.DESCADDR.reg = 0; /* no next descriptor */

    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    NVIC_SetPriority(DMAC_IRQn, 0);
    NVIC_EnableIRQ(DMAC_IRQn);
}

void EVE_start_dma_transfer(void)
{
    dmadescriptor.BTCNT.reg = (EVE_dma_buffer_index * 4U) - 1U;
    dmadescriptor.SRCADDR.reg = (uint32_t) &EVE_dma_buffer[EVE_dma_buffer_index]; /* note: last entry in array + 1 */
    EVE_SPI_SERCOM->SPI.CTRLB.bit.RXEN = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
    EVE_cs_set();
    DMAC->CHCTRLA.bit.ENABLE = 1; /* start sending out EVE_dma_buffer ?*/
    EVE_dma_busy = 42;
}

/* executed at the end of the DMA transfer */
void DMAC_Handler()
{
    DMAC->CHID.reg = EVE_DMA_CHANNEL; /* we only use one channel, so this should not even change */
    DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; /* ack irq */
    while (0U == EVE_SPI_SERCOM->SPI.INTFLAG.bit.TXC); /* wait for the SPI to be done transmitting */
    EVE_SPI_SERCOM->SPI.CTRLB.bit.RXEN = 1; /* switch receiver on by setting RXEN to 1 which is not enable protected */
    EVE_cs_clear();
    EVE_dma_busy = 0;
}

#endif /* DMA */
#endif /* ATSAMC21 */

/* ################################################################## */
/* ################################################################## */

#if defined (__SAME51J19A__) \
    || defined (__SAME51J18A__) \
    || defined (__SAMD51P20A__) \
    || defined (__SAMD51J19A__) \
    || defined (__SAMD51G18A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

void DELAY_MS(uint16_t val)
{
    while (val > 0U)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __asm__ volatile ("nop");
        }
        val--;
    }
}

#if defined (EVE_DMA)

static DmacDescriptor dmadescriptor __attribute__((aligned(16)));
static DmacDescriptor dmawriteback __attribute__((aligned(16)));
uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;

void EVE_init_dma(void)
{
    DMAC->CTRL.reg = 0;
    while (DMAC->CTRL.bit.DMAENABLE);
    DMAC->CTRL.bit.SWRST = 1;
    while (DMAC->CTRL.bit.SWRST); /* wait for the software-reset to be complete */

    DMAC->BASEADDR.reg = (uint32_t) &dmadescriptor;
    DMAC->WRBADDR.reg = (uint32_t) &dmawriteback;
    DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */
    DMAC->Channel[EVE_DMA_CHANNEL].CHCTRLA.reg =
        DMAC_CHCTRLA_BURSTLEN_SINGLE |
        DMAC_CHCTRLA_TRIGACT_BURST |
        DMAC_CHCTRLA_TRIGSRC(EVE_SPI_DMA_TRIGGER);

    dmadescriptor.BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID; /* increase source-address, beat-size = 8-bit */
    dmadescriptor.DSTADDR.reg = (uint32_t) &EVE_SPI_SERCOM->SPI.DATA.reg;
    dmadescriptor.DESCADDR.reg = 0; /* no next descriptor */

    DMAC->Channel[EVE_DMA_CHANNEL].CHINTENSET.bit.TCMPL = 1; /* enable transfer complete interrupt */
    DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */

    NVIC_SetPriority(DMAC_0_IRQn, 0);
    NVIC_EnableIRQ(DMAC_0_IRQn);
}

void EVE_start_dma_transfer(void)
{
    dmadescriptor.BTCNT.reg = (EVE_dma_buffer_index * 4U) - 1U;
    dmadescriptor.SRCADDR.reg = (uint32_t) &EVE_dma_buffer[EVE_dma_buffer_index]; /* note: last entry in array + 1 */
    EVE_SPI_SERCOM->SPI.CTRLB.bit.RXEN = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
    EVE_cs_set();
    DMAC->Channel[EVE_DMA_CHANNEL].CHCTRLA.bit.ENABLE = 1; /* start sending out EVE_dma_buffer */
    EVE_dma_busy = 42;
}

/* executed at the end of the DMA transfer */
void DMAC_0_Handler()
{
    DMAC->Channel[EVE_DMA_CHANNEL].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; /* ack irq */
    while (0U == EVE_SPI_SERCOM->SPI.INTFLAG.bit.TXC); /* wait for the SPI to be done transmitting */
    EVE_SPI_SERCOM->SPI.CTRLB.bit.RXEN = 1; /* switch receiver on by setting RXEN to 1 which is not enable protected */
    EVE_dma_busy = 0;
    EVE_cs_clear();
}

#endif /* DMA */
#endif /* ATSAMx51 */

/* ################################################################## */
/* ################################################################## */

/* set with "build_flags" in platformio.ini or as defines in your build environment */
#if defined (STM32L0) \
    || defined (STM32F0) \
    || defined (STM32F1) \
    || defined (STM32F3) \
    || defined (STM32F4) \
    || defined (STM32G4) \
    || defined (STM32H7) \
    || defined (STM32G0)

#include "EVE_target.h"
#include "EVE_commands.h"

SPI_HandleTypeDef eve_spi_handle;

#if 0
void EVE_init_spi(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;

    /* we have CS on D9 of the Nucleo-64, this is PC7 */
    gpio_init.Pin = EVE_CS;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EVE_CS_PORT, &gpio_init);

    EVE_cs_clear();

    /* we have PDN on D8 of the Nucleo-64, this is PA9 */
    gpio_init.Pin = EVE_PDN;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EVE_PDN_PORT, &gpio_init);

    EVE_pdn_set();

    /* SPI1 GPIO Configuration: PA5 -> SPI1_SCK, PA6 -> SPI1_MISO, PA7 -> SPI1_MOSI */
    gpio_init.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    eve_spi_handle.Instance = EVE_SPI;
    eve_spi_handle.Init.Mode = SPI_MODE_MASTER; 
    eve_spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
    eve_spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
    eve_spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    eve_spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    eve_spi_handle.Init.NSS = SPI_NSS_SOFT;
    eve_spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    eve_spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    eve_spi_handle.Init.TIMode = SPI_TIMODE_DISABLED;
    eve_spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    eve_spi_handle.Init.CRCPolynomial = 3;
    HAL_SPI_Init(&eve_spi_handle);
    __HAL_SPI_ENABLE(&eve_spi_handle);
}
#endif

#if defined (EVE_DMA)

uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;

volatile DMA_HandleTypeDef EVE_dma_tx;

void EVE_init_dma(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();
    eve_dma_handle.Instance = DMA2_Stream3;
    eve_dma_handle.Init.Channel = DMA_CHANNEL_3;
    eve_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    eve_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    eve_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    eve_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    eve_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    eve_dma_handle.Init.Mode = DMA_NORMAL;
    eve_dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    eve_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&eve_dma_handle);
    __HAL_LINKDMA(&eve_spi_handle, hdmatx, eve_dma_handle);
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

void EVE_start_dma_transfer(void)
{
    EVE_cs_set();
    if (HAL_OK == HAL_SPI_Transmit_DMA(&eve_spi_handle, ((uint8_t *) &EVE_dma_buffer[0]) + 1U, ((EVE_dma_buffer_index)* 4U) - 1U))
    {
        EVE_dma_busy = 42;
    }
}

/* DMA-done-Interrupt-Handler */
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&eve_dma_handle);
}

/* Callback for end-of-DMA-transfer */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    EVE_dma_busy = 0;
    EVE_cs_clear();
}

#endif /* DMA */
#endif /* STM32 */

/* ################################################################## */
/* ################################################################## */

#if defined (ESP_PLATFORM) /* ESP32 */

#include "EVE_target.h"

void DELAY_MS(uint16_t ms)
{
    TickType_t ticksMS = pdMS_TO_TICKS(ms);
    if (ticksMS < 2U) ticksMS = 2;
    vTaskDelay(ticksMS);
}

spi_device_handle_t EVE_spi_device = {0};
spi_device_handle_t EVE_spi_device_simple = {0};

static void eve_spi_post_transfer_callback(void)
{
    gpio_set_level(EVE_CS, 1); /* tell EVE to stop listen */
#if defined (EVE_DMA)
        EVE_dma_busy = 0;
#endif
}

void EVE_init_spi(void)
{
    spi_bus_config_t buscfg = {0};
    spi_device_interface_config_t devcfg = {0};
    gpio_config_t io_cfg = {0};

#if ESP_IDF_VERSION_MAJOR <= 4
    io_cfg.intr_type = GPIO_PIN_INTR_DISABLE;
#elif ESP_IDF_VERSION_MAJOR == 5
    io_cfg.intr_type = GPIO_INTR_DISABLE;
#endif
    io_cfg.mode = GPIO_MODE_OUTPUT;
    io_cfg.pin_bit_mask = BIT64(EVE_PDN) | BIT64(EVE_CS);
    gpio_config(&io_cfg);

    gpio_set_level(EVE_CS, 1);
    gpio_set_level(EVE_PDN, 0);

    buscfg.mosi_io_num = EVE_MOSI;
    buscfg.miso_io_num = EVE_MISO;
    buscfg.sclk_io_num = EVE_SCK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz= 4088;

    devcfg.clock_speed_hz = 16U * 1000U * 1000U; /* clock = 16 MHz */
    devcfg.mode = 0;                          /* SPI mode 0 */
    devcfg.spics_io_num = -1;                 /* CS pin operated by app */
    devcfg.queue_size = 3;                    /* we need only one transaction in the que */
    devcfg.address_bits = 24;                 /* 24 bits for the address */
    devcfg.command_bits = 0;                  /* command operated by app */
    devcfg.post_cb = (transaction_cb_t)eve_spi_post_transfer_callback;

    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &EVE_spi_device);

    devcfg.address_bits = 0;
    devcfg.post_cb = 0;
    devcfg.clock_speed_hz = 10U * 1000U * 1000U; /* clock = 10 MHz */
    spi_bus_add_device(SPI2_HOST, &devcfg, &EVE_spi_device_simple);
}

#if defined (EVE_DMA)

uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;

void EVE_init_dma(void)
{
}

void EVE_start_dma_transfer(void)
{
    spi_transaction_t EVE_spi_transaction = {0};
    gpio_set_level(EVE_CS, 0); /* make EVE listen */
    EVE_spi_transaction.tx_buffer = (uint8_t *) &EVE_dma_buffer[1];
    EVE_spi_transaction.length = (EVE_dma_buffer_index-1) * 4U * 8U;
    EVE_spi_transaction.addr = 0x00b02578UL; /* WRITE + REG_CMDB_WRITE; */
    spi_device_queue_trans(EVE_spi_device, &EVE_spi_transaction, portMAX_DELAY);
    EVE_dma_busy = 42;
}

#endif /* DMA */
#endif /* ESP32 */

/* ################################################################## */
/* ################################################################## */

#if defined (RP2040)
/* note: set in platformio.ini by "build_flags = -D RP2040" */

void EVE_init_spi(void)
{
    // chip select is active-low -> initialized to high
    gpio_init(EVE_CS);
    gpio_set_dir(EVE_CS, GPIO_OUT);
    gpio_put(EVE_CS, 1);

    // power-down is active-low -> initialized to low
    gpio_init(EVE_PDN);
    gpio_set_dir(EVE_PDN, GPIO_OUT);
    gpio_put(EVE_PDN, 0);

    spi_init(EVE_SPI, 8000000U);
    gpio_set_function(EVE_MISO, GPIO_FUNC_SPI);
    gpio_set_function(EVE_SCK, GPIO_FUNC_SPI);
    gpio_set_function(EVE_MOSI, GPIO_FUNC_SPI);
}

#if defined (EVE_DMA)

#include "hardware/dma.h"
#include "hardware/irq.h"

uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;
int dma_tx;
dma_channel_config dma_tx_config;

static void EVE_DMA_handler(void)
{
    dma_hw->ints0 = 1U << dma_tx; /* ack irq */
    while ((spi_get_hw(EVE_SPI)->sr & SPI_SSPSR_BSY_BITS) != 0U); /* wait for the SPI to be done transmitting */
    EVE_dma_busy = 0;
    EVE_cs_clear();
}

void EVE_init_dma(void)
{
    dma_tx = dma_claim_unused_channel(true);
    dma_tx_config = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_8);
    channel_config_set_dreq(&dma_tx_config, spi_get_index(EVE_SPI) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    dma_channel_set_irq0_enabled (dma_tx, true);
    irq_set_exclusive_handler(DMA_IRQ_0, EVE_DMA_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void EVE_start_dma_transfer(void)
{
    EVE_cs_set();
    dma_channel_configure(dma_tx, &dma_tx_config,
        &spi_get_hw(EVE_SPI)->dr, // write address
        ((uint8_t *) &EVE_dma_buffer[0]) + 1U, // read address
        (((EVE_dma_buffer_index) * 4U) - 1U), // element count (each element is of size transfer_data_size)
        true); // start transfer
    EVE_dma_busy = 42;
}
#endif /* DMA */

#endif /* RP2040 */

/* ################################################################## */
/* ################################################################## */

#if defined (CPU_S32K148) || defined (CPU_S32K144HFT0VLLT)

void DELAY_MS(uint16_t val)
{
    while (val > 0U)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __asm__ volatile ("nop");
        }
        val--;
    }
}

void EVE_init_spi(void)
{
    /* configure clock for LPSPI instance selected in EVE_target.h */
    PCC->PCCn[EVE_SPI_INDEX] &= ~PCC_PCCn_CGC_MASK;     /* disable clock */
    PCC->PCCn[EVE_SPI_INDEX] &= ~PCC_PCCn_PCS_MASK;     /* clear PCS field */
    PCC->PCCn[EVE_SPI_INDEX] |= PCC_PCCn_PCS(6);        /* PCS = 6 SPLLDIV2_CLK is selected */
    PCC->PCCn[EVE_SPI_INDEX] |= PCC_PCCn_CGC_MASK;      /* enable clock for LPSPIx */

    /* configure LPSPI instance selected in EVE_target.h */
    EVE_SPI->CR = LPSPI_CR_RST_MASK; /* software reset */
    EVE_SPI->CR = 0x00000000; /* disable module for configuration */
    EVE_SPI->CFGR1 = LPSPI_CFGR1_MASTER_MASK; /* master, mode 0 */
    EVE_SPI->TCR = LPSPI_TCR_PRESCALE(0) | LPSPI_TCR_FRAMESZ(7); /* divide peripheral clock by 4, frame size is 8bit */
    EVE_SPI->CCR = LPSPI_CCR_SCKDIV(4); /* SCK cycle is 4+2 = 6 cycles of the peripheral clock */
    EVE_SPI->FCR = 0; /* RXWATER=0: Rx flags set when Rx FIFO >0, TXWATER=0: Tx flags set when Tx FIFO < 1 */
    EVE_SPI->DER = 0x01; /* DMA TX request enabled */
    EVE_SPI->CR |= LPSPI_CR_MEN_MASK; /* MEN = 1 Module is enabled */
}

#if defined (EVE_DMA)

uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index;
volatile uint8_t EVE_dma_busy = 0;

void EVE_init_dma(void)
{
    PCC->PCCn[PCC_DMAMUX_INDEX] |= PCC_PCCn_CGC_MASK; /* DMAMUX */
    DMAMUX->CHCFG[EVE_DMA_CHANNEL] = 0;
    DMAMUX->CHCFG[EVE_DMA_CHANNEL] |= DMAMUX_CHCFG_SOURCE(EVE_DMAMUX_CHCFG_SOURCE);
    DMAMUX->CHCFG[EVE_DMA_CHANNEL] |= DMAMUX_CHCFG_ENBL_MASK;

    S32_NVIC->ISER[(uint32_t)(EVE_DMA_IRQ) >> 5U] = (uint32_t)(1UL << ((uint32_t)(EVE_DMA_IRQ) & (uint32_t)0x1FU)); /* enable DMA IRQ */

    DMA->TCD[EVE_DMA_CHANNEL].CSR = DMA_TCD_CSR_DREQ(1) | DMA_TCD_CSR_INTMAJOR(1); /* automatically clear ERQ bit after one major loop, irq when done */
    DMA->TCD[EVE_DMA_CHANNEL].SOFF = DMA_TCD_SOFF_SOFF(1); /* add 1 after each transfer */
    DMA->TCD[EVE_DMA_CHANNEL].ATTR = 0; /* 8-bit source and destination transfer size */
    DMA->TCD[EVE_DMA_CHANNEL].SLAST = 0;
    DMA->TCD[EVE_DMA_CHANNEL].DADDR = DMA_TCD_DADDR_DADDR((uint32_t) &EVE_SPI->TDR);
    DMA->TCD[EVE_DMA_CHANNEL].DOFF = 0; /* do not increase address after each transfer */
    DMA->TCD[EVE_DMA_CHANNEL].DLASTSGA = 0;
    DMA->TCD[EVE_DMA_CHANNEL].NBYTES.MLNO = DMA_TCD_NBYTES_MLNO_NBYTES(1); /* transfer 1 byte per minor loop */
}

void EVE_start_dma_transfer(void)
{
    uint8_t *bytes = ((uint8_t *) &EVE_dma_buffer[0]) + 1U;
    uint16_t length = (EVE_dma_buffer_index - 1U) * 4U;

    EVE_SPI->TCR |= LPSPI_TCR_RXMSK_MASK; /* disable LPSPI receive */
    EVE_cs_set();

    /* the first three bytes start from an unaligned address so these are send directly */
    for (uint8_t index = 0; index < 3U; index++)
    {
        EVE_SPI->SR |= LPSPI_SR_TDF_MASK; /* clear transmit data flag */
        EVE_SPI->TDR = bytes[index]; /* transmit data */
        while (0U == (EVE_SPI->SR & LPSPI_SR_TDF_MASK));
    }

    DMA->TCD[EVE_DMA_CHANNEL].SADDR = DMA_TCD_SADDR_SADDR((uint32_t) ((&EVE_dma_buffer[1])));
    DMA->TCD[EVE_DMA_CHANNEL].CITER.ELINKNO = DMA_TCD_CITER_ELINKNO_CITER(length) | DMA_TCD_CITER_ELINKNO_ELINK(0);
    DMA->TCD[EVE_DMA_CHANNEL].BITER.ELINKNO = DMA_TCD_BITER_ELINKNO_BITER(length) | DMA_TCD_BITER_ELINKNO_ELINK(0);
    DMA->SERQ = EVE_DMA_CHANNEL; /* start DMA */
    EVE_dma_busy = 42;
}

void EVE_DMA_IRQHandler(void)
{
    /* the end of the DMA major loop */
    DMA->CINT = DMA_CINT_CINT(EVE_DMA_CHANNEL); /* clear the flag */
    while (0U == (EVE_SPI->SR & LPSPI_SR_TCF_MASK)); /* wait for the SPI to be done transmitting */
    EVE_cs_clear();
    EVE_dma_busy = 0;
    EVE_SPI->TCR &= ~LPSPI_TCR_RXMSK_MASK; /* enable LPSPI receive */
}

#endif /* DMA */

#endif /* S32K14x */

/* ################################################################## */
/* ################################################################## */

#if defined (GD32C103)

void DELAY_MS(uint16_t val)
{
    while (val > 0U)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __asm__ volatile ("nop");
        }
        val--;
    }
}

void EVE_init_spi(void)
{
    /* only two valid options, the SPI0 pins are either mapped to GPIOA (default), or GPIOB */ 
    if (EVE_SPI_PORT == GPIOA)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    }
    else
    {
        rcu_periph_clock_enable(RCU_GPIOB);
        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
        gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    }

    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);

    spi_parameter_struct spi_init_struct;
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16; /* SPI_PSC_16 -> 120MHz / 16 = 7.5MHz */
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0);
}

#if defined (EVE_DMA)

uint32_t EVE_dma_buffer[1025U];
volatile uint16_t EVE_dma_buffer_index = 0;
volatile uint8_t EVE_dma_busy = 0;

void EVE_init_dma(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    nvic_irq_enable(DMA0_Channel2_IRQn, 0, 0);
}

void EVE_start_dma_transfer(void)
{
    dma_parameter_struct dma_init_struct;

    dma_deinit(DMA0, DMA_CH2);
    DMA_CHCTL(DMA0, DMA_CH2) = DMA_CHCTL(DMA0, DMA_CH2) | DMA_CHXCTL_FTFIE; /* enable full transfer finish interrupt */

    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = ((uint32_t) &EVE_dma_buffer[0]) + 1U;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = (((EVE_dma_buffer_index) * 4U) - 1U);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct); /* configure SPI0 transmit dma: DMA0-DMA_CH2 */

    EVE_cs_set();

    dma_channel_enable(DMA0, DMA_CH2);
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    EVE_dma_busy = 42;
}

void DMA0_Channel2_IRQHandler(void)
{
    if (dma_interrupt_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH2, DMA_INT_FLAG_G);
        while (SPI_STAT(SPI0) & SPI_STAT_TRANS) {}
        EVE_cs_clear();
        EVE_dma_busy = 0;
    }
}

#endif /* DMA */
#endif /* GD32C103 */

#endif /* __GNUC__ */

/* ################################################################## */
/* ################################################################## */

#if defined (__TI_ARM__)

#if defined (__MSP432P401R__)

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig EVE_Config =
{
    EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
    48000000,                                   // SMCLK  = 48MHZ
    500000,                                    // SPICLK = 1Mhz
    EUSCI_B_SPI_MSB_FIRST,                     // MSB First
    EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,    // Phase
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // High polarity
    EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
};

void EVE_SPI_Init(void)
{
    GPIO_setAsOutputPin(EVE_CS_PORT, EVE_CS);
    GPIO_setAsOutputPin(EVE_PDN_PORT, EVE_PDN);
//  GPIO_setAsInputPinWithPullDownResistor(EVE_INT_PORT,EVE_INT);

    GPIO_setOutputHighOnPin(EVE_CS_PORT, EVE_CS);
    GPIO_setOutputHighOnPin(EVE_PDN_PORT, EVE_PDN);

    GPIO_setAsPeripheralModuleFunctionInputPin(RIVERDI_PORT, RIVERDI_SIMO | RIVERDI_SOMI | RIVERDI_CLK, GPIO_PRIMARY_MODULE_FUNCTION);
    SPI_initMaster(EUSCI_B0_BASE, &EVE_Config);
    SPI_enableModule(EUSCI_B0_BASE);
}

#endif /* __MSP432P401R__ */
#endif /* __TI_ARM__ */

#endif

#endif /*LV_USE_DRAW_EVE*/
