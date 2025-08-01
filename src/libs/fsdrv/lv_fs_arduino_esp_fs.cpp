#include "../../../lvgl.h"
#include "lv_fs_arduino_esp_fs.h"

#if LV_USE_FS_ARDUINO_ESP_FS

#define CHECK_FILE_P() \
    if (!file_p) return LV_FS_RES_NOT_EX;

struct FileWrapper final {
        FileWrapper(const File & file) : file(file) {}
        ~FileWrapper()
        {
            file.close();
        }
        static File & get(void * file_p)
        {
            return static_cast<FileWrapper *>(file_p)->file;
        }
    private:
        File file;
};

struct lv_fs_drv_data_t {
        lv_fs_drv_data_t(FS * fs) :
            m_fs(fs), m_sem(xSemaphoreCreateBinary())
        {
            xSemaphoreGive(m_sem);
        }
        File open(const char * path, const char * mode)
        {
            return m_fs->open(path, mode);
        }
        QueueHandle_t sem() const
        {
            return m_sem;
        }
    private:
        FS * m_fs;
        QueueHandle_t m_sem;
};

struct lock_t final {
        lock_t(const lv_fs_drv_t * drv) : m_sem(static_cast<lv_fs_drv_data_t *>(drv->user_data)->sem())
        {
            while(xSemaphoreTake(m_sem, portMAX_DELAY) == pdFALSE) ;
        }
        ~lock_t()
        {
            xSemaphoreGive(m_sem);
        }
    private:
        QueueHandle_t m_sem;
};

template<typename R>
static lv_fs_res_t check_pos(const R pos)
{
    return static_cast<int32_t>(pos) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

/**
 * Register a driver for the Arduino File System interface
 */
void lv_fs_arduino_esp_fs_init(esp_fs_init_t * esp_fs_init)
{
    auto fs_drv = esp_fs_init->drv;
    lv_fs_drv_init(fs_drv);

    fs_drv->letter = esp_fs_init->letter;
    fs_drv->open_cb = fs_open;
    fs_drv->close_cb = fs_close;
    fs_drv->read_cb = fs_read;
    fs_drv->write_cb = fs_write;
    fs_drv->seek_cb = fs_seek;
    fs_drv->tell_cb = fs_tell;

    fs_drv->user_data = new lv_fs_drv_data_t(esp_fs_init->init());

    fs_drv->dir_close_cb = NULL;
    fs_drv->dir_open_cb = NULL;
    fs_drv->dir_read_cb = NULL;

    lv_fs_drv_register(fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    lock_t lock(drv);

    const char * flags;
    if(mode == LV_FS_MODE_WR)
        flags = FILE_WRITE;
    else if(mode == LV_FS_MODE_RD)
        flags = FILE_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = FILE_WRITE;

    char buf[LV_FS_MAX_PATH_LEN];
    lv_snprintf(buf, sizeof(buf), LV_FS_ARDUINO_ESP_FFAT_PATH "%s", path);

    File file = static_cast<lv_fs_drv_data_t *>(drv->user_data)->open(buf, flags);
    if(!file) {
        return nullptr;
    }

    return new FileWrapper(file);
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    lock_t lock(drv);
    delete static_cast<FileWrapper *>(file_p);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lock_t lock(drv);
    CHECK_FILE_P();
    *br = FileWrapper::get(file_p).read((uint8_t *)buf, btr);
    return check_pos(*br);
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    lock_t lock(drv);
    CHECK_FILE_P();
    *bw = FileWrapper::get(file_p).write((uint8_t *)buf, btw);
    return check_pos(*bw);
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    lock_t lock(drv);
    CHECK_FILE_P();

    SeekMode mode;
    if(whence == LV_FS_SEEK_SET)
        mode = SeekSet;
    else if(whence == LV_FS_SEEK_CUR)
        mode = SeekCur;
    else if(whence == LV_FS_SEEK_END)
        mode = SeekEnd;

    return check_pos(FileWrapper::get(file_p).seek(pos, mode));
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_p variable
 * @param pos_p     pointer to store the result
 * @return          LV_FS_RES_OK: no error or any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    lock_t lock(drv);
    CHECK_FILE_P();
    *pos_p = FileWrapper::get(file_p).position();
    return check_pos(*pos_p);
}

#undef CHECK_FILE_P

#endif // LV_USE_FS_ARDUINO_ESP_FS
