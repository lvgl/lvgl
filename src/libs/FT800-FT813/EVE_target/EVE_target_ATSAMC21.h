/*
@file    EVE_target_ATSAMC21.h
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
- changed EVE_SPI to a numerical value to automatically determine the
    correct SERCOMx_DMAC_ID_TX

*/

/* While the following lines make things a lot easier like automatically */
/* compiling the code for the target you are compiling for, */
/* a few things are expected to be taken care of beforehand. */
/* - setting the Chip-Select and Power-Down pins to Output, */
/* Chip-Select = 1 and Power-Down = 0 */
/* - setting up the SPI which may or not include things like
    - setting the pins for the SPI to output or some alternate I/O function
        or mapping that functionality to that pin
    - if that is an option with the controller your are using you probably
        should set the drive-strength for the SPI pins to high
    - setting the SS pin on AVRs to output in case it is not used
        for Chip-Select or Power-Down
    - setting SPI to mode 0
    - setting SPI to 8 bit with MSB first
    - setting SPI clock to no more than 11 MHz for the init

  For the SPI transfers single 8-Bit transfers are used with
  busy-wait for completion.
  While this is okay for AVRs that run at 16MHz with the SPI at 8 MHz and
  therefore do one transfer in 16 clock-cycles, this is wasteful
  for any 32 bit controller even at higher SPI speeds.

  Check out the section for SAMC21E18A as it has code to transparently add DMA.

  If the define "EVE_DMA" is set the spi_transmit_async() is changed at
  compile time to write in a buffer instead directly to SPI.
  EVE_init() calls EVE_init_dma() which sets up the DMA channel
  and enables an IRQ for end of DMA.
  EVE_start_cmd_burst() is changed from sending the first bytes by SPI to
    resetting the the DMA buffer.
  EVE_end_cmd_burst() just calls EVE_start_dma_transfer() which triggers
    the transfer of the SPI buffer by DMA.
  EVE_busy() does nothing but to report that EVE is busy
    if there is an active DMA transfer.
  At the end of the DMA transfer an IRQ is executed which clears the DMA
    active state and calls EVE_cs_clear() by which the command buffer
    is executed by the command co-processor.
*/

#ifndef EVE_TARGET_ATSAMC21_H
#define EVE_TARGET_ATSAMC21_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (__SAMC21E18A__) \
    || defined (__SAMC21J18A__) \
    || defined (__SAMC21J17A__)
/* note: target as set by AtmelStudio, valid are all from the same family */

#include "sam.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT 0U
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

#if EVE_SPI == 0U
#define EVE_SPI_SERCOM SERCOM0
#define EVE_SPI_DMA_TRIGGER SERCOM0_DMAC_ID_TX
#elif EVE_SPI == 1U
#define EVE_SPI_SERCOM SERCOM1
#define EVE_SPI_DMA_TRIGGER SERCOM1_DMAC_ID_TX
#elif EVE_SPI == 2U
#define EVE_SPI_SERCOM SERCOM2
#define EVE_SPI_DMA_TRIGGER SERCOM2_DMAC_ID_TX
#elif EVE_SPI == 3U
#define EVE_SPI_SERCOM SERCOM3
#define EVE_SPI_DMA_TRIGGER SERCOM3_DMAC_ID_TX
#elif EVE_SPI == 4U
#define EVE_SPI_SERCOM SERCOM4
#define EVE_SPI_DMA_TRIGGER SERCOM4_DMAC_ID_TX
#elif EVE_SPI == 5U
#define EVE_SPI_SERCOM SERCOM5
#define EVE_SPI_DMA_TRIGGER SERCOM5_DMAC_ID_TX
#endif

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
    PORT->Group[EVE_PDN_PORT].OUTCLR.reg = EVE_PDN;
}

static inline void EVE_pdn_clear(void)
{
    PORT->Group[EVE_PDN_PORT].OUTSET.reg = EVE_PDN;
}

static inline void EVE_cs_set(void)
{
    PORT->Group[EVE_CS_PORT].OUTCLR.reg = EVE_CS;
}

static inline void EVE_cs_clear(void)
{
    PORT->Group[EVE_CS_PORT].OUTSET.reg = EVE_CS;
}

static inline void spi_transmit(uint8_t data)
{
    EVE_SPI_SERCOM->SPI.DATA.reg = data;
    while (0U == (EVE_SPI_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)) {}
    (void) EVE_SPI_SERCOM->SPI.DATA.reg; /* dummy read-access to clear SERCOM_SPI_INTFLAG_RXC */
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
    EVE_SPI_SERCOM->SPI.DATA.reg = data;
    while (0U == (EVE_SPI_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)) {}
    return (EVE_SPI_SERCOM->SPI.DATA.reg);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* SAMC2x */

#endif /* __GNUC__ */
#endif /* !Arduino */

#endif /* EVE_TARGET_ATSAMC21_H */
