/**
 * @file lv_evdev.c
 *
 */

/**********************
 *      INCLUDES
 **********************/
#include "lv_evdev_private.h"
#if LV_USE_EVDEV

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h> /*To detect BSD*/
#ifdef BSD
    #include <dev/evdev/input.h>
#else
    #include <linux/input.h>
    #include <sys/inotify.h>
#endif /*BSD*/
#include "../../core/lv_global.h"
#include "../../misc/lv_types.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_async.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../../display/lv_display.h"
#include "../../display/lv_display_private.h"
#include "../../widgets/image/lv_image.h"
#include "../../indev/lv_indev_gesture.h"

/*********************
 *      DEFINES
 *********************/

#define evdev_discovery LV_GLOBAL_DEFAULT()->evdev_discovery
#define EVDEV_DISCOVERY_PATH "/dev/input/"
#define EVDEV_DISCOVERY_PATH_BUF_SIZE 32
#define REL_XY_MASK ((1 << REL_X) | (1 << REL_Y))
#define ABS_XY_MASK ((1 << ABS_X) | (1 << ABS_Y))
#define MAX_TOUCH_POINTS 5

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /*Device*/
    int fd;
    dev_t st_dev;
    ino_t st_ino;
    lv_evdev_type_t type;
    /*Config*/
    bool swap_axes;
    int min_x;
    int min_y;
    int max_x;
    int max_y;
    /*State*/
    int root_x;
    int root_y;
    int key;
    lv_indev_state_t state;
    bool deleting;
    /* Multi-touch support */
#if LV_USE_GESTURE_RECOGNITION
    lv_indev_touch_data_t touch_data[MAX_TOUCH_POINTS]; /* Array of touch points for gesture recognition */
    uint8_t touch_count; /* Number of valid touch points */
    uint8_t current_slot; /* Current touch point slot */
    bool touch_data_changed; /* Flag to indicate if touch data has changed since last SYN_REPORT */
#endif
} lv_evdev_t;

#ifndef BSD
struct _lv_evdev_discovery_t {
    lv_evdev_discovery_cb_t cb;
    void * cb_user_data;
    int inotify_fd;
    bool inotify_watch_active;
    lv_timer_t * timer;
};
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int _evdev_process_key(uint16_t code)
{
    switch(code) {
        case KEY_UP:
            return LV_KEY_UP;
        case KEY_DOWN:
            return LV_KEY_DOWN;
        case KEY_RIGHT:
            return LV_KEY_RIGHT;
        case KEY_LEFT:
            return LV_KEY_LEFT;
        case KEY_ESC:
            return LV_KEY_ESC;
        case KEY_DELETE:
            return LV_KEY_DEL;
        case KEY_BACKSPACE:
            return LV_KEY_BACKSPACE;
        case KEY_ENTER:
            return LV_KEY_ENTER;
        case KEY_NEXT:
        case KEY_TAB:
            return LV_KEY_NEXT;
        case KEY_PREVIOUS:
            return LV_KEY_PREV;
        case KEY_HOME:
            return LV_KEY_HOME;
        case KEY_END:
            return LV_KEY_END;
        default:
            return 0;
    }
}

static int _evdev_calibrate(int v, int in_min, int in_max, int out_min, int out_max)
{
    if(in_min != in_max) v = (v - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return LV_CLAMP(out_min, v, out_max);
}

static lv_point_t _evdev_process_pointer(lv_indev_t * indev, int x, int y)
{
    lv_display_t * disp = lv_indev_get_display(indev);
    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);

    int swapped_x = dsc->swap_axes ? y : x;
    int swapped_y = dsc->swap_axes ? x : y;

    int offset_x = disp->offset_x;
    int offset_y = disp->offset_y;
    int width = disp->hor_res;
    int height = disp->ver_res;

    lv_point_t p;
    p.x = _evdev_calibrate(swapped_x, dsc->min_x, dsc->max_x, offset_x, offset_x + width - 1);
    p.y = _evdev_calibrate(swapped_y, dsc->min_y, dsc->max_y, offset_y, offset_y + height - 1);
    return p;
}

static void _evdev_async_delete_cb(void * user_data)
{
    lv_indev_t * indev = user_data;
    lv_indev_delete(indev);
}

