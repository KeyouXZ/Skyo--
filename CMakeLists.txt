cmake_minimum_required(VERSION 3.14)
project(Skyo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(dpp REQUIRED)
find_package(CURL REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*/*.cpp"
)

message(STATUS "Found source files:")
foreach(SRC ${SOURCES})
    message(STATUS "  ${SRC}")
endforeach()

add_executable(Skyo ${SOURCES})

target_link_libraries(Skyo PRIVATE dpp::dpp CURL::libcurl)

