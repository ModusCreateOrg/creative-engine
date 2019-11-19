### CREATIVE ENGINE CMAKE ####
# Exports:
# ${CREATIVE_ENGINE_INCLUDE_DIRS} - used in INCLUDE_DIRECTORIES(...)
# ${CREATIVE_ENGINE_SOURCE_FILES} - used in ADD_EXECUTABLE(...)
# ${CREATIVE_ENGINE_LINK_LIBRARIES} - used in TARGET_LINK_LIBRARIES(...)

# creative-engine path
if (EXISTS $ENV{CREATIVE_ENGINE_PATH})
    # SET ENV variable in CLion project SETtings
    SET(CREATIVE_ENGINE_PATH $ENV{CREATIVE_ENGINE_PATH})
    #   message(STATUS "********************************* Using user-defined creative-engine path: ${CREATIVE_ENGINE_PATH}")
else ()
    #    SET(CREATIVE_ENGINE_PATH "${CMAKE_SOURCE_DIR}/../creative-engine") # This broke debugging in clion!
    SET(CREATIVE_ENGINE_PATH "${CMAKE_SOURCE_DIR}/creative-engine")
    #   message(STATUS "<<<<<<<<<<<<<<<<<<<<<<<<<< Falling back to default creative-engine path: ${CREATIVE_ENGINE_PATH}")
endif ()

INCLUDE(FindPkgConfig)

# CMake Module path (additional cmake files via find_package)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)

ProcessorCount(N)
if(NOT N EQUAL 0)
    set(N, N*2)         # use hyperthreads - processor count is 4 on an i7 that has 8 cores including hyperthreads
    set(${PROJECT_NAME}_FLAGS -j${N})
    set(${PROJECT_NAME}_args ${${PROJECT_NAME}_args} PARALLEL_LEVEL ${N})
endif()

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
)

#Debug purposes
#ADD_COMPILE_DEFINITIONS(__XTENSA__=true)


# Exports:
# ${CREATIVE_ENGINE_INCLUDE_DIRS} - used in INCLUDE_DIRECTORIES(...)
# ${CREATIVE_ENGINE_SOURCE_FILES} - used in ADD_EXECUTABLE(...)
# ${CREATIVE_ENGINE_LINK_LIBRARIES} - used in TARGET_LINK_LIBRARIES(...)


########## RCOMP #########

# resource compiler
SET(RCOMP "${CREATIVE_ENGINE_PATH}/tools/rcomp")

# build rcomp-src
ADD_CUSTOM_COMMAND(
    OUTPUT rcomp
    COMMAND cd ${CREATIVE_ENGINE_PATH}/tools/rcomp-src && $(MAKE)  # $(MAKE) passes -j to make
    COMMENT "Building rcomp ${CREATIVE_ENGINE_PATH}"
)

# build Resources.bin
ADD_CUSTOM_COMMAND(
    OUTPUT Resources.bin
    COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "Resources.bin" -exec rm -f {} +
    COMMAND find ${CMAKE_CURRENT_BINARY_DIR} -name "BResourceManager.cpp.o" -exec rm -f {} +
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/src && ${RCOMP} Resources.r
    DEPENDS rcomp
    COMMENT "Compiling Resources ${CMAKE_CURRENT_SOURCE_DIR}"
)


# Used in INCLUDE_DIRECTORIES(...)
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

#This is for LibXMP minimal build
ADD_COMPILE_DEFINITIONS(LIBXMP_CORE_PLAYER=true)

# gather creative-engine sources
# Used in ADD_EXECUTABLE(...)
file(GLOB_RECURSE CREATIVE_ENGINE_SOURCE_FILES
    ${CREATIVE_ENGINE_PATH}/src/*.cpp
    ${CREATIVE_ENGINE_PATH}/src/*.c
)


SET(_CE_SDL2_LIBRARIES ${SDL2_LIBRARY})

# could change to "CE_DIRECT_LINUX_CONTROS_ENABLED=true make"
# Direct linux controls (non-SDL2!)
if (DEFINED ENV{CE_DIRECT_LINUX_CONTROS_ENABLED})
    ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DIRECT_LINUX_CONTROLS__=true)
else()
    ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_CONTROLS__=true)
endif()


# Network matrix display
if (DEFINED ENV{CE_NETWORK_DISPLAY_ENABLED})
    ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_NETWORK_DISPLAY__=true)

    INCLUDE_DIRECTORIES(
        ${CREATIVE_ENGINE_PATH}/src/Display/NetworkDisplay
        ${CREATIVE_ENGINE_PATH}/src/Controls/DirectLinuxControls
    )

    FIND_PACKAGE(Threads REQUIRED)
    FIND_PACKAGE(Boost REQUIRED)

    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

    SET(_BOOST_AND_THREADS
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_System}
    )

elseif (DEFINED ENV{CE_DESKTOP_DISPLAY_ENABLED})
    ADD_COMPILE_DEFINITIONS(__MODUS_TARGET_DESKTOP_DISPLAY__=true)

    #Append image for video
    SET(_CE_SDL2_LIBRARIES "${_CE_SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}")

    SET(_BOOST_AND_THREADS "")
endif()

# Used in TARGET_LINK_LIBRARIES(...)
SET(CREATIVE_ENGINE_LINK_LIBRARIES
    ${_CE_SDL2_LIBRARIES}
    ${_BOOST_AND_THREADS}
)

MESSAGE(CREATIVE_ENGINE_LINK_LIBRARIES " " ${CREATIVE_ENGINE_LINK_LIBRARIES})
