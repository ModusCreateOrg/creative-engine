cmake_minimum_required(VERSION 3.12)
#project(tools/rcomp-src/rcomp)
project(creative-engine)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(SDL2 REQUIRED)

# use all the CPU cores and threads
include (ProcessorCount)
ProcessorCount(N)
if (NOT N EQUAL 0)
  set(N, N*2)         # use hyperthreads - processor count is 4 on an i7 that has 8 cores including hyperthreads
  set(${PROJECT_NAME}_FLAGS -j${N})
  set(${PROJECT_NAME}_args ${${PROJECT_NAME}_args} PARALLEL_LEVEL ${N})
endif ()

# creative-engine path
if (EXISTS $ENV{CREATIVE_ENGINE_PATH})
  # SET ENV variable in CLion project SETtings
  SET(CREATIVE_ENGINE_PATH $ENV{CREATIVE_ENGINE_PATH})
  #   message(STATUS "********************************* Using user-defined creative-engine path: ${CREATIVE_ENGINE_PATH}")
else ()
  #    SET(CREATIVE_ENGINE_PATH "${CMAKE_SOURCE_DIR}/../creative-engine") # This broke debugging in clion!
  SET(CREATIVE_ENGINE_PATH ${PROJECT_SOURCE_DIR})
  #   message(STATUS "<<<<<<<<<<<<<<<<<<<<<<<<<< Falling back to default creative-engine path: ${CREATIVE_ENGINE_PATH}")
endif ()

SET(CREATIVE_ENGINE_INCLUDE_DIRS
        ${CREATIVE_ENGINE_PATH}/src
        ${CREATIVE_ENGINE_PATH}/src/BStore
        ${CREATIVE_ENGINE_PATH}/src/BStore/DesktopStore
        ${CREATIVE_ENGINE_PATH}/src/Audio
        ${CREATIVE_ENGINE_PATH}/src/Audio/SDL2Audio
        ${CREATIVE_ENGINE_PATH}/src/Audio/LDKAudio
        ${CREATIVE_ENGINE_PATH}/src/Display
        ${CREATIVE_ENGINE_PATH}/src/Display/DesktopDisplay
        ${CREATIVE_ENGINE_PATH}/src/Display/LDKDisplay
        ${CREATIVE_ENGINE_PATH}/src/Controls
        ${CREATIVE_ENGINE_PATH}/src/Controls/DesktopControls
        ${CREATIVE_ENGINE_PATH}/src/Controls/LDKControls
        ${CREATIVE_ENGINE_PATH}/src/Widgets
        ${CREATIVE_ENGINE_PATH}/src/libxmp
        ${CREATIVE_ENGINE_PATH}/src/libxmp/loaders
        )

INCLUDE_DIRECTORIES(
  ${CREATIVE_ENGINE_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
)

file(GLOB_RECURSE CREATIVE_ENGINE_SOURCE_FILES
        ${CREATIVE_ENGINE_PATH}/src/*.cpp
        ${CREATIVE_ENGINE_PATH}/src/*.c
        )

INCLUDE( ${CMAKE_CURRENT_SOURCE_DIR}/tools/tools.cmake )
INCLUDE( ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cmake )


