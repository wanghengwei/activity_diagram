cmake_minimum_required(VERSION 3.11)

project(demo-rxcpp)

include(FetchContent)
include(ExternalProject)

find_package(Boost COMPONENTS log)

FetchContent_Declare(rxcpp
    URL https://github.com/ReactiveX/RxCpp/archive/4.1.0.zip
    URL_MD5 3af94f312985f82f8c1067e26f65bc5f
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/.deps/rxcpp
)
FetchContent_GetProperties(rxcpp)
if(NOT rxcpp_POPULATED)
    FetchContent_Populate(rxcpp)
endif()

FetchContent_Declare(fmt
    URL https://github.com/fmtlib/fmt/releases/download/5.0.0/fmt-5.0.0.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/.deps/fmt
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR})
endif()

ExternalProject_Add(fruit
    URL https://github.com/google/fruit/archive/v3.1.1.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/.deps/fruit
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/.stage
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_LIBS=OFF
)

include_directories(${CMAKE_SOURCE_DIR}/.stage/include)

find_library(FRUIT_LIB fruit)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Robot.cpp Step.cpp Blueprint.cpp)
target_compile_definitions(demo-rxcpp PUBLIC BOOST_ALL_DYN_LINK)
target_include_directories(demo-rxcpp PRIVATE ${rxcpp_SOURCE_DIR}/Rx/v2/src)
target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES} pthread ${FRUIT_LIB} fmt)


FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/.deps/googletest
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    # include_directories(${googletest_SOURCE_DIR}/googletest/include)
    add_subdirectory(${googletest_SOURCE_DIR})
endif()
add_executable(tests test/test_step.cpp)
target_link_libraries(tests gtest_main)
# target_include_directories(tests PRIVATE ${googletest_SOURCE_DIR}/googletest/include)