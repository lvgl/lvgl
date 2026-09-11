set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR xtensa)

find_program(CMAKE_C_COMPILER xtensa-esp32-elf-gcc REQUIRED)
find_program(CMAKE_ASM_COMPILER xtensa-esp32-elf-gcc REQUIRED)

set(CMAKE_C_FLAGS_INIT "-mlongcalls")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostartfiles")

# Nothing pulls in the ESP-IDF startup code here, so the ROM provided libc functions have to
# be supplied by the test itself, and libgcc/libc have to be named explicitly.
set(LV_MEM_TEST_NEEDS_ROM_STUBS ON)
set(LV_MEM_TEST_EXTRA_LIBS gcc c)

# try_compile cannot link a full executable without the ROM stubs, so check the compiler by
# building a static library instead.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
