set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

add_definitions(-DSTM32)
add_definitions(-DUSE_HAL_DRIVER)

set(MCU_SERIES "stm32f3")


set(ARCH "cortex-m4")


set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)


set(TOOLCHAIN_BIN_PATH "${TOOLCHAIN_BIN_DIR}arm-none-eabi")
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_PATH}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_PATH}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_PATH}-g++)
set(SIZE ${TOOLCHAIN_BIN_PATH}-size)
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_PATH}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_PATH}-objdump)


if (MODULE_BOOTLOADER STREQUAL "ON")
    set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/bootloader/mem_${MCU}.ld")
    set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/bootloader/sections.ld")
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
            CMAKE_BUILD_TYPE STREQUAL "Debug-noloader" OR
            CMAKE_BUILD_TYPE STREQUAL "Release-noloader")
        set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/mem_${MCU}.ld")
        set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/sections.ld")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR
            CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR
            CMAKE_BUILD_TYPE STREQUAL "Debug-loader")
        set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/mem_${MCU}.ld")
        set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/sections.ld")
    endif ()
endif ()


if (MAP_CREATION STREQUAL "ON")
    set(LINK_MAP_CREATION_FLAG "-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
endif ()

if (MODULE_BOOTLOADER STREQUAL "ON")
    set(F_PRINTF_USE "")
else ()
    set(F_PRINTF_USE "-Wl,-u,vfprintf -lm -u _printf_float -u _scanf_float")
endif ()


set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(C_FLAGS_ALL_MODE "-std=gnu99 -fno-exceptions -Wno-type-limits -ffunction-sections -fdata-sections")
set(CPP_FLAGS_ALL_MODE "-std=c++14 -Wno-type-limits -fno-exceptions -ffunction-sections -fdata-sections")
set(LINK_ALL_MODE_FLAGS "-Wall -Wextra -fmessage-length=0 -Xlinker -gc-sections  \
                         --specs=nosys.specs --specs=nano.specs \
                          ${F_PRINTF_USE} \
                          -T ${MEM_LD} -T ${SECTIONS_LD} \
                          -Wl,--print-memory-usage \
                          ${LINK_MAP_CREATION_FLAG}")

set(OPT_DEBUG_LEVEL "")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug-noloader")
    set(OPT_DEBUG_LEVEL "-O0 -g3")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR
        CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR
        CMAKE_BUILD_TYPE STREQUAL "Release-noloader")
    set(OPT_DEBUG_LEVEL "-O2 -g0")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug-loader")
    set(OPT_DEBUG_LEVEL "-O0 -g3")
endif ()

set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} ${OPT_DEBUG_LEVEL} ${CPP_FLAGS_ALL_MODE}")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} ${OPT_DEBUG_LEVEL} ${C_FLAGS_ALL_MODE}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} ${OPT_DEBUG_LEVEL} ${LINK_ALL_MODE_FLAGS}")