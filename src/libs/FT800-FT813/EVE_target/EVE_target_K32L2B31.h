/*
@file    EVE_target_K32L2B31.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-06-24
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
- extracted from EVE_target.h
- made DELAY_MS() more MISRA-C compliant
- basic maintenance: checked for violations of white space and indent rules
- split up the optional default defines to allow to only change what needs
    changing thru the build-environment

*/

#ifndef EVE_TARGET_K32L2B31_H
#define EVE_TARGET_K32L2B31_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (CPU_K32L2B31VLH0A)

#include <stdint.h>
#include "K32L2B31A.h"
#include "fsl_gpio.h"
#include "fsl_spi.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 4
#define EVE_CS_GPIO GPIOD
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 2
#define EVE_PDN_GPIO GPIOD
#endif

#if !defined (EVE_SPI)
#define EVE_SPI SPI1
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 8000U  /* ~1ms at 48MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */

static inline void DELAY_MS(uint16_t val)
{
    for (uint16_t loops = 0; loops < val; loops++)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __asm__ volatile("nop");
        }
    }
}

//void DELAY_MS(uint16_t val);
//void EVE_init_spi(void);
//#define EVE_DMA

static inline void EVE_cs_set(void)
{
    GPIO_PortClear(EVE_CS_GPIO, 1u << EVE_CS); /* set CS low */
}

static inline void EVE_cs_clear(void)
{
    GPIO_PortSet(EVE_CS_GPIO, 1u << EVE_CS); /* set CS high */
}

static inline void EVE_pdn_set(void)
{
    GPIO_PortClear(EVE_PDN_GPIO, 1u << EVE_PDN); /* set PDN low */
}

static inline void EVE_pdn_clear(void)
{
    GPIO_PortSet(EVE_PDN_GPIO, 1u << EVE_PDN); /* set PDN high */
}

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    #define EVE_DMA_CHANNEL 0
    #define EVE_DMAMUX_CHCFG_SOURCE 15 /* this needs to be the EDMA_REQ_LPSPIx_TX */
    #define EVE_DMA_IRQ  DMA0_IRQn
    #define EVE_DMA_IRQHandler DMA0_IRQHandler

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

static inline void spi_transmit(uint8_t data)
{
#if 1
    while (0U == (EVE_SPI->S & SPI_S_SPTEF_MASK)) {}
    EVE_SPI->DL = data;
    while (0U == (EVE_SPI->S & SPI_S_SPTEF_MASK)) {}
    while (0U == (EVE_SPI->S & SPI_S_SPRF_MASK)) {}
    (void) EVE_SPI->DL;
#else
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag)) {}
    SPI_WriteData(EVE_SPI, data);
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag)) {}
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_RxBufferFullFlag))  {}
    (void) SPI_ReadData(EVE_SPI);
#endif
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
}

/* spi_transmit_burst() is only used for cmd-FIFO commands */
/* so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{
#if defined (EVE_DMA)
    EVE_dma_buffer[EVE_dma_buffer_index++] = data;
#else
    spi_transmit_32(data);
#endif
}

static inline uint8_t spi_receive(uint8_t data)
{
#if 0
    while (0U == (EVE_SPI->S & SPI_S_SPTEF_MASK)) {}
    EVE_SPI->DL = data;
    while (0U == (EVE_SPI->S & SPI_S_SPTEF_MASK)) {}
    while (0U == (EVE_SPI->S & SPI_S_SPRF_MASK)) {}
    return (EVE_SPI->DL);
#else
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag)) {}
    SPI_WriteData(EVE_SPI, data);
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag)) {}
    while (0U == (SPI_GetStatusFlags(EVE_SPI) & kSPI_RxBufferFullFlag)) {}
    return (SPI_ReadData(EVE_SPI));
#endif
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* K32L2B3 */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_K32L2B31_H */
