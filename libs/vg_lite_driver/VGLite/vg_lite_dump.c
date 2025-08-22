/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/
#if 0 /*VGLite dump API is not supported yet */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "vg_lite_context.h"
#ifdef _WIN32
    #include <windows.h>
#elif defined(__ZEPHYR__)
    #include <zephyr.h>
    #include <fs/fs.h>
#else
    #include <stdbool.h>
    #include <pthread.h>
    #include <unistd.h>
    #include "png.h"
#endif

#if DUMP_CAPTURE || DUMP_LAST_CAPTURE

static int DumpFlag  = 0;
static void * dump_mutex = NULL;
#if defined(__ZEPHYR__)
typedef struct _vglitesDUMP_FILE_INFO
{
    void *    _debugFile;
    uint32_t   _threadID;
}vglitesDUMP_FILE_INFO;
#else
typedef struct _vglitesDUMP_FILE_INFO
{
    FILE *    _debugFile;
    uint32_t   _threadID;
}vglitesDUMP_FILE_INFO;
#endif

typedef struct _vglitesBUFFERED_OUTPUT * vglitesBUFFERED_OUTPUT_PTR;
typedef struct _vglitesBUFFERED_OUTPUT
{
    int                         indent;
    vglitesBUFFERED_OUTPUT_PTR  prev;
    vglitesBUFFERED_OUTPUT_PTR  next;
}
vglitesBUFFERED_OUTPUT;

static vglitesBUFFERED_OUTPUT     _outputBuffer[1];
static vglitesBUFFERED_OUTPUT_PTR _outputBufferHead = NULL;
static vglitesBUFFERED_OUTPUT_PTR _outputBufferTail = NULL;

#ifdef __linux__
static pthread_mutex_t _printMutex    = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _dumpFileMutex = PTHREAD_MUTEX_INITIALIZER;
#elif defined(__ZEPHYR__)
static K_MUTEX_DEFINE(_printMutex);
static K_MUTEX_DEFINE(_dumpFileMutex);
#endif

/* Alignment with a power of two value. */
#define vglitemALIGN(n, align) \
( \
    ((n) + ((align) - 1)) & ~((align) - 1) \
)
#if defined(__ZEPHYR__)
static void *_SetDumpFile(
    void *File, int CloseOldFile
    );
#else
static FILE *_SetDumpFile(
    FILE *File, int CloseOldFile
    );
#endif

vg_lite_error_t vg_lite_CreateMutex(void **Mutex)
{
#ifdef _WIN32
    void *handle;

    assert(Mutex!=NULL);
    handle = CreateMutex(NULL, 0, NULL);
    
    if (handle == NULL)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    *Mutex = handle;
#elif defined(__ZEPHYR__)
    struct k_mutex *mutex = NULL;
    /* Validate the arguments. */
    assert(Mutex != NULL);

    mutex = vg_lite_os_malloc(sizeof(*mutex));
    if (mutex)
    {
        k_mutex_init(mutex);
        *Mutex = (void *) mutex;
    }
    else
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }
#else
    pthread_mutex_t* mutex = NULL;
    pthread_mutexattr_t   mta;
    /* Validate the arguments. */
    assert(Mutex != NULL);

    /* Allocate memory for the mutex. */
    mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutexattr_init(&mta);

    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

    /* Initialize the mutex. */
    pthread_mutex_init(mutex, &mta);

    /* Destroy mta.*/
    pthread_mutexattr_destroy(&mta);

    /* Return mutex to caller. */
    *Mutex = (void *) mutex;
#endif
    return VG_LITE_SUCCESS;
}

vg_lite_error_t
vg_lite_AcquireMutex(void *Mutex, unsigned int Timeout)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
#ifdef _WIN32
    unsigned long milliSeconds;

    /* Validate the arguments. */
    assert(Mutex != NULL);

    milliSeconds = (Timeout == ((unsigned int) ~0U))
        ? INFINITE
        : Timeout;

    if (WaitForSingleObject((HANDLE) Mutex,
                            milliSeconds) == WAIT_TIMEOUT)
    {
        /* Timeout. */
        return VG_LITE_TIMEOUT;
    }
