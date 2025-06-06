macro(HEADER_DIRECTORIES head_dir return_list)
    file(GLOB_RECURSE new_list ${head_dir}/*.h)
    set(dir_list "")
    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} PATH)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro(HEADER_DIRECTORIES)

macro(EXCLUDE_SOURCE_FILES return_list)
    foreach(var_excl ${EXCLUDE_SRC})
        file(GLOB_RECURSE EXCL_ONE_SRC "${CMAKE_SOURCE_DIR}/${var_excl}")
        list(APPEND EFLIB_EXCL_SRCS ${EXCL_ONE_SRC})
    endforeach(var_excl)
    set(${return_list} ${EFLIB_EXCL_SRCS})
endmacro(EXCLUDE_SOURCE_FILES)

macro(EXCLUDE_DIRECTORIES return_list)
    foreach(var_excl ${EXCLUDE_DIR})
        file(GLOB_RECURSE EXCL_ONE_DIR "${CMAKE_SOURCE_DIR}/submodules/eflib/${var_excl}/*.cpp"
                    "${CMAKE_SOURCE_DIR}/submodules/eflib/${var_excl}/*.c")
        list(APPEND EFLIB_EXCL_DIRS ${EXCL_ONE_DIR})
    endforeach(var_excl)
    set(${return_list} ${EFLIB_EXCL_DIRS})
endmacro(EXCLUDE_DIRECTORIES)

macro(GET_COMMIT_CODES proj_hash_short proj_branch eflib_hash_short eflib_branch)
    find_package(Git)
    if(GIT_FOUND)
        foreach(MAKE_CCODE_DIRS ${CMAKE_SOURCE_DIR} ${EFLIB_DIR} ${FREERTOS_DIR} ${MCU_HARDLIB_DIR})
            # получаем полный хэш коммита
            execute_process(
                    COMMAND git "log" "-1" "--pretty=oneline"
                    OUTPUT_VARIABLE GIT_COMMIT_HASH_FULL
                    WORKING_DIRECTORY ${MAKE_CCODE_DIRS}
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            # получаем короткий хэш коммита
            execute_process(COMMAND git "log" "-1" "--pretty=format:%h"
                    OUTPUT_VARIABLE GIT_COMMIT_HASH_SHORT
                    WORKING_DIRECTORY ${MAKE_CCODE_DIRS}
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            # получаем и парсим имя текущей ветки
            execute_process(
                    COMMAND git branch
                    COMMAND grep \*
                    COMMAND tr -d '\*'
                    COMMAND sed -e s/^[[:space:]]*//
                    OUTPUT_VARIABLE GIT_BRANCH
                    WORKING_DIRECTORY ${MAKE_CCODE_DIRS}
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            string(REGEX REPLACE " .*$" "" PROJ_VERSION_FULL ${GIT_COMMIT_HASH_FULL})
            string(REGEX REPLACE " .*$" "" PROJ_VERSION_SHORT ${GIT_COMMIT_HASH_SHORT})

            if(${MAKE_CCODE_DIRS} STREQUAL CMAKE_SOURCE_DIR)
                set(CCODE_NAME_L proj)
                string(TOUPPER ${CCODE_NAME_L} CCODE_NAME_U)
                set(${proj_hash_short} ${GIT_COMMIT_HASH_SHORT})
                set(${proj_branch} ${GIT_BRANCH})
            endif()
            if(${MAKE_CCODE_DIRS} STREQUAL EFLIB_DIR)
                set(CCODE_NAME_L "eflib")
                string(TOUPPER ${CCODE_NAME_L} CCODE_NAME_U)
                set(${eflib_hash_short} ${GIT_COMMIT_HASH_SHORT})
                set(${eflib_branch} ${GIT_BRANCH})
            endif()

            if(${MAKE_CCODE_DIRS} STREQUAL FREERTOS_DIR)
                set(CCODE_NAME_L "freertos")
                string(TOUPPER ${CCODE_NAME_L} CCODE_NAME_U)
            endif()

            if(${MAKE_CCODE_DIRS} STREQUAL MCU_HARDLIB_DIR)
                set(CCODE_NAME_L "mcu_hardlib")
                string(TOUPPER ${CCODE_NAME_L} CCODE_NAME_U)
            endif()

            file(WRITE ${CMAKE_BINARY_DIR}/commit_codes/${CCODE_NAME_L}_ccode.h
                "#define ${CCODE_NAME_U}_CCODE \"${PROJ_VERSION_FULL}\" \n"
                "#define ${CCODE_NAME_U}_BRANCH_NAME \"${GIT_BRANCH}\"")
        endforeach(MAKE_CCODE_DIRS)
    else()
        set(GIT_COMMIT_HASH "Git not found.")
    endif()
endmacro()

macro(CHOOSE_STARTUP_FILE_FOR MCU startup_s_file)
    string(TOLOWER ${MCU} MCU_L)
    set(${startup_s_file} "startup_${MCU_L}.s")
endmacro()

function(get_date_time)
    execute_process(
            COMMAND date +%d/%m/%Y\ %H:%M
            OUTPUT_VARIABLE BUILD_DATA_TIME
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(HEADER_FILE "${CMAKE_BINARY_DIR}/commit_codes/build_date_time.h")

    file(WRITE ${HEADER_FILE}
            "#pragma once\n\n"
            "#define BUILD_DATE_TIME \"${BUILD_DATA_TIME}\"")
endfunction()

# Helper function to add preprocesor definition of __FILENAME__
# to pass the filename without directory path for debugging use.
#
# Note that in header files this is not consistent with
# __FILE__ and __LINE__ since __FILENAME__ will be the
# compilation unit source file name (.c/.cpp).
#
# Example:
#
#   define_filename_for_sources(my_target)
#
# Will add -D__FILENAME__="filename" for each source file depended on
# by my_target, where filename is the name of the file.
#
function(define_filename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Add the __FILENAME__=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME)
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${sourcefile}" APPEND
            PROPERTY COMPILE_DEFINITIONS "__FILENAME__=\"${basename}\"")
    endforeach()
endfunction()
