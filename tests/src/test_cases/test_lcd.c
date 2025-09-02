#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

#define TEST_LCD_WIDTH 64
#define TEST_LCD_HEIGHT 32
#define TEST_LCD_X_GAP 10
#define TEST_LCD_Y_GAP 20
#define TEST_LCD_GAMMA 30
#define TEST_LCD_BUF_SIZE 32

typedef struct {
    uint8_t cmd_buf[TEST_LCD_BUF_SIZE];
    uint8_t param_buf[TEST_LCD_BUF_SIZE];
    uint8_t cmd_size;
    uint8_t param_size;
} test_lcd_ctx_t;

typedef lv_display_t * (*lcd_create_cb_t)(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                          lv_lcd_send_cmd_cb_t send_cmd_cb, lv_lcd_send_color_cb_t send_color_cb);
typedef void (*lcd_set_gap_cb_t)(lv_display_t * disp, uint16_t x, uint16_t y);
typedef void (*lcd_set_invert_cb_t)(lv_display_t * disp, bool invert);
typedef void (*lcd_set_gamma_curve_cb_t)(lv_display_t * disp, uint8_t gamma);
typedef void (*lcd_send_cmd_list_cb_t)(lv_display_t * disp, const uint8_t * cmd_list);

static const uint8_t test_cmd_list[] = {
    0xAA, 4, 0x11, 0x22, 0x33, 0x44,
    LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF
};

static void test_disp_resolution_with_rotation(lv_display_t * disp)
{
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);

    switch(lv_display_get_rotation(disp)) {
        case LV_DISPLAY_ROTATION_0:
        case LV_DISPLAY_ROTATION_180:
            TEST_ASSERT_EQUAL_INT32(hor_res, TEST_LCD_WIDTH);
            TEST_ASSERT_EQUAL_INT32(ver_res, TEST_LCD_HEIGHT);
            break;

        case LV_DISPLAY_ROTATION_90:
        case LV_DISPLAY_ROTATION_270:
            TEST_ASSERT_EQUAL_INT32(hor_res, TEST_LCD_HEIGHT);
            TEST_ASSERT_EQUAL_INT32(ver_res, TEST_LCD_WIDTH);
            break;

        default:
            TEST_ASSERT(false);
            break;
    }
}

/* Mock functions */
static void lcd_send_cmd_cb(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, const uint8_t * param,
                            size_t param_size)
{
    TEST_ASSERT_NOT_NULL(disp);

    test_disp_resolution_with_rotation(disp);

    TEST_ASSERT_NOT_NULL(cmd);
    TEST_ASSERT_GREATER_THAN(0, cmd_size);

    test_lcd_ctx_t * ctx = lv_display_get_user_data(disp);
    if(!ctx) return;

    TEST_ASSERT(cmd_size > 0 && cmd_size <= TEST_LCD_BUF_SIZE);
    lv_memcpy(ctx->cmd_buf, cmd, cmd_size);
    ctx->cmd_size = cmd_size;

    if(param) {
        lv_memcpy(ctx->param_buf, param, LV_MIN(param_size, TEST_LCD_BUF_SIZE));
    }
    ctx->param_size = param_size;
}

static void lcd_send_color_cb(lv_display_t * disp, const uint8_t * cmd, size_t cmd_size, uint8_t * param,
                              size_t param_size)
{
    /* Use the same function as for cmd */
    lcd_send_cmd_cb(disp, cmd, cmd_size, param, param_size);
}

static void test_disp_full_rotation(lv_display_t * disp)
{
    static const lv_display_rotation_t rotations[] = {
        LV_DISPLAY_ROTATION_0,
        LV_DISPLAY_ROTATION_90,
        LV_DISPLAY_ROTATION_180,
        LV_DISPLAY_ROTATION_270,
    };

    for(size_t i = 0; i < sizeof(rotations) / sizeof(rotations[0]); i++) {
        lv_display_set_rotation(disp, rotations[i]);
        TEST_ASSERT(lv_display_get_rotation(disp) == rotations[i]);
    }
}

static void flush_wait_cb(lv_display_t * disp)
{
    TEST_ASSERT_NOT_NULL(disp);
}

