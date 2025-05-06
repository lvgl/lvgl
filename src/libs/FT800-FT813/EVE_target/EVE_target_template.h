/*
@file    EVE_target_template.h
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
- split up the optional default defines to allow to only change what needs
    changing thru the build-environment

*/

#ifndef EVE_TARGET_TEMPLATE_H
#define EVE_TARGET_TEMPLATE_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

/* note: target as set by the compiler, the IDE or the project settings */
#if defined (MYTARGET)

#include "mytarget.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT 0
#define EVE_CS PORT_PA05
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT 0U
#define EVE_PDN PORT_PA03
#endif

#if !defined (EVE_SPI)
#define EVE_SPI 0U
#endif

#if defined (EVE_DMA) && !defined (EVE_DMA_CHANNEL)
#define EVE_DMA_CHANNEL 0U
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 8000U  /* ~1ms at 48MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */

// #define EVE_DMA /* to be defined in the build-environment */

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

void DELAY_MS(uint16_t val);

static inline void EVE_pdn_set(void)
{
    /* set PD low to put EVE in Power-Down */
}

static inline void EVE_pdn_clear(void)
{
    /* set PD high to start EVE */
}

static inline void EVE_cs_set(void)
{
    /* set CS low to start communication */
}

static inline void EVE_cs_clear(void)
{
    /* set CS high to end communication */
}

static inline void spi_transmit(uint8_t data)
{
    /* send data over SPI */
    /* wait for end of SPI transfer */
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
    /* send data over SPI */
    /* wait for end of SPI transfer */
    /* return (byte received from SPI) */
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* MYTARGET */
#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_TEMPLATE_H */
