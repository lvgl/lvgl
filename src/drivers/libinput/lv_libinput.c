/**
 * @file lv_libinput.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev_private.h"
#include "lv_libinput_private.h"

#if LV_USE_LIBINPUT

#include "../../display/lv_display_private.h"

#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <libinput.h>
#include <pthread.h>
#include <string.h>

#if LV_LIBINPUT_BSD
    #include <dev/evdev/input.h>
#else
    #include <linux/input.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_libinput_device {
    lv_libinput_capability capabilities;
    char * path;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool _rescan_devices(void);
static bool _add_scanned_device(char * path, lv_libinput_capability capabilities);
static void _reset_scanned_devices(void);

static void * _poll_thread(void * data);

lv_libinput_event_t * _get_event(lv_libinput_t * state);
bool _event_pending(lv_libinput_t * state);
lv_libinput_event_t * _create_event(lv_libinput_t * state);

static void _read(lv_indev_t * indev, lv_indev_data_t * data);
static void _read_pointer(lv_libinput_t * state, struct libinput_event * event);
static void _read_keypad(lv_libinput_t * state, struct libinput_event * event);

static int _open_restricted(const char * path, int flags, void * user_data);
static void _close_restricted(int fd, void * user_data);

static void _delete(lv_libinput_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

static struct _lv_libinput_device * devices = NULL;
static size_t num_devices = 0;

static const int timeout = 100; // ms
static const nfds_t nfds = 1;

static const struct libinput_interface interface = {
    .open_restricted = _open_restricted,
    .close_restricted = _close_restricted,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_libinput_capability lv_libinput_query_capability(struct libinput_device * device)
{
    lv_libinput_capability capability = LV_LIBINPUT_CAPABILITY_NONE;
    if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_KEYBOARD)
       && (libinput_device_keyboard_has_key(device, KEY_ENTER) || libinput_device_keyboard_has_key(device, KEY_KPENTER))) {
        capability |= LV_LIBINPUT_CAPABILITY_KEYBOARD;
    }
    if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_POINTER)) {
        capability |= LV_LIBINPUT_CAPABILITY_POINTER;
    }
    if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_TOUCH)) {
        capability |= LV_LIBINPUT_CAPABILITY_TOUCH;
    }
    return capability;
}

char * lv_libinput_find_dev(lv_libinput_capability capabilities, bool force_rescan)
{
    char * path = NULL;
    lv_libinput_find_devs(capabilities, &path, 1, force_rescan);
    return path;
}

size_t lv_libinput_find_devs(lv_libinput_capability capabilities, char ** found, size_t count, bool force_rescan)
{
    if((!devices || force_rescan) && !_rescan_devices()) {
        return 0;
    }

    size_t num_found = 0;

    for(size_t i = 0; i < num_devices && num_found < count; ++i) {
        if(devices[i].capabilities & capabilities) {
            found[num_found] = devices[i].path;
            num_found++;
        }
    }

    return num_found;
}

lv_indev_t * lv_libinput_create(lv_indev_type_t indev_type, const char * dev_path)
{
    lv_libinput_t * dsc = lv_malloc_zeroed(sizeof(lv_libinput_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    dsc->libinput_context = libinput_path_create_context(&interface, NULL);
    if(!dsc->libinput_context) {
        LV_LOG_ERROR("libinput_path_create_context failed: %s", strerror(errno));
        _delete(dsc);
        return NULL;
    }

    dsc->libinput_device = libinput_path_add_device(dsc->libinput_context, dev_path);
    if(!dsc->libinput_device) {
        _delete(dsc);
        return NULL;
    }

    dsc->libinput_device = libinput_device_ref(dsc->libinput_device);
    if(!dsc->libinput_device) {
        _delete(dsc);
        return NULL;
    }

    dsc->fd = libinput_get_fd(dsc->libinput_context);

    /* Prepare poll */
    dsc->fds[0].fd = dsc->fd;
    dsc->fds[0].events = POLLIN;
    dsc->fds[0].revents = 0;

#if LV_LIBINPUT_XKB
    struct xkb_rule_names names = LV_LIBINPUT_XKB_KEY_MAP;
    lv_xkb_init(&(dsc->xkb), names);
