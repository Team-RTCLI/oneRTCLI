cmake_minimum_required (VERSION 3.3)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
include(cmake/platform.cmake)

project(RTCLI_VM VERSION 0.0.1)

# Build Options.
option(RTCLI_VM_LIB "Build VM as Static Library." ON)
option(RTCLI_TESTS "Build VM as Static Library." ON)

# Detect Platforms.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_64BIT 1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PLATFORM_32BIT 1)
else()
    message(FATAL_ERROR "Unknown Architecture is specified.")
endif()

# Define Global Constants.
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})

if(PLATFORM_64BIT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug64)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebugInfo64)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release64)
elseif(PLATFORM_32BIT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebugInfo)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
else()
    message(FATAL_ERROR "Unknown Architecture is specified.")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

# Define Macros.
if(RTCLI_VM_LIB)
    add_definitions("-DRTCLI_BUILD_LIB")
    message(STATUS "Build as lib.")
endif()

# 
set(CLANG_DISABLED_WARNINGS "-Wno-unknown-attributes")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        ${CLANG_DISABLED_WARNINGS}
    )
endif()

if(NOT "cxx_std_11" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    message(FATAL_ERROR "C++11 Support is required to compile RTCLI")
endif()

file(GLOB_RECURSE 
    HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx
)
file(GLOB_RECURSE 
    SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx

    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl
)
file(GLOB_RECURSE 
    PLATFORM_SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.hxx

    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${RTCLI_PLATFORM}/*.inl
)

if(RTCLI_VM_LIB)
    add_library(RTCLI_VM STATIC ${SRC_FILES} ${PLATFORM_SRC_FILES})
else()
    add_library(RTCLI_VM SHARED ${SRC_FILES} ${PLATFORM_SRC_FILES})
endif()

target_include_directories(RTCLI_VM  
    PUBLIC 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE 
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

if(RTCLI_TESTS)
    add_subdirectory(tests)
endif()