/*
@file    EVE_target_Arduino_Teensy4.h
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
- changed #include "EVE_cpp_wrapper.h" to #include "../EVE_cpp_wrapper.h"
- added ARDUINO_TEENSY40 to the Teensy 4 target

*/

#ifndef EVE_TARGET_ARDUINO_TEENSY4_H
#define EVE_TARGET_ARDUINO_TEENSY4_H

#if defined (ARDUINO)

#include <stdint.h>
#include <Arduino.h>
#include "../EVE_cpp_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (ARDUINO_TEENSY41) || defined (ARDUINO_TEENSY40)

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 10
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 9
#endif
/* you may define these in your build-environment to use different settings */

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
    wrapper_spi_transmit(data);
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
    return (wrapper_spi_receive(data));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* Teensy41 */

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_TARGET_ARDUINO_TEENSY4_H */
