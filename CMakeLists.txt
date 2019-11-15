cmake_minimum_required(VERSION 3.12)
#project(tools/rcomp-src/rcomp)
project(creative-engine)

set(CMAKE_CXX_STANDARD 11)

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
  SET(CREATIVE_ENGINE_PATH ".")
  #   message(STATUS "<<<<<<<<<<<<<<<<<<<<<<<<<< Falling back to default creative-engine path: ${CREATIVE_ENGINE_PATH}")
endif ()

INCLUDE_DIRECTORIES(
  ${CREATIVE_ENGINE_PATH}/src
  ${CREATIVE_ENGINE_PATH}/tools/rcomp-src
)

add_executable(rcomp
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/rcomp.cpp tools/rcomp-src/rcomp.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/RawFile.cpp tools/rcomp-src/RawFile.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/BMPFile.cpp tools/rcomp-src/BMPFile.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/lib/RawBitmap.cpp tools/rcomp-src/lib/RawBitmap.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/ResourceFile.cpp tools/rcomp-src/ResourceFile.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/lib/utils.cpp
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/TileMap.cpp tools/rcomp-src/TileMap.h
        ${CREATIVE_ENGINE_PATH}/src/Panic.cpp src/Panic.h
        ${CREATIVE_ENGINE_PATH}/src/Memory.cpp src/Memory.h
        ${CREATIVE_ENGINE_PATH}/src/BBase.cpp src/BBase.h
        ${CREATIVE_ENGINE_PATH}/src/BBitmap.cpp src/BBitmap.h
        ${CREATIVE_ENGINE_PATH}/src/BList.cpp src/BList.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/SpriteSheet.cpp tools/rcomp-src/SpriteSheet.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/TMXFile.cpp tools/rcomp-src/TMXFile.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/tmx/TMXTileSet.cpp tools/rcomp-src/tmx/TMXTileSet.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/tmx/TMXMap.cpp tools/rcomp-src/tmx/TMXMap.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/tmx/TMXLayer.cpp tools/rcomp-src/tmx/TMXLayer.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/lib/BSymbolTable.cpp tools/rcomp-src/lib/BSymbolTable.h
        )

add_executable(test
        ${CREATIVE_ENGINE_PATH}/test/main.cpp
        )

