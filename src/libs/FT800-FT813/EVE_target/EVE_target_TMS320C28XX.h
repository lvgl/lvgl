/*
@file    EVE_target_TMS320C28XX.h
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
- changed EVE_DELAY_1MS to allow setup thru the build-environment

*/

#ifndef EVE_TARGET_TMS320C28XX_H
#define EVE_TARGET_TMS320C28XX_H

/* this is for TIs C2000 compiled with their ti-cgt-c2000 compiler */
/* which does not define this many symbols */
#if defined (__TMS320C28XX__)

/* the designated target actually is a TMS320F28335 */
/* credit for this goes to David Sakal-Sega */
/* note: the SPI unit of the TMS320F28335 does not support DMA, */
/* using one of the UARTs in SPI mode would allow DMA */

#include <stdint.h>
#include <DSP2833x_Device.h>

/* this architecture does not actually know what a byte is, */
/* uint_least8_t is 16 bits wide */
typedef uint_least8_t uint8_t;

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_DELAY_1MS)
/* 150MHz -> 6.67ns per cycle, 5 cycles for the loop itself and 8 NOPs */
/* -> 1ms / (6.67ns * 13) = 11532 */
#define EVE_DELAY_1MS 12000U
#endif
/* you may define these in your build-environment to use different settings */

static inline void DELAY_MS(uint16_t val)
{
    for (uint16_t loops = 0; loops < val; loops++)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            asm(" RPT #7 || NOP");
        }
    }
}


static inline void EVE_pdn_set(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO14 = 1U; /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
    GpioDataRegs.GPASET.bit.GPIO14 = 1U; /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO19 = 1U; /* CS low */
}

static inline void EVE_cs_clear(void)
{
    /* wait 60 cycles to make sure CS is not going high too early */
    asm(" RPT #60 || NOP"); 
    GpioDataRegs.GPASET.bit.GPIO19 = 1U; /* CS high */
}

static inline void spi_transmit(uint8_t data)
{
    /* start transfer, looks odd with data = uint8_t but uint8_t actually */
    /* is 16 bits wide on this controller */
    SpiaRegs.SPITXBUF = (data & 0xFFU) << 8U;
    /* wait for transmission to complete */
    while (0U == SpiaRegs.SPISTS.bit.INT_FLAG) {}
    (void)SpiaRegs.SPIRXBUF; /* dummy read to clear the flags */
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
    /* start transfer */
    SpiaRegs.SPITXBUF = (data & 0xFFU) << 8U;
    /* wait for transmission to complete */
    while (0U == SpiaRegs.SPISTS.bit.INT_FLAG) {}
    /* data is right justified in SPIRXBUF */
    return (SpiaRegs.SPIRXBUF & 0x00FFU);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif

#endif /* EVE_TARGET_TMS320C28XX_H */
