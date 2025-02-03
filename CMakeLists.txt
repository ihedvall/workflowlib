# Copyright 2022 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(workflowlib
        VERSION 1.0
        DESCRIPTION "C++ library for running external scripts."
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Static libraries are preferred" OFF)
option(WORKFLOW_DOC "If doxygen is installed, then build documentation in Release mode" OFF)
option(WORKFLOW_TEST "If Google Test is installed, then build the unit tests" OFF)
#option(WORKFLOW_TOOLs "If disabled, only the library is built" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

include(CMakePrintHelpers)
include(script/utillib.cmake)
include(script/boost.cmake)
include(script/expat.cmake)


if (WORKFLOW_TEST)
    include(script/openssl.cmake)
    include(script/googletest.cmake)
endif()

if (WORKFLOW_DOC)
    include(script/doxygen.cmake)
endif()

#include(script/wxwidgets.cmake)

add_library(workflow
        src/eventengine.cpp include/workflow/eventengine.h
        src/event.cpp include/workflow/event.h
        src/workflow.cpp include/workflow/workflow.h
        src/workflowengine.cpp src/workflowengine.h
        src/itask.cpp include/workflow/itask.h
        src/parameter.cpp include/workflow/parameter.h
        src/parametercontainer.cpp include/workflow/parametercontainer.h
        src/workflowserver.cpp include/workflow/workflowserver.h
        src/device.cpp include/workflow/device.h
        src/initdirectorydata.cpp src/initdirectorydata.h
        src/template_names.icc
        src/scandirectorydata.cpp src/scandirectorydata.h
        src/sysloginput.cpp src/sysloginput.h
        src/syslogpublisher.cpp src/syslogpublisher.h
        src/runsyslogschedule.cpp src/runsyslogschedule.h
        src/itaskfactory.cpp include/workflow/itaskfactory.h
        src/defaulttemplatefactory.cpp src/defaulttemplatefactory.h
)

target_include_directories(workflow PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> )

target_include_directories(workflow PRIVATE ${utillib_SOURCE_DIR}/include)
target_include_directories(workflow PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(workflow PRIVATE expat)

cmake_print_properties(TARGETS workflow PROPERTIES INCLUDE_DIRECTORIES)

target_compile_definitions(workflow PRIVATE XML_STATIC)

if (MSVC)
    target_compile_definitions(workflow PRIVATE _WIN32_WINNT=0x0A00)
endif ()

if (WORKFLOW_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()

#if (DOXYGEN_FOUND AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
#
#   set(DOXYGEN_REPEAT_BRIEF NO)
#    set(DOXYGEN_PROJECT_NAME "PubSubLib")
#    set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen/utillib.css)
#    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/manual)
#    doxygen_add_docs( doc_pubsub ALL
#            include/pubsub doxygen
#            COMMENT "Publish/Subscribe Library generating API documentation with Doxygen")
#endif()
