enable_testing()

add_executable(SwapChainCreation 
    SwapChainCreation.cpp 
)
add_executable(SwapChainCreationWebGpu 
    SwapChainCreation.cpp 
)
target_compile_definitions(SwapChainCreationWebGpu PRIVATE -DTEST_WEBGPU)

target_link_libraries(SwapChainCreation 
   gtest_main
)
target_link_libraries(SwapChainCreationWebGpu 
   gtest_main
)
gtest_discover_tests(SwapChainCreation)
gtest_discover_tests(SwapChainCreationWebGpu)