enable_testing()

add_executable(InitRequest InitRequestTest.cpp test.cpp)

target_link_libraries(InitRequest 
   gtest_main
)
gtest_discover_tests(InitRequest)