/*
@file    EVE_target_ICCAVR.h
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

#ifndef EVE_TARGET_ICCAVR_H
#define EVE_TARGET_ICCAVR_H

#if !defined (ARDUINO)
#if defined (__IMAGECRAFT__)
#if defined (_AVR)

#include <iccioavr.h>

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT PORTB
#define EVE_CS (1U << PB5)
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT PORTB
#define EVE_PDN (1U << PB4)
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 2000U /* maybe ~1ms at 16MHz clock */
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
    EVE_PDN_PORT &= ~EVE_PDN;   /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
    EVE_PDN_PORT |= EVE_PDN;    /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
    EVE_CS_PORT &= ~EVE_CS; /* cs low */
}

static inline void EVE_cs_clear(void)
{
    EVE_CS_PORT |= EVE_CS;  /* cs high */
}

static inline void spi_transmit(uint8_t data)
{
    SPDR = data; /* start transmission */
    /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
    while (!(SPSR & (1U << SPIF))) {}
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
    SPDR = data; /* start transmission */
    /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
    while (!(SPSR & (1U << SPIF))) {}
    return (SPDR);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* _AVR */
#endif /* __IMAGECRAFT__ */
#endif /* !Arduino */
#endif /* EVE_TARGET_ICCAVR_H */
