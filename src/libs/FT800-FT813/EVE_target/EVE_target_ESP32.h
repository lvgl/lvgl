/*
@file    EVE_target_ESP32.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-12-09
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
- changed a couple of {0U} to {}
- changed back a couple of {} to {0} as C and C++ are not the same thing...

*/

#ifndef EVE_TARGET_ESP32_H
#define EVE_TARGET_ESP32_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (ESP_PLATFORM)

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS GPIO_NUM_13
#endif

#if !defined (EVE_PDN)
#define EVE_PDN GPIO_NUM_12
#endif

#if !defined (EVE_SCK)
#define EVE_SCK GPIO_NUM_18
#endif

#if !defined (EVE_MISO)
#define EVE_MISO GPIO_NUM_19
#endif

#if !defined (EVE_MOSI)
#define EVE_MOSI GPIO_NUM_23
#endif
/* you may define these in your build-environment to use different settings */

extern spi_device_handle_t EVE_spi_device;
extern spi_device_handle_t EVE_spi_device_simple;

#define EVE_DMA

void DELAY_MS(uint16_t ms);

void EVE_init_spi(void);

static inline void EVE_cs_set(void)
{
    spi_device_acquire_bus(EVE_spi_device_simple, portMAX_DELAY);
    gpio_set_level(EVE_CS, 0U);
}

static inline void EVE_cs_clear(void)
{
    gpio_set_level(EVE_CS, 1U);
    spi_device_release_bus(EVE_spi_device_simple);
}

static inline void EVE_pdn_set(void)
{
    gpio_set_level(EVE_PDN, 0U);
}

static inline void EVE_pdn_clear(void)
{
    gpio_set_level(EVE_PDN, 1U);
}

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

static inline void spi_transmit(uint8_t data)
{
    spi_transaction_t trans = {0};
    trans.length = 8U;
    trans.rxlength = 0;
    trans.flags = SPI_TRANS_USE_TXDATA;
    trans.tx_data[0U] = data;
    spi_device_polling_transmit(EVE_spi_device_simple, &trans);
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_transaction_t trans = {0};
    trans.length = 32U;
    trans.rxlength = 0;
    trans.flags = 0;
    trans.tx_buffer = &data;
    spi_device_polling_transmit(EVE_spi_device_simple, &trans);
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
    spi_transaction_t trans = {0};
    trans.length = 8U;
    trans.rxlength = 8U;
    trans.flags = (SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA);
    trans.tx_data[0U] = data;
    spi_device_polling_transmit(EVE_spi_device_simple, &trans);

    return (trans.rx_data[0U]);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* ESP_PLATFORM */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_STM32_H */
