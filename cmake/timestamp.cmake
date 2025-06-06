set(HEADER_FILE "${CMAKE_SOURCE_DIR}/cfg/timestamp.h")

string(TIMESTAMP BUILD_DATE_TIME %d/%m/%Y\ %H:%M)

file(WRITE ${HEADER_FILE} "#pragma once\n\n#define BUILD_DATE_TIME \"${BUILD_DATE_TIME}\"")