#endif /* LV_LIBINPUT_XKB */

    /* Create indev */
    lv_indev_t * indev = lv_indev_create();
    if(!indev) {
        _delete(dsc);
        return NULL;
    }
    lv_indev_set_type(indev, indev_type);
    lv_indev_set_read_cb(indev, _read);
    lv_indev_set_driver_data(indev, dsc);

    /* Set up thread & lock */
    pthread_mutex_init(&dsc->event_lock, NULL);
    pthread_create(&dsc->worker_thread, NULL, _poll_thread, dsc);

    return indev;
}

void lv_libinput_delete(lv_indev_t * indev)
{
    _delete(lv_indev_get_driver_data(indev));
    lv_indev_delete(indev);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * rescan all attached evdev devices and store capable ones into the static devices array for quick later filtering
 * @return true if the operation succeeded
 */
static bool _rescan_devices(void)
{
    _reset_scanned_devices();

    DIR * dir;
    struct dirent * ent;
    if(!(dir = opendir("/dev/input"))) {
        perror("unable to open directory /dev/input");
        return false;
    }

    struct libinput * context = libinput_path_create_context(&interface, NULL);

    while((ent = readdir(dir))) {
        if(strncmp(ent->d_name, "event", 5) != 0) {
            continue;
        }

        /* 11 characters for /dev/input/ + length of name + 1 NUL terminator */
        char * path = malloc((11 + strlen(ent->d_name) + 1) * sizeof(char));
        if(!path) {
            perror("could not allocate memory for device node path");
            libinput_unref(context);
            _reset_scanned_devices();
            return false;
        }
        strcpy(path, "/dev/input/");
        strcat(path, ent->d_name);

        struct libinput_device * device = libinput_path_add_device(context, path);
        if(!device) {
            perror("unable to add device to libinput context");
            free(path);
            continue;
        }

        /* The device pointer is guaranteed to be valid until the next libinput_dispatch. Since we're not dispatching events
         * as part of this function, we don't have to increase its reference count to keep it alive.
         * https://wayland.freedesktop.org/libinput/doc/latest/api/group__base.html#gaa797496f0150b482a4e01376bd33a47b */

        lv_libinput_capability capabilities = lv_libinput_query_capability(device);

        libinput_path_remove_device(device);

        if(capabilities == LV_LIBINPUT_CAPABILITY_NONE) {
            free(path);
            continue;
        }

        if(!_add_scanned_device(path, capabilities)) {
            free(path);
            libinput_unref(context);
            _reset_scanned_devices();
            return false;
        }
    }

    libinput_unref(context);
    return true;
}

/**
 * add a new scanned device to the static devices array, growing its size when necessary
 * @param path device file path
 * @param capabilities device input capabilities
 * @return true if the operation succeeded
 */
static bool _add_scanned_device(char * path, lv_libinput_capability capabilities)
{
    /* Double array size every 2^n elements */
    if((num_devices & (num_devices + 1)) == 0) {
        struct _lv_libinput_device * tmp = realloc(devices, (2 * num_devices + 1) * sizeof(struct _lv_libinput_device));
        if(!tmp) {
            perror("could not reallocate memory for devices array");
            return false;
        }
        devices = tmp;
    }

    devices[num_devices].path = path;
    devices[num_devices].capabilities = capabilities;
    num_devices++;

    return true;
}

/**
 * reset the array of scanned devices and free any dynamically allocated memory
 */
static void _reset_scanned_devices(void)
{
    if(!devices) {
        return;
    }

    for(size_t i = 0; i < num_devices; ++i) {
        free(devices[i].path);
    }
    free(devices);

    devices = NULL;
    num_devices = 0;
}

static void * _poll_thread(void * data)
{
    lv_libinput_t * dsc = (lv_libinput_t *)data;
    struct libinput_event * event;
    int rc = 0;

    LV_LOG_INFO("libinput: poll worker started");

    while(true) {
        rc = poll(dsc->fds, nfds, timeout);
        switch(rc) {
            case -1:
                perror(NULL);
                __attribute__((fallthrough));
            case 0:
                if(dsc->deinit) {
                    dsc->deinit = false; /* Signal that we're done */
                    return NULL;
                }
                continue;
            default:
                break;
        }
        libinput_dispatch(dsc->libinput_context);
        pthread_mutex_lock(&dsc->event_lock);
        while((event = libinput_get_event(dsc->libinput_context)) != NULL) {
            _read_pointer(dsc, event);
            _read_keypad(dsc, event);
            libinput_event_destroy(event);
        }
        pthread_mutex_unlock(&dsc->event_lock);
        LV_LOG_INFO("libinput: event read");
    }

    return NULL;
}

lv_libinput_event_t * _get_event(lv_libinput_t * dsc)
{
    if(dsc->start == dsc->end) {
        return NULL;
    }

    lv_libinput_event_t * evt = &dsc->points[dsc->start];

    if(++dsc->start == LV_LIBINPUT_MAX_EVENTS)
        dsc->start = 0;

    return evt;
}

bool _event_pending(lv_libinput_t * dsc)
{
    return dsc->start != dsc->end;
}

lv_libinput_event_t * _create_event(lv_libinput_t * dsc)
{
    lv_libinput_event_t * evt = &dsc->points[dsc->end];

    if(++dsc->end == LV_LIBINPUT_MAX_EVENTS)
        dsc->end = 0;

    /* We have overflowed the buffer, start overwriting
     * old events.
     */
    if(dsc->end == dsc->start) {
        LV_LOG_INFO("libinput: overflowed event buffer!");
        if(++dsc->start == LV_LIBINPUT_MAX_EVENTS)
            dsc->start = 0;
    }

    memset(evt, 0, sizeof(lv_libinput_event_t));

    return evt;
}

static void _read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_libinput_t * dsc = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(dsc);

    pthread_mutex_lock(&dsc->event_lock);

    lv_libinput_event_t * evt = _get_event(dsc);

    if(!evt)
        evt = &dsc->last_event; /* indev expects us to report the most recent state */

    data->point = evt->point;
    data->state = evt->pressed;
    data->key = evt->key_val;
    data->continue_reading = _event_pending(dsc);

    dsc->last_event = *evt; /* Remember the last event for the next call */

    pthread_mutex_unlock(&dsc->event_lock);

    if(evt)
        LV_LOG_TRACE("libinput_read: (%04d, %04d): %d continue_reading? %d", data->point.x, data->point.y, data->state,
                     data->continue_reading);
}

