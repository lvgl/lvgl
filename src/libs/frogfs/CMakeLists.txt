cmake_minimum_required(VERSION 3.16)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/files.cmake)

if(DEFINED ESP_PLATFORM)
    if(IDF_TARGET STREQUAL "esp8266")
        idf_component_register(
        SRCS
            ${libfrogfs_SRC}
        INCLUDE_DIRS
            ${libfrogfs_INC}
        PRIV_REQUIRES
            vfs
        REQUIRES
            spi_flash
        )
    else()
        idf_component_register(
        SRCS
            ${libfrogfs_SRC}
        INCLUDE_DIRS
            ${libfrogfs_INC}
        PRIV_REQUIRES
            esp_partition
            vfs
        REQUIRES
            spi_flash
        )
    endif()
endif()
