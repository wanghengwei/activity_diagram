cmake_minimum_required(VERSION 3.11)

project(demo-rxcpp LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
include(ExternalProject)

find_package(Boost COMPONENTS log)

FetchContent_Declare(rxcpp
    # URL https://github.com/ReactiveX/RxCpp/archive/4.1.0.zip
    URL http://downloads.tac.com/dev/libs/RxCpp/4.1.0.zip
    URL_MD5 3af94f312985f82f8c1067e26f65bc5f
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/rxcpp
)
FetchContent_GetProperties(rxcpp)
if(NOT rxcpp_POPULATED)
    FetchContent_Populate(rxcpp)
endif()

FetchContent_Declare(fmt
    URL http://downloads.tac.com/dev/libs/fmt/fmt-5.0.0.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/fmt
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR})
endif()

ExternalProject_Add(fruit
    URL http://downloads.tac.com/dev/libs/fruit/v3.1.1.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/fruit
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/_stage
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_LIBS=OFF
)

include_directories(${CMAKE_SOURCE_DIR}/_stage/include)

find_library(FRUIT_LIB fruit)

#add_executable(${CMAKE_PROJECT_NAME} main.cpp Robot.cpp Step.cpp Blueprint.cpp)
#target_compile_definitions(demo-rxcpp PUBLIC BOOST_ALL_DYN_LINK)
#target_include_directories(demo-rxcpp PRIVATE ${rxcpp_SOURCE_DIR}/Rx/v2/src)
#target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES} pthread ${FRUIT_LIB} fmt)


FetchContent_Declare(googletest
    URL http://downloads.tac.com/dev/libs/googletest/release-1.8.0.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/googletest
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    # include_directories(${googletest_SOURCE_DIR}/googletest/include)
    add_subdirectory(${googletest_SOURCE_DIR})
endif()
#add_executable(tests test/test_step.cpp)
#target_link_libraries(tests gtest_main)
# target_include_directories(tests PRIVATE ${googletest_SOURCE_DIR}/googletest/include)

add_subdirectory(src/lib)
add_subdirectory(src/test)
