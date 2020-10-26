if(ESP_PLATFORM)

file(GLOB_RECURSE SOURCES src/*.c)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS . src)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_SKIP")
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")

else()
message(FATAL_ERROR "Unknown platform.")
endif()
