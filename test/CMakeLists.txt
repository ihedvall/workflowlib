# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestWorkflow
        VERSION 1.0
        DESCRIPTION "Google unit tests for the workflow library"
        LANGUAGES CXX)

add_executable(test_workflow
        test_parameter.cpp test_parameter_container.cpp test_event.cpp test_runner.cpp test_workflowserver.cpp)

target_include_directories(test_workflow PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${utillib_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS})

if (MINGW)
    target_link_options(test_workflow PRIVATE -static -fstack-protector)
elseif (MSVC)
    target_compile_definitions(test_workflow PRIVATE -D_WIN32_WINNT=0x0A00)
endif ()

target_link_libraries(test_workflow PRIVATE workflow)
target_link_libraries(test_workflow PRIVATE util)
target_link_libraries(test_workflow PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_workflow PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_workflow PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

if (WIN32)
    #target_link_libraries(test_mdf PRIVATE iconv)
    target_link_libraries(test_workflow PRIVATE ws2_32)
    target_link_libraries(test_workflow PRIVATE mswsock)
endif ()

include(GoogleTest)
gtest_discover_tests(test_workflow)


