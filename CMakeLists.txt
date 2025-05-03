cmake_minimum_required(VERSION 3.13)
project(DES VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


set(CMAKE_PREFIX_PATH "D:/boost_1_87_0/")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.87.0)

set(EXECUTABLE_OUTPUT_PATH "bin")

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build_cloud.nosync/)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
SET(EXECUTABLE_OUTPUT_PATH ${dir})
SET(LIBRARY_OUTPUT_PATH ${dir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall ${use_openMP}")
set(CMAKE_CURRENT_SOURCE_DIR "src/")

set(src main.cpp DES_coder.cpp DES_round_key.cpp DES_coder_function.cpp DEAL_128_round_key.cpp DEAL_128_coder.cpp)
list(TRANSFORM src PREPEND ${CMAKE_CURRENT_SOURCE_DIR})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${src})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})