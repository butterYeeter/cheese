cmake_minimum_required(VERSION 3.20)
project(uitest LANGUAGES C CXX)

file(GLOB_RECURSE UI_SOURCES src/*.cpp)

# message(${FILES})

add_library(uitest_lib SHARED ${UI_SOURCES} ${USER_SOURCES})
target_include_directories(uitest_lib PUBLIC
  ./include
  ${USER_INCLUDES}
)

add_custom_command(
    TARGET uitest_lib
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:uitest_lib>"
        ${CMAKE_BINARY_DIR}
)

if(WIN32)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)
    target_link_libraries(uitest_lib PRIVATE glfw3dll)
else()
    target_link_libraries(uitest_lib PRIVATE glfw)
endif()

message("${USER_SOURCES}")

set_target_properties(uitest_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