#elif defined(__ZEPHYR__)
    k_timeout_t z_timeout;

    /* Validate the arguments. */
    assert(Mutex != NULL);

    z_timeout = (Timeout == ((unsigned int) ~0U)) ? K_FOREVER : K_MSEC(Timeout);

    if (k_mutex_lock(Mutex, z_timeout))
    {
        /* Timeout. */
        return VG_LITE_TIMEOUT;
    }
#else
    pthread_mutex_t *mutex;
    /* Validate the arguments. */
    assert(Mutex != NULL);

    /* Cast the pointer. */
    mutex = (pthread_mutex_t *) Mutex;

    /* Test for infinite. */
    if (Timeout == ((uint32_t) ~0U))
    {
        /* Lock the mutex. */
        if (pthread_mutex_lock(mutex))
        {
            /* Some error. */
            status = VG_LITE_GENERIC_IO;
        }
        else
        {
            /* Success. */
            status = VG_LITE_SUCCESS;
        }
    }
    else
    {
        /* Try locking the mutex. */
        if (pthread_mutex_trylock(mutex))
        {
            /* Assume timeout. */
            status = VG_LITE_TIMEOUT;

            /* Loop while not timeout. */
            while (Timeout-- > 0)
            {
                /* Try locking the mutex. */
                if (pthread_mutex_trylock(mutex) == 0)
                {
                    /* Success. */
                    status = VG_LITE_SUCCESS;
                    break;
                }

                /* Sleep 1 millisecond. */
                usleep(1000);
            }
        }
        else
        {
            /* Success. */
            status = VG_LITE_SUCCESS;
        }
    }
#endif
    return status;
}

vg_lite_error_t
vg_lite_ReleaseMutex(void *Mutex)
{
#ifdef _WIN32
        /* Validate the arguments. */
    assert(Mutex != NULL);

    /* Release the fast mutex. */
    if (!ReleaseMutex((void *) Mutex))
    {
        return VG_LITE_NOT_SUPPORT;
    }
#elif defined(__ZEPHYR__)
    /* Validate the arguments. */
    assert(Mutex != NULL);

    k_mutex_unlock(Mutex);
#else
    pthread_mutex_t *mutex;

    /* Validate the arguments. */
    assert(Mutex != NULL);

    /* Cast the pointer. */
    mutex = (pthread_mutex_t *) Mutex;

    /* Release the mutex. */
    pthread_mutex_unlock(mutex);
#endif
    return VG_LITE_SUCCESS;
}

#define vglitemLOCKDUMP() \
{ \
    if (dump_mutex == NULL) \
    {    \
        assert(vg_lite_CreateMutex(&dump_mutex) == VG_LITE_SUCCESS); \
    } \
    assert(vg_lite_AcquireMutex(dump_mutex, ((unsigned int) ~0U)) == VG_LITE_SUCCESS); \
}

#define vglitemUNLOCKDUMP() \
    assert(vg_lite_ReleaseMutex(dump_mutex) == VG_LITE_SUCCESS);

#define STACK_THREAD_NUMBER 1

static uint32_t                         _usedFileSlot = 0;
static uint32_t                         _currentPos = 0;
static vglitesDUMP_FILE_INFO            _FileArray[STACK_THREAD_NUMBER];

#define vglitemOPT_VALUE(ptr)           (((ptr) == NULL) ? 0 : *(ptr))

vg_lite_error_t vg_lite_PrintStrVSafe(
    char * String,
    size_t StringSize,
    unsigned int * Offset,
    const char * Format,
    va_list Arguments
    )
{
    unsigned int offset = vglitemOPT_VALUE(Offset);
    vg_lite_error_t status = VG_LITE_SUCCESS;

    /* Verify the arguments. */
    assert(String != NULL);
    assert(StringSize > 0);
    assert(Format != NULL);

    if (offset < StringSize - 1)
    {
        /* Print into the string. */
        int n = vsnprintf(String + offset,
                             StringSize - offset,
                             Format,
                             Arguments);

        if (n < 0 || n >= (int)(StringSize - offset))
        {
            status = VG_LITE_GENERIC_IO;
        }
        else if (Offset)
        {
            *Offset = offset + n;
        }
    }
    else
    {
        status = VG_LITE_OUT_OF_RESOURCES;
    }

    /* Success. */
    return status;
}

