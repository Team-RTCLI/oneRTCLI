cmake_minimum_required (VERSION 3.3)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(RTCLI)

include(cmake/platform.cmake)
include(cmake/vulkan.cmake)

# Build Options.
option(RTCLI_VM_LIB "Build VM as Static Library." ON)
option(RTCLI_TESTS "Build Test Targets." ON)

# Define Macros.
if(RTCLI_VM_LIB)
    add_definitions("-DRTCLI_BUILD_LIB")
    message(STATUS "Build as lib.")
endif()

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

add_subdirectory(vm)
add_subdirectory(gpu)

if(RTCLI_TESTS)
    add_subdirectory(tests)
endif()