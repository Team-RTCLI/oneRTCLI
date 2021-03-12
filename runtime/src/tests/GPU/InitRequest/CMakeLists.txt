enable_testing()

add_executable(CGPUDeviceTest 
   DeviceInitialize.cpp 
   VkDeviceExts.cpp 
   D3DDeviceExts.cpp
)

target_link_libraries(CGPUDeviceTest 
   gtest_main
)
gtest_discover_tests(CGPUDeviceTest)