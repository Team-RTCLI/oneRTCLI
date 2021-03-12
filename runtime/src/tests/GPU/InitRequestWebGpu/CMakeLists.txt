enable_testing()

add_executable(CGPUWebGpuDeviceTest 
   WebGpuInitialize.cpp 
)

target_link_libraries(CGPUWebGpuDeviceTest 
   gtest_main
)
gtest_discover_tests(CGPUWebGpuDeviceTest)