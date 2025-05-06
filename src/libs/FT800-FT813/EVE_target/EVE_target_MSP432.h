/*
@file    EVE_target_MSP432.h
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

#ifndef EVE_TARGET_MSP432_H
#define EVE_TARGET_MSP432_H

#if defined (__TI_ARM__)

#if defined (__MSP432P401R__)

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT GPIO_PORT_P5
#define EVE_CS GPIO_PIN0 /* P5.0 */
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT GPIO_PORT_P5
#define EVE_PDN GPIO_PIN1 /* P5.1 */
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 8000U /* ~1ms at 48MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */

#define RIVERDI_PORT GPIO_PORT_P1
#define RIVERDI_SIMO BIT6   /* P1.6 */
#define RIVERDI_SOMI BIT7   /* P1.7 */
#define RIVERDI_CLK BIT5    /* P1.5 */

void EVE_SPI_Init(void);

static inline void DELAY_MS(uint16_t val)
{
    for (uint16_t loops = 0; loops < val; loops++)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __nop();
        }
    }
}

static inline void EVE_pdn_set(void)
{
//   GPIO_setOutputLowOnPin(EVE_PDN_PORT,EVE_PDN); /* Power-Down low */
    P5OUT &= ~EVE_PDN;   /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
//  GPIO_setOutputHighOnPin(EVE_PDN_PORT,EVE_PDN); /* Power-Down high */
    P5OUT |= EVE_PDN;    /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
//  GPIO_setOutputLowOnPin(EVE_CS_PORT,EVE_CS);   /* CS low */
    P5OUT &= ~EVE_CS;   /* CS low */
}

static inline void EVE_cs_clear(void)
{
//  GPIO_setOutputHighOnPin(EVE_CS_PORT,EVE_CS);    /* CS high */
    P5OUT |= EVE_CS;    /* CS high */
}

static inline void spi_transmit(uint8_t data)
{
//  SPI_transmitData(EUSCI_B0_BASE, data);
//  while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    UCB0TXBUF_SPI = data;
    while (!(UCB0IFG_SPI & UCTXIFG)) {} /* wait for transmission to complete */
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
//  SPI_transmitData(EUSCI_B0_BASE, data);
//  while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
//  return (EUSCI_B_CMSIS(EUSCI_B0_BASE)->RXBUF);

    UCB0TXBUF_SPI = data;
    while (!(UCB0IFG_SPI & UCTXIFG)) {} /* wait for transmission to complete */
    return (UCB0RXBUF_SPI);
    }

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* __MSP432P401R__ */

#endif /* __TI_ARM */

#endif /* EVE_TARGET_MSP432_H */
