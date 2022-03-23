cmake_minimum_required(VERSION 3.16)

project(FLOW)

# project type
set(CMAKE_BUILD_TYPE Release)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# specify the C++ standard
set(CMAKE_CXX_COMPILER "g++-11")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(THREADS_PREFER_PTHREAD_FLAG ON)

# pch
# target_precompile_headers(FLOW PUBLIC Dense)

# custom names for dirs
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(PROJECT_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/dep)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BIN_DIR})

set(SOURCES
    ${PROJECT_SOURCE_DIR}/flow.cpp
    ${PROJECT_SOURCE_DIR}/e57handle.cpp
    ${PROJECT_SOURCE_DIR}/geometry.cpp
    ${PROJECT_SOURCE_DIR}/state.cpp
    ${PROJECT_SOURCE_DIR}/fluids.cpp
)

add_executable(flow ${SOURCES})
add_subdirectory(src/display)

# find xerces library needed for .so
set(XERCES_DIR ${PROJECT_DEPENDENCIES_DIR}/xerces)
find_library(XERCES_LIB
    NAMES xerces
    HINTS ${XERCES_DIR}/lib)

# find E57RefImpl for static linking
set(E57_DIR ${PROJECT_DEPENDENCIES_DIR}/E57RefImpl)
find_library(E57_LIB
    NAMES E57RefImpl
    HINTS ${E57_DIR}/lib)

# pthreads
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

target_include_directories(flow PUBLIC ${PROJECT_INCLUDE_DIR})

target_include_directories(
    flow
    PUBLIC ${PROJECT_SRC_DIR}
    PUBLIC ${PROJECT_INCLUDE_DIR}
    PUBLIC ${PROJECT_INCLUDE_DIR}/Eigen
    PUBLIC ${XML_DIR}/include
    PUBLIC ${E57_DIR}/include
    PUBLIC ${SDL2_INCLUDE_DIRS}
    PUBLIC ${OPENGL_INCLUDE_DIR}
    PUBLIC ${GLEW_INCLUDE_DIRS}
)

target_link_libraries(flow
    ${E57_LIB}
    ${XERCES_LIB}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    Threads::Threads
)