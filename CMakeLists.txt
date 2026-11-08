cmake_minimum_required(VERSION 3.16)
project(wavefront_rt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(rt src/main.cpp)
target_include_directories(rt
	PUBLIC include
	PRIVATE src/headers
)
target_compile_options(rt PRIVATE -Wall -Wextra -Werror)
