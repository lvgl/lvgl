/**
 * @file lv_fs_uefi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_UEFI && LV_USE_UEFI

#include "../../drivers/uefi/lv_uefi_private.h"

#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#if LV_FS_UEFI_LETTER == '\0'
    #error "LV_FS_UEFI_LETTER must be set to a valid value"
#else
    #if (LV_FS_UEFI_LETTER < 'A') || (LV_FS_UEFI_LETTER > 'Z')
        #if LV_FS_DEFAULT_DRIVE_LETTER != '\0' /*When using default drive letter, strict format (X:) is mandatory*/
            #error "LV_FS_UEFI_LETTER must be an upper case ASCII letter"
        #else /*Lean rules for backward compatibility*/
            #warning LV_FS_UEFI_LETTER should be an upper case ASCII letter. \
            Using a slash symbol as drive letter should be replaced with LV_FS_DEFAULT_DRIVE_LETTER mechanism
        #endif
    #endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_uefi_fs_file_context_t {
    EFI_FILE_PROTOCOL * interface;
} lv_uefi_fs_file_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_fs_drv_uefi_init(lv_fs_drv_t * drv, char fs_drive_letter, EFI_HANDLE fs_handle);
static void lv_fs_drv_uefi_deinit(lv_fs_drv_t * drv);

static void lv_fs_uefi_lvgl_path_to_uefi_path(CHAR16 * path);
static void lv_fs_uefi_uefi_path_to_lvgl_path(CHAR16 * path);
static bool lv_fs_uefi_is_dot_path(CONST CHAR16 * path);
static bool lv_fs_uefi_is_dir(EFI_FILE_PROTOCOL * dir);
static bool lv_fs_uefi_is_file(EFI_FILE_PROTOCOL * file);
static EFI_FILE_INFO * lv_fs_uefi_get_info(EFI_FILE_PROTOCOL * file);

static bool lv_fs_uefi_ready_cb(lv_fs_drv_t * drv);

