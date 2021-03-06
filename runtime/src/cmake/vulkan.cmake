find_package(Vulkan)

if(${Vulkan_FOUND})
    include_directories(${Vulkan_INCLUDE_DIRS})
    message(STATUS "Found VK SDK. Include Dir: ${Vulkan_INCLUDE_DIRS}")
endif()