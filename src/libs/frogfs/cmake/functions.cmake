cmake_minimum_required(VERSION 3.16)
set(frogfs_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

macro(generate_frogfs_rules)
    if(NOT ESP_PLATFORM)
        set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    cmake_parse_arguments(ARG "" "CONFIG;NAME" "TOOLS" ${ARGN})
    if(NOT DEFINED ARG_CONFIG)
        set(ARG_CONFIG frogfs.yaml)
    endif()
    if(NOT DEFINED ARG_NAME)
        set(ARG_NAME frogfs)
    endif()
    foreach(TOOL ARG_TOOLS)
        set(TOOLS ${TOOLS} --tools ${TOOL})
    endforeach()
    set(OUTPUT ${BUILD_DIR}/CMakeFiles/${ARG_NAME})

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(Python3_VENV ${BUILD_DIR}/CMakeFiles/frogfs-venv)
    if(CMAKE_HOST_WIN32)
        set(Python3_VENV_EXECUTABLE ${Python3_VENV}/Scripts/python.exe)
    else()
        set(Python3_VENV_EXECUTABLE ${Python3_VENV}/bin/python)
    endif()

    if(NOT TARGET frogfs_venv)
        add_custom_target(frogfs_venv
            COMMAND ${Python3_EXECUTABLE} -m venv ${Python3_VENV}
            COMMENT "Initializing Python virtualenv"
            BYPRODUCTS ${Python3_VENV}
        )
        file(GLOB_RECURSE venv FOLLOW_SYMLINKS LIST_DIRECTORIES true CONFIGURE_DEPENDS "${Python3_VENV}/*")
        set_property(TARGET frogfs_venv PROPERTY ADDITIONAL_CLEAN_FILES "${venv}")
    endif()

    if (NOT TARGET frogfs_requirements)
        add_custom_target(frogfs_requirements
            COMMAND ${Python3_VENV_EXECUTABLE} -m pip install -r ${frogfs_DIR}/requirements.txt
            COMMAND ${CMAKE_COMMAND} -E touch ${Python3_VENV}.stamp
            DEPENDS ${frogfs_DIR}/requirements.txt
            BYPRODUCTS ${Python3_VENV}.stamp
            COMMENT "Installing Python requirements"
        )
        add_dependencies(frogfs_requirements frogfs_venv)
    endif()

    add_custom_target(frogfs_preprocess_${ARG_NAME}
        COMMAND ${Python3_VENV_EXECUTABLE} -B ${frogfs_DIR}/tools/mkfrogfs.py -C ${CMAKE_SOURCE_DIR} ${TOOLS} ${ARG_CONFIG} ${BUILD_DIR} ${OUTPUT}.bin
        DEPENDS ${ARG_CONFIG}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        BYPRODUCTS ${BUILD_DIR}/${ARG_NAME}-cache ${BUILD_DIR}/${ARG_NAME}-cache-state.json ${OUTPUT}.bin
        COMMENT "Running mkfrogfs.py for ${ARG_NAME}.bin"
        USES_TERMINAL
    )

    file(GLOB_RECURSE cache LIST_DIRECTORIES true CONFIGURE_DEPENDS "${BUILD_DIR}/${ARG_NAME}-cache/*")
    set_property(TARGET frogfs_preprocess_${ARG_NAME} PROPERTY ADDITIONAL_CLEAN_FILES "${cache}")
    file(GLOB_RECURSE node_modules FOLLOW_SYMLINKS LIST_DIRECTORIES true CONFIGURE_DEPENDS "${BUILD_DIR}/node_modules/*")
    set_property(TARGET frogfs_preprocess_${ARG_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${node_modules};${BUILD_DIR}/node_modules")

    if (${frogfs_DIR}/requirements.txt IS_NEWER_THAN ${Python3_VENV}.stamp)
        add_dependencies(frogfs_preprocess_${ARG_NAME} frogfs_requirements)
    endif()
endmacro()

function(target_add_frogfs target)
    LIST(REMOVE_AT ARGV 0)
    generate_frogfs_rules(${ARGV})

    add_custom_command(OUTPUT ${OUTPUT}_bin.c
        COMMAND ${Python3_VENV_EXECUTABLE} -B ${frogfs_DIR}/tools/bin2c.py ${OUTPUT}.bin ${OUTPUT}_bin.c
        DEPENDS ${OUTPUT}.bin
        COMMENT "Generating frogfs source file ${ARG_NAME}_bin.c"
    )
    target_sources(${target} PRIVATE ${OUTPUT}_bin.c)
endfunction()

function(declare_frogfs_bin)
    generate_frogfs_rules(${ARGV})

    add_custom_target(generate_${ARG_NAME}_bin
        DEPENDS frogfs_preprocess_${ARG_NAME} ${OUTPUT}.bin
    )
endfunction()
