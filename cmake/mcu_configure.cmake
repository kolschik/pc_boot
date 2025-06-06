add_definitions(
        -D${HAL_MCU_SERIES_NAME}
        -DHSE_VALUE=${HSE}
        -DHSI_VALUE=${HSI}
        -D${PLATFORM_TYPE}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug-noloader" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DBUILD_WITH_DISABLED_WATCHDOG)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR
        CMAKE_BUILD_TYPE STREQUAL "Debug-loader" OR
        CMAKE_BUILD_TYPE STREQUAL "Release-loader")
    add_definitions(-DUSE_BOOTLOADER)
endif ()

set(MCU_HARDLIB_DIR "${CMAKE_SOURCE_DIR}/submodules/mcu_hardlib")
set(EFLIB_DIR "${CMAKE_SOURCE_DIR}/submodules/eflib")
set(FREERTOS_DIR "${CMAKE_SOURCE_DIR}/submodules/freertos")

include(${CMAKE_SOURCE_DIR}/cmake/Service.cmake)

#-----------------------------------------------------------------------------
# Получаем хэши коммитов и имена веток, пишем их в файлы *_ccode.h и запоминаем короткие хэши для вывода информации
GET_COMMIT_CODES(PROJ_VERSION PROJ_BRANCH EFLIB_VERSION EFLIB_BRANCH)

#-----------------------------------------------------------------------------
# Печатаем системную информацию и все такое
message(STATUS "\n\n=============================================")
message(STATUS "            - General -")
message(STATUS "Project version:        ${PROJ_VERSION}")
message(STATUS "Project branch:         ${PROJ_BRANCH}")
message(STATUS "Eflib version:          ${EFLIB_VERSION}")
message(STATUS "Eflib branch:           ${EFLIB_BRANCH}")
message(STATUS "CMake version:          ${CMAKE_VERSION}")
message(STATUS "System:                 ${CMAKE_SYSTEM}")
message(STATUS "Processor:              ${ARCH}")
message(STATUS "Build type:             ${CMAKE_BUILD_TYPE}")
message(STATUS "             - Build -")
message(STATUS "Compiler version:       ${CMAKE_C_COMPILER_ID} ${C_COMPILER_VERSION}")
message(STATUS "C compiler:             ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler:           ${CMAKE_CXX_COMPILER}")
message(STATUS "Linker:                 ${CMAKE_LINKER}")
message(STATUS "Archiver:               ${CMAKE_AR}")
message(STATUS "            - Options -")
message(STATUS "Verbose:                ${CMAKE_VERBOSE_MAKEFILE}")
message(STATUS "Disassembly elf:        ${DISASEMMBLY_ELF}")
message(STATUS "\n=============================================\n\n")

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER_CASE)

file(GLOB_RECURSE USER_PROJ_SRC
        "${CMAKE_SOURCE_DIR}/user_code/*.cpp"
        "${CMAKE_SOURCE_DIR}/bsp/*.cpp"
)


list(APPEND ${PROJECT_NAME}_SRC ${USER_PROJ_SRC})

HEADER_DIRECTORIES("${CMAKE_SOURCE_DIR}/user_code" USERCODE_INC_DIRS)
HEADER_DIRECTORIES("${CMAKE_SOURCE_DIR}/bsp" BSP_INC_DIRS)

include_directories(
        ${CMAKE_SOURCE_DIR}/cfg
        ${BSP_INC_DIRS}
        ${USERCODE_INC_DIRS}
        ${CMAKE_BINARY_DIR}/commit_codes
)

if (PLATFORM_TYPE STREQUAL "HARD")
    SET(OUT_FILE "${PROJECT_NAME}.elf")
elseif (PLATFORM_TYPE STREQUAL "SOFT")
    SET(OUT_FILE "${PROJECT_NAME}")
endif ()

add_executable(${OUT_FILE} ${${PROJECT_NAME}_SRC})

if (PLATFORM_TYPE STREQUAL "HARD")
    if (MODULE_MCU_HARDLIB STREQUAL "ON")
        add_subdirectory(${MCU_HARDLIB_DIR} ${CMAKE_BINARY_DIR}/libs/mcu_hardlib)
        target_link_libraries(${OUT_FILE} PRIVATE MCU_HARDLIB)
    endif ()

    target_include_directories(${OUT_FILE} PRIVATE ${MCU_HARDLIB_INC_DIR})
elseif (PLATFORM_TYPE STREQUAL "SOFT")
    find_package(Threads)
    target_link_libraries(${OUT_FILE} ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (MODULE_FREERTOS STREQUAL "ON")
    add_subdirectory(${FREERTOS_DIR} ${CMAKE_BINARY_DIR}/libs/freertos)
    target_link_libraries(${OUT_FILE} PRIVATE FREERTOS)
    target_include_directories(${OUT_FILE} PRIVATE "${FREERTOS_INC_DIR}")
endif ()

if (MODULE_EFLIB STREQUAL "ON")
    include(${CMAKE_SOURCE_DIR}/submodules/eflib/add_eflib_def.cmake)
    add_subdirectory(${EFLIB_DIR} ${CMAKE_BINARY_DIR}/libs/eflib)
    target_link_libraries(${OUT_FILE} PRIVATE EFLIB)
    target_include_directories(${OUT_FILE} PRIVATE "${EFLIB_INC_DIR}")
endif ()

string(TIMESTAMP TODAY "%d%m%y")
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TARGET_NAME)
string(TOLOWER ${DEVICE} DEVICE_TYPE)
set(OUTPUT_FILENAME "${PROJECT_NAME}_${PROJ_VERSION}_${BUILD_TARGET_NAME}_${TODAY}")

set(BIN_FILE ${PROJECT_BINARY_DIR}/${OUTPUT_FILENAME}.bin)
set(ASM_FILE ${PROJECT_BINARY_DIR}/${OUTPUT_FILENAME}.asm)

if (PLATFORM_TYPE STREQUAL "HARD")
    add_custom_command(TARGET ${OUT_FILE} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${OUT_FILE}> ${BIN_FILE}
            COMMENT "Building ${BIN_FILE}")

    if (DISASEMMBLY_ELF STREQUAL "ON")
        add_custom_command(TARGET ${OUT_FILE} POST_BUILD
                COMMENT "Disassembly elf"
                COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${OUT_FILE}> >> ${ASM_FILE})
    endif ()
endif ()

if (PLATFORM_TYPE STREQUAL "HARD")
    add_custom_command(TARGET ${OUT_FILE} POST_BUILD
            COMMENT "Invoking: cross ARM GNU print size"
            COMMAND ${SIZE} ${OUT_FILE})
elseif (PLATFORM_TYPE STREQUAL "SOFT")
    add_custom_command(TARGET ${OUT_FILE} POST_BUILD
            COMMENT "Invoking: cross GNU print size"
            COMMAND ${SIZE} ${OUT_FILE})
endif ()

add_custom_target(build_num_update
        COMMAND ${CMAKE_COMMAND} -P "cmake/build_num_gen.cmake"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Updating build number")
add_dependencies(${OUT_FILE} build_num_update)

add_custom_target(timestamp
        COMMAND ${CMAKE_COMMAND} -P "cmake/timestamp.cmake"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Updating build date and time")
add_dependencies(${OUT_FILE} timestamp)
