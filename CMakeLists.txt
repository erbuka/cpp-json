cmake_minimum_required(VERSION 3.20)

project(json)   

enable_testing()

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES "src/*.cpp")

add_library(json ${SRC_FILES})
target_include_directories(json PUBLIC include PRIVATE src)

file(GLOB TEST_FILES "test/*.cpp")

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE json)
    target_include_directories(${TEST_NAME} PRIVATE src)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
