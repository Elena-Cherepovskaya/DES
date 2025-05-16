cmake_minimum_required(VERSION 3.28)
project(lab_1 VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
SET(Boost_DIR "D:/mingwNew/mingw64/boost_1_87_0/stage/lib/cmake/Boost-1.87.0")
find_package(Boost 1.70 REQUIRED COMPONENTS random CONFIG)

set(CMAKE_CURRENT_SOURCE_DIR "src/")

set(src main.cpp DES_round_key.cpp DES_coder_function.cpp DEAL_128_round_key.cpp)
list(TRANSFORM src PREPEND ${CMAKE_CURRENT_SOURCE_DIR})

if(Boost_FOUND)
    add_executable(${PROJECT_NAME} ${src})
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::random)
endif()
