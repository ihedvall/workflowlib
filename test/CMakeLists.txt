# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestWorkflow
        VERSION 1.0
        DESCRIPTION "Google unit tests for the workflow library"
        LANGUAGES CXX)

add_executable(test_workflow
        test_parameter.cpp test_parameter_container.cpp)

target_include_directories(test_workflow PRIVATE
        ../include
        ../src
        ${utillib_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

if (MINGW)
    target_link_options(test_workflow PRIVATE -static -fstack-protector)
elseif (MSVC)
    target_compile_definitions(test_workflow PRIVATE -D_WIN32_WINNT=0x0A00)
endif ()

target_link_libraries(test_workflow PRIVATE workflow)
target_link_libraries(test_workflow PRIVATE util)
target_link_libraries(test_workflow PRIVATE expat)
target_link_libraries(test_workflow PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_workflow PRIVATE ${GTEST_BOTH_LIBRARIES})

if (WIN32)
    #target_link_libraries(test_mdf PRIVATE iconv)
    target_link_libraries(test_workflow PRIVATE ws2_32)
    target_link_libraries(test_workflow PRIVATE mswsock)
endif ()

include(GoogleTest)
gtest_discover_tests(test_workflow)


