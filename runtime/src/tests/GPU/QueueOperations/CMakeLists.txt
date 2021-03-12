enable_testing()

add_executable(QueueOperations 
    QueueOperations.cpp 
)
add_executable(QueueOperationsWebGpu 
    QueueOperations.cpp 
)
target_compile_definitions(QueueOperationsWebGpu PRIVATE -DTEST_WEBGPU)

target_link_libraries(QueueOperations 
   gtest_main
)
target_link_libraries(QueueOperationsWebGpu 
   gtest_main
)
gtest_discover_tests(QueueOperations)
gtest_discover_tests(QueueOperationsWebGpu)