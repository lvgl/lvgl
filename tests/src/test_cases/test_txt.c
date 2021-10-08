#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static const char color_cmd = LV_TXT_COLOR_CMD[0];

void test_txt_next_line_should_handle_empty_string(void);
void test_txt_should_identify_valid_start_of_command(void);
void test_txt_should_identify_invalid_start_of_command(void);
void test_txt_should_identify_scaped_command_in_parameter(void);
void test_txt_should_skip_color_parameter_in_parameter(void);
void test_txt_should_reset_state_when_receiving_color_cmd_while_processing_commands(void);
void test_txt_should_identify_space_after_parameter(void);
void test_txt_should_insert_string_into_another(void);
void test_txt_should_handle_null_pointers_when_inserting(void);

void test_txt_should_identify_valid_start_of_command(void)
{
    uint32_t character = color_cmd;
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_WAIT;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_TRUE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_PAR);
}

void test_txt_should_identify_invalid_start_of_command(void)
{
    uint32_t character = '$';
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_WAIT;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_FALSE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_WAIT);
}

void test_txt_should_identify_scaped_command_in_parameter(void)
{
    uint32_t character = color_cmd;
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_PAR;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_FALSE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_WAIT);
}

void test_txt_should_skip_color_parameter_in_parameter(void)
{
    uint32_t character = '$';
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_PAR;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_TRUE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_PAR);
}

void test_txt_should_reset_state_when_receiving_color_cmd_while_processing_commands(void)
{
    uint32_t character = color_cmd;
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_IN;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_TRUE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_WAIT);
}

void test_txt_should_identify_space_after_parameter(void)
{
    uint32_t character = ' ';
    lv_text_cmd_state_t state = LV_TEXT_CMD_STATE_PAR;

    bool is_cmd = _lv_txt_is_cmd(&state, character);

    TEST_ASSERT_TRUE(is_cmd);
    TEST_ASSERT_EQUAL_UINT8(state, LV_TEXT_CMD_STATE_IN);
}

void test_txt_should_insert_string_into_another(void)
{
    const char *msg = "Hello ";
    const char *suffix = "World";
    char target[20] = {0};
    size_t msg_len = strlen(msg);

    strcpy(target, msg);

    _lv_txt_ins(target, msg_len, suffix);

    TEST_ASSERT_EQUAL_STRING("Hello World", target);
}

void test_txt_should_handle_null_pointers_when_inserting(void)
{
    const char *msg = "Hello ";
    char target[20] = {0};
    size_t msg_len = strlen(msg);

    strcpy(target, msg);

    _lv_txt_ins(target, msg_len, NULL);

    TEST_ASSERT_EQUAL_STRING("Hello ", target);
}

/* See #2615 for more information */
void test_txt_next_line_should_handle_empty_string(void)
{
    const lv_font_t *font_ptr = NULL;
    lv_coord_t letter_space = 0;
    lv_coord_t max_width = 0;
    lv_text_flag_t flag = LV_TEXT_FLAG_NONE;

    uint32_t next_line = _lv_txt_get_next_line("", font_ptr, letter_space, max_width, flag);

    TEST_ASSERT_EQUAL_UINT32(0, next_line);
}

#endif