static void _evdev_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);

    /*Update dsc with buffered events*/
    struct input_event in = { 0 };
    ssize_t br;
    while((br = read(dsc->fd, &in, sizeof(in))) > 0) {
        if(in.type == EV_REL) {
            if(in.code == REL_X) dsc->root_x += in.value;
            else if(in.code == REL_Y) dsc->root_y += in.value;
        }
        else if(in.type == EV_ABS) {
#if LV_USE_GESTURE_RECOGNITION
            if(in.code == ABS_MT_SLOT) {
                if(in.value >= MAX_TOUCH_POINTS) {
                    dsc->current_slot = MAX_TOUCH_POINTS - 1;
                    dsc->touch_count = MAX_TOUCH_POINTS;
                    LV_LOG_WARN("Touch point slot out of range, setting to max: %d", MAX_TOUCH_POINTS - 1);
                }
                else {
                    dsc->current_slot = in.value;
                    dsc->touch_count = LV_MAX(dsc->touch_count, dsc->current_slot + 1);
                    LV_LOG_TRACE("Slot changed to %d, touch_count=%d", dsc->current_slot, dsc->touch_count);
                }
            }
            else
#endif
                if(in.code == ABS_X || in.code == ABS_MT_POSITION_X) {
                    dsc->root_x = in.value;
#if LV_USE_GESTURE_RECOGNITION
                    if(in.code == ABS_MT_POSITION_X && dsc->current_slot < MAX_TOUCH_POINTS) {
                        dsc->touch_data[dsc->current_slot].point.x = in.value;
                        dsc->touch_data_changed = true;
                        LV_LOG_TRACE("MT_X update: slot=%d, x=%d", dsc->current_slot, in.value);
                    }
#endif
                }
                else if(in.code == ABS_Y || in.code == ABS_MT_POSITION_Y) {
                    dsc->root_y = in.value;
#if LV_USE_GESTURE_RECOGNITION
                    if(in.code == ABS_MT_POSITION_Y && dsc->current_slot < MAX_TOUCH_POINTS) {
                        dsc->touch_data[dsc->current_slot].point.y = in.value;
                        dsc->touch_data_changed = true;
                        LV_LOG_TRACE("MT_Y update: slot=%d, y=%d", dsc->current_slot, in.value);
                    }
#endif
                }
                else if(in.code == ABS_MT_TRACKING_ID) {
                    if(in.value == -1) dsc->state = LV_INDEV_STATE_RELEASED;
                    else dsc->state = LV_INDEV_STATE_PRESSED;
#if LV_USE_GESTURE_RECOGNITION
                    if(in.value == -1) {
                        if(dsc->current_slot < MAX_TOUCH_POINTS) {
                            dsc->touch_data[dsc->current_slot].state = LV_INDEV_STATE_RELEASED;
                            dsc->touch_data_changed = true;
                            LV_LOG_TRACE("Touch slot %d released", dsc->current_slot);

                            dsc->touch_count = 0;
                            for(int i = 0; i < MAX_TOUCH_POINTS; i++) {
                                if(dsc->touch_data[i].state == LV_INDEV_STATE_PRESSED) {
                                    dsc->touch_count = i + 1;
                                }
                            }
                        }
                    }
                    else {
                        if(dsc->current_slot < MAX_TOUCH_POINTS) {
                            dsc->touch_data[dsc->current_slot].state = LV_INDEV_STATE_PRESSED;
                            dsc->touch_data[dsc->current_slot].id = dsc->current_slot;
                            dsc->touch_count = LV_MAX(dsc->touch_count, dsc->current_slot + 1);
                            dsc->touch_data_changed = true;
                            LV_LOG_TRACE("Touch slot %d pressed, touch_count=%d", dsc->current_slot, dsc->touch_count);
                        }
                    }
#endif
                }
        }
        else if(in.type == EV_KEY) {
            if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                if(in.value == 0) dsc->state = LV_INDEV_STATE_RELEASED;
                else if(in.value == 1) dsc->state = LV_INDEV_STATE_PRESSED;
            }
            else {
                dsc->key = _evdev_process_key(in.code);
                if(dsc->key) {
                    dsc->state = in.value ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
                    data->continue_reading = true; /*Keep following events in buffer for now*/
                    break;
                }
            }
        }
