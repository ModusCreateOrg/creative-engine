cmake_minimum_required(VERSION 3.12)
project(creative_engine_font_example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-exception-spec-mismatch")
set(CMAKE_VERBOSE_MAKEFILE ON)

# resource compiler
set(RCOMP "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/rcomp")

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_BINARY_DIR} /usr/local/include)

# external libraries
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

# build rcomp
add_custom_command(OUTPUT rcomp
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/../../tools && make
        OUTPUTS rcomp
        COMMENT "Building rcomp"
)

# build Resources.bin
add_custom_command(OUTPUT Resources.bin
        COMMAND @echo
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/src && ${RCOMP} Resources.r
        OUTPUTS Resources.bin
        DEPENDS rcomp
        COMMENT "Compiling Resources ${CMAKE_CURRENT_SOURCE_DIR}"
)

# gather creative-engine sources
file(GLOB_RECURSE CREATIVE_ENGINE RELATIVE ${CMAKE_SOURCE_DIR} "../../src/*.cpp")

# gather example sources
file(GLOB_RECURSE SRC RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

# LibXMP
add_compile_definitions(LIBXMP_CORE_PLAYER=true)
add_compile_definitions(LIBXMP_CORE_DISABLE_IT=true)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/../../src/libxmp/loaders/prowizarde
    ${CMAKE_SOURCE_DIR}/../../src/libxmp/loaders
    ${CMAKE_SOURCE_DIR}/../../src/libxmp
)

file(GLOB LIBXMP_SRC
    "${CMAKE_SOURCE_DIR}/../../src/libxmp/loaders/*.h"
    "${CMAKE_SOURCE_DIR}/../../src/libxmp/loaders/*.c"
    "${CMAKE_SOURCE_DIR}/../../src/libxmp/*.h"
    "${CMAKE_SOURCE_DIR}/../../src/libxmp/*.c"
)

file(GLOB_RECURSE CREATIVE_ENGINE_FONT_EXAMPLE RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(
        creative_engine_font_example
        Resources.bin
        ${LIBXMP_SRC}
        ${CREATIVE_ENGINE}
        ${CREATIVE_ENGINE_FONT_EXAMPLE}
)

# make app
if(APPLE)
        SET_TARGET_PROPERTIES(creative_engine_font_example PROPERTIES MACOSX_BUNDLE TRUE)
        INSTALL(TARGETS creative_engine_font_example DESTINATION ".")
endif(APPLE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} -L/usr/local/lib ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})





