cmake_minimum_required(VERSION 3.11)

project(demo-rxcpp LANGUAGES CXX)

enable_testing()

option(USE_OFFICIAL_DOWNLOADS "" OFF)
if(${USE_OFFICIAL_DOWNLOADS})
    set(RXCPP_URL https://github.com/ReactiveX/RxCpp/archive/4.1.0.zip)
    set(FMTLIB_URL https://github.com/fmtlib/fmt/releases/download/5.0.0/fmt-5.0.0.zip)
    set(FRUIT_URL https://github.com/google/fruit/archive/v3.1.1.zip)
    set(GOOGLETEST_URL https://github.com/google/googletest/archive/release-1.8.0.zip)
else()
    set(RXCPP_URL http://downloads.tac.com/dev/libs/RxCpp/4.1.0.zip)
    set(FMTLIB_URL http://downloads.tac.com/dev/libs/fmt/fmt-5.0.0.zip)
    set(FRUIT_URL http://downloads.tac.com/dev/libs/fruit/v3.1.1.zip)
    set(GOOGLETEST_URL http://downloads.tac.com/dev/libs/googletest/release-1.8.0.zip)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

include(FetchContent)
include(ExternalProject)

find_package(Boost COMPONENTS log)
include_directories(${Boost_INCLUDE_DIR})

FetchContent_Declare(rxcpp
    URL ${RXCPP_URL}
    URL_MD5 3af94f312985f82f8c1067e26f65bc5f
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/rxcpp
)
FetchContent_GetProperties(rxcpp)
if(NOT rxcpp_POPULATED)
    FetchContent_Populate(rxcpp)
endif()

FetchContent_Declare(fmt
    URL ${FMTLIB_URL}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/fmt
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR})
endif()

ExternalProject_Add(fruit
    URL ${FRUIT_URL}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/fruit
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/_stage
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_LIBS=OFF
)

include_directories(${CMAKE_SOURCE_DIR}/_stage/include)

find_library(FRUIT_LIB fruit)

FetchContent_Declare(googletest
    URL ${GOOGLETEST_URL}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_deps/googletest
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR})
endif()

add_subdirectory(src/lib)
add_subdirectory(src/test)
