/*
@file    EVE_target_GD32C103.h
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

#ifndef EVE_TARGET_GD32C103_H
#define EVE_TARGET_GD32C103_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (GD32C103)
/* note: set in platformio.ini by "build_flags = -D GD32C103" */

#include "gd32c10x.h"

/* note: SPI0 is used */

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT GPIOA
#define EVE_CS GPIO_PIN_4
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT GPIOA
#define EVE_PDN GPIO_PIN_3
#endif

#if !defined (EVE_SPI_PORT)
#define EVE_SPI_PORT GPIOA
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 20000U  /* ~1ms at 120MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */


#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif


void DELAY_MS(uint16_t val);
void EVE_init_spi(void);

static inline void EVE_pdn_set(void)
{
    gpio_bit_reset(EVE_PDN_PORT,EVE_PDN);
}

static inline void EVE_pdn_clear(void)
{
    gpio_bit_set(EVE_PDN_PORT,EVE_PDN);
}

static inline void EVE_cs_set(void)
{
    gpio_bit_reset(EVE_CS_PORT,EVE_CS);
}

static inline void EVE_cs_clear(void)
{
    gpio_bit_set(EVE_CS_PORT,EVE_CS);
}

static inline void spi_transmit(uint8_t data)
{
    SPI_DATA(SPI0) = (uint32_t) data;
    while (SPI_STAT(SPI0) & SPI_STAT_TRANS) {}
    (void) SPI_DATA(SPI0); /* dummy read to clear the flags */
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
    SPI_DATA(SPI0) = (uint32_t) data;
    while (SPI_STAT(SPI0) & SPI_STAT_TRANS) {}
    while (0U == (SPI_STAT(SPI0) & SPI_STAT_RBNE)) {}
    return ((uint8_t) SPI_DATA(SPI0));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* GD32C103 */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_GD32C103_H */
