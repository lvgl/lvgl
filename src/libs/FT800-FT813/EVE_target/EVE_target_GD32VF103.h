/*
@file    EVE_target_GD32VF103.h
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
- made EVE_DELAY_1MS changeable thru the build-environment

*/

#ifndef EVE_TARGET_GD32VF103_H
#define EVE_TARGET_GD32VF103_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (__riscv)

//#warning Compiling for GD32VF103CBT6

#include "gd32vf103.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 18000U /* ~1ms at 108MHz Core-Clock, according to my Logic-Analyzer */
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

static inline void EVE_pdn_set(void)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_1);
}

static inline void EVE_pdn_clear(void)
{
    gpio_bit_set(GPIOB,GPIO_PIN_1);
}

static inline void EVE_cs_set(void)
{
    gpio_bit_reset(GPIOB,GPIO_PIN_0);
}

static inline void EVE_cs_clear(void)
{
    gpio_bit_set(GPIOB,GPIO_PIN_0);
}

static inline void spi_transmit(uint8_t data)
{
        SPI_DATA(SPI0) = (uint32_t) data;
        while (SPI_STAT(SPI0) & SPI_STAT_TRANS) {}
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
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
        SPI_DATA(SPI0) = (uint32_t) data;
        while (SPI_STAT(SPI0) & SPI_STAT_TRANS) {}
        return ((uint8_t) SPI_DATA(SPI0));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* __riscv */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_GD32VF103_H */
