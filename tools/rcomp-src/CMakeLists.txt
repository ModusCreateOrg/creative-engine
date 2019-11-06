cmake_minimum_required(VERSION 3.12)
project(rcomp)

set(CMAKE_CXX_STANDARD 11)

# creative-engine path
if (EXISTS $ENV{CREATIVE_ENGINE_PATH})
  # SET ENV variable in CLion project SETtings
  SET(CREATIVE_ENGINE_PATH $ENV{CREATIVE_ENGINE_PATH})
  #   message(STATUS "********************************* Using user-defined creative-engine path: ${CREATIVE_ENGINE_PATH}")
else ()
  #    SET(CREATIVE_ENGINE_PATH "${CMAKE_SOURCE_DIR}/../creative-engine") # This broke debugging in clion!
  SET(CREATIVE_ENGINE_PATH "../../../creative-engine")
  #   message(STATUS "<<<<<<<<<<<<<<<<<<<<<<<<<< Falling back to default creative-engine path: ${CREATIVE_ENGINE_PATH}")
endif ()

# use all the CPU cores and threads
ProcessorCount(N)
if (NOT N EQUAL 0)
  set(N, N*2)         # use hyperthreads - processor count is 4 on an i7 that has 8 cores including hyperthreads
  set(${PROJECT_NAME}_FLAGS -j${N})
  set(${PROJECT_NAME}_args ${${PROJECT_NAME}_args} PARALLEL_LEVEL ${N})
endif ()

INCLUDE_DIRECTORIES(
  ${CREATIVE_ENGINE_PATH}/src
  ${CREATIVE_ENGINE_PATH}/tools/rcomp-src
)

add_executable(tools
  rcomp.cpp rcomp.h
  RawFile.cpp RawFile.h
  BMPFile.cpp BMPFile.h
  RawBitmap.cpp RawBitmap.h
  ResourceFile.cpp ResourceFile.h
  utils.cpp
  TileMap.cpp TileMap.h
  ../../src/Panic.cpp ../../src/Panic.h
  ../../src/Memory.cpp ../../src/Memory.h
  ../../src/BBase.cpp ../../src/BBase.h
  ../../src/BBitmap.cpp ../../src/BBitmap.h
  SpriteSheet.cpp SpriteSheet.h TMX.cpp TMX.h TileMapBase.h LayerInfo.h)

