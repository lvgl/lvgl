/*
@file    EVE_target_AVR.h
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

#ifndef EVE_TARGET_AVR_H
#define EVE_TARGET_AVR_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (__AVR__) && !defined (__AVR_XMEGA__)

#include <avr/io.h>
#include <avr/pgmspace.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define DELAY_MS(ms) _delay_ms(ms)

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT PORTB
#define EVE_CS      (1U << PB5)
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT    PORTB
#define EVE_PDN     (1U << PB4)
#endif
/* you may define these in your build-environment to use different settings */

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
#if 1
    SPDR = data; /* start transmission */
    /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
    while (!(SPSR & (1U << SPIF))) {}
#else
/* software-spi example */
    uint8_t spiIndex  = 0x80;

    for (uint8_t count = 0; count < 8U; count++) // Output each bit of spiOutByte
    {
        if (data & spiIndex) // Output MOSI Bit
        {
            PORTC |= (1U << PORTC1);
        }
        else
        {
            PORTC &= ~(1U << PORTC1);
        }

        PORTA |= (1U << PORTA1); // toggle SCK
        PORTA &= ~(1U << PORTA1);

        spiIndex >>= 1U;
    }
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
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
#if 1
    SPDR = data; /* start transmission */
    /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
    while (!(SPSR & (1U << SPIF))) {}
    return (SPDR);
#else
    uint8_t spiIndex  = 0x80U;
    uint8_t spiInByte = 0;

    for (uint8_t count = 0; count < 8U; count++) // Output each bit of spiOutByte
    {
        if (data & spiIndex) // Output MOSI Bit
        {
            PORTC |= (1U << PORTC1);
        }
        else
        {
            PORTC &= ~(1U << PORTC1);
        }

        PORTA |= (1U << PORTA1); // toggle SCK
        PORTA &= ~(1U << PORTA1);

        if (PINC & (1U << PORTC0))
        {
            spiInByte |= spiIndex;
        }

        spiIndex >>= 1U;
    }
    return (spiInByte);
#endif
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    /* do we have an AVR with more than 64kB FLASH memory? */
#if defined (__AVR_HAVE_ELPM__) 
    return (pgm_read_byte_far(data));
#else
    return (pgm_read_byte_near(data));
#endif
}

#endif /* AVR */

#endif /* __GNUC__ */
#endif /* !Arduino */

#endif /* EVE_TARGET_AVR_H */