vg_lite_error_t vg_lite_PrintStrSafe(
    char * String,
    size_t StringSize,
    unsigned int * Offset,
    const char* Format,
    ...
    )
{
    va_list arguments;
    vg_lite_error_t status = VG_LITE_SUCCESS;

    /* Verify the arguments. */
    assert(String != NULL);
    assert(StringSize > 0);
    assert(Format != NULL);


    /* Route through vgliteoOS_PrintStrVSafe. */
    va_start(arguments, Format);
    status = vg_lite_PrintStrVSafe(String, StringSize,
                                   Offset,
                                   Format, arguments);

    va_end(arguments);

    return status;
}
#if defined(__ZEPHYR__)
void
vg_lite_SetDebugFile(
    const char* FileName
    )
{
    void *debugFile;

    if (FileName != NULL)
    {
        /* Don't change it to 'w' !!!*/
        debugFile = vg_lite_os_fopen(FileName, "a");
        if (debugFile)
        {
            _SetDumpFile(debugFile, 1);
        }
    }
}
#else
void
vg_lite_SetDebugFile(
    const char* FileName
    )
{
    FILE *debugFile;

    if (FileName != NULL)
    {
        /* Don't change it to 'w' !!!*/
        debugFile = fopen(FileName, "a");
        if (debugFile)
        {
            _SetDumpFile(debugFile, 1);
        }
    }
}
#endif

/******************************************************************************\
****************************** OS-dependent Macros *****************************
\******************************************************************************/
#ifdef __linux__
#   define vglitemGETTHREADID() \
    (uint32_t) pthread_self()

#   define vglitemGETPROCESSID() \
    getpid()

#elif defined (__ZEPHYR__)
#   define vglitemGETTHREADID() \
    (uint32_t) k_current_get()

#   define vglitemGETPROCESSID() \
    (uint32_t) k_current_get()

#else
#   define vglitemLOCKSECTION() \
    static HANDLE __lockHandle__; \
    \
    if (__lockHandle__ == NULL) \
    { \
        __lockHandle__ = CreateMutex(NULL, 0, NULL); \
    } \
    \
    WaitForSingleObject(__lockHandle__, INFINITE)

#   define vglitemUNLOCKSECTION() \
    ReleaseMutex(__lockHandle__)

#   define vglitemGETPROCESSID() \
    GetCurrentProcessId()

#   define vglitemGETTHREADID() \
    GetCurrentThreadId()
#endif

#if defined(__linux__) || defined (__ZEPHYR__)
#ifdef __STRICT_ANSI__  /* ANSI C does not have snprintf, vsnprintf functions */
#   define vglitemSPRINTF(Destination, Size, Message, Value) \
        sprintf(Destination, Message, Value)

#   define vglitemVSPRINTF(Destination, Size, Message, Arguments) \
        vsprintf(Destination, Message, Arguments)
#else
#   define vglitemSPRINTF(Destination, Size, Message, Value) \
        snprintf(Destination, Size, Message, Value)

#   define vglitemVSPRINTF(Destination, Size, Message, Arguments) \
        vsnprintf(Destination, Size, Message, Arguments)
#endif

#   define vglitemSTRCAT(Destination, Size, String) \
    strncat(Destination, String, Size)
#else
#   define vglitemSPRINTF(Destination, Size, Message, Value) \
    sprintf_s(Destination, Size, Message, Value)

#   define vglitemVSPRINTF(Destination, Size, Message, Arguments) \
    vsnprintf_s(Destination, Size, Size, Message, Arguments)

#   define vglitemSTRCAT(Destination, Size, String) \
    strcat_s(Destination, Size, String)

#endif

vg_lite_error_t vgliteoDUMP_SetDumpFlag(int DumpState)
{
    DumpFlag = DumpState;

    return VG_LITE_SUCCESS;
}

