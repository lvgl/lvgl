#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

#if LV_USE_EVDEV && defined(__linux__)

#include <linux/input.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void write_input_event(int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct input_event ev;
    lv_memzero(&ev, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    ssize_t w = write(fd, &ev, sizeof(ev));
    TEST_ASSERT_EQUAL_INT((ssize_t)sizeof(ev), w);
}

/**
 * Reproduces a fast tap whose press and release land in the kernel's evdev
 * buffer between two calls to the driver's read callback. _evdev_read drains
 * the whole buffer in one go, so the press transition must be reported on
 * the first call (with continue_reading set so the release is delivered on
 * the next call).
 */
void test_evdev_fast_tap_press_must_not_be_dropped(void)
{
    int sv[2];
    TEST_ASSERT_EQUAL_INT(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv));

    /* The driver takes ownership of sv[1] and closes it on indev delete. */
    lv_indev_t * indev = lv_evdev_create_fd(LV_INDEV_TYPE_POINTER, sv[1]);
    TEST_ASSERT_NOT_NULL(indev);
    lv_evdev_set_calibration(indev, 0, 0, 800, 480);

    /* Queue a complete tap (press + release) before the driver reads anything. */
    write_input_event(sv[0], EV_ABS, ABS_X, 100);
    write_input_event(sv[0], EV_ABS, ABS_Y, 100);
    write_input_event(sv[0], EV_KEY, BTN_TOUCH, 1);
    write_input_event(sv[0], EV_SYN, SYN_REPORT, 0);
    write_input_event(sv[0], EV_KEY, BTN_TOUCH, 0);
    write_input_event(sv[0], EV_SYN, SYN_REPORT, 0);

    lv_indev_read_cb_t read_cb = lv_indev_get_read_cb(indev);
    TEST_ASSERT_NOT_NULL(read_cb);

    lv_indev_data_t data;
    lv_memzero(&data, sizeof(data));
    read_cb(indev, &data);

    /* The press transition must be visible to LVGL on this read. */
    TEST_ASSERT_EQUAL_INT(LV_INDEV_STATE_PRESSED, data.state);
    /* And the driver must ask to be re-invoked so the release is reported next. */
    TEST_ASSERT_TRUE(data.continue_reading);

    lv_memzero(&data, sizeof(data));
    read_cb(indev, &data);
    TEST_ASSERT_EQUAL_INT(LV_INDEV_STATE_RELEASED, data.state);

    lv_indev_delete(indev);
    close(sv[0]);
}

#endif /*LV_USE_EVDEV && __linux__*/
#endif /*LV_BUILD_TEST*/