#if LV_USE_GESTURE_RECOGNITION
        else if(in.type == EV_SYN && in.code == SYN_REPORT) {
            /* Handle gesture recognition at sync event */
            if(dsc->touch_count > 0 && dsc->touch_data_changed) {
                LV_LOG_TRACE("=== SYN_REPORT: touch_count=%d ===", dsc->touch_count);
                for(int i = 0; i < MAX_TOUCH_POINTS; i++) {
                    if(dsc->touch_data[i].state == LV_INDEV_STATE_PRESSED || dsc->touch_data[i].state == LV_INDEV_STATE_RELEASED) {
                        LV_LOG_TRACE("Slot %d: state=%s, raw(%d, %d)",
                                     i,
                                     dsc->touch_data[i].state == LV_INDEV_STATE_PRESSED ? "PRESSED" : "RELEASED",
                                     dsc->touch_data[i].point.x, dsc->touch_data[i].point.y);
                    }
                }

                /* Create a temporary array with calibrated coordinates for gesture recognition */
                lv_indev_touch_data_t calibrated_touch_data[MAX_TOUCH_POINTS];

                int active_touches = 0;

                for(int i = 0; i < MAX_TOUCH_POINTS; i++) {
                    if(dsc->touch_data[i].state == LV_INDEV_STATE_PRESSED || dsc->touch_data[i].state == LV_INDEV_STATE_RELEASED) {
                        calibrated_touch_data[active_touches] = dsc->touch_data[i];

                        lv_point_t calib_point = _evdev_process_pointer(indev, dsc->touch_data[i].point.x, dsc->touch_data[i].point.y);
                        calibrated_touch_data[active_touches].point = calib_point;

                        LV_LOG_TRACE("Touch %d (slot %d): state=%s, raw(%d, %d) -> calib(%d, %d)",
                                     active_touches, i,
                                     dsc->touch_data[i].state == LV_INDEV_STATE_PRESSED ? "PRESSED" : "RELEASED",
                                     dsc->touch_data[i].point.x, dsc->touch_data[i].point.y,
                                     calib_point.x, calib_point.y);
                        active_touches++;
                    }
                }

                LV_LOG_TRACE("Gesture recognition: %d touches detected", active_touches);
                lv_indev_gesture_recognizers_update(indev, calibrated_touch_data, active_touches);
                lv_indev_gesture_recognizers_set_data(indev, data);

                /* Clear RELEASED touch points after gesture recognition to prevent duplicate processing */
                for(int i = 0; i < MAX_TOUCH_POINTS; i++) {
                    if(dsc->touch_data[i].state == LV_INDEV_STATE_RELEASED) {
                        /* Mark touch point as invalid by zeroing out the data */
                        dsc->touch_data[i].point.x = 0;
                        dsc->touch_data[i].point.y = 0;
                        dsc->touch_data[i].id = -1; /* Mark as invalid */
                        /* Note: We keep the RELEASED state for this frame, it will be naturally
                         * cleared when new touch events come in or when all touches end */
                        LV_LOG_TRACE("Cleared released touch point slot %d", i);
                    }
                }

                dsc->touch_data_changed = false;
            }
        }
#endif
    }

    if(!dsc->deleting && br == -1 && errno != EAGAIN) {
        if(errno == ENODEV) {
            LV_LOG_INFO("evdev device was removed");
        }
        else {
            LV_LOG_ERROR("read failed: %s", strerror(errno));
        }
        lv_async_call(_evdev_async_delete_cb, indev);
        dsc->deleting = true;
    }

    /*Process and store in data*/
    switch(lv_indev_get_type(indev)) {
        case LV_INDEV_TYPE_KEYPAD:
            data->state = dsc->state;
            data->key = dsc->key;
            break;
        case LV_INDEV_TYPE_POINTER:
#if LV_USE_GESTURE_RECOGNITION
            if(dsc->touch_count > 0) {
                data->state = dsc->touch_data[0].state;
                data->point = _evdev_process_pointer(indev, dsc->touch_data[0].point.x, dsc->touch_data[0].point.y);
            }
            else {
                data->state = dsc->state;
                data->point = _evdev_process_pointer(indev, dsc->root_x, dsc->root_y);
            }
#else
            data->state = dsc->state;
            data->point = _evdev_process_pointer(indev, dsc->root_x, dsc->root_y);
#endif
            break;
        default:
            break;
    }
}

