add_executable(tests
    test_step.cpp
)
target_link_libraries(tests gtest gmock_main activity_diagram)
target_compile_definitions(tests PUBLIC BOOST_ALL_DYN_LINK)

add_test(NAME tests COMMAND tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})