static void * lv_fs_uefi_open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t lv_fs_uefi_close_cb(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t lv_fs_uefi_read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t lv_fs_uefi_write_cb(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t lv_fs_uefi_seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t lv_fs_uefi_tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * lv_fs_uefi_dir_open_cb(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t lv_fs_uefi_dir_read_cb(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t lv_fs_uefi_dir_close_cb(lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_simple_file_system = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_loaded_image = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_file_info = EFI_FILE_INFO_ID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register a driver for the File system interface
 */
void lv_fs_uefi_init(void)
{
    EFI_LOADED_IMAGE_PROTOCOL * interface_loaded_image = NULL;
    EFI_HANDLE fs_handle = NULL;

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    interface_loaded_image = lv_uefi_protocol_open(gLvEfiImageHandle, &_uefi_guid_loaded_image);
    LV_ASSERT_NULL(interface_loaded_image);

    fs_handle = interface_loaded_image->DeviceHandle;

    if(fs_handle == NULL) return;

    lv_uefi_protocol_close(gLvEfiImageHandle, &_uefi_guid_loaded_image);

    /*Add a simple driver to open images*/
    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->uefi_fs_drv);
    lv_fs_drv_uefi_init(fs_drv_p, LV_FS_UEFI_LETTER, fs_handle);

    lv_fs_drv_register(fs_drv_p);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool lv_fs_uefi_ready_cb(lv_fs_drv_t * drv)
{
    EFI_HANDLE fs_handle = (EFI_HANDLE)drv->user_data;

    return fs_handle != NULL;
}

static void * lv_fs_uefi_open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    EFI_STATUS status;
    EFI_FILE_PROTOCOL * fs_root = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * fs_interface = NULL;
    CHAR16 path_ucs2[LV_FS_MAX_PATH_LENGTH + 1];
    lv_uefi_fs_file_context_t * file_ctx = NULL;
    UINT64 uefi_mode = 0;

    EFI_HANDLE fs_handle = (EFI_HANDLE)drv->user_data;

    fs_interface = lv_uefi_protocol_open(fs_handle, &_uefi_guid_simple_file_system);
    if(fs_interface == NULL) {
        LV_LOG_WARN("[lv_uefi] Unable to open file system protocol.");
        goto error;
    }

    status = fs_interface->OpenVolume(fs_interface, &fs_root);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open file system root.");
        goto error;
    }

    if(lv_uefi_ascii_to_ucs2(path, path_ucs2, LV_FS_MAX_PATH_LENGTH + 1) == 0) {
        LV_LOG_WARN("[lv_uefi] Unable to convert the ASCII path into an UCS-2 path.");
        goto error;
    }

    lv_fs_uefi_lvgl_path_to_uefi_path(path_ucs2);

    file_ctx = lv_calloc(1, sizeof(lv_uefi_fs_file_context_t));
    LV_ASSERT_MALLOC(file_ctx);

    if(mode == LV_FS_MODE_WR) {
        uefi_mode = EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
    }
    else {
        uefi_mode = EFI_FILE_MODE_READ;
    }

    status = fs_root->Open(
                 fs_root,
                 &file_ctx->interface,
                 path_ucs2,
                 uefi_mode,
                 0);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open file '%s'.", path);
        goto error;
    }

    if(!lv_fs_uefi_is_file(file_ctx->interface)) {
        goto error;
    }

    goto finish;

error:
    if(file_ctx != NULL) {
        if(file_ctx->interface != NULL) file_ctx->interface->Close(file_ctx->interface);
        lv_free(file_ctx);
        file_ctx = NULL;
    }

finish:
    if(fs_interface != NULL) lv_uefi_protocol_close(fs_handle, &_uefi_guid_simple_file_system);
    if(fs_root != NULL) fs_root->Close(fs_root);

    return file_ctx;
}

static lv_fs_res_t lv_fs_uefi_close_cb(lv_fs_drv_t * drv, void * file_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    status = file_ctx->interface->Close(file_ctx->interface);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    lv_free(file_ctx);

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_uefi_read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINTN buf_size = btr;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    status = file_ctx->interface->Read(
                     file_ctx->interface,
                     &buf_size,
                     buf);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    *br = (uint32_t) buf_size;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_uefi_write_cb(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINTN buf_size = btw;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    status = file_ctx->interface->Write(
                     file_ctx->interface,
                     &buf_size,
                     (VOID *)buf);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    file_ctx->interface->Flush(file_ctx->interface);

    *bw = (uint32_t) buf_size;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_uefi_seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINT64 new_pos;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    if(whence == LV_FS_SEEK_END) {
        status = file_ctx->interface->SetPosition(
                         file_ctx->interface,
                         UINT64_MAX);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        status = file_ctx->interface->GetPosition(
                         file_ctx->interface,
                         &new_pos);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        if(new_pos < pos) {
            new_pos = 0;
        }
        else {
            new_pos -= pos;
        }
    }
    else if(whence == LV_FS_SEEK_SET) {
        new_pos = pos;
    }
    else if(whence == LV_FS_SEEK_CUR) {
        status = file_ctx->interface->GetPosition(
                         file_ctx->interface,
                         &new_pos);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        new_pos += pos;
    }
    else {
        return LV_FS_RES_INV_PARAM;
    }

    status = file_ctx->interface->SetPosition(
                     file_ctx->interface,
                     new_pos);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_fs_uefi_tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINT64 pos;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    status = file_ctx->interface->GetPosition(
                     file_ctx->interface,
                     &pos);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    if(pos > UINT32_MAX) return LV_FS_RES_UNKNOWN;

    *pos_p = (uint32_t) pos;

    return LV_FS_RES_OK;
}

static void * lv_fs_uefi_dir_open_cb(lv_fs_drv_t * drv, const char * path)
{
    EFI_STATUS status;
    EFI_FILE_PROTOCOL * fs_root = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * fs_interface = NULL;
    CHAR16 path_ucs2[LV_FS_MAX_PATH_LENGTH + 1];
    lv_uefi_fs_file_context_t * file_ctx = NULL;
    UINT64 mode = 0;
    UINT64 attributes = 0;

    EFI_HANDLE fs_handle = (EFI_HANDLE)drv->user_data;

    fs_interface = lv_uefi_protocol_open(fs_handle, &_uefi_guid_simple_file_system);
    if(fs_interface == NULL) {
        LV_LOG_WARN("[lv_uefi] Unable to open file system protocol.");
        goto error;
    }

    status = fs_interface->OpenVolume(fs_interface, &fs_root);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open file system root.");
        goto error;
    }

    if(path != NULL && path[0] != '\0') {
        if(lv_uefi_ascii_to_ucs2(path, path_ucs2, LV_FS_MAX_PATH_LENGTH + 1) == 0) {
            LV_LOG_WARN("[lv_uefi] Unable to convert the ASCII path into an UCS-2 path.");
            goto error;
        }
    }
    else {
        path_ucs2[0] = '\\';
        path_ucs2[1] = '\0';
    }

    lv_fs_uefi_lvgl_path_to_uefi_path(path_ucs2);

    file_ctx = lv_calloc(1, sizeof(lv_uefi_fs_file_context_t));
    LV_ASSERT_MALLOC(file_ctx);

    mode = EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
    attributes = EFI_FILE_DIRECTORY;

    status = fs_root->Open(
                 fs_root,
                 &file_ctx->interface,
                 path_ucs2,
                 mode,
                 attributes);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open directory '%s'.", path);
        goto error;
    }

    if(!lv_fs_uefi_is_dir(file_ctx->interface)) {
        goto error;
    }

    goto finish;

error:
    if(file_ctx != NULL) {
        if(file_ctx->interface != NULL) {
            file_ctx->interface->Close(file_ctx->interface);
        }
        lv_free(file_ctx);
        file_ctx = NULL;
    }

finish:
    if(fs_interface != NULL) lv_uefi_protocol_close(fs_handle, &_uefi_guid_simple_file_system);
    if(fs_root != NULL) fs_root->Close(fs_root);

    return file_ctx;
}

static lv_fs_res_t lv_fs_uefi_dir_read_cb(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len)
{
    lv_fs_res_t return_code;
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)rddir_p;

    EFI_FILE_INFO * info = NULL;
    UINTN size;

    CONST CHAR16 * fn_ucs2;

    if(fn == NULL || fn_len == 0) return LV_FS_RES_INV_PARAM;
    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    // skip . and ..
    do {
        if(info != NULL) lv_free(info);
        info = NULL;
        size = 0;
        status = file_ctx->interface->Read(
                         file_ctx->interface,
                         &size,
                         info);
        if(status == EFI_SUCCESS && size == 0) {
            return_code = LV_FS_RES_OK;
            *fn = '\0';
            goto finish;
        }
        else if(status != EFI_BUFFER_TOO_SMALL) {
            return_code = LV_FS_RES_NOT_EX;
            goto error;
        }

        info = lv_calloc(1, size);
        LV_ASSERT_MALLOC(info);

        status = file_ctx->interface->Read(
                         file_ctx->interface,
                         &size,
                         info);
        if(status != EFI_SUCCESS) {
            return_code = LV_FS_RES_HW_ERR;
            goto error;
        }
    } while(lv_fs_uefi_is_dot_path(info->FileName));

    lv_fs_uefi_uefi_path_to_lvgl_path(info->FileName);

    // skip leading \ and /
    for(fn_ucs2 = info->FileName; *fn_ucs2 != L'\0'; fn_ucs2++) {
        if(*fn_ucs2 != L'\\' && *fn_ucs2 != L'/') {
            break;
        }
    }

    if((info->Attribute & EFI_FILE_DIRECTORY) != 0) {
        if(fn_len == 0) {
            return_code = LV_FS_RES_UNKNOWN;
            goto error;
        }
        fn[0] = '/';
        fn++;
        fn_len--;
    }

    if(lv_uefi_ucs2_to_ascii(fn_ucs2, fn, fn_len) == 0) {
        LV_LOG_WARN("[lv_uefi] Unable to convert the UCS-2 path into an ascii path.");
        return_code = LV_FS_RES_UNKNOWN;
        goto error;
    }

    return_code = LV_FS_RES_OK;
    goto finish;

error:

finish:
    if(info) lv_free(info);

    return return_code;
}