static void _evdev_indev_delete_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_event_get_target(e);
    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);
    lv_async_call_cancel(_evdev_async_delete_cb, indev);
    close(dsc->fd);
    lv_free(dsc);
}

#ifndef BSD
static void _evdev_discovery_indev_try_create(const char * file_name)
{
    if(0 != lv_strncmp(file_name, "event", 5)) {
        return;
    }

    char dev_path[EVDEV_DISCOVERY_PATH_BUF_SIZE];
    lv_snprintf(dev_path, sizeof(dev_path), EVDEV_DISCOVERY_PATH "%s", file_name);

    lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_NONE, dev_path);
    if(indev == NULL) return;

    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);

    /* Compare this new evdev's unique identity with the already registered ones.
     * If a match is found, it means the user has already added it and a duplicate
     * should not be added automatically -- although it is valid for `lv_evdev_create`
     * to be explicitly called with the same path by the user -- or an edge case
     * has occurred where discoverey has just been started and a new device was
     * connected between the creation of the inotify watcher and the initial full
     * scan of the directory with `readdir`.
     */
    lv_indev_t * ex_indev = NULL;
    while(NULL != (ex_indev = lv_indev_get_next(ex_indev))) {
        if(ex_indev == indev || lv_indev_get_read_cb(ex_indev) != _evdev_read) continue;
        lv_evdev_t * ex_dsc = lv_indev_get_driver_data(ex_indev);
        if(!ex_dsc->deleting && dsc->st_dev == ex_dsc->st_dev && dsc->st_ino == ex_dsc->st_ino) {
            /* an indev for this exact device instance already exists */
            lv_indev_delete(indev);
            return;
        }
    }

    lv_evdev_discovery_t * ed = evdev_discovery;
    if(ed->cb) {
        ed->cb(indev, dsc->type, ed->cb_user_data);
    }
}

static bool _evdev_discovery_inotify_try_init_watcher(int inotify_fd)
{
    int inotify_wd = inotify_add_watch(inotify_fd, EVDEV_DISCOVERY_PATH, IN_CREATE);
    if(inotify_wd == -1) {
        if(errno != ENOENT) {
            LV_LOG_ERROR("inotify_add_watch failed: %s", strerror(errno));
        }
        return false;
    }

    DIR * dir = opendir(EVDEV_DISCOVERY_PATH);
    if(dir == NULL) {
        if(errno != ENOENT) {
            LV_LOG_ERROR("opendir failed: %s", strerror(errno));
        }
        inotify_rm_watch(inotify_fd, inotify_wd);
        return false;
    }
    while(1) {
        struct dirent * dirent = readdir(dir);
        if(dirent == NULL) break; /* only possible error is EBADF, so no errno check needed */
        _evdev_discovery_indev_try_create(dirent->d_name);
        if(evdev_discovery == NULL) {
            /* was stopped by the callback. cleanup was already done */
            closedir(dir);
            return false;
        }
    }
    closedir(dir);

    return true;
}

