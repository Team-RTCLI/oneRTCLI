project(RTCLI_VM VERSION 0.0.1)

# 
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        "-Wno-unknown-attributes" 
        "-Wno-visibility"
    )
endif()

if(NOT "cxx_std_11" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    message(FATAL_ERROR "C++11 Support is required to compile RTCLI")
endif()

file(GLOB_RECURSE 
    HEADER_FILES 
    ${ROOT_DIR}/include/rtcli/*.h
    ${ROOT_DIR}/include/rtcli/*.hpp
    ${ROOT_DIR}/include/rtcli/*.hxx
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
    $<BUILD_INTERFACE:${ROOT_DIR}/include>
    PRIVATE 
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