static void test_lcd_generic_mipi(lcd_create_cb_t create_cb,
                                  lcd_set_gap_cb_t set_gap_cb,
                                  lcd_set_invert_cb_t set_invert_cb,
                                  lcd_set_gamma_curve_cb_t set_gamma_curve_cb,
                                  lcd_send_cmd_list_cb_t send_cmd_list_cb)
{
    lv_display_t * disp = create_cb(TEST_LCD_WIDTH, TEST_LCD_HEIGHT, LV_LCD_FLAG_NONE, lcd_send_cmd_cb,
                                    lcd_send_color_cb);
    TEST_ASSERT_NOT_NULL(disp);

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(64, 64, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_display_set_draw_buffers(disp, draw_buf, NULL);
    lv_display_set_flush_wait_cb(disp, flush_wait_cb);

    test_lcd_ctx_t ctx;
    lv_memzero(&ctx, sizeof(test_lcd_ctx_t));
    lv_display_set_user_data(disp, &ctx);

    lv_lcd_generic_mipi_driver_t * driver = lv_display_get_driver_data(disp);

    if(set_gap_cb) {
        set_gap_cb(disp, TEST_LCD_X_GAP, TEST_LCD_Y_GAP);
        TEST_ASSERT_EQUAL_UINT16(driver->x_gap, TEST_LCD_X_GAP);
        TEST_ASSERT_EQUAL_UINT16(driver->y_gap, TEST_LCD_Y_GAP);
    }

    if(set_invert_cb) {
        set_invert_cb(disp, true);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_buf[0], LV_LCD_CMD_ENTER_INVERT_MODE);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_size, 1);

        set_invert_cb(disp, false);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_buf[0], LV_LCD_CMD_EXIT_INVERT_MODE);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_size, 1);
    }

    if(set_gamma_curve_cb) {
        set_gamma_curve_cb(disp, TEST_LCD_GAMMA);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_buf[0], LV_LCD_CMD_SET_GAMMA_CURVE);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_size, 1);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_buf[0], TEST_LCD_GAMMA);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_size, 1);
    }

    if(send_cmd_list_cb) {
        send_cmd_list_cb(disp, test_cmd_list);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_buf[0], 0xAA);
        TEST_ASSERT_EQUAL_UINT8(ctx.cmd_size, 1);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_buf[0], 0x11);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_buf[1], 0x22);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_buf[2], 0x33);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_buf[3], 0x44);
        TEST_ASSERT_EQUAL_UINT8(ctx.param_size, 4);
    }

    lv_lcd_generic_mipi_set_address_mode(disp, true, true, true, true);
    TEST_ASSERT_TRUE(driver->madctl_reg & LV_LCD_BIT_RGB_ORDER__BGR);
    TEST_ASSERT_TRUE(driver->mirror_x);
    TEST_ASSERT_TRUE(driver->mirror_y);
    TEST_ASSERT_TRUE(driver->swap_xy);

    lv_lcd_generic_mipi_set_address_mode(disp, false, false, false, false);
    TEST_ASSERT_FALSE(driver->madctl_reg & LV_LCD_BIT_RGB_ORDER__BGR);
    TEST_ASSERT_FALSE(driver->mirror_x);
    TEST_ASSERT_FALSE(driver->mirror_y);
    TEST_ASSERT_FALSE(driver->swap_xy);

    test_disp_full_rotation(disp);

    lv_refr_now(disp);

    lv_display_delete(disp);
    lv_draw_buf_destroy(draw_buf);
}

static void lcd_delay_cb(uint32_t ms)
{
    TEST_ASSERT_GREATER_THAN_UINT32(0, ms);
    lv_tick_inc(ms);
}

void setUp(void)
{
    lv_delay_set_cb(lcd_delay_cb);
}

void tearDown(void)
{
    lv_delay_set_cb(NULL);
}

void test_lcd_st7735(void)
{
    test_lcd_generic_mipi(lv_st7735_create, lv_st7735_set_gap, lv_st7735_set_invert, lv_st7735_set_gamma_curve,
                          lv_st7735_send_cmd_list);
}

