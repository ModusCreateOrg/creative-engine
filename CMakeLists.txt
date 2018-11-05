cmake_minimum_required(VERSION 3.12)
project(creative-engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-exception-spec-mismatch")
set(CMAKE_VERBOSE_MAKEFILE ON)

# resource compiler
set(RCOMP "${CMAKE_CURRENT_SOURCE_DIR}/tools/rcomp")

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} 
${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/test/src ${CMAKE_BINARY_DIR} /usr/local/include)

# external libraries
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

# build rcomp
add_custom_command(OUTPUT rcomp
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/tools && make
        OUTPUTS rcomp
        COMMENT "Building rcomp")

# build Resources.bin
add_custom_command(OUTPUT Resources.bin
        #        COMMAND ls ${RCOMP}
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/test && ${RCOMP} Resources.r
        OUTPUTS Resources.bin
        DEPENDS rcomp
        COMMENT "Compiling Resources ${CMAKE_CURRENT_SOURCE_DIR} ")

# gather creative-engine s\ources
file(GLOB_RECURSE CREATIVE_ENGINE RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

# gather test sources
file(GLOB_RECURSE TEST_SRC RELATIVE ${CMAKE_SOURCE_DIR} "test/main.cpp")

# LibXMP
add_compile_definitions(LIBXMP_CORE_PLAYER=true)
add_compile_definitions(LIBXMP_CORE_DISABLE_IT=true)
include_directories(
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/src/libxmp/loaders/prowizarde
    ${CMAKE_SOURCE_DIR}/src/libxmp/loaders
    ${CMAKE_SOURCE_DIR}/src/libxmp
)

file(GLOB LIBXMP_SRC
    "${CMAKE_SOURCE_DIR}/src/libxmp/loaders/*.h"
    "${CMAKE_SOURCE_DIR}/src/libxmp/loaders/*.c"
    "${CMAKE_SOURCE_DIR}/src/libxmp/*.h"
    "${CMAKE_SOURCE_DIR}/src/libxmp/*.c"
)

add_executable(creative-engine-test
        Resources.bin
        ${LIBXMP_SRC}
        ${CREATIVE_ENGINE}
        ${TEST_SRC}
        )

TARGET_LINK_LIBRARIES(creative-engine-test -L/usr/local/lib ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})





