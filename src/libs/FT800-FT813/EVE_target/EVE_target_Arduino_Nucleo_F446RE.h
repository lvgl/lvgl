/*
@file    EVE_target_Arduino_Nucleo_F446RE.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-07-16
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
- changed #include "EVE_cpp_wrapper.h" to #include "../EVE_cpp_wrapper.h"
- EVE_SPI could not actually be configured as "SPI1" is not just a number,
  changed parameter to EVE_SPI_UNIT and "1" is the only valid option, for now

*/

#ifndef EVE_TARGET_ARDUINO_NUCLEO_F446RE_H
#define EVE_TARGET_ARDUINO_NUCLEO_F446RE_H

#if defined (ARDUINO)

#include <stdint.h>
#include <Arduino.h>
#include "../EVE_cpp_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (ARDUINO_NUCLEO_F446RE)

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_spi.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 10
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 8
#endif

#if !defined (EVE_SPI_UNIT)
#define EVE_SPI_UNIT 1U
#endif
/* you may define these in your build-environment to use different settings */

#if EVE_SPI_UNIT == 1U
#define EVE_SPI SPI1
#else
#error SPI1 only with PA5 = SCK, PA6 = MISO and PA7 = MOSI
#endif

void EVE_init_spi(void);

#define EVE_DMA

#if defined (EVE_DMA)
extern uint32_t EVE_dma_buffer[1025U];
extern volatile uint16_t EVE_dma_buffer_index;
extern volatile uint8_t EVE_dma_busy;

void EVE_init_dma(void);
void EVE_start_dma_transfer(void);
#endif

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

static inline void spi_transmit(uint8_t data)
{
    // SPI.transfer(data);
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {}
    LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXNE */
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
    // return (SPI.transfer(data));
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {}
    return (LL_SPI_ReceiveData8(EVE_SPI));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* ARDUINO_NUCLEO_F446RE */

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_TARGET_ARDUINO_NUCLEO_F446RE_H */
