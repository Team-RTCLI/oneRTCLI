enable_testing()

add_executable(DeviceInitialize 
   DeviceInitialize.cpp 
)
add_executable(WebGpuDeviceInitialize 
   DeviceInitialize.cpp 
)
target_compile_definitions(WebGpuDeviceInitialize PRIVATE -DTEST_WEBGPU)

target_link_libraries(DeviceInitialize 
   gtest_main
)
target_link_libraries(WebGpuDeviceInitialize 
   gtest_main
)
gtest_discover_tests(DeviceInitialize)
gtest_discover_tests(WebGpuDeviceInitialize)