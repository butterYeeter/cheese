cmake_minimum_required(VERSION 3.20)

project(main VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX _d)

set(MINGW_BIN_DIR "C:/Users/Raaid/Documents/Programming/tools/mingw64/bin")

if(WIN32)
  set(GLFW_LIB_NAME glfw3dll)
  link_directories(${PROJECT_SOURCE_DIR}/lib/windows)
  set(LIB_PLATFORM_DIR "./lib/windows")
  set(LIB_PLATFORM_SUFFIX "dll")
elseif(UNIX)
  set(GLFW_LIB_NAME glfw)
  find_package(glfw3 3.3 REQUIRED)
  set(LIB_PLATFORM_DIR "$./lib/linux")
  set(LIB_PLATFORM_SUFFIX "so")
endif()


file(GLOB_RECURSE APP_SOURCES src/*.c)
file(GLOB LIB_FILES "${LIB_PLATFORM_DIR}/*.${LIB_PLATFORM_SUFFIX}")
# message("${LIB_FILES}")

add_executable(main ${APP_SOURCES})
target_include_directories(main PUBLIC
    ./include
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/resources"
  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
  COMMENT "Copying resources..."
)

foreach(LIB_FILE ${LIB_FILES})
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${LIB_FILE}"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying library: ${LIB_FILE}"
  )
endforeach()


set(USER_SOURCES ${PROJECT_SOURCE_DIR}/src/ui.cpp)
set(USER_INCLUDES ${PROJECT_SOURCE_DIR}/include)
# message("User sources:\n${USER_SOURCES}\n\nUser includes:\n${USER_INCLUDES}\n")

add_subdirectory(uitest)

install(TARGETS main
        RUNTIME_DEPENDENCY_SET runtime_set
        RUNTIME DESTINATION .
        COMPONENT Runtime)

install(
  RUNTIME_DEPENDENCY_SET runtime_deps
  DIRECTORIES 
    "${MINGW_BIN_DIR}"                # MinGW DLLs (e.g., C:/mingw64/bin)
    "${CMAKE_SOURCE_DIR}/lib/windows"  # GLFW3.dll location
    "${CMAKE_BINARY_DIR}/uitest"   # Path to libui_test.dll
  PRE_INCLUDE_REGEXES ".*"            # Include all dependencies
  POST_EXCLUDE_REGEXES ".*system32/.*"  # Exclude system DLLs
  COMPONENT Runtime
)

install(
  FILES
    "${MINGW_BIN_DIR}/libstdc++-6.dll"
    "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
    "${MINGW_BIN_DIR}/libwinpthread-1.dll"
    "${CMAKE_SOURCE_DIR}/lib/windows/glfw3.dll"  # GLFW3.dll
    # "${CMAKE_BINARY_DIR}/uitest/libuitest_lib_d.dll"
  DESTINATION bin
  COMPONENT Runtime
)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/resources
  DESTINATION .
)

target_link_libraries(main PRIVATE
    ${GLFW_LIB_NAME}
    uitest_lib
)


if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
  target_sources(main PRIVATE ${PROJECT_SOURCE_DIR}/lib/windows/glfw3.dll)
  set_target_properties(main PROPERTIES
      WIN32_EXECUTABLE TRUE
  )
endif()