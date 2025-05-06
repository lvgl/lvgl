/*
@file    EVE_target_Arduino_generic.h
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
- split up the optional default defines to allow to only change what needs
    changing thru the build-environment
- changed #include "EVE_cpp_wrapper.h" to #include "../EVE_cpp_wrapper.h"

*/

#ifndef EVE_TARGET_ARDUINO_GENERIC_H
#define EVE_TARGET_ARDUINO_GENERIC_H

#if defined (ARDUINO)

#include <stdint.h>
#include <Arduino.h>
#include "../EVE_cpp_wrapper.h"

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
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    return (wrapper_spi_receive(data));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_TARGET_ARDUINO_GENERIC_H */
