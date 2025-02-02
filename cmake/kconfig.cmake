import_kconfig(CONFIG_ ${DOTCONFIG} keys)

set(CONFIG_HEADER "${CMAKE_BINARY_DIR}/config.h")

# Create the header file content
file(WRITE ${AUTOCONF_H} "/* Auto-generated config.h */\n")

foreach(key ${keys})
    string(REPLACE "CONFIG_" "" stripped_key ${key})
    set(value ${${key}})
    if("${value}" STREQUAL "y")
        set(value 1)
    elseif("${value}" STREQUAL "n")
        set(value 0)
    endif()

    file(APPEND ${AUTOCONF_H} "#define ${stripped_key} ${${value}}\n")
endforeach()
