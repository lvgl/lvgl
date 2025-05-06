/*
@file    EVE_target_Test.h
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
- new target for software tests
- basic maintenance: checked for violations of white space and indent rules

*/

#ifndef EVE_TARGET_TEST_H
#define EVE_TARGET_TEST_H

#if defined (SOFTWARE_TEST)

#include <stdint.h>

/* ToDo: checkout Google Test / Mock */

extern uint8_t EVE_spi_test_buffer[8192];
extern uint8_t EVE_spi_test_buffer_index;
extern uint8_t EVE_spi_test_buffer_receive[32];

struct
{
    uint8_t called;
} Test_EVE_pdn_set;

struct
{
    uint8_t called;
} Test_EVE_pdn_clear;

struct
{
    uint8_t called;
} Test_EVE_cs_set;

struct
{
    uint8_t called;
} Test_EVE_cs_clear;

struct
{
    uint8_t called;
} Test_EVE_spi_transmit;

struct
{
    uint8_t called;
} Test_EVE_spi_transmit_32;

#define EVE_DELAY_1MS 4U  /* no real delay needed for the software tests */

static inline void DELAY_MS(uint16_t val)
{
    for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
    {
        __asm__ volatile("nop");
    }
}
static inline void EVE_pdn_set(void)
{
    Test_EVE_pdn_set.called = Test_EVE_pdn_set.called + 1U;
}

static inline void EVE_pdn_clear(void)
{
    Test_EVE_pdn_clear.called = Test_EVE_pdn_clear.called + 1U;
}

static inline void EVE_cs_set(void)
{
    Test_EVE_cs_set.called = Test_EVE_cs_set.called + 1U;
    EVE_spi_test_buffer_index = 0;
}

static inline void EVE_cs_clear(void)
{
    Test_EVE_cs_clear.called = Test_EVE_cs_clear.called + 1U;
}

static inline void spi_transmit(uint8_t data)
{
    Test_EVE_spi_transmit.called = Test_EVE_spi_transmit.called + 1U;
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = data;
    EVE_spi_test_buffer_index++;
}

static inline void spi_transmit_32(uint32_t data)
{
    Test_EVE_spi_transmit_32.called = Test_EVE_spi_transmit_32.called + 1U;
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = ((uint8_t)(data & 0x000000ffUL));
    EVE_spi_test_buffer_index++;
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = ((uint8_t)(data >> 8U));
    EVE_spi_test_buffer_index++;
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = ((uint8_t)(data >> 16U));
    EVE_spi_test_buffer_index++;
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = ((uint8_t)(data >> 24U));
    EVE_spi_test_buffer_index++;
}

/* spi_transmit_burst() is only used for cmd-FIFO commands */
/* so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{
    Test_EVE_spi_transmit_burst.called = Test_EVE_spi_transmit_burst.called + 1U;
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    Test_EVE_spi_transmit.called = Test_EVE_spi_receive.called + 1U;
    uint8_t result = EVE_spi_test_buffer_receive[EVE_spi_test_buffer_index];
    EVE_spi_test_buffer[EVE_spi_test_buffer_index] = data;
    EVE_spi_test_buffer_index++;
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    Test_EVE_fetch_flash_byte.called = Test_EVE_fetch_flash_byte.called + 1U;
    (void) *p_data;
    return (0x00);
}

#endif /* SOFTWARE_TEST */

#endif /* EVE_TARGET_TEST_H */