static lv_fs_res_t lv_fs_uefi_dir_close_cb(lv_fs_drv_t * drv, void * rddir_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)rddir_p;

    if(file_ctx == NULL || file_ctx->interface == NULL) return LV_FS_RES_INV_PARAM;

    status = file_ctx->interface->Close(file_ctx->interface);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    lv_free(file_ctx);

    return LV_FS_RES_OK;
}

static void lv_fs_drv_uefi_init(lv_fs_drv_t * drv, char fs_drive_letter, EFI_HANDLE fs_handle)
{
    LV_ASSERT_NULL(drv);
    LV_ASSERT_NULL(fs_handle);

    lv_fs_drv_init(drv);

    drv->letter = fs_drive_letter;
    drv->cache_size = 0;

    drv->ready_cb = lv_fs_uefi_ready_cb;
    drv->open_cb = lv_fs_uefi_open_cb;
    drv->close_cb = lv_fs_uefi_close_cb;
    drv->read_cb = lv_fs_uefi_read_cb;
    drv->write_cb = lv_fs_uefi_write_cb;
    drv->seek_cb = lv_fs_uefi_seek_cb;
    drv->tell_cb = lv_fs_uefi_tell_cb;

    drv->dir_open_cb = lv_fs_uefi_dir_open_cb;
    drv->dir_read_cb = lv_fs_uefi_dir_read_cb;
    drv->dir_close_cb = lv_fs_uefi_dir_close_cb;

    drv->user_data = (void *) fs_handle;
}