static void _read_pointer(lv_libinput_t * dsc, struct libinput_event * event)
{
    struct libinput_event_touch * touch_event = NULL;
    struct libinput_event_pointer * pointer_event = NULL;
    lv_libinput_event_t * evt = NULL;
    enum libinput_event_type type = libinput_event_get_type(event);
    int slot = 0;

    switch(type) {
        case LIBINPUT_EVENT_TOUCH_MOTION:
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_UP:
            touch_event = libinput_event_get_touch_event(event);
            break;
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_BUTTON:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
            pointer_event = libinput_event_get_pointer_event(event);
            break;
        default:
            return; /* We don't care about this events */
    }

    /* We need to read unrotated display dimensions directly from the driver because libinput won't account
     * for any rotation inside of LVGL */
    lv_display_t * disp = lv_display_get_default();

    /* ignore more than 2 fingers as it will only confuse LVGL */
    if(touch_event && (slot = libinput_event_touch_get_slot(touch_event)) > 1)
        return;

    evt = _create_event(dsc);

    const int32_t hor_res = disp->physical_hor_res > 0 ? disp->physical_hor_res : disp->hor_res;
    const int32_t ver_res = disp->physical_ver_res > 0 ? disp->physical_ver_res : disp->ver_res;

    switch(type) {
        case LIBINPUT_EVENT_TOUCH_MOTION:
        case LIBINPUT_EVENT_TOUCH_DOWN: {
                lv_point_t point;
                point.x = (int32_t)LV_CLAMP(INT32_MIN, libinput_event_touch_get_x_transformed(touch_event, hor_res) - disp->offset_x,
                                            INT32_MAX);
                point.y = (int32_t)LV_CLAMP(INT32_MIN, libinput_event_touch_get_y_transformed(touch_event, ver_res) - disp->offset_y,
                                            INT32_MAX);
                if(point.x < 0 || point.x > disp->hor_res || point.y < 0 || point.y > disp->ver_res) {
                    break; /* ignore touches that are out of bounds */
                }
                evt->point = point;
                evt->pressed = LV_INDEV_STATE_PRESSED;
                dsc->slots[slot].point = evt->point;
                dsc->slots[slot].pressed = evt->pressed;
                break;
            }
        case LIBINPUT_EVENT_TOUCH_UP:
            /*
             * We don't support "multitouch", but libinput does. To make fast typing with two thumbs
             * on a keyboard feel good, it's necessary to handle two fingers individually. The edge
             * case here is if you press a key with one finger and then press a second key with another
             * finger. No matter which finger you release, it will count as the second finger releasing
             * and ignore the first because LVGL only stores a single (the latest) pressed state.
             *
             * To work around this, we detect the case where one finger is released while the other is
             * still pressed and insert dummy events so that both release events trigger at the correct
             * position.
             */
            if(slot == 0 && dsc->slots[1].pressed == LV_INDEV_STATE_PRESSED) {
                /* The first finger is released while the second finger is still pressed.
                 * We turn P1 > P2 > R1 > R2 into P1 > P2 > (P1) > R1 > (P2) > R2.
                 */

                /* Inject the dummy press event for the first finger */
                lv_libinput_event_t * synth_evt = evt;
                synth_evt->pressed = LV_INDEV_STATE_PRESSED;
                synth_evt->point = dsc->slots[0].point;

                /* Append the real release event for the first finger */
                evt = _create_event(dsc);
                evt->pressed = LV_INDEV_STATE_RELEASED;
                evt->point = dsc->slots[0].point;

                /* Inject the dummy press event for the second finger */
                synth_evt = _create_event(dsc);
                synth_evt->pressed = LV_INDEV_STATE_PRESSED;
                synth_evt->point = dsc->slots[1].point;
            }
            else if(slot == 1 && dsc->slots[0].pressed == LV_INDEV_STATE_PRESSED) {
                /* The second finger is released while the first finger is still pressed.
                 * We turn P1 > P2 > R2 > R1 into P1 > P2 > R2 > (P1) > R1.
                 */

                /* Append the real release event for the second finger */
                evt->pressed = LV_INDEV_STATE_RELEASED;
                evt->point = dsc->slots[1].point;

                /* Inject the dummy press event for the first finger */
                lv_libinput_event_t * synth_evt = _create_event(dsc);
                synth_evt->pressed = LV_INDEV_STATE_PRESSED;
                synth_evt->point = dsc->slots[0].point;
            }
            else {
                evt->pressed = LV_INDEV_STATE_RELEASED;
                evt->point = dsc->slots[slot].point;
            }

            dsc->slots[slot].pressed = evt->pressed;
            break;
        case LIBINPUT_EVENT_POINTER_MOTION:
            dsc->pointer_position.x = (int32_t)LV_CLAMP(0, dsc->pointer_position.x + libinput_event_pointer_get_dx(pointer_event),
                                                        disp->hor_res - 1);
            dsc->pointer_position.y = (int32_t)LV_CLAMP(0, dsc->pointer_position.y + libinput_event_pointer_get_dy(pointer_event),
                                                        disp->ver_res - 1);
            evt->point.x = dsc->pointer_position.x;
            evt->point.y = dsc->pointer_position.y;
            evt->pressed = dsc->pointer_button_down;
            break;
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE: {
                lv_point_t point;
                point.x = (int32_t)LV_CLAMP(INT32_MIN, libinput_event_pointer_get_absolute_x_transformed(pointer_event,
                                                                                                         hor_res) - disp->offset_x, INT32_MAX);
                point.y = (int32_t)LV_CLAMP(INT32_MIN, libinput_event_pointer_get_absolute_y_transformed(pointer_event,
                                                                                                         ver_res) - disp->offset_y, INT32_MAX);
                if(point.x < 0 || point.x > disp->hor_res || point.y < 0 || point.y > disp->ver_res) {
                    break; /* ignore pointer events that are out of bounds */
                }
                evt->point = point;
                evt->pressed = dsc->pointer_button_down;
                break;
            }
        case LIBINPUT_EVENT_POINTER_BUTTON: {
                enum libinput_button_state button_state = libinput_event_pointer_get_button_state(pointer_event);
                dsc->pointer_button_down = button_state == LIBINPUT_BUTTON_STATE_RELEASED ? LV_INDEV_STATE_RELEASED :
                                           LV_INDEV_STATE_PRESSED;
                evt->point.x = dsc->pointer_position.x;
                evt->point.y = dsc->pointer_position.y;
                evt->pressed = dsc->pointer_button_down;
            }
        default:
            break;
    }
}

