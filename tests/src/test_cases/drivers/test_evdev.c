#define _GNU_SOURCE

#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/input.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

static char input_dir[PATH_MAX];
static char device_path[PATH_MAX];
static bool deny_device_access;
static unsigned int device_open_attempts;
static lv_indev_t * discovered[4];
static unsigned int discovered_count;
static lv_indev_t * owned_indev;
static int pipe_fds[2];

int __real_open(const char * path, int flags, ...);
DIR * __real_opendir(const char * path);
int __real_inotify_add_watch(int fd, const char * path, uint32_t mask);
int __real_ioctl(int fd, unsigned long request, ...);
int __wrap_open(const char * path, int flags, ...);
DIR * __wrap_opendir(const char * path);
int __wrap_inotify_add_watch(int fd, const char * path, uint32_t mask);
int __wrap_ioctl(int fd, unsigned long request, ...);

int __wrap_open(const char * path, int flags, ...)
{
    mode_t mode = 0;
    if(flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
    }
    if(input_dir[0] && strcmp(path, "/dev/input/event0") == 0) {
        device_open_attempts++;
        if(deny_device_access) {
            errno = EACCES;
            return -1;
        }
        path = device_path;
    }
    return __real_open(path, flags, mode);
}

DIR * __wrap_opendir(const char * path)
{
    return __real_opendir(input_dir[0] && strcmp(path, "/dev/input/") == 0 ? input_dir : path);
}

int __wrap_inotify_add_watch(int fd, const char * path, uint32_t mask)
{
    if(input_dir[0] && strcmp(path, "/dev/input/") == 0) {
        path = input_dir;
    }
    return __real_inotify_add_watch(fd, path, mask);
}

int __wrap_ioctl(int fd, unsigned long request, ...)
{
    va_list args;
    va_start(args, request);
    void * arg = va_arg(args, void *);
    va_end(args);
    struct stat device_stat;
    struct stat fd_stat;
    if(input_dir[0] && stat(device_path, &device_stat) == 0 && fstat(fd, &fd_stat) == 0 &&
       device_stat.st_dev == fd_stat.st_dev && device_stat.st_ino == fd_stat.st_ino &&
       _IOC_TYPE(request) == 'E' && _IOC_NR(request) >= 0x20 && _IOC_NR(request) <= 0x20 + EV_MAX) {
        memset(arg, 0, _IOC_SIZE(request));
        if(_IOC_NR(request) == 0x20 + EV_KEY && _IOC_SIZE(request) > KEY_A / 8)
            ((unsigned char *)arg)[KEY_A / 8] = 1u << (KEY_A % 8);
        return (int)_IOC_SIZE(request);
    }
    return __real_ioctl(fd, request, arg);
}

void setUp(void)
{
    input_dir[0] = device_path[0] = '\0';
    deny_device_access = false;
    device_open_attempts = discovered_count = 0;
    memset(discovered, 0, sizeof(discovered));
    owned_indev = NULL;
    pipe_fds[0] = pipe_fds[1] = -1;
}

void tearDown(void)
{
    lv_evdev_discovery_stop();
    if(owned_indev) lv_evdev_delete(owned_indev);
    for(unsigned int i = 0; i < discovered_count; i++) lv_evdev_delete(discovered[i]);
    for(unsigned int i = 0; i < 2; i++) {
        if(pipe_fds[i] >= 0) close(pipe_fds[i]);
    }
    if(device_path[0]) unlink(device_path);
    if(input_dir[0]) rmdir(input_dir);
}

static void discovered_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
{
    LV_UNUSED(user_data);
    TEST_ASSERT_EQUAL(LV_EVDEV_TYPE_KEY, type);
    TEST_ASSERT_LESS_THAN_UINT32(4, discovered_count);
    discovered[discovered_count++] = indev;
}

void test_evdev_get_fd_rejects_other_input_drivers(void)
{
    TEST_ASSERT_EQUAL_INT(-1, lv_evdev_get_fd(NULL));
    owned_indev = lv_indev_create();
    TEST_ASSERT_NOT_NULL(owned_indev);
    TEST_ASSERT_EQUAL_INT(-1, lv_evdev_get_fd(owned_indev));
    lv_indev_delete(owned_indev);
    owned_indev = NULL;
}

void test_evdev_get_fd_keeps_driver_ownership(void)
{
    TEST_ASSERT_EQUAL_INT(0, pipe(pipe_fds));
    owned_indev = lv_evdev_create_fd(LV_INDEV_TYPE_KEYPAD, pipe_fds[0]);
    TEST_ASSERT_NOT_NULL(owned_indev);
    int fd = pipe_fds[0];
    pipe_fds[0] = -1;
    TEST_ASSERT_EQUAL_INT(fd, lv_evdev_get_fd(owned_indev));
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, fcntl(fd, F_GETFD));
    lv_evdev_delete(owned_indev);
    owned_indev = NULL;
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, fcntl(fd, F_GETFD));
    TEST_ASSERT_EQUAL_INT(EBADF, errno);
}

void test_evdev_discovery_retries_permissions_and_deduplicates(void)
{
    strcpy(input_dir, "/tmp/lvgl-evdev-XXXXXX");
    TEST_ASSERT_NOT_NULL(mkdtemp(input_dir));
    TEST_ASSERT_LESS_THAN_INT((int)sizeof(device_path),
                              snprintf(device_path, sizeof(device_path), "%s/event0", input_dir));
    int fd = __real_open(device_path, O_CREAT | O_WRONLY, 0600);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, fd);
    close(fd);

    /* Simulate udev granting access after IN_CREATE, even when CI runs as root. */
    deny_device_access = true;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_evdev_discovery_start(discovered_cb, NULL));
    TEST_ASSERT_EQUAL_UINT32(1, device_open_attempts);
    TEST_ASSERT_EQUAL_UINT32(0, discovered_count);
    deny_device_access = false;
    TEST_ASSERT_EQUAL_INT(0, chmod(device_path, 0640));
    lv_test_wait(LV_DEF_REFR_PERIOD * 2);
    TEST_ASSERT_EQUAL_UINT32(1, discovered_count);

    unsigned char keys[KEY_A / 8 + 1] = {0};
    fd = lv_evdev_get_fd(discovered[0]);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, fd);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys));
    TEST_ASSERT_BITS_HIGH(1u << (KEY_A % 8), keys[KEY_A / 8]);

    TEST_ASSERT_EQUAL_INT(0, chmod(device_path, 0600));
    lv_test_wait(LV_DEF_REFR_PERIOD * 2);
    TEST_ASSERT_EQUAL_UINT32(1, discovered_count);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, fcntl(fd, F_GETFD));

    TEST_ASSERT_EQUAL_INT(0, unlink(device_path));
    int replacement = __real_open(device_path, O_CREAT | O_WRONLY, 0600);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, replacement);
    close(replacement);
    lv_test_wait(LV_DEF_REFR_PERIOD * 2);
    TEST_ASSERT_EQUAL_UINT32(2, discovered_count);
}
#endif
