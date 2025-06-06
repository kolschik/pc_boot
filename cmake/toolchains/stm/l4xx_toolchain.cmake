set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

add_definitions(-DSTM32)
add_definitions(-DUSE_HAL_DRIVER)

set(MCU_SERIES "stm32l4")
if (PLATFORM_TYPE STREQUAL "HARD")
	set(ARCH "cortex-m4")
elseif (PLATFORM_TYPE STREQUAL "SOFT")
	set(ARCH "posix")
endif ()

SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)

if (PLATFORM_TYPE STREQUAL "HARD")
	set(TOOLCHAIN_BIN_PATH "${TOOLCHAIN_BIN_DIR}arm-none-eabi")
	SET(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_PATH}-gcc)
	SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_PATH}-g++)
	set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_PATH}-g++)
	set(SIZE ${TOOLCHAIN_BIN_PATH}-size)
	set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_PATH}-objcopy)
    set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_PATH}-objdump)
else ()
	SET(CMAKE_C_COMPILER gcc)
	SET(CMAKE_CXX_COMPILER g++)
	set(SIZE "size")
endif ()

if (MAP_CREATION STREQUAL "ON")
	set(LINK_MAP_CREATION_FLAG "-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
endif ()

# выбираем LD скрипты для конкретного типа билда
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR
		CMAKE_BUILD_TYPE STREQUAL "Debug-noloader" OR
		CMAKE_BUILD_TYPE STREQUAL "Release-noloader")
	set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/mem_${MCU}.ld")
	set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/debug/sections.ld")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR CMAKE_BUILD_TYPE STREQUAL "Debug-loader")
	set(MEM_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/mem_${MCU}.ld")
	set(SECTIONS_LD "${CMAKE_CURRENT_SOURCE_DIR}/submodules/mcu_hardlib/stm/${MCU_SERIES}/ld/release/sections.ld")
endif()

set(LINK_ALL_MODE_FLAGS "-Wall -Wextra -fmessage-length=0 -Xlinker -gc-sections  --specs=nosys.specs --specs=nano.specs \
						 -Wl,-u,vfprintf -lm -u _printf_float -u _scanf_float -T ${MEM_LD} -T ${SECTIONS_LD} -Wl,--print-memory-usage ${LINK_MAP_CREATION_FLAG}")
set(C_FLAGS_ALL_MODE "-std=gnu99 -fno-exceptions -Werror -Wno-type-limits -ffunction-sections -fdata-sections")
set(CPP_FLAGS_ALL_MODE "-std=c++14 -Werror -Wno-type-limits -fno-exceptions -ffunction-sections -fdata-sections")
set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug-noloader")
	set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -O0 -g3 ${CPP_FLAGS_ALL_MODE}")
	set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -O0 -g3 ${C_FLAGS_ALL_MODE}")
	set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} -O0 -g3 ${LINK_ALL_MODE_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR
		CMAKE_BUILD_TYPE STREQUAL "Release-loader" OR
		CMAKE_BUILD_TYPE STREQUAL "Release-noloader" OR
		CMAKE_BUILD_TYPE STREQUAL "Debug-loader")
	set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -Os -g0 ${CPP_FLAGS_ALL_MODE}")
	set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -Os -g0 ${C_FLAGS_ALL_MODE}")
	set(CMAKE_EXE_LINKER_FLAGS_INIT "${COMMON_FLAGS} -Os -g0 ${LINK_ALL_MODE_FLAGS}")
endif()
