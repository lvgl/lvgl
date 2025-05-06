/*
@file    EVE_target_RP2040.h
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

#ifndef EVE_TARGET_RP2040_H
#define EVE_TARGET_RP2040_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (RP2040)
/* note: set in platformio.ini by "build_flags = -D RP2040" */

#include "pico/stdlib.h"
#include "hardware/spi.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 5
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 6
#endif

#if !defined (EVE_SCK)
#define EVE_SCK 2
#endif

#if !defined (EVE_MOSI)
#define EVE_MOSI 3
#endif

#if !defined (EVE_MISO)
#define EVE_MISO 4
#endif

#if !defined (EVE_SPI)
#define EVE_SPI spi0
#endif
/* you may define these in your build-environment to use different settings */

// #define EVE_DMA /* to be defined in the build-environment */

#define DELAY_MS(ms) sleep_ms(ms)

void EVE_init_spi(void);

static inline void EVE_cs_set(void)
{
    gpio_put(EVE_CS, 0U);
}

static inline void EVE_cs_clear(void)
{
    gpio_put(EVE_CS, 1U);
}

static inline void EVE_pdn_set(void)
{
    gpio_put(EVE_PDN, 0U);
}

static inline void EVE_pdn_clear(void)
{
    gpio_put(EVE_PDN, 1U);
}

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

static inline void spi_transmit(uint8_t data)
{
    spi_write_blocking(EVE_SPI, &data, 1U);
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_write_blocking(EVE_SPI, (uint8_t *) &data, 4U);
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
    uint8_t result;

    spi_write_read_blocking(EVE_SPI, &data, &result, 1U);
    return (result);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* RP2040 */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_RP2040_H */