#if defined(__ZEPHYR__)
static void *_SetDumpFile(void *File, int CloseOldFile)
{
    void *oldFile = NULL;
    uint32_t selfThreadID = vglitemGETTHREADID();
    uint32_t pos;
    uint32_t tmpCurPos;
    k_mutex_lock(&_dumpFileMutex, K_FOREVER);
    tmpCurPos = _currentPos;

    /* Find if this thread has already been recorded */
    for (pos = 0; pos < _usedFileSlot; pos++)
    {
        if (selfThreadID == _FileArray[pos]._threadID)
        {
            if (_FileArray[pos]._debugFile != NULL &&
                _FileArray[pos]._debugFile != File &&
                CloseOldFile)
            {
                /* Close the earliest existing file handle. */
                vg_lite_os_fclose(_FileArray[pos]._debugFile);
                _FileArray[pos]._debugFile =  NULL;
            }

            oldFile = _FileArray[pos]._debugFile;
            /* Replace old file by new file */
            _FileArray[pos]._debugFile = File;
            goto exit;
        }
    }

    /* Test if we have exhausted our thread buffers. One thread one buffer. */
    if (tmpCurPos == STACK_THREAD_NUMBER)
    {
        goto error;
    }

    /* Record this new thread */
    _FileArray[tmpCurPos]._debugFile = File;
    _FileArray[tmpCurPos]._threadID = selfThreadID;
    _currentPos = ++tmpCurPos;

    if (_usedFileSlot < STACK_THREAD_NUMBER)
    {
        _usedFileSlot++;
    }

exit:
    k_mutex_unlock(&_dumpFileMutex);
    return oldFile;

error:
    k_mutex_unlock(&_dumpFileMutex);
    printf("ERROR: Not enough dump file buffers. Buffer num = %d", STACK_THREAD_NUMBER);

    return oldFile;
}

#else

static FILE *_SetDumpFile(FILE *File, int CloseOldFile)
{
    FILE *oldFile = NULL;
    uint32_t selfThreadID = vglitemGETTHREADID();
    uint32_t pos;
    uint32_t tmpCurPos;

#ifdef _WIN32
    vglitemLOCKSECTION();
#else 
    pthread_mutex_lock(&_dumpFileMutex);
#endif
    tmpCurPos = _currentPos;

    /* Find if this thread has already been recorded */
    for (pos = 0; pos < _usedFileSlot; pos++)
    {
        if (selfThreadID == _FileArray[pos]._threadID)
        {
            if (_FileArray[pos]._debugFile != NULL &&
                _FileArray[pos]._debugFile != File &&
                CloseOldFile)
            {
                /* Close the earliest existing file handle. */
                fclose(_FileArray[pos]._debugFile);
                _FileArray[pos]._debugFile =  NULL;
            }

            oldFile = _FileArray[pos]._debugFile;
            /* Replace old file by new file */
            _FileArray[pos]._debugFile = File;
            goto exit;
        }
    }

    /* Test if we have exhausted our thread buffers. One thread one buffer. */
    if (tmpCurPos == STACK_THREAD_NUMBER)
    {
        goto error;
    }

    /* Record this new thread */
    _FileArray[tmpCurPos]._debugFile = File;
    _FileArray[tmpCurPos]._threadID = selfThreadID;
    _currentPos = ++tmpCurPos;

    if (_usedFileSlot < STACK_THREAD_NUMBER)
    {
        _usedFileSlot++;
    }

#ifdef __linux__
exit:
    pthread_mutex_unlock(&_dumpFileMutex);
    return oldFile;

error:
    pthread_mutex_unlock(&_dumpFileMutex);
    printf("ERROR: Not enough dump file buffers. Buffer num = %d", STACK_THREAD_NUMBER);
#else
exit:
    vglitemUNLOCKSECTION();
    return oldFile;

error:
    vglitemUNLOCKSECTION();
    printf("ERROR: Not enough dump file buffers. Buffer num = %d", STACK_THREAD_NUMBER);
#endif
    return oldFile;
}
#endif

#if defined(__ZEPHYR__)
void * _GetDumpFile()
{
    uint32_t selfThreadID;
    uint32_t pos = 0;
    void* retFile = NULL;

    k_mutex_lock(&_dumpFileMutex, K_FOREVER);

    if (_usedFileSlot == 0)
    {
        goto exit;
    }

    selfThreadID = vglitemGETTHREADID();
    for (; pos < _usedFileSlot; pos++)
    {
        if (selfThreadID == _FileArray[pos]._threadID)
        {
            retFile = _FileArray[pos]._debugFile;
            goto exit;
        }
    }

exit:
    k_mutex_unlock(&_dumpFileMutex);

    return retFile;
}

#else

