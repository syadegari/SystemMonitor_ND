cmake_minimum_required(VERSION 3.15.0)
project(monitor)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(monitor ${SOURCES})

set_property(TARGET monitor PROPERTY CXX_STANDARD 17)
target_link_libraries(monitor ${CURSES_LIBRARIES})
# TODO: Run -Werror in CI.
target_compile_options(monitor PRIVATE -Wall -Wextra)


add_subdirectory(externals/googletest)
file(GLOB SOURCE_EXCEPT_MAIN "src/*.cpp")
list(REMOVE_ITEM SOURCE_EXCEPT_MAIN  "src/main.cpp")
# enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(unittest 

tests/format_test.cpp
tests/read_file_test.cpp 
tests/data_holder_test.cpp

 src/utils.cpp
 src/format.cpp)
# add_executable(runUnitTests ${TEST_SOURCES})
# add_executable(unittest ${TEST_SOURCES} ${SOURCE_EXCEPT_MAIN})

target_link_libraries(unittest gtest_main)