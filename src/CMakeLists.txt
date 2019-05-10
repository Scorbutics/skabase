project(skabase)

include_directories(${GSL_DIR})
include_directories(${SKALOG_DIR})

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(skabase ${SOURCES})