FILE * _GetDumpFile()
{
    uint32_t selfThreadID;
    uint32_t pos = 0;
    FILE* retFile = NULL;

#ifdef __linux__
    pthread_mutex_lock(&_dumpFileMutex);
#else
    vglitemLOCKSECTION();
#endif

    if (_usedFileSlot == 0)
    {
        goto exit;
    }

    selfThreadID = vglitemGETTHREADID();
    for (; pos < _usedFileSlot; pos++)
    {
        if (selfThreadID == _FileArray[pos]._threadID)
        {
            retFile = _FileArray[pos]._debugFile;
            goto exit;
        }
    }

exit:
#ifdef __linux__
    pthread_mutex_unlock(&_dumpFileMutex); 
#else
    vglitemUNLOCKSECTION();
#endif
    return retFile;
}
#endif

#ifdef __linux__
#define vglitemOUTPUT_STRING(File, String) \
    fprintf(((File == NULL) ? stderr : File), "%s", String);
#elif defined (__ZEPHYR__)
#define vglitemOUTPUT_STRING(File, String) \
    { \
        printk("%s", String); \
    }
#else
#define vglitemOUTPUT_STRING(File, String) \
    if (File != NULL) { \
        fprintf(File, "%s", String); \
    } else { \
        OutputDebugString(String); \
    }
#endif

static void OutputString(FILE *File, const char *String)
{
    if (String != NULL) {
        vglitemOUTPUT_STRING(File, String);
    }
}

static void _Print(FILE *File, const char *Message, va_list Arguments)
{
    /* Output to file or debugger. */

    int i, j, n, indent;
    static char buffer[4096];
    vglitesBUFFERED_OUTPUT_PTR outputBuffer = NULL;
#ifdef _WIN32
    static uint32_t prevThreadID;
    uint32_t threadID;

    vglitemLOCKSECTION();
    /* Get the current thread ID. */
    threadID = vglitemGETTHREADID();
#elif defined(__ZEPHYR__)
    k_mutex_lock(&_printMutex, K_FOREVER);
#else
    pthread_mutex_lock(&_printMutex);
#endif

    /* Initialize output buffer list. */
    if (_outputBufferHead == NULL)
    {
        for (i = 0; i < 1; i += 1)
        {
            if (_outputBufferTail == NULL)
            {
                _outputBufferHead = &_outputBuffer[i];
            }
            else
            {
                _outputBufferTail->next = &_outputBuffer[i];
            }

            _outputBuffer[i].prev = _outputBufferTail;
            _outputBuffer[i].next =  NULL;

            _outputBufferTail = &_outputBuffer[i];
        }
    }

    outputBuffer = _outputBufferHead;

#ifdef _Win32
    /* Update the previous thread value. */
    prevThreadID = threadID;
#endif

    if (strcmp(Message, "$$FLUSH$$") == 0)
    {
        OutputString(File, NULL);
#ifdef _WIN32
    vglitemUNLOCKSECTION();
#elif defined(__ZEPHYR__)
    k_mutex_unlock(&_printMutex);
#else
     pthread_mutex_unlock(&_printMutex);
#endif
        return;
    }

    i = 0;

    if (strncmp(Message, "--", 2) == 0)
    {
        if (outputBuffer->indent == 0)
        {
            OutputString(File,"ERROR: indent=0\n");
        }

        outputBuffer->indent -= 2;
    }

    indent = outputBuffer->indent % 40;

    for (j = 0; j < indent; ++j)
    {
        buffer[i++] = ' ';
    }

    if (indent != outputBuffer->indent)
    {
        i += vglitemSPRINTF(
            buffer + i, sizeof(buffer) - i, " <%d> ", outputBuffer->indent
            );
        buffer[sizeof(buffer) - 1] = '\0';
    }

    /* Print message to buffer. */
    n = vglitemVSPRINTF(buffer + i, sizeof(buffer) - i, Message, Arguments);
    buffer[sizeof(buffer) - 1] = '\0';

    if ((n <= 0) || (buffer[i + n - 1] != '\n'))
    {
        /* Append new-line. */
        vglitemSTRCAT(buffer, sizeof(buffer) - strlen(buffer) - 1, "\n");
        buffer[sizeof(buffer) - 1] = '\0';
    }

    /* Output to debugger. */
    OutputString(File, buffer);

    if (strncmp(Message, "++", 2) == 0)
    {
        outputBuffer->indent += 2;
    }
#ifdef __linux__
    pthread_mutex_unlock(&_printMutex);
#elif defined(__ZEPHYR__)
    k_mutex_unlock(&_printMutex);
#else
    vglitemUNLOCKSECTION();
#endif
}

