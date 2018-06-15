add_executable(tests
    test_step.cpp
)
target_link_libraries(tests gtest_main blueprint)

add_test(NAME tests COMMAND tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})