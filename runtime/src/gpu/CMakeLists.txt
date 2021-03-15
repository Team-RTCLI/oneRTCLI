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

target_include_directories(CGPU  
    PUBLIC 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${ROOT_DIR}/include>
    PRIVATE 
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})


## LINK WEBGPU LIB
target_link_directories(CGPU
    PUBLIC
    $<BUILD_INTERFACE:${ROOT_DIR}/nativelibs/webgpu/${RTCLI_PLATFORM}/x64/release>
    $<INSTALL_INTERFACE:nativelibs/webgpu/${RTCLI_PLATFORM}/x64/release>
)
set(DAWN_COMPS dawn_native dawn_platform dawn_proc)
target_link_libraries(CGPU
    PUBLIC
    ${DAWN_COMPS} #${Vulkan_LIBRARY} 
)

if(MSVC)
    foreach(DAWN_COMP ${DAWN_COMPS})
        add_custom_command(TARGET CGPU POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ROOT_DIR}/nativelibs/webgpu/${RTCLI_PLATFORM}/x64/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/${DAWN_COMP}.dll 
            $<$<CONFIG:Debug>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}>$<$<CONFIG:Release>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>$<$<CONFIG:RelWithDebInfo>:${${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}}>$<$<CONFIG:MinSizeRel>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>/${DAWN_COMP}.dll
        )
    endforeach()
endif(MSVC)