enable_testing()

add_executable(CGPUDeviceTest 
   DeviceInitialization.cpp 
   VkDeviceExts.cpp 
   test.cpp
)

target_link_libraries(CGPUDeviceTest 
   gtest_main
)
gtest_discover_tests(CGPUDeviceTest)