#define vglitemDEBUGPRINT(FileHandle, Message) \
{ \
    va_list arguments; \
    va_start(arguments, Message); \
    _Print(FileHandle, Message, arguments); \
    va_end(arguments); \
}

void vgliteoOS_Print(const char * Message, ...)
{
    vglitemDEBUGPRINT(_GetDumpFile(), Message);
}

void _SetDumpFileInfo()
{
#define DUMP_FILE_PREFIX   "hal"

    char dump_file[128];
    unsigned int offset = 0;

    /* Customize filename as needed. */
    vg_lite_PrintStrSafe(dump_file,
        sizeof(dump_file),
        &offset,
        "%s%s_dump_pid-%d_tid-%d_%s.log",
        vgliteDUMP_PATH,
        DUMP_FILE_PREFIX,
#ifdef _WIN32
        (void *)(uintptr_t)(GetCurrentProcessId()),
        (void *)(uintptr_t)GetCurrentThreadId(),
#elif defined(__ZEPHYR__)
        (void *)(uintptr_t)k_current_get(),
        (void *)(uintptr_t)k_current_get(),
#else
        (void *)(uintptr_t)getpid(),
        (void *)pthread_self(),
#endif
        vgliteDUMP_KEY);

    vg_lite_SetDebugFile(dump_file);
    vgliteoDUMP_SetDumpFlag(1);
}

vg_lite_error_t vglitefDump(char * Message, ...)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
    unsigned offset = 0;
    va_list args;
    char buffer[180];

    if (!DumpFlag)
    {
        return status;
    }

    va_start(args, Message);
    status = vg_lite_PrintStrVSafe(buffer, sizeof(buffer),
        &offset,
        Message, args);
    assert(status == VG_LITE_SUCCESS);
    va_end(args);

    vgliteoOS_Print("%s", buffer);;

    return status;
}

