set(HEADER_FILE "${CMAKE_SOURCE_DIR}/cfg/build_number.h")
set(CACHE_FILE "${CMAKE_SOURCE_DIR}/cfg/.build_num.txt")

# Reading data from file + incrementation
if (EXISTS ${CACHE_FILE})
    file(READ ${CACHE_FILE} INCREMENTED_VALUE)
    math(EXPR INCREMENTED_VALUE "${INCREMENTED_VALUE}+1")
else()
    set(INCREMENTED_VALUE "1")
endif ()

# Update cache
file(WRITE ${CACHE_FILE} "${INCREMENTED_VALUE}")

# Create the header
file(WRITE ${HEADER_FILE} "#ifndef BUILD_NUMBER_H\n#define BUILD_NUMBER_H\n\n#define BUILD_NUMBER \"${INCREMENTED_VALUE}\"\n\n#endif")