void test_lcd_st7789(void)
{
    test_lcd_generic_mipi(lv_st7789_create, lv_st7789_set_gap, lv_st7789_set_invert, lv_st7789_set_gamma_curve,
                          lv_st7789_send_cmd_list);
}

void test_lcd_st7796(void)
{
    test_lcd_generic_mipi(lv_st7796_create, lv_st7796_set_gap, lv_st7796_set_invert, NULL,
                          lv_st7796_send_cmd_list);

    /* NOTE: lv_st7796_set_gamma_curve is not supported, the setting should have no effect */
    lv_st7796_set_gamma_curve(NULL, 0);
}

void test_lcd_ili9341(void)
{
    test_lcd_generic_mipi(lv_ili9341_create, lv_ili9341_set_gap, lv_ili9341_set_invert, lv_ili9341_set_gamma_curve,
                          lv_ili9341_send_cmd_list);
}

static uint32_t lv_ft81x_encode_read_address(uint32_t address)
{
    return lv_swap_bytes_32(address << 8);
}

static void lcd_ft81x_spi_cb(lv_display_t * disp, lv_ft81x_spi_operation_t operation, void * data, uint32_t length)
{
    TEST_ASSERT_NOT_NULL(disp);

    test_disp_resolution_with_rotation(disp);

    static uint32_t reg_addr = 0;

    /* Mocked FT81X registers */
#define REG_ID               0x302000UL
#define REG_CPURESET         0x302020UL
#define REG_CMD_READ         0x3020f8UL
#define REG_CMD_WRITE        0x3020fcUL

    switch(operation) {
        case LV_FT81X_SPI_OPERATION_CS_ASSERT:
            break;

        case LV_FT81X_SPI_OPERATION_CS_DEASSERT:
            break;

        case LV_FT81X_SPI_OPERATION_SEND:
            reg_addr = 0;
            lv_memcpy(&reg_addr, data, LV_MIN(length, sizeof(reg_addr)));
            break;

        case LV_FT81X_SPI_OPERATION_RECEIVE:
            if(reg_addr == lv_ft81x_encode_read_address(REG_ID)) {
                *(uint8_t *)data = 0x7c;
            }
            else if(reg_addr == lv_ft81x_encode_read_address(REG_CPURESET)) {
                *(uint8_t *)data = 0;
            }
            else if(reg_addr == lv_ft81x_encode_read_address(REG_CMD_READ)) {
                *(uint16_t *)data = 0;
            }
            else if(reg_addr == lv_ft81x_encode_read_address(REG_CMD_WRITE)) {
                *(uint16_t *)data = 0;
            }
            break;

        default:
            TEST_FAIL_MESSAGE("Invalid operation");
            break;
    }

    if(data) {
        TEST_ASSERT_GREATER_THAN_UINT32(0, length);
    }
}

void test_lcd_ft81x(void)
{
    lv_ft81x_parameters_t params;
    lv_memzero(&params, sizeof(lv_ft81x_parameters_t));
    params.hor_res = TEST_LCD_WIDTH;
    params.ver_res = TEST_LCD_HEIGHT;

    lv_draw_buf_t * partial_buf = lv_draw_buf_create(TEST_LCD_WIDTH, TEST_LCD_HEIGHT, LV_COLOR_FORMAT_RGB565,
                                                     LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(partial_buf);

    uint32_t user_data = 0x1234;

    lv_display_t * disp = lv_ft81x_create(
                              &params,
                              partial_buf->data,
                              partial_buf->data_size,
                              lcd_ft81x_spi_cb,
                              &user_data);
    TEST_ASSERT_NOT_NULL(disp);

    const uint32_t * user_data_ptr = lv_ft81x_get_user_data(disp);
    TEST_ASSERT_EQUAL_PTR(user_data_ptr, &user_data);
    TEST_ASSERT_EQUAL_UINT32(*user_data_ptr, 0x1234);

    test_disp_full_rotation(disp);

    lv_refr_now(disp);

    lv_display_delete(disp);

    lv_draw_buf_destroy(partial_buf);
}

#endif
