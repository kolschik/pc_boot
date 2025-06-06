set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

set(MCU_SERIES "stm32g4")

if (PLATFORM_TYPE STREQUAL "HARD")
    set(ARCH "cortex-m4")
elseif (PLATFORM_TYPE STREQUAL "SOFT")
    set(ARCH "posix")
endif ()

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

if (WIN32)
    set(WIN_EXE_SUFFIX ".exe")
endif()

# Legacy (delete later)
set(PLATFORM_TYPE "HARD")

if (PLATFORM_TYPE STREQUAL "HARD")
    if (TOOLCHAIN_BIN_PATH STREQUAL "")
        # use cross-compiler from PATH
        set(TOOLCHAIN_BIN_PATH "arm-none-eabi")
    else ()
        # use specified cross-compiler
        set(TOOLCHAIN_BIN_PATH "${TOOLCHAIN_BIN_DIR}arm-none-eabi")
    endif ()
    set(TOOLCHAIN_BIN_PATH "${TOOLCHAIN_BIN_DIR}arm-none-eabi")
    set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_PATH}-gcc${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_PATH}-g++${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_PATH}-g++${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-g++)
    set(SIZE ${TOOLCHAIN_BIN_PATH}-size${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-size)
    set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_PATH}-objcopy${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-objcopy)
    set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_PATH}-objdump${WIN_EXE_SUFFIX} CACHE PATH arm-none-eabi-objdump)
else ()
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(SIZE "size")
endif ()

if (PLATFORM_TYPE STREQUAL "HARD")
    if (MODULE_BOOTLOADER STREQUAL "ON")
        set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/bootloader/mem_${MCU}.ld")
        set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/bootloader/sections.ld")
    else ()
        if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug-noloader" OR
                CMAKE_BUILD_TYPE STREQUAL "Release-noloader")
            set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/mem_${MCU}.ld")
            set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/sections.ld")
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR CMAKE_BUILD_TYPE STREQUAL "Debug-loader")
            set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/mem_${MCU}.ld")
            set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/sections.ld")
        endif ()
    endif ()
endif ()

if (MAP_CREATION STREQUAL "ON")
    set(LINK_MAP_CREATION_FLAG "-Wl,-Map=${PROJECT_NAME}.map")
endif ()

if (MODULE_BOOTLOADER STREQUAL "ON")
    set(F_PRINTF_USE "")
else ()
    set(F_PRINTF_USE "-Wl,-u,vfprintf -lm -u _printf_float -u _scanf_float")
endif ()

if (PLATFORM_TYPE STREQUAL "HARD")
    set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections")
    set(C_FLAGS_ALL_MODE "-std=gnu99 -fno-exceptions -Wno-type-limits")
    set(CPP_FLAGS_ALL_MODE "-std=c++14 -Wno-type-limits -fno-exceptions -fno-rtti")
    set(LINK_ALL_MODE_FLAGS "-Wall -Wextra -fmessage-length=0 -Xlinker -gc-sections  \
                              --specs=nosys.specs --specs=nano.specs \
						      -T ${MEM_LD} -T ${SECTIONS_LD} \
						      -Wl,--print-memory-usage \
                              ${LINK_MAP_CREATION_FLAG}")
elseif (PLATFORM_TYPE STREQUAL "SOFT")
    set(C_FLAGS_ALL_MODE "-m32 -std=gnu99 -fno-exceptions -Wno-type-limits -ffunction-sections -fdata-sections")
    set(CPP_FLAGS_ALL_MODE "-m32 -std=c++14 -Wno-type-limits -fno-exceptions -ffunction-sections -fdata-sections")
    set(LINK_ALL_MODE_FLAGS "-m32 -Wall -Wextra -fmessage-length=0 -Xlinker -gc-sections  \
                              -nostartfiles -nodefaultlibs \
						      -T ${MEM_LD} -T ${SECTIONS_LD} \
						      -Wl,--print-memory-usage \
                              ${LINK_MAP_CREATION_FLAG}")

    option(TRACE_FUNCTIONS "Trace function calls using instrument-functions")
    if (TRACE_FUNCTIONS)
        add_definitions(-DTRACE_FUNCTIONS)
        set(GCC_COVERAGE_COMPILE_FLAGS "-finstrument-functions")
        target_compile_options(FreeRTOS_Emulator PUBLIC ${GCC_COVERAGE_COMPILE_FLAGS})
    endif (TRACE_FUNCTIONS)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug-noloader")
    set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -O1 -g3 ${CPP_FLAGS_ALL_MODE}")
    set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -O1 -g3 ${C_FLAGS_ALL_MODE}")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} -O1 -g3 ${LINK_ALL_MODE_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR
        CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR
        CMAKE_BUILD_TYPE STREQUAL "Debug-loader" OR
        CMAKE_BUILD_TYPE STREQUAL "Release-noloader")
    set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -O2 -g0 ${CPP_FLAGS_ALL_MODE}")
    set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -O2 -g0 ${C_FLAGS_ALL_MODE}")
    set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} -O2 -g0 ${LINK_ALL_MODE_FLAGS}")
endif ()
