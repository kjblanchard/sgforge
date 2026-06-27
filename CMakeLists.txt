cmake_minimum_required(VERSION 3.10)
set(LIBRARY_NAME sgforge)
option(SGFORGE_INSTALL_TOOLS "Install sgforge binary" ON)
option(SGFORGE_INSTALL_LIB "Install sgforge library" ON)
project(${LIBRARY_NAME} C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_MESSAGE LAZY)
if(UNIX OR APPLE)
  list(
    APPEND
    COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wdeprecated
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wshadow
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2)
endif(UNIX OR APPLE)
include(FetchContent)
add_library(${LIBRARY_NAME} STATIC ./src/unpack.c ./src/header.c
                                   ./src/directory.c)
 target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(FORGE_EXE ${LIBRARY_NAME}_creator)
add_executable(${FORGE_EXE} ./src/main.c)
if(NOT TARGET sgtools)
if(DEFINED ENV{GITHUB_ACTIONS} OR NOT SYSTEM_PACKAGES )
  message(STATUS "Building on GitHub Actions - fetching sgtools")
  FetchContent_Declare(
    sgtools
    GIT_REPOSITORY https://github.com/kjblanchard/sgtools.git
    GIT_TAG master)
  FetchContent_MakeAvailable(sgtools)
else()
  message(STATUS "Using local sgtools")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sgtools
                   ${CMAKE_BINARY_DIR}/sgtools)
endif(NOT TARGET sgtools)
endif()
target_link_libraries(${LIBRARY_NAME} PUBLIC sgtools)
if(WIN32)
  target_link_libraries(${FORGE_EXE} PUBLIC ws2_32)
  target_link_libraries(${LIBRARY_NAME} PUBLIC ws2_32)
endif()
target_link_libraries(${FORGE_EXE} PRIVATE ${LIBRARY_NAME})
target_include_directories(${FORGE_EXE} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${FORGE_EXE} PROPERTIES OUTPUT_NAME sgforge)

if(SGFORGE_INSTALL_TOOLS)
  install(TARGETS ${FORGE_EXE} RUNTIME DESTINATION bin)
endif()

if(SGFORGE_INSTALL_LIB)
  install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/sgforge DESTINATION include)

endif()
