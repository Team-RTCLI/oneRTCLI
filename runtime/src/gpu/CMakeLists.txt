#target_compile_definitions(-D "BACKEND_WEB_GPU")
project(CGPU)

file(GLOB_RECURSE 
    HEADER_FILES 
    ${ROOT_DIR}/include/cgpu/*.h
    ${ROOT_DIR}/include/cgpu/*.hpp
    ${ROOT_DIR}/include/cgpu/*.hxx
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
add_library(CGPU ${HEADER_FILES} ${SRC_FILES})

target_include_directories(RTCLI_VM  
    PUBLIC 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${ROOT_DIR}/include>
    PRIVATE 
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})