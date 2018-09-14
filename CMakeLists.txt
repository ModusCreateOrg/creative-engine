cmake_minimum_required(VERSION 3.9)
project(creative_engine)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(creative_engine
        images/modus.logo.svg
        include/BAnimSprite.h
        include/BBase.h
        include/BBitmap.h
        include/BGameEngine.h
        include/BList.h
        include/BPlayfield.h
        include/BProcess.h
        include/BResourceManager.h
        include/BSoundPlayer.h
        include/BSprite.h
        include/BTypes.h
        include/BViewPort.h
        include/Controls.h
        include/Display.h
        include/GameEngine.h
        include/Panic.h
        lib/BAnimSprite.cpp
        lib/BBase.cpp
        lib/BBitmap.cpp
        lib/BGameEngine.cpp
        lib/BList.cpp
        lib/BPlayfield.cpp
        lib/BProcess.cpp
        lib/BResourceManager.cpp
        lib/BSoundPlayer.cpp
        lib/BSprite.cpp
        lib/BTypes.cpp
        lib/BViewPort.cpp
        lib/Controls.cpp
        lib/Display.cpp
        lib/Panic.cpp
        CODE_OF_CONDUCT.md
        CONTRIBUTING.md
        LICENSE
        README.md)
