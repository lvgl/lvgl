/*
@file    EVE_target_XMEGA.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-04-12
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- extracted from EVE_target.h
- basic maintenance: checked for violations of white space and indent rules

*/

#ifndef EVE_TARGET_XMEGA_H
#define EVE_TARGET_XMEGA_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (__AVR_XMEGA__)

#include <avr/io.h>
#include <avr/pgmspace.h>
#ifndef F_CPU
    #define F_CPU 32000000UL
#endif
#include <util/delay.h>

#define DELAY_MS(ms) _delay_ms(ms)

#if !defined (EVE_CS)
    #define EVE_CS_PORT     PORTC
    #define EVE_CS          PIN4_bm
    #define EVE_PDN_PORT    PORTC
    #define EVE_PDN_PIN     PIN1_bm
    #define EVE_SPI         SPIC    /* the SPI port to use */
#endif

static inline void EVE_pdn_set(void)
{
    EVE_PDN_PORT.OUTCLR = EVE_PDN_PIN;  /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
    EVE_PDN_PORT.OUTSET = EVE_PDN_PIN;  /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
    EVE_CS_PORT.OUTCLR = EVE_CS;    /* cs low */
}

static inline void EVE_cs_clear(void)
{
    EVE_CS_PORT.OUTSET= EVE_CS;     /* cs high */
}

static inline void spi_transmit(uint8_t data)
{
    EVE_SPI.DATA = data;
    while (!(EVE_SPI.STATUS & 0x80)) {}  // wait for transmit complete
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
    EVE_SPI.DATA = data;
    while (!(EVE_SPI.STATUS & 0x80)) {}  // wait for transmit complete
    return (EVE_SPI.DATA);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
#if defined (__AVR_HAVE_ELPM__) /* we have an AVR with more than 64kB FLASH memory */
    return (pgm_read_byte_far(p_data));
#else
    return (pgm_read_byte_near(p_data));
#endif
}

#endif /* XMEGA */

#endif /* __GNUC__ */
#endif /* !Arduino */

#endif /* EVE_TARGET_XMEGA_H */