static void lv_fs_drv_uefi_deinit(lv_fs_drv_t * drv)
{
    LV_ASSERT_NULL(drv);
    drv->user_data = NULL;
}

static void lv_fs_uefi_lvgl_path_to_uefi_path(CHAR16 * path)
{
    if(path == NULL) return;

    for(; *path != '\0'; path++) {
        if(*path == L'/') *path = L'\\';
    }
}

static void lv_fs_uefi_uefi_path_to_lvgl_path(CHAR16 * path)
{
    if(path == NULL) return;

    for(; *path != '\0'; path++) {
        if(*path == L'\\') *path = L'/';
    }
}

static bool lv_fs_uefi_is_dot_path(CONST CHAR16 * path)
{
    if(path == NULL) return FALSE;

    if(path[0] == L'.' && path[1] == L'\0') return TRUE;
    if(path[0] == L'.' && path[1] == L'.' && path[2] == L'\0') return TRUE;

    return FALSE;
}

static bool lv_fs_uefi_is_dir(EFI_FILE_PROTOCOL * dir)
{
    UINT64 attributes;

    if(dir == NULL) return FALSE;

    EFI_FILE_INFO * info = lv_fs_uefi_get_info(dir);
    if(info == NULL) return FALSE;

    attributes = info->Attribute;
    lv_free(info);

    return (attributes & EFI_FILE_DIRECTORY) != 0;
}

static bool lv_fs_uefi_is_file(EFI_FILE_PROTOCOL * file)
{
    UINT64 attributes;

    if(file == NULL) return FALSE;

    EFI_FILE_INFO * info = lv_fs_uefi_get_info(file);
    if(info == NULL) return FALSE;

    attributes = info->Attribute;
    lv_free(info);

    return (attributes & EFI_FILE_DIRECTORY) == 0;
}

static EFI_FILE_INFO * lv_fs_uefi_get_info(EFI_FILE_PROTOCOL * file)
{
    EFI_STATUS status;
    EFI_FILE_INFO * info = NULL;
    UINTN size = 0;

    status = file->GetInfo(file, &_uefi_guid_file_info, &size, info);
    if(status != EFI_BUFFER_TOO_SMALL) return NULL;

    info = lv_calloc(1, size);
    LV_ASSERT_MALLOC(info);

    status = file->GetInfo(file, &_uefi_guid_file_info, &size, info);
    if(status != EFI_SUCCESS) {
        lv_free(info);
        return NULL;
    }

    return info;
}

#else /* LV_FS_UEFI_LETTER == 0*/

#if defined(LV_FS_UEFI_LETTER) && LV_FS_UEFI_LETTER != '\0'
    #warning "LV_FS_UEFI is not enabled but LV_FS_UEFI_LETTER is set"
#endif

#endif
