#ifndef EVE_TARGET_H
#define EVE_TARGET_H

#include "../../draw/eve/lv_draw_eve_target.h"
#include "../../tick/lv_tick.h"
#include "../../misc/lv_utils.h"

static inline void DELAY_MS(uint16_t ms)
{
    lv_delay_ms(ms);
}

static inline void EVE_cs_set(void)
{
    lv_draw_eve_target_spi_cs(true);
}

static inline void EVE_cs_clear(void)
{
    lv_draw_eve_target_spi_cs(false);
}

static inline void EVE_pdn_set(void)
{
    lv_draw_eve_target_powerdown(true);
}

static inline void EVE_pdn_clear(void)
{
    lv_draw_eve_target_powerdown(false);
}

static inline void spi_transmit(uint8_t data)
{
    lv_draw_eve_target_spi_transmit(&data, 1);
}

static inline void spi_transmit_32(uint32_t data)
{
#if LV_BIG_ENDIAN_SYSTEM
    data = lv_swap_bytes_32(data);
#endif
    lv_draw_eve_target_spi_transmit((uint8_t *) &data, 4);
}

static inline void spi_transmit_burst(uint32_t data)
{
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    uint8_t byte;
    lv_draw_eve_target_spi_receive(&byte, 1);

    return byte;
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* EVE_TARGET_H_ */