static void _read_keypad(lv_libinput_t * dsc, struct libinput_event * event)
{
    struct libinput_event_keyboard * keyboard_event = NULL;
    enum libinput_event_type type = libinput_event_get_type(event);
    lv_libinput_event_t * evt = NULL;
    switch(type) {
        case LIBINPUT_EVENT_KEYBOARD_KEY:
            evt = _create_event(dsc);
            keyboard_event = libinput_event_get_keyboard_event(event);
            enum libinput_key_state key_state = libinput_event_keyboard_get_key_state(keyboard_event);
            uint32_t code = libinput_event_keyboard_get_key(keyboard_event);
#if LV_LIBINPUT_XKB
            evt->key_val = lv_xkb_process_key(&(dsc->xkb), code, key_state == LIBINPUT_KEY_STATE_PRESSED);
#else
            switch(code) {
                case KEY_BACKSPACE:
                    evt->key_val = LV_KEY_BACKSPACE;
                    break;
                case KEY_ENTER:
                    evt->key_val = LV_KEY_ENTER;
                    break;
                case KEY_PREVIOUS:
                    evt->key_val = LV_KEY_PREV;
                    break;
                case KEY_NEXT:
                    evt->key_val = LV_KEY_NEXT;
                    break;
                case KEY_UP:
                    evt->key_val = LV_KEY_UP;
                    break;
                case KEY_LEFT:
                    evt->key_val = LV_KEY_LEFT;
                    break;
                case KEY_RIGHT:
                    evt->key_val = LV_KEY_RIGHT;
                    break;
                case KEY_DOWN:
                    evt->key_val = LV_KEY_DOWN;
                    break;
                case KEY_TAB:
                    evt->key_val = LV_KEY_NEXT;
                    break;
                case KEY_HOME:
                    evt->key_val = LV_KEY_HOME;
                    break;
                case KEY_END:
                    evt->key_val = LV_KEY_END;
                    break;
                default:
                    evt->key_val = 0;
                    break;
            }
#endif /* LV_LIBINPUT_XKB */
            if(evt->key_val != 0) {
                /* Only record button state when actual output is produced to prevent widgets from refreshing */
                evt->pressed = (key_state == LIBINPUT_KEY_STATE_RELEASED) ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;

                // just release the key immediately after it got pressed.
                // but don't handle special keys where holding a key makes sense
                if(evt->key_val != LV_KEY_BACKSPACE &&
                   evt->key_val != LV_KEY_UP &&
                   evt->key_val != LV_KEY_LEFT &&
                   evt->key_val != LV_KEY_RIGHT &&
                   evt->key_val != LV_KEY_DOWN &&
                   key_state == LIBINPUT_KEY_STATE_PRESSED) {
                    lv_libinput_event_t * release_evt = _create_event(dsc);
                    release_evt->pressed = LV_INDEV_STATE_RELEASED;
                    release_evt->key_val = evt->key_val;
                }
            }
            break;
        default:
            break;
    }
}

static int _open_restricted(const char * path, int flags, void * user_data)
{
    LV_UNUSED(user_data);
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void _close_restricted(int fd, void * user_data)
{
    LV_UNUSED(user_data);
    close(fd);
}

static void _delete(lv_libinput_t * dsc)
{
    if(dsc->fd)
        dsc->deinit = true;

    /* Give worker thread a whole second to quit */
    for(int i = 0; i < 100; i++) {
        if(!dsc->deinit)
            break;
        usleep(10000);
    }

    if(dsc->deinit) {
        LV_LOG_ERROR("libinput worker thread did not quit in time, cancelling it");
        pthread_cancel(dsc->worker_thread);
    }

    if(dsc->libinput_device) {
        libinput_path_remove_device(dsc->libinput_device);
        libinput_device_unref(dsc->libinput_device);
    }

    if(dsc->libinput_context) {
        libinput_unref(dsc->libinput_context);
    }

#if LV_LIBINPUT_XKB
    lv_xkb_deinit(&(dsc->xkb));
#endif /* LV_LIBINPUT_XKB */

    lv_free(dsc);
}

#endif /* LV_USE_LIBINPUT */
