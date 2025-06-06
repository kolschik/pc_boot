macro(get_hash_data_time)
    find_package(Git)
    if(GIT_FOUND)
        get_commit_hash(long GIT_COMMIT_HASH_FULL)
        execute_process(
                COMMAND date +%Y%m%d%H%M
                OUTPUT_VARIABLE BUILD_DATA_TIME
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        transform_string_to_char_array(${BUILD_DATA_TIME} RESULT_BUILD_DATA_TIME)
        transform_string_to_char_array(${GIT_COMMIT_HASH_FULL} RESULT_GIT_HASH)

        string(LENGTH GIT_COMMIT_HASH_FULL GIT_HASH_LENGHT)
        if(${GIT_HASH_LENGHT} LESS 64)
            string(REPLACE "}" ", 0}" RESULT_GIT_HASH ${RESULT_GIT_HASH})
        endif()

        file(WRITE ${CMAKE_SOURCE_DIR}/cfg/mc_bootloader_ccode.h
                "#define COMMIT_HASH_CODE ${RESULT_GIT_HASH} \n"
                "#define BUILD_DATE_TIME ${RESULT_BUILD_DATA_TIME}")
    else()
        message(WARNING "Git not found!")
    endif()
endmacro()


function(get_commit_hash HASH_LENGTH OUTSTR)
    find_package(Git)
    if(GIT_FOUND)
        if ("${HASH_LENGTH}" STREQUAL "long")
            execute_process(
                    COMMAND git "log" "-1" "--pretty=oneline"
                    OUTPUT_VARIABLE GIT_COMMIT_HASH_FULL
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            # регулярка обрезает все, что после пробела (там всякий мусор)
            string(REGEX REPLACE " .*$" "" GIT_COMMIT_HASH_FULL ${GIT_COMMIT_HASH_FULL})
            set(${OUTSTR} ${GIT_COMMIT_HASH_FULL} PARENT_SCOPE)
        elseif("${HASH_LENGTH}" STREQUAL "short")
            execute_process(COMMAND git "log" "-1" "--pretty=format:%h"
                    OUTPUT_VARIABLE GIT_COMMIT_HASH_SHORT
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            set(${OUTSTR} ${GIT_COMMIT_HASH_SHORT} PARENT_SCOPE)
        else()
            message(FATAL_ERROR "Unable to get commit hash: invalid parameter.\
			Usage: get_git_commit(<mode> <output>)\
				<mode> - long/short\
				<output> - your output variable")
        endif()
    else()
        message(WARNING "Git not found!")
    endif()
endfunction()


function(transform_string_to_char_array INSTR OUTSTR)
    execute_process(
            COMMAND echo ${INSTR}
            COMMAND sed "s|.|\'&\', |g"
            OUTPUT_VARIABLE RESULTSTR
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REGEX REPLACE "[,]$" "" RESULTSTR ${RESULTSTR})
    string(CONCAT RESULTSTR "{" "${RESULTSTR}" "}")

    set(${OUTSTR} ${RESULTSTR} PARENT_SCOPE)
endfunction(transform_string_to_char_array)
