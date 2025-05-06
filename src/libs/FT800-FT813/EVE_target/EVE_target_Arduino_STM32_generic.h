/*
@file    EVE_target_Arduino_STM32_generic.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-07-22
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
- added check and code for optional macro parameter EVE_SPI_BOOST

*/

/*
optional parameter: EVE_SPI_BOOST
If this is defined thru the project options (-DEVE_SPI_BOOST), then the STM32
LL-HAL library is used directly instead of going thru the SPI class.
It is still necessary to setup the SPI properly thru the class like this:
SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
And in order for this to work the SPI unit that is used needs to be configured
as well, for example: -DEVE_SPI=SPI1

Test with STM32C031C6, SPI clock 8MHz:
Arduino_Core_STM32 2.5.0:
built with -O2, RAM = 5252 bytes, Flash = 26564 bytes
16 bit read: 30.2µs
8 bit read: 23.2µs
231 byte buffer transfer: 587µs

Test with STM32C031C6, SPI clock 8MHz:
Arduino_Core_STM32 2.6.0 + pull-request #2082
built with -O2, RAM = 5252 bytes, Flash = 26624 bytes
16 bit read: 16.06µs
8 bit read: 13.38µs
231 byte buffer transfer: 474.92µs

EVE_SPI_BOOST defined:
built with -O2, RAM = 5252 bytes, Flash = 27468 bytes
16 bit read: 13.6µs
8 bit read: 11.4µs
231 byte buffer transfer: 451µs

*/

#ifndef EVE_TARGET_ARDUINO_STM32_GENERIC_H
#define EVE_TARGET_ARDUINO_STM32_GENERIC_H 

#if defined (ARDUINO)

#include <stdint.h>
#include <Arduino.h>
#include "../EVE_cpp_wrapper.h"
#include "stm32yyxx_ll_spi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 10
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 8
#endif
/* you may define these in your build-environment to use different settings */

#define DELAY_MS(ms) delay(ms)

static inline void EVE_pdn_set(void)
{
    digitalWrite(EVE_PDN, LOW); /* go into power-down */
}

static inline void EVE_pdn_clear(void)
{
    digitalWrite(EVE_PDN, HIGH); /* power up */
}

static inline void EVE_cs_set(void)
{
    digitalWrite(EVE_CS, LOW); /* make EVE listen */
}

static inline void EVE_cs_clear(void)
{
    digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
}

#define EVE_DMA /* no DMA for now, "just" buffer transfers */

#if defined (EVE_DMA)
extern uint32_t EVE_dma_buffer[1025U];
extern volatile uint16_t EVE_dma_buffer_index;
extern volatile uint8_t EVE_dma_busy;

void EVE_init_dma(void);
void EVE_start_dma_transfer(void);
#endif

static inline void spi_transmit(uint8_t data)
{
#if !defined (EVE_SPI_BOOST)
    wrapper_spi_transmit(data);
#else
#if defined(SPI_SR_TXP)
    while (!LL_SPI_IsActiveFlag_TXP(EVE_SPI));
#else
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI));
#endif
    LL_SPI_TransmitData8(EVE_SPI, data);

#if defined(SPI_SR_RXP)
      while (!LL_SPI_IsActiveFlag_RXP(EVE_SPI));
#else
      while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI));
#endif
      LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI receive flag */
#endif
}

static inline void spi_transmit_32(uint32_t data)
{
#if !defined (EVE_SPI_BOOST)
    wrapper_spi_transmit_32(data);
#else
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
#endif
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
#if !defined (EVE_SPI_BOOST)
    return (wrapper_spi_receive(data));
#else
#if defined(SPI_SR_TXP)
    while (!LL_SPI_IsActiveFlag_TXP(EVE_SPI));
#else
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI));
#endif
    LL_SPI_TransmitData8(EVE_SPI, data);

#if defined(SPI_SR_RXP)
      while (!LL_SPI_IsActiveFlag_RXP(EVE_SPI));
#else
      while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI));
#endif
      return (LL_SPI_ReceiveData8(EVE_SPI));
#endif
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_TARGET_ARDUINO_STM32_GENERIC_H */
