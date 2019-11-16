
FILE(GLOB RCOMP_SOURCE_FILES
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/*.cpp
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/*.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/tmx/*.cpp
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/tmx/*.h
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/lib/*.cpp
        ${CREATIVE_ENGINE_PATH}/tools/rcomp-src/lib/*.h
        )

ADD_EXECUTABLE(rcomp
        ${RCOMP_SOURCE_FILES}
        ${CREATIVE_ENGINE_PATH}/src/Panic.cpp ${CREATIVE_ENGINE_PATH}/src/Panic.h
        ${CREATIVE_ENGINE_PATH}/src/Memory.cpp ${CREATIVE_ENGINE_PATH}/src/Memory.h
        ${CREATIVE_ENGINE_PATH}/src/BBase.cpp ${CREATIVE_ENGINE_PATH}/src/BBase.h
        ${CREATIVE_ENGINE_PATH}/src/BBitmap.cpp ${CREATIVE_ENGINE_PATH}/src/BBitmap.h
        ${CREATIVE_ENGINE_PATH}/src/BList.cpp ${CREATIVE_ENGINE_PATH}/src/BList.h
        )
TARGET_INCLUDE_DIRECTORIES( rcomp PUBLIC   ${CREATIVE_ENGINE_PATH}/tools/rcomp-src )
