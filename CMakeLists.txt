cmake_minimum_required(VERSION 3.16)

project(EigenTemplate VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_definitions( "/D_CRT_SECURE_NO_WARNINGS /MP /openmp" )
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/EIGEN)

file(GLOB H_FILES src/*.h)
file(GLOB CPP_FILES src/*.cpp)

set(SOURCE_LIST ${CPP_FILES} ${H_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_LIST})