static void _evdev_discovery_timer_cb(lv_timer_t * tim)
{
    LV_UNUSED(tim);
    lv_evdev_discovery_t * ed = evdev_discovery;
    LV_ASSERT_NULL(ed);

    if(!ed->inotify_watch_active) {
        ed->inotify_watch_active = _evdev_discovery_inotify_try_init_watcher(ed->inotify_fd);
        return;
    }

    union {
        struct inotify_event in_ev;
        uint8_t buf[sizeof(struct inotify_event) + NAME_MAX + 1];
    } in_data;
    ssize_t br;
    while((br = read(ed->inotify_fd, &in_data, sizeof(in_data))) > 0) {
        struct inotify_event * in_ev_p;
        for(uint8_t * in_data_buf_p = in_data.buf;
            in_data_buf_p < in_data.buf + br;
            in_data_buf_p += sizeof(struct inotify_event) + in_ev_p->len) {
            in_ev_p = (struct inotify_event *)in_data_buf_p;
            if(in_ev_p->mask & IN_IGNORED) {
                /* /dev/input/ was deleted because the last device was removed.
                 * The watch was removed implicitly. It will try to be
                 * recreated the next time the timer runs.
                 */
                ed->inotify_watch_active = false;
                return;
            }
            if(!(in_ev_p->mask & IN_ISDIR) && in_ev_p->len) {
                _evdev_discovery_indev_try_create(in_ev_p->name);
                if(evdev_discovery == NULL) return; /* was stopped by the callback */
            }
        }
    }
    if(br == -1 && errno != EAGAIN) {
        LV_LOG_ERROR("inotify read failed: %s", strerror(errno));
        lv_evdev_discovery_stop();
    }
}
#endif /*BSD*/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_evdev_create_fd(lv_indev_type_t indev_type, int fd)
{
    lv_evdev_t * dsc = lv_malloc_zeroed(sizeof(lv_evdev_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) goto err_malloc;

    dsc->fd = fd;

    struct stat sb;
    if(0 != fstat(dsc->fd, &sb)) {
        LV_LOG_ERROR("fstat failed: %s", strerror(errno));
        goto err_after_malloc;
    }
    dsc->st_dev = sb.st_dev;
    dsc->st_ino = sb.st_ino;

    if(indev_type == LV_INDEV_TYPE_NONE) {
        uint32_t rel_bits = 0;
        if(ioctl(dsc->fd, EVIOCGBIT(EV_REL, sizeof(rel_bits)), &rel_bits) >= 0) {
            /* if this device can emit relative X and Y events, it shall be a pointer indev */
            if((rel_bits & REL_XY_MASK) == REL_XY_MASK) {
                indev_type = LV_INDEV_TYPE_POINTER;
                dsc->type = LV_EVDEV_TYPE_REL;
            }
        }
        else {
            LV_LOG_WARN("ioctl EVIOCGBIT(EV_REL, ...) failed: %s", strerror(errno));
        }
    }

    if(indev_type == LV_INDEV_TYPE_NONE) {
        uint32_t abs_bits = 0;
        if(ioctl(dsc->fd, EVIOCGBIT(EV_ABS, sizeof(abs_bits)), &abs_bits) >= 0) {
            /* if this device can emit absolute X and Y events, it shall be a pointer indev */
            if((abs_bits & ABS_XY_MASK) == ABS_XY_MASK) {
                indev_type = LV_INDEV_TYPE_POINTER;
                dsc->type = LV_EVDEV_TYPE_ABS;
            }
        }
        else {
            LV_LOG_WARN("ioctl EVIOCGBIT(EV_ABS, ...) failed: %s", strerror(errno));
        }
    }

    if(indev_type == LV_INDEV_TYPE_NONE) {
        uint32_t key_bits[KEY_MAX / 32 + 1] = {0};
        if(ioctl(dsc->fd, EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits) >= 0) {
            /* if this device can emit any key events, it shall be a keypad indev */
            for(int32_t i = 0; i < (int32_t)(sizeof(key_bits) / sizeof(uint32_t)); i++) {
                if(key_bits[i]) {
                    indev_type = LV_INDEV_TYPE_KEYPAD;
                    dsc->type = LV_EVDEV_TYPE_KEY;
                    break;
                }
            }
        }
        else {
            LV_LOG_WARN("ioctl EVIOCGBIT(EV_KEY, ...) failed: %s", strerror(errno));
        }
    }

    if(indev_type == LV_INDEV_TYPE_NONE) {
        goto err_after_malloc;
    }

    if(fcntl(dsc->fd, F_SETFL, O_NONBLOCK) < 0) {
        LV_LOG_ERROR("fcntl failed: %s", strerror(errno));
        goto err_after_malloc;
    }

    /* Detect the minimum and maximum values of the input device for calibration. */

    if(indev_type == LV_INDEV_TYPE_POINTER) {
        struct input_absinfo absinfo;
        if(ioctl(dsc->fd, EVIOCGABS(ABS_X), &absinfo) == 0) {
            dsc->min_x = absinfo.minimum;
            dsc->max_x = absinfo.maximum;
        }
        else {
            LV_LOG_INFO("ioctl EVIOCGABS(ABS_X) failed: %s", strerror(errno));
        }
        if(ioctl(dsc->fd, EVIOCGABS(ABS_Y), &absinfo) == 0) {
            dsc->min_y = absinfo.minimum;
            dsc->max_y = absinfo.maximum;
        }
        else {
            LV_LOG_INFO("ioctl EVIOCGABS(ABS_Y) failed: %s", strerror(errno));
        }
    }

    lv_indev_t * indev = lv_indev_create();
    if(indev == NULL) goto err_after_malloc;
    lv_indev_set_type(indev, indev_type);
    lv_indev_set_read_cb(indev, _evdev_read);
    lv_indev_set_driver_data(indev, dsc);
    lv_indev_add_event_cb(indev, _evdev_indev_delete_cb, LV_EVENT_DELETE, NULL);

    return indev;

err_after_malloc:
    lv_free(dsc);
err_malloc:
    close(fd);
    return NULL;
}

lv_indev_t * lv_evdev_create(lv_indev_type_t indev_type, const char * dev_path)
{
    int fd = open(dev_path, O_RDONLY | O_NOCTTY | O_CLOEXEC);
    if(fd < 0) {
        LV_LOG_WARN("open failed: %s", strerror(errno));
        return NULL;
    }

    return lv_evdev_create_fd(indev_type, fd);
}

lv_result_t lv_evdev_discovery_start(lv_evdev_discovery_cb_t cb, void * user_data)
{
#ifndef BSD
    lv_evdev_discovery_t * ed = NULL;
    int inotify_fd = -1;
    lv_timer_t * timer = NULL;

    ed = lv_malloc_zeroed(sizeof(lv_evdev_discovery_t));
    LV_ASSERT_MALLOC(ed);
    if(ed == NULL) return LV_RESULT_INVALID;
    evdev_discovery = ed;

    ed->cb = cb;
    ed->cb_user_data = user_data;

    inotify_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if(inotify_fd == -1) {
        LV_LOG_ERROR("inotify_init1 failed: %s", strerror(errno));
        goto err_out;
    }
    ed->inotify_fd = inotify_fd;

    ed->inotify_watch_active = _evdev_discovery_inotify_try_init_watcher(inotify_fd);
    if(evdev_discovery == NULL) return LV_RESULT_OK; /* was stopped by the callback. cleanup was already done */

    timer = lv_timer_create(_evdev_discovery_timer_cb, LV_DEF_REFR_PERIOD, NULL);
    if(timer == NULL) goto err_out;
    ed->timer = timer;

    return LV_RESULT_OK;

err_out:
    if(timer != NULL) lv_timer_delete(timer);
    if(inotify_fd != -1) close(inotify_fd);
    lv_free(ed);
    evdev_discovery = NULL;
    return LV_RESULT_INVALID;

#else /*BSD*/
    return LV_RESULT_INVALID;
#endif
}

lv_result_t lv_evdev_discovery_stop(void)
{
#ifndef BSD
    lv_evdev_discovery_t * ed = evdev_discovery;
    if(ed == NULL) return LV_RESULT_INVALID;

    if(ed->timer) lv_timer_delete(ed->timer);
    close(ed->inotify_fd);
    lv_free(ed);

    evdev_discovery = NULL;
    return LV_RESULT_OK;
#else
    return LV_RESULT_INVALID;
#endif
}

void lv_evdev_set_swap_axes(lv_indev_t * indev, bool swap_axes)
{
    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);
    dsc->swap_axes = swap_axes;
}

void lv_evdev_set_calibration(lv_indev_t * indev, int min_x, int min_y, int max_x, int max_y)
{
    lv_evdev_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);
    dsc->min_x = min_x;
    dsc->min_y = min_y;
    dsc->max_x = max_x;
    dsc->max_y = max_y;
}

void lv_evdev_delete(lv_indev_t * indev)
{
    lv_indev_delete(indev);
}

void lv_evdev_deinit(void)
{
    lv_evdev_discovery_stop();
}

#endif /*LV_USE_EVDEV*/
