cmake_minimum_required(VERSION 3.20)
project(uitest LANGUAGES CXX)

file(GLOB_RECURSE UI_SOURCES src/*.cpp)

# message(${FILES})

add_library(uitest_lib STATIC ${UI_SOURCES})
target_include_directories(uitest_lib PUBLIC
  ./include
)

if(WIN32)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)
    target_link_libraries(uitest_lib PRIVATE glfw3dll)
else()
    target_link_libraries(uitest_lib PRIVATE glfw)
endif()

set_target_properties(uitest_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