vg_lite_error_t vglitefDumpBuffer(char *Tag, size_t Physical, void * Logical, size_t Offset, size_t Bytes)
{
    unsigned int * ptr = (unsigned int *) Logical + (Offset >> 2);
    size_t bytes   = vglitemALIGN(Bytes + (Offset & 3), 4);

    if (!DumpFlag)
    {
        return VG_LITE_SUCCESS;
    }

    vglitemLOCKDUMP();

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("@[%s 0x%08X 0x%08X", Tag, Physical + (Offset & ~3), bytes);
#endif

    while (bytes >= 16)
    {
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP("  0x%08X 0x%08X 0x%08X 0x%08X",
                ptr[0], ptr[1], ptr[2], ptr[3]);
#else
        vglitemDUMP("  0x%08X", ptr[0]);
        vglitemDUMP("  0x%08X", ptr[1]);
        if (bytes == 16 && (ptr[2] == 0) && (ptr[3] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else {
            vglitemDUMP("  0x%08X", ptr[2]);
            vglitemDUMP("  0x%08X", ptr[3]);
        }
#endif

        ptr   += 4;
        bytes -= 16;
    }

    switch (bytes)
    {
    case 12:
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP("  0x%08X 0x%08X 0x%08X", ptr[0], ptr[1], ptr[2]);
#else
        vglitemDUMP("  0x%08X", ptr[0]);
        if ((ptr[1] == 0) && (ptr[2] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else
        {
            vglitemDUMP("  0x%08X", ptr[1]);
            vglitemDUMP("  0x%08X", ptr[2]);
        }
#endif
        break;

    case 8:
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP("  0x%08X 0x%08X", ptr[0], ptr[1]);
#else
        if ((ptr[0] == 0) && (ptr[1] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else
        {
            vglitemDUMP("  0x%08X", ptr[0]);
            vglitemDUMP("  0x%08X", ptr[1]);
        }
#endif
        break;

    case 4:
        vglitemDUMP("  0x%08X", ptr[0]);
        break;
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP("] -- %s", Tag);
#else
    vglitemDUMP("---This command end----");
#endif

    vglitemUNLOCKDUMP();

    return VG_LITE_SUCCESS;
}

#endif /* DUMP_CAPTURE */

#if DUMP_LAST_CAPTURE
vg_lite_error_t vglitefDumpBuffer_single(char* Tag, size_t Physical, void* Logical, size_t Offset, size_t Bytes)
{
    unsigned int* ptr = (unsigned int*)Logical + (Offset >> 2);
    size_t bytes = vglitemALIGN(Bytes + (Offset & 3), 4);

    if (!DumpFlag)
    {
        return VG_LITE_SUCCESS;
    }

    vglitemLOCKDUMP();

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP_single("@[%s 0x%08X 0x%08X", Tag, Physical + (Offset & ~3), bytes);
#endif

    while (bytes >= 16)
    {
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP_single("  0x%08X 0x%08X 0x%08X 0x%08X",
            ptr[0], ptr[1], ptr[2], ptr[3]);
#else
        vglitemDUMP("  0x%08X", ptr[0]);
        vglitemDUMP("  0x%08X", ptr[1]);
        if (bytes == 16 && (ptr[2] == 0) && (ptr[3] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else {
            vglitemDUMP("  0x%08X", ptr[2]);
            vglitemDUMP("  0x%08X", ptr[3]);
        }
#endif

        ptr += 4;
        bytes -= 16;
    }

    switch (bytes)
    {
    case 12:
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP_single("  0x%08X 0x%08X 0x%08X", ptr[0], ptr[1], ptr[2]);
#else
        vglitemDUMP("  0x%08X", ptr[0]);
        if ((ptr[1] == 0) && (ptr[2] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else
        {
            vglitemDUMP("  0x%08X", ptr[1]);
            vglitemDUMP("  0x%08X", ptr[2]);
        }
#endif
        break;

    case 8:
#if !DUMP_COMMAND_CAPTURE
        vglitemDUMP_single("  0x%08X 0x%08X", ptr[0], ptr[1]);
#else
        if ((ptr[0] == 0) && (ptr[1] == 0))
        {
            printf("This two commands is 0x00000000\n");
        }
        else
        {
            vglitemDUMP("  0x%08X", ptr[0]);
            vglitemDUMP("  0x%08X", ptr[1]);
        }
#endif
        break;

    case 4:
        vglitemDUMP_single("  0x%08X", ptr[0]);
        break;
    }

#if !DUMP_COMMAND_CAPTURE
    vglitemDUMP_single("] -- %s", Tag);
#else
    vglitemDUMP("---This command end----");
#endif

    vglitemUNLOCKDUMP();

    return VG_LITE_SUCCESS;
}
#endif

vg_lite_error_t vg_lite_dump_png(const char *name, vg_lite_buffer_t *buffer)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
#ifdef __linux__
    uint8_t *memory, *p, *q;
    int x, y;
    png_image image;
    uint16_t color;

    if (buffer->format == VG_LITE_L8) {
        /* Construct the PNG image structure. */
        png_image image;
        memset(&image, 0, (sizeof image));

        image.version = PNG_IMAGE_VERSION;
        image.width   = buffer->width;
        image.height  = buffer->height;
        image.format  = PNG_FORMAT_GRAY;

        /* Write the PNG image. */
        return png_image_write_to_file(&image, name, 0, buffer->memory, buffer->stride, NULL);
    }

    /* Allocate RGB memory buffer. */
    memory = malloc(buffer->width * buffer->height * 3);
    if (memory == NULL) {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    for (y = 0; y < buffer->height; y++) {
        p = (uint8_t*) buffer->memory + y * buffer->stride;
        q = memory + y * buffer->width * 3;
        for (x = 0; x < buffer->width; x++, q += 3) {
            switch (buffer->format) {
                case VG_LITE_RGBA5658_PLANAR:
                case VG_LITE_ARGB8565_PLANAR:
                case VG_LITE_RGB565:
                    color = *(uint16_t *)p;
                    p += 2;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

                case VG_LITE_ABGR8565_PLANAR:
                case VG_LITE_BGRA5658_PLANAR:
                case VG_LITE_BGR565:
                case OPENVG_sRGB_565:
                case OPENVG_sRGB_565_PRE:
                case OPENVG_lRGB_565:
                case OPENVG_lRGB_565_PRE:
                    color = *(uint16_t *)p;
                    p += 2;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ABGR8565:
                    p += 1;
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 2;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_BGRA5658:
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 3;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ARGB8565:
                    p += 1;
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 2;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

                case VG_LITE_RGBA5658:
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 3;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

                case VG_LITE_RGB888:
                    q[0] = p[0];
                    q[1] = p[1];
                    q[2] = p[2];
                    p += 3;
                    break;

                case VG_LITE_BGR888:
                    q[0] = p[2];
                    q[1] = p[1];
                    q[2] = p[0];
                    p += 3;
                    break;

                case VG_LITE_RGBA8888:
                case VG_LITE_RGBX8888:
                case OPENVG_sABGR_8888:
                case OPENVG_sXBGR_8888:
                    q[0] = p[0];
                    q[1] = p[1];
                    q[2] = p[2];
                    p += 4;
                    break;

                case VG_LITE_ARGB8888:
                case VG_LITE_XRGB8888:
                case OPENVG_sBGRA_8888:
                case OPENVG_sBGRX_8888:
                    q[0] = p[1];
                    q[1] = p[2];
                    q[2] = p[3];
                    p += 4;
                    break;

                case VG_LITE_BGRA8888:
                case VG_LITE_BGRX8888:
                case OPENVG_sARGB_8888:
                case OPENVG_sXRGB_8888:
                    q[0] = p[2];
                    q[1] = p[1];
                    q[2] = p[0];
                    p += 4;
                    break;

                case VG_LITE_ABGR8888:
                case VG_LITE_XBGR8888:
                case OPENVG_sRGBA_8888:
                case OPENVG_sRGBX_8888:
                    q[0] = p[3];
                    q[1] = p[2];
                    q[2] = p[1];
                    p += 4;
                    break;

                case VG_LITE_RGBA4444:
                case OPENVG_sABGR_4444:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[0] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[2] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_BGRA4444:
                case OPENVG_sARGB_4444:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[2] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[0] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_ABGR4444:
                case OPENVG_sRGBA_4444:
                    color = *(uint16_t*)p;
                    color = (color >> 4);
                    p += 2;
                    q[2] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[0] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_ARGB4444:
                case OPENVG_sBGRA_4444:
                    color = *(uint16_t*)p;
                    color = (color >> 4);
                    p += 2;
                    q[0] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[2] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_BGRA5551:
                case OPENVG_sARGB_1555:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[0] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ABGR1555:
                case OPENVG_sRGBA_5551:
                    color = *(uint16_t*)p;
                    color = (color >> 1);
                    p += 2;
                    q[0] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_RGBA5551:
                case OPENVG_sABGR_1555:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[2] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ARGB1555:
                case OPENVG_sBGRA_5551:
                    color = *(uint16_t*)p;
                    color = (color >> 1);
                    p += 2;
                    q[2] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_RGBA2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[0] = (color & 0x03) << 2 | (color & 0x03) << 4 | (color & 0x03) << 6 |(color & 0x03);
                    q[1] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[2] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    break;
                    
                case VG_LITE_BGRA2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[2] = (color & 0x03) << 2 | (color & 0x03) << 4 | (color & 0x03) << 6 |(color & 0x03);
                    q[1] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[0] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    break;

                case VG_LITE_ARGB2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[0] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[1] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    q[2] = (color & 0xC0) << 2 | (color & 0xC0) << 4 | (color & 0xC0) << 6 |(color & 0xC0);
                    break;

                case VG_LITE_ABGR2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[2] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[1] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    q[0] = (color & 0xC0) << 2 | (color & 0xC0) << 4 | (color & 0xC0) << 6 |(color & 0xC0);
                    break;

                case VG_LITE_A8:
                case VG_LITE_L8:
                    q[0] = q[1] = q[2] = p[0];
                    p++;
                    break;

                case VG_LITE_YUYV:
                    /* YUYV not supported yet. */

                default:
                    break;
            }
        }
    }

    /* Construct the PNG image structure. */
    memset(&image, 0, (sizeof image));

    image.version = PNG_IMAGE_VERSION;
    image.width   = buffer->width;
    image.height  = buffer->height;
    image.format  = PNG_FORMAT_RGB;

    /* Write the PNG image. */
    status = png_image_write_to_file(&image, name, 0, memory, buffer->width * 3, NULL);

    /* Free the RGB memory buffer.*/
    free(memory);

#endif /* __linux__ */

    /* Success. */
    return status;
}
#endif
