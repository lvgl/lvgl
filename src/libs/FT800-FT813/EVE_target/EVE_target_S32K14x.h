/*
@file    EVE_target_S32K14x.h
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
- basic maintenance: checked for violations of white space and indent rules
- split up the optional default defines to allow to only change what needs
    changing thru the build-environment

*/

#ifndef EVE_TARGET_S32K14X_H
#define EVE_TARGET_S32K14X_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (CPU_S32K148) || defined (CPU_S32K144HFT0VLLT)

#if defined (CPU_S32K144HFT0VLLT)
#include "S32K144.h"
#endif

#if defined (CPU_S32K148)
#include "S32K148.h"
#endif

#include <stdint.h>

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 5U
#define EVE_CS_GPIO PTB
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 14U
#define EVE_PDN_GPIO PTD
#endif

#if !defined (EVE_SPI)
/* LPSPI0 on J2 header: PTB2 = SCK, PTB3 = MISO, PTB4 = MOSI */
#define EVE_SPI LPSPI0
#endif

#if !defined (EVE_SPI_INDEX)
#define EVE_SPI_INDEX PCC_LPSPI0_INDEX
#endif

#if !defined (EVE_DELAY_1MS)
//#define EVE_DELAY_1MS 15000U   /* maybe ~1ms at 112MHz Core-Clock */
#define EVE_DELAY_1MS 5300U  /* maybe ~1ms at 48MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */

// #define EVE_DMA /* to be defined in the build-environment */

void DELAY_MS(uint16_t val);
void EVE_init_spi(void);

static inline void EVE_cs_set(void)
{
    EVE_CS_GPIO->PCOR = (1U << EVE_CS); /* set CS low */
}

static inline void EVE_cs_clear(void)
{
    EVE_CS_GPIO->PSOR = (1U << EVE_CS); /* set CS high */
}

static inline void EVE_pdn_set(void)
{
    EVE_PDN_GPIO->PCOR = (1U << EVE_PDN); /* set PDN low */
}

static inline void EVE_pdn_clear(void)
{
    EVE_PDN_GPIO->PSOR = (1U << EVE_PDN); /* set PDN high */
}

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    #define EVE_DMA_CHANNEL 0U
    #define EVE_DMAMUX_CHCFG_SOURCE 15U /* this needs to be the EDMA_REQ_LPSPIx_TX */
    #define EVE_DMA_IRQ  DMA0_IRQn
    #define EVE_DMA_IRQHandler DMA0_IRQHandler

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

static inline void spi_transmit(uint8_t data)
{
    EVE_SPI->SR |= LPSPI_SR_RDF_MASK; /* clear Receive Data Flag */
    EVE_SPI->TDR = data; /* transmit data */
    while (0U == (EVE_SPI->SR & LPSPI_SR_RDF_MASK));
    (void) EVE_SPI->RDR; /* dummy read-access to clear Receive Data Flag */
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
    EVE_SPI->SR |= LPSPI_SR_RDF_MASK; /* clear Receive Data Flag */
    EVE_SPI->TDR = data; /* transmit data */
    while (0U == (EVE_SPI->SR & LPSPI_SR_RDF_MASK));
    return (EVE_SPI->RDR);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* S32K14x */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_S32K14X_H */
